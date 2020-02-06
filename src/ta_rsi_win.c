/*
   Copyright (c) <2020> <Joao Costa>
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
	double last_gain;
	double last_loss;
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
    fprintf(stderr, "Init %i done\n", (*(int *) args->args[1]));
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

    if (data->current >= *periods + 1) {
        if (data->avg_loss == 0) {
            return 100;
        }
        //fprintf(stderr, "rsi=%f\n", 100 - 100 / ( 1 + (data->avg_gain / data->avg_loss)));
        return 100 - 100 / ( 1 + (data->avg_gain / data->avg_loss));
    } else {
        *is_null = 1;
        return 0.0;
    }
}

void ta_rsi_win_clear(UDF_INIT *initid, char *is_null, char *error) {


    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    double sum_of_gains = 0.0, sum_of_losses = 0.0;

    data->previous_close = data->current_close;
    data->current = data->current + 1;

    //fprintf(stderr, "clear\tprev_c=%f\n", data->previous_close);

    if (data->current < data->periods + 1) {
        if (data->next_gain_index + 1 < data->periods ) {
            data->next_gain_index = data->next_gain_index + 1;
            data->next_loss_index = data->next_loss_index + 1;
        } else {
            data->next_gain_index = 0;
            data->next_loss_index = data->periods;
        }
        return;
    } else if (data->current == data->periods + 1) {
            for (int i = 0; i < data->periods; i++) {
                sum_of_gains += data->values[i];
            }
            for (int i = data->periods; i < data->periods * 2; i++) {
                sum_of_losses += data->values[i];
            }
            data->avg_gain = sum_of_gains / data->periods;
            data->avg_loss = sum_of_losses / data->periods;
            //fprintf(stderr, "clear\tavg_g=%f\tavg_l=%f\n", data->avg_gain, data->avg_loss);
    }

    data->last_gain = data->avg_gain;
    data->last_loss = data->avg_loss;
    //fprintf(stderr, "clear\tlast_g=%f\tlast_l=%f\n", data->last_gain, data->last_loss);

    //fflush(stderr);
}

void ta_rsi_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    int *periods = (int *)args->args[1];
    double *value = (double*)args->args[0];
    double gain = 0.0, loss = 0.0;
    double change;

    data->current_close = *value;

    if (data->current > 0) {
        change = data->current_close - data->previous_close;
        if (change > 0) {
            gain = change;
        } else {
            loss = data->previous_close - data->current_close;
        }

        data->values[data->next_gain_index] = gain;
        data->values[data->next_loss_index] = loss;

        //fprintf(stderr, "add\tlast_g=%f\tlast_l=%f\n", data->last_gain, data->last_loss);
        if (data->current >= *periods) {
            data->avg_gain = (data->last_gain * (*periods - 1) + gain ) / *periods;
            data->avg_loss = (data->last_loss * (*periods - 1) + loss ) / *periods;
            //fprintf(stderr, "add\tcurr_c=%f\n", data->current_close);
            //fprintf(stderr, "add\tavg_g=%f\tavg_l=%f\n", data->avg_gain, data->avg_loss);
        } else {
            *is_null = 1;
        }
    } else {
        *is_null = 1;
    }

    //fflush(stderr);
}

void ta_rsi_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  double *value = (double*)args->args[0];

  //fprintf(stderr, "reset %f\n", *value);
  //fflush(stderr);

}

void ta_rsi_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  //fprintf(stderr, "remove\n");
  //fflush(stderr);

}
