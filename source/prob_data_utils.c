#include <stdio.h>
#include <string.h>
#include <mysql.h>

const int DETERMINISTIC = 0;
const int PROBABILITY = 1;

void finish_with_error(char* error){
    fprintf(stderr, "%s",error);
    exit(1);
}


