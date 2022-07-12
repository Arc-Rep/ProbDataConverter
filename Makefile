
# Project Data
PROJ_NAME=probdataconv

# .c files
C_SOURCE=$(wildcard ./source/*.c)
 
# .h files
H_SOURCE=$(wildcard ./source/*.h)
 
# Object files
OBJ=$(subst .c,.o,$(subst source,objects,$(C_SOURCE)))

#libraries
LIBS = -lmysqlclient

#Compiler and flags
CC = gcc
CFLAGS =-c				\
		-Wall

INC = -I/usr/include/mysql

# Command used at clean target
RM = rm -rf

all: objFolder $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	@ echo 'Building binary using GCC linker: $@'
	$(CC) $^ $(LIBS) -o $@
	@ echo 'Finished building binary: $@'
	@ echo ' '

./objects/mysql_data.o: ./source/mysql_data.c ./source/mysql_data.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(INC) $(CFLAGS) -o $@
	@ echo ' '

./objects/prob_data_utils.o: ./source/prob_data_utils.c ./source/prob_data_utils.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(INC) $(CFLAGS) -o $@
	@ echo ' '

./objects/%.o: ./source/%.c ./source/%.h
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(INC) $(CFLAGS) -o $@
	@ echo ' '

./objects/main.o: ./source/main.c $(H_SOURCE)
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(INC) $(CFLAGS) -o $@
	@ echo ' '

objFolder:
	@ mkdir -p objects
	@ mkdir -p output

clean:
	@ $(RM) ./objects/*.o $(PROJ_NAME) *~
	@ rmdir objects