#include <stdio.h>
#include <string.h>
#include <mysql.h>


void close_connection(MYSQL* con) {
	mysql_close(con);
}

void mysql_finish_with_error(MYSQL *con) {
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);
}

MYSQL_RES* do_query(MYSQL* con, char* query) {
	
	if (mysql_query(con, query)) mysql_finish_with_error(con);

	return mysql_store_result(con);
}

MYSQL* establish_connection(char* mysqlhost, char* mysqluser, char* mysqlpass, char* database) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	if (mysql_real_connect(con, mysqlhost, mysqluser, mysqlpass, database, 
		 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}
	
	return con;
}

char** get_table_list(MYSQL_RES *result) {
	int current_row = 0;
	MYSQL_ROW row;
	unsigned long number_of_tables = mysql_num_rows(result), *predicate_length;
	char** prolog_predicates = (char**)malloc(sizeof(char*) * number_of_tables);

	while ((row = mysql_fetch_row(result)))
	{
		predicate_length = mysql_fetch_lengths(result);
		prolog_predicates[current_row] = malloc(sizeof(char) * (predicate_length[0] + 1));
		strcpy(prolog_predicates[current_row], row[0]);
		current_row++;
	}

	mysql_data_seek(result, 0);
	return prolog_predicates;
}


long int get_head_predicates(MYSQL* con, char*** head_predicates) {

	MYSQL_RES *sql_table_result = do_query(con, "SHOW TABLES;");
	*head_predicates = get_table_list(sql_table_result);
	
	return mysql_num_rows(sql_table_result);
}

// useful to conclude the probabilistic values attributed to each example or PBK fact

double find_maximum_column_value(MYSQL* con, char* table, char* column_to_be_converted) {
	char* value_query = (char*)malloc(sizeof(char) * (30 + strlen(column_to_be_converted) + strlen(table)));
	MYSQL_ROW row;
	double max, temp;

	sprintf(value_query, "select %s from %s;", column_to_be_converted, table);

	MYSQL_RES* result = do_query(con, value_query);

	for(int i = 0; (row = mysql_fetch_row(result)); i++) {
		temp = strtod(row[0], NULL);
		if (i == 0) max = temp;
		else if (temp > max) max = temp;
	}
	return max;
}

