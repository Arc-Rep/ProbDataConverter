# ProbDataConverter

ProbDataConverter consists on a database converter and generator that takes MySQL deterministic relational databases and adapts them to a Probabilistic Logic Programming (PLP) seting to be used alongside i.e. Probabilistic Inductive Logic Programming (PILP)-based learners such as ProbFOIL+, SkiLL or SafeLearner. It therefore divides the database into two probabilistic fact sets: one for the Probabilistic Example (PE) set and another for the Probabilistic Background Knowledge (PBK) one. 

## Dependencies
* A Linux based operating system or Windows subsystem for Linux;
* MySQL (the target database must be active at the time of operation);
* Make and C compiler 

## Installation
ProbDataConverter makes use of the make paradigm in order to establish an easy and straightforward compilation procedure. To do this, simply create a bash terminal that points to the project's folder and write:

    make

And that's it! Should the whole make instance be successful, ProbDataConverter has been installed on your computer and is ready to go.

#### Note before starting
Please make sure you have the MySQL service instance running before making starting and that you have an available database to be fetched by ProbDataConverter. To do this, write the following command:

    service mysql start

## How to use
In order to use ProbDataConverter to convert a MySQL database to a probabilistic database have a batch that is pointing to the project's root folder and write the following command while changing the machine host (most surely will be localhost), MySQL user, password and target database to their correspondent names according to your setup:

    probdataconv -c 'machine_host' 'mysql_user' 'mysql_password' 'database_name'

ProbDataConv will then analyze all the tables within the database and will ask which is the target table that is to be converted into the probabilistic examples (PE), which are the focus concept that your PILP-based learner will attempt to, well... learn. 
This table is supposed to have one column detaining all examples and another with values that are to be turned into the corresponding example's probabilistic values (let's call this column the PV). ProbDataConverter supports number or string based values to be converted to probabilities. 
 - Should the PV be of the form of a string or a boolean (containing values such as 'true'/'false' or 'success'/'fail') you will be asked to write which of the values will be attributed to positive examples and which will be negative ones (if you write 'success', all examples that have a value called success in the PV column will be treated as having probability 1). All examples with PV values that are different than the one you wrote will have probability 0. 
 - Should the PV be of the form of a quantitative value, you can choose to establish a maximum PV that if reached or surpassed, makes the corresponding example be of value 1. Every other value is associated with a relative value between that maximum value and 0 (i.e. half the maximum value becomes 0.5). You may choose not to write this maximum value and ProbDataConverter will search for the largest PV found in the table and treat that value as the maximum.
 - Should you skip the PV establishment phase of any table, all entries are treated as tautologies and have probability 1 associated.
 
Afterwards, ProbDataConverter will analyze each other table present within the database and repeat the process so that it can understand which column is to be treated as the PV for that table entry. This process is similar to the PE preparation. Be sure to make the example column of each table (the concept you are attempting to learn) to be the first column of each of these tables.

ProbDataConverter then sends an output.pl file to the project root folder that is your probabilistic database. You can write the probdataconv command along with many parameters that customize its runtime to your preferences. These are:
    -c \[MYSQL_HOST\] \[MYSQL_USER\] \[MYSQL_PASS\] \[MYSQL_DATABASE\], invoke mysql connection
    -h, display help
    -o \[OUTPUT_FILE_NAME\], define output name for the probabilistic database (default: output)
    -t 'problog_format'/'skill_format', define probabilistic database format (default: problog)





