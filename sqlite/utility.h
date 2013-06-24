
#include <ferite.h>

typedef struct __ferite_sqlite {
	FeriteScript *script;
	FeriteVariable *list;
} FeriteSQLite;

int ferite_sqlite_exec_callback( void *data, int argc, char **argv, char **column_names );