#include <ferite.h>
#include <aspell.h>

#include "utility.h"


FeriteVariable *create_array_from_word_list(FeriteScript *script, const AspellWordList *words ) {
	const char *word;
	int count = 1;
	char name[MAX_WORD_LENGTH];
	FeriteVariable *array, *v;
	AspellStringEnumeration *elements = aspell_word_list_elements( words );
		
	array = ferite_create_uarray_variable( script, "aspell_word_list", 
					       FE_ARRAY_DEFAULT_SIZE, FE_STATIC );
	while ( ( word = aspell_string_enumeration_next( elements )) != NULL  ) {
		
		snprintf(name, MAX_WORD_LENGTH, "word_%d", count++);
		v = ferite_create_string_variable_from_ptr(script, name, (char *)word, 0, 
							   FE_CHARSET_DEFAULT, FE_ALLOC);
		ferite_uarray_add( script, VAUA( array ), v, name, FE_ARRAY_ADD_AT_END );
	}
	return array;      
}
