
#include <sqlite3.h>
#include "utility.h"

int ferite_sqlite_exec_callback( void *data, int argc, char **argv, char **column_names ) {
	FeriteSQLite *callback_data = data;
	FeriteVariable *list = ferite_create_uarray_variable(callback_data->script, "row", argc, FE_STATIC);
	int i;

	for( i = 0; i < argc; i++ ) {
		FeriteVariable *value = ferite_create_string_variable_from_ptr(callback_data->script, "value", argv[i], 0, FE_CHARSET_DEFAULT, FE_STATIC);
		ferite_uarray_add(callback_data->script, VAUA(list), value, column_names[i], FE_ARRAY_ADD_AT_END);
	}
	
	ferite_uarray_add(callback_data->script, VAUA(callback_data->list), list, NULL, FE_ARRAY_ADD_AT_END);
	
	return 0;
}
