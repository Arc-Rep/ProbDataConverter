#ifndef PROB_DATA_UTILS_H
#define PROB_DATA_UTILS_H

typedef enum {
	NO_PROB_CONVERSION = 0,
	TAUTOLOGY,
	BINARY_PROB,
	QUANTITATIVE_PROB,
	TBD_PROB
} PROB_CONVERSION;

typedef enum {
	SKILL_FORMAT,
	PROBLOG_FORMAT
} OUTPUT_FORMAT;

extern const int DETERMINISTIC;
extern const int PROBABILITY;

void finish_with_error(char* error);

#endif

