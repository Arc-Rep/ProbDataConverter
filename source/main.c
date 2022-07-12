#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prob_data_utils.h"
#include "mysql_data.h"
#include "prob_data_args.h"



double obtain_row_probability(MYSQL_ROW row, PROB_CONVERSION prob_conversion, 
								double maximum, char* truth_value, int conversion_index) {
	double result_prob = 1;
	if (prob_conversion == BINARY_PROB)
	{
		if (strcmp(row[conversion_index], truth_value) == 0)
			result_prob = true;
		else result_prob = false;
	}
	else if (prob_conversion == QUANTITATIVE_PROB)
	{
		double row_prob = strtod(row[conversion_index], NULL);
		result_prob = row_prob / maximum;
	}
	else result_prob = 1;

	if (result_prob > 1) return 1;
	else if (result_prob < 0) return 0;

	return result_prob;
}

void convert_result_to_problog(MYSQL_RES *result, char* table_name, PROB_CONVERSION prob_conversion_method, 
			int column_prob_conversion_index, int maximum, char* truth_value, FILE* prolog_file, OUTPUT_FORMAT fact_format) {

	int num_fields = mysql_num_fields(result), current_row = 0, skip_row;
	MYSQL_ROW row = mysql_fetch_row(result);
	unsigned long *column_size, row_size;
	double predicate_prob;
	char* problog_statement, *predicate_arguments;

	while ((row = mysql_fetch_row(result)))
	{
		skip_row = 0;
		row_size = 0;
		column_size = mysql_fetch_lengths(result);
		for (int i = 0; i < num_fields; i++) 
		{
			if(column_size[i])
				row_size += column_size[i];
			else skip_row = 1;									//later add option for NULL values to be represented as that 
		}

		if (!skip_row)
		{
			predicate_prob = obtain_row_probability(row, prob_conversion_method, maximum, truth_value, column_prob_conversion_index);
			problog_statement = (char*) malloc(sizeof(char) * (30 + strlen(table_name) + num_fields * 3 + row_size + predicate_prob / 10));
			predicate_arguments = (char*) malloc(sizeof(char) * (num_fields * 3 + row_size));
			strcpy(predicate_arguments, "");

			for (int i = 0; i < num_fields; i++)
			{
				if (i != column_prob_conversion_index || prob_conversion_method == TAUTOLOGY)
				{
					strcat(predicate_arguments, row[i]);
					if (i < num_fields - 1 && ((prob_conversion_method == TAUTOLOGY) ||
						(!(i == num_fields - 2) && (column_prob_conversion_index == num_fields - 1))))
						strcat(problog_statement, ", ");
				}
			}

			if (fact_format == PROBLOG_FORMAT) 
				sprintf(problog_statement, "%.4lf::%s(%s).\n", predicate_prob, table_name, predicate_arguments);

			else if (fact_format == SKILL_FORMAT) 
				sprintf(problog_statement, "prob_example(%s(%s), %d,%.4lf).\n",table_name, predicate_arguments, (!predicate_prob ? 0 : 1) , predicate_prob);
			
			//print to file
			fprintf(prolog_file, "%s", problog_statement);
			printf("%s", problog_statement);
		}
		current_row++;
	}

	//reset row offset
	mysql_data_seek(result, 0);
	printf("\n\n");

}

long int assert_example_predicate_type(char** head_predicates, int number_of_predicates) {
	char buf[50], *end;
	long int n;

	printf("The following different predicate types have been found.\n");

	for (int i = 1; i <= number_of_predicates; i++) {
		printf( "%d. %s\n", i, head_predicates[i-1]);
	}

	printf( "Please select the index of the predicate that will be the focus of the induction process.\n");

	while (fgets(buf, sizeof(buf), stdin)) {
		n = strtol(buf, &end, 10);
		if (end == buf || *end != '\n') {
			printf( "Please enter an integer: ");
		}
		else if (n < 1 || n > number_of_predicates)
			printf( "Please write a valid predicate index:");
		else break;
	}

	return n - 1;
}





void prepare_fact_generation(MYSQL* con ,char* example_predicate, FILE* example_file, int fact_format) {

	// check if column types and returns index of column to turn to probability
	char* query_type = (char*)malloc(sizeof(char) * (100 + strlen(example_predicate))), // 100 for the size of the SQL SELECT statements
		*query_column_name = (char*)malloc(sizeof(char) * (100 + strlen(example_predicate)));
	long int row_index = 0, chosen_index, index_chosen = 0, maximum_chosen = 0, single_arg_choice = 0, prob_generation_method = TBD_PROB;
	double maximum = 0;
	char buf[20], *end,	 *str_truth = NULL;

	sprintf(query_type, "select data_type from information_schema.columns where table_name = '%s';", example_predicate);
	sprintf(query_column_name, "select column_name from information_schema.columns where table_name = '%s';", example_predicate);

	MYSQL_RES* types = do_query(con, query_type),* columns = do_query(con, query_column_name);
	MYSQL_ROW row_type, row_column;

	if (!mysql_num_rows(columns))
	{
		printf("NOTE: skipping current empty table.\n");
		return;
	}

	else if (mysql_num_rows(columns) != 1)
	{
		printf("Table contains following fields:\n");

		while ((row_column = mysql_fetch_row(columns)) && (row_type = mysql_fetch_row(types))) {
			printf("%lu. %s of type %s\n", row_index + 1, row_column[0], row_type[0]);
			row_index++;
		}

		printf("\nWhich of these should be translated as the fact's probabilistic value? (Enter to skip)\n");

		while (!index_chosen && fgets(buf, sizeof(buf), stdin)) {

			if (strcmp(buf, "\n") == 0) {
				prob_generation_method = TAUTOLOGY;
				index_chosen = 1;
			}

			chosen_index = strtol(buf, &end, 10);

			if (*end != '\n') {
				printf("Please enter an integer: ");
			}
			else if (chosen_index < 1 || chosen_index > row_index) {
				printf("Please write a valid column index:");
			}
			else { index_chosen = 1; }

		}
		chosen_index--;
		mysql_data_seek(columns, chosen_index);
		row_column = mysql_fetch_row(columns);
		mysql_data_seek(types, chosen_index);
		row_type = mysql_fetch_row(types);

		if (strcmp(row_type[0], "int") == 0 || strcmp(row_type[0], "decimal") == 0) 
		{
			prob_generation_method = QUANTITATIVE_PROB;
			printf("Use which maximum relative value? (Enter to find maximum automatically)\n");
			while (!maximum_chosen && fgets(buf, sizeof(buf), stdin)) {
				maximum = strtod(buf, &end);
				if (end == buf) {
					maximum = find_maximum_column_value(con, example_predicate, row_column[0]);
					maximum_chosen = 1;
				}
				else if (*end != '\n') {
					printf("Please enter some value: ");
				}
				else maximum_chosen = 1;
			}

		}

		else if (strcmp(row_type[0], "varchar") == 0) 
		{
			str_truth = (char*)malloc(sizeof(char) * 40);
			prob_generation_method = BINARY_PROB;
			printf("Which value of column %s should be treated as the resulting predicate's success?\n", example_predicate);
			fgets(str_truth, sizeof(str_truth), stdin);
			str_truth[strcspn(str_truth, "\n")] = 0;
			printf("\nUsing %s as the truth value for predicates of type %s\n", str_truth, example_predicate);
		}
		else 
		{
			printf("Data type only compatible with tautology-mode probabilistic assertion.\n");
			prob_generation_method = TAUTOLOGY;
			index_chosen = 1;
		}
	}
	else 
	{
		printf("Table contains only one argument. Do you want to treat every entry as a truth (1) or merely ignore the table (2)?\n");
		while (!single_arg_choice && fgets(buf, sizeof(buf), stdin)) {
			single_arg_choice = strtod(buf, &end);

			if (*end != '\n' || end == buf || (single_arg_choice != 1 && single_arg_choice != 2)) 
			{
				printf("Please enter some permitted value: ");
				single_arg_choice = 0;
			}
			else if(single_arg_choice == 1) 
			{
				prob_generation_method = TAUTOLOGY;
				chosen_index = 0;
			}
			else if(single_arg_choice == 2)
				prob_generation_method = NO_PROB_CONVERSION;
		}
		
	}

	// ask which value to become true/false or which value to become the relative maximum, if any

	if (prob_generation_method)
	{
		// create probabilistic facts and append them to file
		char* table_query = (char*)malloc(sizeof(char) * (20 + strlen(example_predicate)));
		sprintf(table_query, "select * from %s;", example_predicate);
		MYSQL_RES* result = do_query(con, table_query);
		convert_result_to_problog(result, example_predicate, prob_generation_method, chosen_index, maximum, str_truth, example_file, fact_format);
		
	}
	
}


void generate_examples(MYSQL* con, char* output_file, char* example_predicate) {
	char* example_filename = (char*) malloc(sizeof(char) * (strlen(output_file) + strlen("output/_examples.pl")));
	sprintf(example_filename, "output/%s_examples.pl", output_file);

	FILE *example_file = fopen(example_filename, "w+");

	prepare_fact_generation(con, example_predicate, example_file, example_format);

	fclose(example_file);
}

void generate_probabilistic_knowledge(MYSQL* con, char* output_file, char** head_predicates, int example_index, int number_of_predicates) {
	char* prob_backgroud_filename = (char*) malloc(sizeof(char) * (strlen(output_file) + strlen("output/_prob_background.pl")));
	sprintf(prob_backgroud_filename, "output/%s_prob_background.pl", output_file);

	FILE *knowledge_base = fopen(prob_backgroud_filename, "w+");

	for (int i = 0; i < number_of_predicates; i++) {

		if(i != example_index) 
		{
			printf("Extracting probabilistic knowledge from table %s.\n\n", head_predicates[i]);
			prepare_fact_generation(con, head_predicates[i], knowledge_base, PROBLOG_FORMAT);
		}
	}
	fclose(knowledge_base);
}

void create_problog_database(MYSQL* con, char* output_file, char** head_predicates, int number_of_predicates) {
	int example_index = assert_example_predicate_type(head_predicates, number_of_predicates);

	printf("So, the examples are of predicate type %s.\n", head_predicates[example_index]);

	generate_examples(con, output_file, head_predicates[example_index]);
	generate_probabilistic_knowledge(con, output_file, head_predicates, example_index, number_of_predicates);
}




int main(int argc, char **argv) { 
	int number_of_predicates;
	MYSQL* con;
	char** head_predicates = NULL, *output_file = (char*) malloc(sizeof(char) * 100);
	
	// Process provided command

	con = process_arguments(argv, argc, output_file);
	number_of_predicates = get_head_predicates(con, &head_predicates);

	if (!head_predicates || !number_of_predicates) 
	{
		mysql_close(con);
		finish_with_error("ERROR: No SQL tables found\n");
	}
	
	create_problog_database(con, output_file, head_predicates, number_of_predicates);

	return 0;

}