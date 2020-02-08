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
   CREATE FUNCTION ta_tr RETURNS REAL SONAME 'lib_mysqludf_ta.so';
   DROP FUNCTION IF EXISTS ta_tr;
 */

typedef struct ta_tr_data_ {
	double previous_close;
	int init;
} ta_tr_data;

DLLEXP my_bool ta_tr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	ta_tr_data* data;

	initid->maybe_null = 1;

	if (args->arg_count != 3) {
		strcpy(message, "ta_tr() requires three arguments");
		return 1;
	}

	if (args->arg_type[0] == DECIMAL_RESULT)
		args->arg_type[0] = REAL_RESULT;
	else if (args->arg_type[0] != REAL_RESULT) {
		strcpy(message, "ta_tr() requires a real");
		return 1;
	}

	if (args->arg_type[1] == DECIMAL_RESULT)
		args->arg_type[1] = REAL_RESULT;
	else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message, "ta_tr() requires a real");
		return 1;
	}

	if (args->arg_type[2] == DECIMAL_RESULT)
		args->arg_type[2] = REAL_RESULT;
	else if (args->arg_type[2] != REAL_RESULT) {
		strcpy(message, "ta_tr() requires a real");
		return 1;
	}

	if (!(data = (ta_tr_data *)malloc(sizeof(ta_tr_data)))) {
		strcpy(message, "ta_tr() couldn't allocate memory");
		return 1;
	}

	data->previous_close = 0.0;
	data->init = 0;

	initid->ptr = (char*)data;
/*
    fprintf(stderr, "Init ta_tr %i done\n", (*(int *) args->args[1]));
    fflush(stderr);
 */
	return 0;
}

DLLEXP void ta_tr_deinit(UDF_INIT *initid)
{
	free(initid->ptr);
}

DLLEXP double ta_tr(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	ta_tr_data *data = (ta_tr_data *)initid->ptr;
	double *high = (double *)args->args[0];
	double *low = (double *)args->args[1];
	double *close = (double *)args->args[2];

	if (close == NULL) {
		*is_null = 1;
		return 0.0;
	}

	data->previous_close = *close;

	if (data->init) {
        return (*high > data->previous_close ? *high : data->previous_close) - (*low < data->previous_close ? *low : data->previous_close);
	} else {
		data->init = 1;
		*is_null = 1;
		return 0.0;
	}
}
