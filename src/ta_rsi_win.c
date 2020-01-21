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
   CREATE AGGREGATE FUNCTION ta_rsi_win RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION ta_rsi_win;
 */

typedef struct ta_rsi_win_data_ {
	int current;
	double avg_gain;
	double avg_loss;
	double previous_close;
} ta_rsi_win_data;


DLLEXP my_bool ta_rsi_win_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_rsi_win_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 2) {
		strcpy(message, "ta_rsi_win() requires two arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_rsi_win() requires a real");
		return 1;
	}

	if (args->arg_type[1] != INT_RESULT) {
		strcpy(message, "ta_rsi_win() requires an integer");
		return 1;
	}

	if (!(data = (ta_rsi_win_data*)malloc(sizeof(ta_rsi_win_data)))) {
		strcpy(message, "ta_rsi_win() couldn't allocate memory");
		return 1;
	}

	data->avg_gain = 0.0;
	data->avg_loss = 0.0;
	data->current = -1;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_rsi_win %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_rsi_win_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

DLLEXP double ta_rsi_win(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
	double *value = (double *)args->args[0];
	int *periods = (int *)args->args[1];
	double currentGain = 0, currentLoss = 0;

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_rsi_win() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

	if (data->current == 1) {
		data->previous_close = *value;
		*is_null = 1;
		return 0.0;
	}

	if ((*periods) + 1 >= data->current) {
		if (*value > data->previous_close)
			data->avg_gain += *value - data->previous_close;
		else
			data->avg_loss += data->previous_close - *value;

		data->previous_close = *value;
		*is_null = 1;
		return 0.0;
	} else {
		if (*value > data->previous_close)
			currentGain = *value - data->previous_close;
		else
			currentLoss = data->previous_close - *value;

		if ((*periods) + 2 == data->current) {
			data->avg_gain = (data->avg_gain + currentGain) / *periods;
			data->avg_loss = (data->avg_loss + currentLoss) / *periods;
		} else {
			data->avg_gain = (data->avg_gain * (*periods - 1) + currentGain ) / *periods;
			data->avg_loss = (data->avg_loss * (*periods - 1) + currentLoss ) / *periods;
		}

		data->previous_close = *value;
		return 100 - 100 / ( 1 + (data->avg_gain / data->avg_loss));
	}
}

void ta_rsi_win_clear(UDF_INIT *initid, char *is_null, char *error) {


    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;

	data->current = data->current + 1;

    fprintf(stderr, "clear ta_rsi_win, current=%i\n", data->current);
    fflush(stderr);
}

void ta_rsi_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    int *periods = (int *)args->args[1];
    double *value = (double*)args->args[0];

	if (data->current == 0) {
		data->current = 1;
		data->previous_close = *value;
		*is_null = 1;
	}

    fprintf(stderr, "add ta_rsi_win periods=%i value=%f\n", *periods, *value);
    fflush(stderr);

}

void ta_rsi_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "reset ta_rsi_win\n");
  int *periods = (int *)args->args[1];
  double *value = (double*)args->args[0];

  fprintf(stderr, "reset ta_rsi_win %i %f\n", *periods, *value);
  fflush(stderr);

}

void ta_rsi_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "remove ta_rsi_win\n");
  fflush(stderr);

}
