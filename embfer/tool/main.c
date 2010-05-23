#include "../libembfer/embfer.h"

int main( int argc, char **argv )
{
    if( argc > 1 ) {
        FeriteString *embfer_con = NULL;
        ferite_init( argc, argv );
        
        embfer_init();
        embfer_set_output_fnc( "Console.println" );
        embfer_con = embfer_convert_from_file( argv[1] );
        if( embfer_con != NULL )
            printf( "%s\n", embfer_con->data );
        else
            printf( "Unable to load file %s\n", argv[1] );
        embfer_deinit();
        
        ferite_deinit();
    }
    return 0;    
}
