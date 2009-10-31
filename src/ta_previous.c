/*
   Copyright (c) <2009> <João Costa>
   Dual licensed under the MIT and GPL licenses.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <ctype.h>

extern int getNextSlot(int, int );

/*
   CREATE FUNCTION ta_previous RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION previous;
 */

typedef struct ta_previous_data {
	int current;
	int next_slot;
	double values[];
} ta_previous_data;

my_bool ta_previous_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_previous_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 2) {
		strcpy(message, "ta_previous() requires two arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_previous() requires a real");
		return 1;
	}

	if (args->arg_type[1] != INT_RESULT) {
		strcpy(message, "ta_previous() requires an integer");
		return 1;
	}

	if (!(data = (ta_previous_data *)malloc(sizeof(ta_previous_data) + (*(int *)args->args[1] + 1) * sizeof(double)))) {
		strcpy(message, "ta_previous() couldn't allocate memory");
		return 1;
	}

	data->current   = 0;
	data->next_slot = 0;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_previous %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

void ta_previous_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

double ta_previous(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_previous_data *data = (ta_previous_data *)initid->ptr;
	int *periods = (int *)args->args[1];

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_previous() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

	data->current = data->current + 1;
	data->values[data->next_slot] = *((double*)args->args[0]);
	data->next_slot = getNextSlot(data->next_slot, *periods + 1);

	if (*periods + 1 > data->current) {
		*is_null = 1;
		return 0.0;
	} else
		return data->values[data->next_slot];
}
