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
   CREATE AGGREGATE FUNCTION ta_atr_win RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION IF EXISTS ta_atr_win;
 */

typedef struct ta_atr_win_data_ {
    int current;
    double previous_close;
    double current_close;
    double previous_period_atr;
    double current_period_atr;
    double next_period_previous_atr;
    double values[];
} ta_atr_win_data;

DLLEXP my_bool ta_atr_win_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    ta_atr_win_data* data;
    int *periods = (int *)args->args[3];

    initid->maybe_null = 1;

    if (args->arg_count != 4) {
        strcpy(message, "ta_atr_win() requires four arguments");
        return 1;
    }

    if (args->arg_type[0] == DECIMAL_RESULT)
        args->arg_type[0] = REAL_RESULT;
    else if (args->arg_type[0] != REAL_RESULT) {
        strcpy(message, "ta_atr_win() requires a real as 1st argument");
        return 1;
    }

    if (args->arg_type[1] == DECIMAL_RESULT)
        args->arg_type[1] = REAL_RESULT;
    else if (args->arg_type[1] != REAL_RESULT) {
        strcpy(message, "ta_atr_win() requires a real as 2nd argument");
        return 1;
    }

    if (args->arg_type[2] == DECIMAL_RESULT)
        args->arg_type[2] = REAL_RESULT;
    else if (args->arg_type[2] != REAL_RESULT) {
        strcpy(message, "ta_atr_win() requires a real as 3rd argument");
        return 1;
    }

    if (args->arg_type[3] != INT_RESULT) {
        strcpy(message, "ta_atr_win() requires an integer as 4th argument");
        return 1;
    }

    if (!(data = (ta_atr_win_data *)malloc(sizeof(ta_atr_win_data) + *periods * sizeof(double)))) {
        strcpy(message, "ta_atr_win() couldn't allocate memory");
        return 1;
    }

    data->current = -1;
    data->previous_close = 0.0;
    data->current_period_atr = 0.0;
    data->previous_period_atr = 0.0;

    initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_atr_win %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
    return 0;
}

DLLEXP void ta_atr_win_deinit(UDF_INIT *initid)
{
    free(initid->ptr);
}

DLLEXP double ta_atr_win(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    ta_atr_win_data *data = (ta_atr_win_data *)initid->ptr;
    double *high = (double *)args->args[0];
    double *low = (double *)args->args[1];
    double *close = (double *)args->args[2];
    double sum = 0.0;
    int *periods = (int *)args->args[3];

    if (close == NULL) {
        *is_null = 1;
        return 0.0;
    }

    if (data->current > *periods) {
        double current_period_tr = (*high > data->previous_close ? *high : data->previous_close) - (*low < data->previous_close ? *low : data->previous_close);
        double current_period_atr = ((data->previous_period_atr * (*periods-1)) + current_period_tr) / *periods;
        data->next_period_previous_atr = current_period_atr;
        //fprintf(stderr, "atr\tprevious_atr=%f,current_tr=%f,calc_current_atr=%f\n", data->previous_period_atr, current_period_tr, current_period_atr);
        return current_period_atr;
    } else if (data->current == *periods) {
        data->values[data->current-1] = (*high > data->previous_close ? *high : data->previous_close) - (*low < data->previous_close ? *low : data->previous_close);
        for (int i = 0; i < *periods; i++) {
            sum += data->values[i];
            //fprintf(stderr, "values[%i] = %f\n", i, data->values[i]);
        }
        data->current_period_atr = sum / *periods;
        data->previous_period_atr = data->current_period_atr;
        data->next_period_previous_atr = data->current_period_atr;
        //fprintf(stderr, "avg = %f\n", data->current_period_atr );
        return data->current_period_atr;
    } else if (data->current > 0) {
        data->values[data->current-1] = (*high > data->previous_close ? *high : data->previous_close) - (*low < data->previous_close ? *low : data->previous_close);
    } else {
        *is_null = 1;
        return 0.0;
    }
    *is_null = 1;
    return 0.0;

}

void ta_atr_win_clear(UDF_INIT *initid, char *is_null, char *error) {
    ta_atr_win_data *data = (ta_atr_win_data *)initid->ptr;


    data->current++;
    if (data->current > 0) {
        data->previous_close = data->current_close;
        data->previous_period_atr = data->next_period_previous_atr;
        //fprintf(stderr,"clear\tSet previous_period_atr=%f\n", data->previous_period_atr);
    } else {
        *is_null = 1;
    }
    //fprintf(stderr, "clear\tcurrent=%i\n", data->current);
}

void ta_atr_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    ta_atr_win_data *data = (ta_atr_win_data *)initid->ptr;
    //double *high = (double *)args->args[0];
    //double *low = (double *)args->args[1];
    double *close = (double *)args->args[2];

    data->current_close = *close;

    //fprintf(stderr, "add\thigh=%f,low=%f,close=%f,last_c=%f,curr_=%f\n", *high, *low, *close, data->previous_close, data->current_close);
}

void ta_atr_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    //double *value = (double*)args->args[0];

    //fprintf(stderr, "reset %f\n", *value);
    //fflush(stderr);

}

void ta_atr_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

    //fprintf(stderr, "remove\n");
    //fflush(stderr);

}
