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
   CREATE AGGREGATE FUNCTION ta_tr_win RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION IF EXISTS ta_tr_win;
 */

typedef struct ta_tr_win_data_ {
	double previous_close;
	double current_close;
	short int init;
} ta_tr_win_data;

DLLEXP my_bool ta_tr_win_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_tr_win_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 3) {
		strcpy(message, "ta_tr_win() requires three arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_tr_win() requires a real");
		return 1;
	}

	if (args->arg_type[1] == DECIMAL_RESULT)
		args->arg_type[1] = REAL_RESULT;
	else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message, "ta_tr_win() requires a real");
		return 1;
	}

	if (args->arg_type[2] == DECIMAL_RESULT)
		args->arg_type[2] = REAL_RESULT;
	else if (args->arg_type[2] != REAL_RESULT) {
		strcpy(message, "ta_tr_win() requires a real");
		return 1;
	}

	if (!(data = (ta_tr_win_data *)malloc(sizeof(ta_tr_win_data)))) {
		strcpy(message, "ta_tr_win() couldn't allocate memory");
		return 1;
	}

	data->previous_close = 0.0;
	data->init = -1;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_tr_win %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_tr_win_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

DLLEXP double ta_tr_win(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_tr_win_data *data = (ta_tr_win_data *)initid->ptr;
	double *high = (double *)args->args[0];
	double *low = (double *)args->args[1];
	double *close = (double *)args->args[2];

	if (close == NULL) {
		*is_null = 1;
		return 0.0;
	}

    //fprintf(stderr, "tr\thigh=%f,low=%f,close=%f,previous_close=%f\n", *high, *low, *close, data->previous_close);
    //fflush(stderr);

    if (data->init > 0) {
        return (*high > data->previous_close ? *high : data->previous_close) - (*low < data->previous_close ? *low : data->previous_close);
    } else {
        *is_null = 1;
        return 0.0;
    }
}

void ta_tr_win_clear(UDF_INIT *initid, char *is_null, char *error) {
    ta_tr_win_data *data = (ta_tr_win_data *)initid->ptr;

    if (data->init > 0) {
        data->previous_close = data->current_close;
    } else {
        *is_null = 1;
        data->init++;
    }
    //fprintf(stderr, "clear\tinit=%i\n", data->init);
    //fflush(stderr);
}

void ta_tr_win_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
    ta_tr_win_data *data = (ta_tr_win_data *)initid->ptr;
    //double *high = (double *)args->args[0];
    //double *low = (double *)args->args[1];
	double *close = (double *)args->args[2];

    data->current_close = *close;
    if (data->init < 1) {
        *is_null = 1;
    }
    //fprintf(stderr, "add\thigh=%f,low=%f,close=%f,last_c=%f\n", *high, *low, *close, data->previous_close);
    //fflush(stderr);
}

void ta_tr_win_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  //double *value = (double*)args->args[0];

  //fprintf(stderr, "reset %f\n", *value);
  //fflush(stderr);

}

void ta_tr_win_remove(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {

  //fprintf(stderr, "remove\n");
  //fflush(stderr);

}
