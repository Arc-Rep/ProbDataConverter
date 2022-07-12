#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include "prob_data_args.h"
#include "prob_data_utils.h"
#include "mysql_data.h"

PROVIDED_ARGUMENTS example_format = SKILL_FORMAT;

// Display help message

void arg_h_display(){
    printf("USAGE: probdataconv [PARAMETER] [ARGUMENT]\n"
				"-c, invoke mysql connection - args: [MYSQL_HOST] [MYSQL_USER] [MYSQL_PASS] [MYSQL_DATABASE]\n"
				"-h, invoke help\n"
				"-o, define output name for the probabilistic database\n"
				"-t, define probabilistic database format (default: problog) - args: problog_format/skill_format\n"
				);
}

// Define example format verification for either SKILL, or learners that use direct problog syntax (SLIPCOVER)    

void arg_t_prob_format(char* format){

    if (strcmp(format, "skill_format") == 0)
		example_format = SKILL_FORMAT;
			
	else if (strcmp(format, "problog_format") == 0)
		example_format = PROBLOG_FORMAT;

    else finish_with_error("Error: Please specify a probabilistic database format (\"skill_format\"/\"problog_format)\"\n");

}

MYSQL* arg_c_connection(char* host, char* user, char* pass, char* database){
    MYSQL* con = establish_connection(host, user, pass, database);

	if(!con) mysql_finish_with_error(con);

    return con;	
}

/** check if provided arguments are acceptable and resolve command **/

MYSQL* process_arguments(char** argv, int argc, char* output_file) {
	MYSQL* con = NULL;
	PROVIDED_ARGUMENTS arg_types = NO_PROV_ARGS;


	for (int i = 1; i < argc; i++) {	//starts at 1 to ignore command head

		// Display help message

		if (strcmp(argv[i], "-h") == 0)
		{
			arg_types = arg_types | H_ARG;

			if(argc == 2) arg_h_display();
			else finish_with_error("Error: To obtain help, please only type: probdataconv -h\n");
			exit(1);
		}


		// Define example format verification for either SKILL, or learners that use direct problog syntax (SLIPCOVER)

		else if (strcmp(argv[i], "-f") == 0)
		{

			if(arg_types & F_ARG) finish_with_error("Error: Database format already specified. Please do not repeat arguments.\n");

			arg_types = arg_types | F_ARG;

			if (!(i + 1 < argc)) finish_with_error("Error: Please specify a database format.\n");
			
			else arg_t_prob_format(argv[i+1]);

			i++;
		}

		// Define connection to MYSQL database

		else if (strcmp(argv[i], "-c") == 0)
		{

			if(arg_types & C_ARG) finish_with_error("Error: Connection already specified. Please do not repeat arguments.\n");

			arg_types = arg_types | C_ARG;

			if (i + 4 < argc) con = arg_c_connection(argv[i+1], argv[i+2], argv[i+3], argv[i+4]);
			else 
				finish_with_error("Error: Please insert all required connection arguments in the form of: \n"
			   		   "probdataconv -c \"mysql_host\" \"mysql_user\" \"mysql_pass\" \"mysql_database\" *other_parameters*\n");
	
			
			i = i + 4;
		}

		// Define output file to save probabilistic database
		
		else if (strcmp(argv[i], "-o") == 0)
		{
			if(arg_types & O_ARG) finish_with_error("Error: Output project name already specified. Please do not repeat arguments.\n");
			
			arg_types = arg_types | O_ARG;

			if(i + 1 < argc) strcpy(output_file, argv[i + 1]);
			else finish_with_error("Error: Please define an output project name for your resulting probabilistic database.\n");
			
			i++;
		}
	}

	if(!con) finish_with_error("Error: No mysql database connection was specified. Quitting...\n");

	if(!O_ARG) strcpy(output_file, "output"); // should no output file be specified, take output as a default
	
	return con;	
} 