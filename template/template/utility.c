#include "utility.h"
#include "ferite.h"

void tag_run( FeriteScript * script, struct tag_ctx *ctx, struct node *tree, 
	      FeriteVariable *current, FeriteVariable *previous, int level )
{
    FeriteVariable *v;
    FeriteString *str;
    FeriteIterator *iter;
    FeriteVariable *o;
    char name[200];
    int i;
    FeriteVariable *used = NULL; 

    while( tree ) {
 	    used = ( tree->type & TAG_UP_ONE_LEVEL ? previous : current);

        if( tree->type == TAG_INCLUDE ) {
            tag_run(script, ctx, tree->child, current, previous, level );
        }
        if( tree->type & TAG_LOOP ) {

            o = ( tree->type & TAG_ROOT ) ? ctx->mother : used;
            v = tag_expand( script, tree, o );

            if( v && v->type == F_VAR_UARRAY ) {
                for( i = 0; i < VAUA( v )->size; i++ ) {
			if( i == 0 ) 
				tag_run( script, ctx, tree->child, VAUA( v )->array[i], VAUA( v )->array[i], level + 1 );
			else 
				tag_run( script, ctx, tree->child, VAUA( v )->array[i],VAUA( v )->array[i-1], level + 1 );
		}
            } else if ( ctx->debug )
                ferite_buffer_printf( script,ctx->buf, "[%.*s doesn't exist(loop)]", tree->name_length, tree->name );
        } 

	if( tree->type & TAG_CONDITION )
	{
	    int state = 0;
	    o = ( tree->type & TAG_ROOT ) ? ctx->mother : used;
	    v = tag_expand( script, tree, o );
	    if( v == NULL )
	    {
		if(tree->type & TAG_INVERSED) {
		    v = current;
		    state =1;	
		}

		if( ctx->debug )
		    ferite_buffer_printf( script,ctx->buf, "[%.*s doesn't exist]",tree->name_length,tree->name);
	    } else {
		int vstate = ferite_variable_is_false( script, v );
		if(vstate && (tree->type & TAG_INVERSED)) {
		    state = 1;
		    v = used;
		} else if (!vstate && !(tree->type & TAG_INVERSED)) {
		    state = 1;
		} else {
		    if( ctx->debug > 1 )
			ferite_buffer_printf( script,ctx->buf, "[%.*s was false]",tree->name_length,tree->name);
		}
	    }
			
	    if(state) 
		tag_run( script, ctx, tree->child, v, current, level+1 );
	}
	
        if( tree->type == TAG_START ) {
            ferite_buffer_add( script, ctx->buf, tree->start, tree->length );
        }

        if( tree->type & TAG_SUBSTITUTE ) {
            o = ( tree->type & TAG_ROOT ) ? ctx->mother : used;
            v = tag_expand( script, tree, o );
            if( v == NULL ) {
                if( ctx->debug )
                    ferite_buffer_printf( script,ctx->buf, "[%.*s was empty]", tree->name_length, tree->name );
            } else {
                if( v->type == F_VAR_UARRAY ) {
                    if( VAUA( v )->size == 0 ) {
                        if( ctx->debug )
                            ferite_buffer_printf( script,ctx->buf, "[%.*s: array size == 0]", tree->name_length, tree->name );
                    } else {
                        if( tree->iter == VAUA( v )->size )
                            tree->iter = 0;
                        str = ferite_variable_to_str( script, VAUA( v )->array[tree->iter], 0 );
                        tree->iter++;
                    }
                } else {
                    str = ferite_variable_to_str( script, v, 0 );
                }
                ferite_buffer_add( script, ctx->buf, str->data, str->length );
                ferite_str_destroy( script, str );
            }
        }
        tree = tree->next;
    }
}

FeriteVariable *tag_expand( FeriteScript * script, struct node *node, FeriteVariable *o )
{
    char name[200];
    int i, start = 0;
    FeriteVariable *v = NULL;
    FeriteFunction *fnc;
    FeriteVariable *object = o;
    FeriteVariable **args;
    if( node->name_length == 0 )
	return object;
    for ( i = 0; i <= node->name_length; i++ ) {
        if ( node->name[i] == '.' || node->name_length == i ) {
            strncpy( name, node->name + start, i - start );
            name[i - start] = '\0';
            switch( object->type )
	    {
		case F_VAR_OBJ:
		    v = ferite_object_get_var( script, VAO(object), name );
		    if( v == NULL )
		    {
			if( VAO(object) && (fnc = ferite_object_get_function( script, VAO(object), name )) )
		   	    v = ferite_call_function( script, VAO(object), NULL, fnc, NULL );
		    }
		    break;
		case F_VAR_UARRAY:
		    v = ferite_uarray_get_from_string( script, VAUA(object), name );
		    break;
		default:
		    return NULL;
	    }
	    if ( v ) {
		object = v;
	    }
	    start = i + 1;
        }
    }
    return v;
}

void tag_free( struct node *tree )
{
    struct node *node = tree, *tmp;
	
    while (node) {
		if (node->child)
			tag_free(node->child);
		tmp = node;
		node = node->next;
		ffree_ngc(tmp);
    }
}
void tag_dump( FeriteBuffer *buf, struct node *tree, int level )
{
    char *sep = "                                                    ";
    struct node *node = tree;
    while( node )
    {
	    ferite_buffer_printf( FE_NoScript, buf, "%.*s", level, sep );
	    if( node->type & TAG_NOMOREDATA )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_NOMOREDATA]" );
	    if( node->type & TAG_UP_ONE_LEVEL )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_UP_ONE_LEVEL]" );
	    if( node->type & TAG_SUBSTITUTE )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_SUBSTITUTE]" );
	    if( node->type & TAG_LOOP )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_LOOP]" );
	    if( node->type & TAG_CONDITION )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_CONDITION]" );
	    if( node->type & TAG_START )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_START]" );
	    if( node->type & TAG_END )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_END]" );
	    if( node->type & TAG_ROOT )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_ROOT]" );
	    if( node->type & TAG_INCLUDE )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_INCLUDE]" );
	    if( node->type & TAG_INVERSED )
		    ferite_buffer_add_str( FE_NoScript, buf, "[TAG_INVERSED]" );
	    
	    ferite_buffer_printf( FE_NoScript,buf, "%.*s\n", node->name_length, node->name );
	    if( node->child )
		    tag_dump( buf, node->child, level + 1 );
	    
	    node = node->next;
    }
}
struct node *tag_parse( FeriteScript * script, struct tag_ctx *ctx, char *str )
{
    struct node *node, *root, *current = NULL;
    int state = 0, sp = 0, ret;
    struct node *st[101];

    if ( str == NULL )
        return NULL;

    memset( st, 0, sizeof( struct node * ) * 100 );

    while ( 1 ) {
        node = fcalloc( 1, sizeof( struct node ) );
        ret = tag_get_piece( ctx, node, &str );
        if ( ret == TAG_ERROR ) {
	    ferite_set_error(script, 0, "Error while parsing: %.*s",20, node->name);
	    tag_free(root);
	    ffree(node);
            return NULL;
        }

        if ( ret == TAG_NOMOREDATA )
            break;

        if ( node->type == TAG_INCLUDE ) {
            int fd;
            struct stat in;
            char *ptr, *p, buf[200], newpath[200], *oldpath;
            FeriteVariable *v;
            struct node *tmp;

            if ( ctx->path && node->name[0] != '/' )
                snprintf( buf, 200, "%s/%.*s", ctx->path, node->name_length, node->name );
            else
                snprintf( buf, 200, "%.*s", node->name_length, node->name );

	    if( (p = strrchr( buf, '/' ) ) )
	    {
	        snprintf( newpath, 200, "%.*s", (int)(p - buf), buf );
	        oldpath = ctx->path;
	        ctx->path = newpath;
	    }
            if ( ( v = ferite_uarray_get_from_string( script, VAUA( ctx->parts ), buf ) ) ) {
                node->child = tag_parse( script, ctx, FE_STR2PTR( v ) );
            } else if ( ( fd = open( buf, O_RDONLY ) ) != -1 ) {
                fstat( fd, &in );
                ptr = fmalloc( in.st_size + 1 );
                read( fd, ptr, in.st_size );
                close( fd );
                v = fe_new_str( "part", (in.st_size) ? ptr: NULL, in.st_size, FE_CHARSET_DEFAULT );
                ffree( ptr );
		node->child = tag_parse( script, ctx, FE_STR2PTR( v ) );
                ferite_uarray_add( script, VAUA( ctx->parts ), v, buf, FE_ARRAY_ADD_AT_END );
	    } else {
                node->type = TAG_START;
                node->start = "[include failed]";
                node->length = 17;
            }
	    if( p )
	    {
	        ctx->path = oldpath;
	    }
        }
	if ( !sp && !st[sp] ) {
	     root = node;
	}

        if ( state & TAG_CONDITION || state & TAG_LOOP ) {
            if ( current )
                current->child = node;
            current = NULL;
        }

        if ( node->type & TAG_CONDITION || node->type & TAG_LOOP ) {
            if ( current )
                current->next = node;
            st[sp] = node;
            sp++;
        }

        if ( node->type & TAG_SUBSTITUTE || node->type == TAG_START  || node->type == TAG_INCLUDE) {
            if ( current )
                current->next = node;
            st[sp] = node;
        }

        if ( node->type == TAG_END ) {
            sp--;
	    if( sp < 0 )
		break;
            current = st[sp];
	} else {
	    current = node;
	}
        state = node->type;
    }

    if ( sp ) 
	ferite_set_error(script, 0, "Unbalanced tree");

    return root;
}

int tag_get_piece( struct tag_ctx *ctx, struct node *node, char **ostr )
{
    char *delim = "%%%";
    char *p, *str = *ostr;
    if ( str == NULL || *str == '\0' )
        return TAG_NOMOREDATA;

    if ( *str == '[' ) {
	node->name = ++str;
        while ( *str && !( *str == ':' || *str == ']' ) )
            str++;
        node->name_length = str - node->name;
        if ( *str == ':' ) {
            if ( *node->name == '@' ) {
                node->name++;
                node->name_length--;
                node->type = TAG_LOOP;
            } else if ( *node->name == '!' ) {
                node->name++;
                node->name_length--;
                node->type = TAG_CONDITION;
                node->type |= TAG_INVERSED;
	    } else {
                node->type = TAG_CONDITION;
	    }
        } else if ( *node->name == '#' ) {
            if ( strncmp( "include", node->name + 1, 7 ) == 0 ) {
                node->type = TAG_INCLUDE;
                node->name += 9;
                node->name_length -= 9;
            }
        } else if ( *str == ']' ) {
            node->type = TAG_SUBSTITUTE;
        } else
            return TAG_ERROR;
        if ( *node->name == '.' ) {
            node->name++;
            node->name_length--;
            node->type |= TAG_ROOT;
        }

	else if ( *node->name == '<' ) {
            node->name++;
            node->name_length--;
            node->type |= TAG_UP_ONE_LEVEL;
        }

        node->start = ++str;
    } else if ( *str == ']' ) {
	node->start = ++str;
        node->length = 0;
        node->type = TAG_END;
        node->name = "END";
        node->name_length = 3;
    } else if( str[0] == delim[0] && str[1] == delim[1] && str[2] == delim[2] ) {
	str += 3;
	node->start = str;
	if( ( str = strstr( str, delim ) ) == NULL )
	    return TAG_ERROR;
        node->type = TAG_START;
	node->name = "TXT";
	node->name_length = 3;
	node->length = str - node->start;
	str += 3;
    } else {
        node->start = str;
	while ( *str && !( *str == '['  || *str == ']'  ||
		 ( str[0] == delim[0] && str[1] == delim[0] && str[2] == delim[2] ) ) )
	{
	    str++;
        }
        node->type = TAG_START;
        node->name = "TXT";
        node->name_length = 3;
        node->length = str - node->start;
    }
    *ostr = str;
    return node->type;
}
