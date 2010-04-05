/*
   Copyright (c) <2010> <João Costa>
   Dual licensed under the MIT and GPL licenses.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <ctype.h>
#include "ta_libmysqludf_ta.h"

/*
   CREATE FUNCTION ta_sum RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION ta_sum;
 */

int getNextSlot(int currentSlot, int ringSize)
{
	if (currentSlot + 1 == ringSize)
		return 0;
	return ++currentSlot;
}

typedef struct ta_sum_data_ {
	double sum;
	int current;
	double last_value;
	int next_slot;
	double values[];
} ta_sum_data;

DLLEXP my_bool ta_sum_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_sum_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 2) {
		strcpy(message, "ta_sum() requires two arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_sum() requires a real");
		return 1;
	}

	if (args->arg_type[1] != INT_RESULT) {
		strcpy(message, "ta_sum() requires an integer");
		return 1;
	}

	if (!(data = (ta_sum_data *)malloc(sizeof(ta_sum_data) + (*(int *)args->args[1]) * sizeof(double)))) {
		strcpy(message, "ta_sum() couldn't allocate memory");
		return 1;
	}

	data->sum = 0;
	data->current = 0;
	data->next_slot = 0;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_sum %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_sum_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

DLLEXP double ta_sum(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_sum_data *data = (ta_sum_data *)initid->ptr;
	int *periods = (int *)args->args[1];

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_sum() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

	data->current = data->current + 1;
	data->values[data->next_slot] = *((double*)args->args[0]);
	data->next_slot = getNextSlot(data->next_slot, *periods);

	if (*periods > data->current) {
		*is_null = 1;
		return 0.0;
	} else {
		double sum = 0.0;
		int i;
		for (i = 0; i < *periods; i++)
			sum += data->values[i];

		return sum;
	}
}
