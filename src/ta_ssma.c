/*
   Copyright (c) <2020> <Joao Costa>
   Dual licensed under the MIT and GPL licenses.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mysql.h>
#include <ctype.h>
#include "ta_libmysqludf_ta.h"

extern int getNextSlot(int, int );

/*
   CREATE FUNCTION ta_ssma RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION IF EXISTS ssma;
 */

typedef struct ta_ssma_data_ {
	int current;
	double last_value;
	double values[];
} ta_ssma_data;

DLLEXP my_bool ta_ssma_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_ssma_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 2) {
		strcpy(message, "ta_ssma() requires two arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_ssma() requires a real");
		return 1;
	}

	if (args->arg_type[1] != INT_RESULT) {
		strcpy(message, "ta_ssma() requires an integer");
		return 1;
	}

	if (!(data = (ta_ssma_data *)malloc(sizeof(ta_ssma_data) + (*(int *)args->args[1]) * sizeof(double)))) {
		strcpy(message, "ta_ssma() couldn't allocate memory");
		return 1;
	}

	data->current = 0;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_ssma %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_ssma_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

DLLEXP double ta_ssma(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_ssma_data *data = (ta_ssma_data *)initid->ptr;
	int *periods = (int *)args->args[1];
	int periods_minus_one = *periods - 1;
	double *current_value = (double*)args->args[0];

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_ssma() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

    if (periods_minus_one < data->current) {
	    data->last_value = ((data->last_value * (periods_minus_one)) + *current_value) / *periods;
	} else if (periods_minus_one == data->current) {
		double sum = 0.0;
		int i = 0;
	    data->values[data->current++] = *current_value;
		for (i = 0; i < *periods; i++)
			sum += data->values[i];

		data->last_value = sum / *periods;
	} else {
	    data->values[data->current++] = *current_value;
		*is_null = 1;
		return 0.0;
	}
	return data->last_value;
}
