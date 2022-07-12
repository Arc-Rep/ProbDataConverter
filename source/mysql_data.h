#ifndef MYSQL_DATA_H
#define MYSQL_DATA_H

void mysql_finish_with_error(MYSQL *con);
MYSQL* establish_connection(char* mysqlhost, char* mysqluser, char* mysqlpass, char* database);
void close_connection(MYSQL* con);
char** get_table_list(MYSQL_RES *result);
long int get_head_predicates(MYSQL* con, char*** head_predicates);
MYSQL_RES* do_query(MYSQL* con, char* query);
double find_maximum_column_value(MYSQL* con, char* table, char* column_to_be_converted);

#endif