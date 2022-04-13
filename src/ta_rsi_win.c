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

/*
   CREATE AGGREGATE FUNCTION ta_rsi_win RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION ta_rsi_win;
 */

typedef struct ta_rsi_win_data_ {
	int current;
	int periods;
	double avg_gain;
	double avg_loss;
	double last_avg_gain;
	double last_avg_loss;
	double previous_close;
	double current_close;
	int next_gain_index;
	int next_loss_index;
	double values[];
} ta_rsi_win_data;


/*
void debug_data(char *label, ta_rsi_win_data *data) {

	fprintf(stderr, "[%s]\n", label);
	fprintf(stderr, "  current = %d\n", data->current);
	fprintf(stderr, "  periods = %d\n", data->periods);
	fprintf(stderr, "  avg_gain = %f\n", data->avg_gain);
	fprintf(stderr, "  avg_loss = %f\n", data->avg_loss);
	fprintf(stderr, "  last_avg_gain = %f\n", data->last_avg_gain);
	fprintf(stderr, "  last_avg_loss = %f\n", data->last_avg_loss);
	fprintf(stderr, "  previous_close = %f\n", data->previous_close);
	fprintf(stderr, "  current_close = %f\n", data->current_close);
	fprintf(stderr, "  GAINS\n  -----\n");
	for (int i = 0; i < data->periods; i++) {
		fprintf(stderr, "    values[%d] = %f\n", i, data->values[i]);
	}
	fprintf(stderr, "  LOSSES\n  -----\n");
	for (int i = data->periods; i < data->periods * 2; i++) {
		fprintf(stderr, "    values[%d] = %f\n", i, data->values[i]);
	}

}
*/

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
	data->current_close = 0;
	data->previous_close = 0;
	data->next_gain_index = *periods-1;
	data->next_loss_index = *periods * 2 - 1;
	for (int i=0; i<*periods * 2; i++) {
		data->values[i] = 0;
	}

	initid->ptr = (char*)data;
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
        double sum_of_gains = 0, sum_of_losses = 0;

	if (args->args[0] == NULL) {
		if (data->current > 0) {
			strcpy(error, "ta_rsi_win() Can't handle NULL values in middle of dataset");
			return 1;
		}
		*is_null = 1;
		return 0.0;
	}

    //debug_data("win begin", data);
    if (data->current == *periods) {
        for (int i = 0; i < data->periods; i++) {
            sum_of_gains += data->values[i];
        }
        for (int i = data->periods; i < data->periods * 2; i++) {
            sum_of_losses += data->values[i];
        }

        data->avg_gain = sum_of_gains / *periods;
        data->avg_loss = sum_of_losses / *periods;
    } else if (data->current < *periods) {
        *is_null = 1;
        return 0.0;
    }

    //debug_data("win end", data);
    if (data->avg_loss == 0) {
        return 100;
    }
    return 100 - 100 / ( 1 + (data->avg_gain / data->avg_loss));
}

void ta_rsi_win_clear(UDF_INIT *initid, char *is_null, char *error) {


    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    //fprintf(stderr, "\n");
    //debug_data("clear begin", data);

	if (data->current_close == 0 && data->previous_close == 0) {
		fprintf(stderr, "[clear] Not ready yet\n");
		data->current = data->current + 1;
		return;
	}

    data->previous_close = data->current_close;
    data->current = data->current + 1;


        if (data->next_gain_index + 1 < data->periods ) {
            data->next_gain_index = data->next_gain_index + 1;
            data->next_loss_index = data->next_loss_index + 1;
        } else {
            data->next_gain_index = 0;
            data->next_loss_index = data->periods;
        }
    if (data->current < data->periods + 1) {
        return;
    }

    data->last_avg_gain = data->avg_gain;
    data->last_avg_loss = data->avg_loss;
    //debug_data("clear end", data);

    //fflush(stderr);
}

void ta_rsi_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    ta_rsi_win_data *data = (ta_rsi_win_data *)initid->ptr;
    int *periods = (int *)args->args[1];
    double *value = (double*)args->args[0];
    double gain = 0.0, loss = 0.0;
    double change;
    //char s[1000];
    //sprintf(s,"add begin value = %f", *value);
    //debug_data(s, data);

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

        if (data->current >= *periods) {
            data->avg_gain = (data->last_avg_gain * (*periods - 1) + gain ) / *periods;
            data->avg_loss = (data->last_avg_loss * (*periods - 1) + loss ) / *periods;
        } else {
            *is_null = 1;
        }
    } else {
        *is_null = 1;
    }

    //debug_data("add end", data);
    //fflush(stderr);
}

void ta_rsi_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  //double *value = (double*)args->args[0];

  //fprintf(stderr, "reset %f\n", *value);
  //fflush(stderr);

}

void ta_rsi_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  //fprintf(stderr, "remove\n");
  //fflush(stderr);

}
