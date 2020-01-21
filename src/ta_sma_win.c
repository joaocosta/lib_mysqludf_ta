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
   CREATE FUNCTION ta_sma_win RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION sma_win;
 */

typedef struct ta_sma_win_data_ {
	double sum;
	int current;
	double last_value;
	int next_slot;
	int ring_size;
	double values[];
} ta_sma_win_data;

void moveToNextSlot(ta_sma_win_data *data)
{
    if (data->next_slot + 1 != data->ring_size) {
        data->next_slot = data->next_slot+1;
    } else {
        data->next_slot = 0;
    }
}

DLLEXP my_bool ta_sma_win_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_sma_win_data* data;
	int *periods = (int *)args->args[1];

	initid->maybe_null = 1;

	if (args->arg_count != 2) {
		strcpy(message, "ta_sma_win() requires two arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_sma_win() requires a real");
		return 1;
	}

	if (args->arg_type[1] != INT_RESULT) {
		strcpy(message, "ta_sma_win() requires an integer");
		return 1;
	}

	if (!(data = (ta_sma_win_data *)malloc(sizeof(ta_sma_win_data) + *periods * sizeof(double)))) {
		strcpy(message, "ta_sma_win() couldn't allocate memory");
		return 1;
	}

	data->ring_size = *periods;
	data->sum = 0;
	data->current = 0;
	data->next_slot = -1;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_sma_win %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_sma_win_deinit(UDF_INIT *initid)
{
    free(initid->ptr);
}

DLLEXP double ta_sma_win(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_sma_win_data *data = (ta_sma_win_data *)initid->ptr;
	int *periods = (int *)args->args[1];
	double *value = (double*)args->args[0];

    fprintf(stderr, "cal ta_sma_win %i %f arg_count=%i, current = %i\n", *periods, *value, args->arg_count, data->current);
    fflush(stderr);
	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_sma_win() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

	if (*periods > data->current) {
		*is_null = 1;
		return 0.0;
	} else {
		double sum = 0.0;
		int i = 0;
		for (i = 0; i < *periods; i++)
			sum += data->values[i];

        for (i = 0; i < *periods; i++) {
            fprintf(stderr, "cal ta_sma_win values[%i]=%f\n" , i, data->values[i]);
        }
        fprintf(stderr, "cal ta_sma_win sum=%f\n" , sum);
        fflush(stderr);

		return sum / *periods;
	}
}

void ta_sma_win_clear(UDF_INIT *initid, char *is_null, char *error) {


    ta_sma_win_data *data = (ta_sma_win_data *)initid->ptr;
    data->current = data->current + 1;
    moveToNextSlot(data);

    fprintf(stderr, "clear ta_sma_win, current=%i, next=%i\n", data->current, data->next_slot);
    fflush(stderr);
}

void ta_sma_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    ta_sma_win_data *data = (ta_sma_win_data *)initid->ptr;
    int *periods = (int *)args->args[1];
    double *value = (double*)args->args[0];

    data->values[data->next_slot] = *value;

    fprintf(stderr, "add ta_sma_win periods=%i value=%f, next_slot=%i\n", *periods, *value, data->next_slot);
    fflush(stderr);

}

void ta_sma_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "reset ta_sma_win\n");
  int *periods = (int *)args->args[1];
  double *value = (double*)args->args[0];

  fprintf(stderr, "reset ta_sma_win %i %f\n", *periods, *value);
  fflush(stderr);

}

void ta_sma_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "remove ta_sma_win\n");
  fflush(stderr);

}
