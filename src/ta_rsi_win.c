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
	int periods;
	double avg_gain;
	double avg_loss;
	double previous_close;
	double current_close;
	int next_gain_index;
	int next_loss_index;
	double values[];
} ta_rsi_win_data;


DLLEXP my_bool ta_rsi_win_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_rsi_win_data* data;
	int *periods = (int *)args->args[1];

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

	if (!(data = (ta_rsi_win_data*)malloc(sizeof(ta_rsi_win_data) + *periods * sizeof(double) * 2))) {
		strcpy(message, "ta_rsi_win() couldn't allocate memory");
		return 1;
	}

	data->avg_gain = 0.0;
	data->avg_loss = 0.0;
	data->current = -1;
	data->periods = *periods;
    data->next_gain_index = *periods-1;
    data->next_loss_index = *periods * 2 - 1;
    for (int i=0; i<*periods * 2; i++) {
        data->values[i] = 0;
    }

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
	int *periods = (int *)args->args[1];

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_rsi_win() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

    if (data->current >= *periods) {
        if (data->avg_loss == 0) {
            return 100;
        }
        return 100 - 100 / ( 1 + (data->avg_gain / data->avg_loss));
    } else {
        *is_null = 1;
        return 0.0;
    }
}

void ta_rsi_win_clear(UDF_INIT *initid, char *is_null, char *error) {


    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;

    data->previous_close = data->current_close;
    data->current = data->current + 1;

    if (data->next_gain_index + 1 < data->periods ) {
        data->next_gain_index = data->next_gain_index + 1;
        data->next_loss_index = data->next_loss_index + 1;
    } else {
        data->next_gain_index = 0;
        data->next_loss_index = data->periods;
    }

//    fprintf(stderr, "\n\nclear\nprevious_close=%f\ncurrent=%i\ngain index = %i\nloss index = %i\n\n", data->previous_close, data->current, data->next_gain_index, data->next_loss_index);
//    fflush(stderr);
}

void ta_rsi_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    int *periods = (int *)args->args[1];
    double *value = (double*)args->args[0];
    double gain = 0.0, loss = 0.0;
    double sum_of_gains = 0.0, sum_of_losses = 0.0;
    double change;

    data->current_close = *value;

//    fprintf(stderr, "current=%i\ncurrent_close=%f\n\n", data->current, data->current_close);

    if (data->current > 0) {
        change = data->current_close - data->previous_close;
        if (change > 0) {
            gain = change;
        } else {
            loss = data->previous_close - data->current_close;
        }

//        fprintf(stderr, "change = %f\ngain = %f\nloss = %f\n", change, gain, loss);

        data->values[data->next_gain_index] = gain;
        data->values[data->next_loss_index] = loss;
//        for (int i=0; i<*periods * 2; i++) {
//            fprintf(stderr, "[%s] values[%i] = %f\n", (i < *periods ? "gain" : "loss"), i, data->values[i]);
//        }

        if (data->current >= *periods) {
            for (int i = 0; i < *periods; i++) {
                sum_of_gains += data->values[i];
            }
            for (int i = *periods; i < *periods * 2; i++) {
                sum_of_losses += data->values[i];
            }
            data->avg_gain = sum_of_gains / *periods;
            data->avg_loss = sum_of_losses / *periods;
//            fprintf(stderr, "avg_gain = %f\navg_loss = %f\n", data->avg_gain, data->avg_loss);
        } else {
//            fprintf(stderr, "avg_gain = null\navg_loss = null\n");
            *is_null = 1;
        }
    } else {
//        fprintf(stderr, "change = null\ngain = null\nloss = null\n");
        *is_null = 1;
    }

//    fflush(stderr);
}

void ta_rsi_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "reset ta_rsi_win\n");
  double *value = (double*)args->args[0];

  fprintf(stderr, "reset ta_rsi_win %f\n", *value);
  fflush(stderr);

}

void ta_rsi_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  fprintf(stderr, "remove ta_rsi_win\n");
  fflush(stderr);

}
