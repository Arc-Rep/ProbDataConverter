
#ifndef PROB_DATA_ARGS_H
#define PROB_DATA_ARGS_H

typedef enum{
	NO_PROV_ARGS  = 0,
	H_ARG = 0x1,
	O_ARG = 0x10,
	F_ARG = 0x100,
	C_ARG = 0x1000
} PROVIDED_ARGUMENTS;

extern PROVIDED_ARGUMENTS example_format;

#endif

MYSQL* process_arguments(char** argv, int argc, char* output_file);

