#include <stdlib.h>
#include <stdio.h>
#include "uxml.h"
#include <ferite.h>

uxml_dump( struct node *tree, int level )
{
   char *l = "-----------------------------------------------------";
   while( tree )
   {
	  printf("%.*s %.*s %.*s\n",level+1,l,tree->name_length,tree->name,tree->length,tree->start);
	  if(tree->child)
	    uxml_dump(tree->child,level+1);
	  tree = tree->next;
   }
}
uxml_free( struct node *tree )
{
   struct node *tmp;
   struct uxml_arg *arg;
   while( tree )
   {
	  if(tree->child)
		uxml_free( tree->child );
	  tmp = tree;
	  tree = tree->next;
	  while( tmp->args )
	  {
		 arg = tmp->args;
		 tmp->args = tmp->args->next;
		 ffree( arg );
	  }
	  ffree( tmp );
   }
}
FeriteVariable *uxml_make_child_object( FeriteScript *script, struct uxml_ctx *ctx, struct node *node )
{
   struct uxml_ctx *ox;
   FeriteClass *cls;
   FeriteVariable *obj;

   if( node == NULL )
	 FE_RETURN_NULL_OBJECT;
   cls = ferite_find_class( script, script->mainns, "uXML" );
   obj = ferite_build_object( script, cls );
   ox = VAO(obj)->odata = fcalloc( 1, sizeof(struct uxml_ctx) );
   ox->tree = node;
   ox->buf = ctx->buf;
   return obj;
}
void uxml_add_leaf( struct uxml_ctx *ctx, struct node *tree, FeriteString *name, FeriteString *val )
{
   unsigned int i, hash = 0;
   struct node *node = fcalloc( 1, sizeof( struct node ) );
   node->name = ferite_buffer_alloc( ctx->buf, name->length );
   memcpy( node->name, name->data, name->length );
   node->start = ferite_buffer_alloc( ctx->buf, val->length );
   memcpy( node->start, val->data, val->length );

   for( i = 0, hash = 0; i < name->length; i++ )
	 hash = name->data[i] + 31 * hash;
   node->id = hash;
   node->name_length = name->length;
   node->length = val->length;
   node->next = tree->child;
   tree->child = node;
}
FeriteVariable *uxml_path_to_long( FeriteScript *script, FeriteString *path, struct node *tree )
{
   struct node *node;
   if( strcmp(".",path->data) == 0 )
	 node = tree;
   else if( (node = uxml_path_search( path, tree )) == NULL )
	 FE_RETURN_LONG( 0 );
   FE_RETURN_LONG( strtol( node->start, NULL, 10 ) );
}
FeriteVariable *uxml_path_to_double( FeriteScript *script, FeriteString *path, struct node *tree )
{
   struct node *node;
   if( strcmp(".",path->data) == 0 )
	 node = tree;
   else if( (node = uxml_path_search( path, tree )) == NULL )
	 FE_RETURN_LONG( 0 );
   FE_RETURN_DOUBLE( strtod( node->start, NULL ) );
}
FeriteVariable *uxml_path_to_string( FeriteScript *script, FeriteString *path, struct node *tree )
{
   struct node *node;
   char *p;
   if( strcmp(".",path->data) == 0 )
	 node = tree;
   else if( (node = uxml_path_search( path, tree )) == NULL )
	 return fe_new_str( "error", "", 0, FE_CHARSET_DEFAULT );
   p = ( node->length == 0 ) ? NULL: node->start;
   return fe_new_str( "uxml_string", p, node->length, FE_CHARSET_DEFAULT );
}
FeriteVariable *uxml_path_to_array( FeriteScript *script, FeriteString *path, struct node *tree )
{
   FeriteVariable *a,*v;
   struct node *node, *first;
   char *p;
   if( strcmp(".",path->data) == 0 )
	 first = node = tree;
   else
	 first = node = uxml_path_search( path, tree );
   a = ferite_create_uarray_variable( script, "uxml_array", FE_ARRAY_DEFAULT_SIZE, FE_STATIC );
   while( node )
   {
	  if( node->name_length == first->name_length &&
		  memcmp( first->name, node->name, first->name_length ) == 0 )
	  {
		 p = ( node->length == 0 ) ? NULL: node->start;
		 v = fe_new_str( "uxml_string", p, node->length, FE_CHARSET_DEFAULT );
		 ferite_uarray_add( script, VAUA(a), v, NULL, FE_ARRAY_ADD_AT_END);
	  }
	  node = node->next;
   }
   return a;
}
struct node *uxml_path_search( FeriteString *path, struct node *tree )
{
   struct node *found = NULL, *node = tree;
   char name[200];
   int i, start = 0, length;
   unsigned int hash = 0;

   for( i = 0; i <= path->length ; i++ )
   {
	  if( path->data[i] == '.' ||  path->length == i )
	  {
		 length = i++ - start;
		 found = NULL;
		 while( node )
		 {
			if( node->id == hash &&
				length == node->name_length &&
				memcmp( path->data + start, node->name, node->name_length ) == 0 )
			{
			   found = node;
			   node = node->child;
			   break;
			}
			node = node->next;
		 }
		 hash = path->data[i] + 31 * 0;
		 start = i;
	  }
	  else
	  {
		 hash = path->data[i] + 31 * hash;
	  }
   }
   return found;
}
int uxml_create_ferite_object_tree( FeriteScript *script, struct uxml_ctx *ctx, struct node *tree, FeriteVariable *mother, int mode )
{
   FeriteVariable *v,*a,*new_var;
   FeriteClass *cls;
   char *p, name[200];
   while( tree )
   {
	  snprintf( name, 200, "%.*s", tree->name_length, tree->name );
	  if( tree->child )
	  {
		 cls = ferite_find_class( script, script->mainns, "uXML_object" );
		 new_var = ferite_new_object( script, cls, NULL );
		 UNMARK_VARIABLE_AS_DISPOSABLE(new_var);
		 ffree( new_var->name );
		 new_var->name = fstrdup( name );
		 uxml_create_ferite_object_tree( script, ctx, tree->child, new_var, mode );
	  }
	  else if( mode == UXML_REAL )
	  {
		 p = ( tree->length == 0) ? NULL: tree->start;
		 new_var = fe_new_str( name, p, tree->length, FE_CHARSET_DEFAULT );
	  }
	  else
	  {
		 /* UXML_REFS */
		 new_var = uxml_make_child_object( script, ctx, tree );
		 ffree( new_var->name );
		 new_var->name = fcalloc( 1,  tree->name_length + 1 );
		 strncpy(new_var->name, tree->name, tree->name_length );
	  }
	  if( (v = ferite_object_get_var( script, VAO(mother), name )) )
	  {
		 if( mode == UXML_REAL && v->type != F_VAR_UARRAY )
		 {
			a = ferite_create_uarray_variable( script, name, FE_ARRAY_DEFAULT_SIZE, FE_STATIC );
			ferite_uarray_add( script, VAUA(a), ferite_duplicate_variable(script, v, NULL ), NULL, FE_ARRAY_ADD_AT_END);
			ferite_uarray_add( script, VAUA(a), new_var, NULL, FE_ARRAY_ADD_AT_END);
			ferite_object_set_var( script, VAO(mother), name, a );
		 }
		 else if( mode == UXML_REAL )
		 {
			ferite_uarray_add( script, VAUA(v), new_var, NULL, FE_ARRAY_ADD_AT_END);
		 }
		 else
		 {
			ferite_object_set_var( script, VAO(mother), name, new_var );
		 }
	  }
	  else
		ferite_object_set_var( script, VAO(mother), name, new_var );

	  tree = tree->next;
   }
   return 0;
}

struct node *uxml_parse( FeriteScript *script, char *str )
{
   struct node *node, *tree = NULL, *st[100];
   int i, sp = 0, state = -1;
   unsigned int hash;

   memset(st,0,sizeof(struct node *) * 100);
   while( 1 )
   {
	  node = fcalloc( 1, sizeof(struct node) );
	  if( uxml_get(node, str, TAG_ANY) == -1 )
		break;
	  str = node->name + node->name_length;
	  for( i = 0, hash = 0; i < node->name_length ; i++ )
	  {
		 hash = node->name[i] + 31 * hash;
	  }
	  node->id = hash;
	  if( !sp && !st[sp] )
	  {
		 tree = node;
	  }
	  if( node->type & TAG_COMMENT )
	  {
		ffree( node );
		continue;
	  }
	  if( node->type & TAG_START )
	  {
		if ( sp && state == TAG_START )
		{
		   st[sp - 1]->child = node;
		}
		if ( state == TAG_END )
		{
		   st[sp]->next = node;
		}
		st[sp++] = node;
 	  }
	  if( node->argstring )
	  {
	          if( !uxml_parse_arguments( script, node ) )
	          {
	                  ferite_error( script, 9999, "Failed to parse arguments for tag %.*s",
		                          node->name_length, node->name );
	                  break;
	          }
	          /* uxml_dump_arguments( node ); */
	  }

	  if ( node->type & TAG_END )
	  {
		 sp--;
		 if ( sp < 0 )
		 {
			ferite_set_error( script, 0, "Premature closing, named: %.*s", node->name_length,node->name );
			return 0;
		 }
		 if ( !(st[sp]->name_length == node->name_length) || memcmp(st[sp]->name,node->name,node->name_length) != 0 )
		 {
			ferite_set_error( script, 0,"Unbalansed uxml, or wrong nesting named: %.*s",node->name_length,node->name );
			return 0;
		 }
		 st[sp]->length = node->name - 2 - st[sp]->start;
		 if ( node->type == TAG_END )
		 {
			state = node->type;
			ffree( node );
			continue;
		 }
		 else
		 {
			node->type = TAG_END;
		 }
	  }
	  state = node->type;
   }
   if( sp )
   {
          while( st[sp] == NULL && sp )
                 sp--;
	  ferite_set_error( script, 0, "Unbalanced tree, tree is ending at: %.*s, when it should end at: %.*s",
						st[sp]->name_length, st[sp]->name,
						st[0]->name_length, st[0]->name );
   }
   return tree;
}
int uxml_dump_arguments( struct node *node )
{
   struct uxml_arg *arg = node->args;
   while( arg )
   {
	  printf("%.*s: %.*s\n", arg->name_length, arg->name, arg->length, arg->start );
	  arg = arg->next;
   }
}
int uxml_parse_arguments( FeriteScript *script, struct node *node )
{
   struct uxml_arg *arg = NULL, *current = NULL;
   int c, i, eatwsp = 1, state = UXML_ARG_FIN;
/*
    printf("Parse arguments %.*s %.*s\n",
	 node->name_length,node->name, node->arglength, node->argstring);
*/
   for( i = 0; i <= node->arglength ; i++ )
   {
	  if( eatwsp )
	  {
		 while( isspace(node->argstring[i]) )
		   i++;
		 eatwsp = 0;
	  }
	  if( state == UXML_ARG_FIN )
	  {
		 if( !isalnum( node->argstring[i] ) )
		   break;
		 state = UXML_ARG_KEY;
		 arg = fcalloc( 1, sizeof( struct uxml_arg ) );
		 arg->name = node->argstring + i;
	  }
	  if( state == UXML_ARG_KEY )
	  {
		 if( isspace( node->argstring[i] ) )
		 {
			arg->name_length = (node->argstring + i) - arg->name;
			while( isspace( node->argstring[i] ) )
			  i++;
			if( node->argstring[i] != '=' )
			  break;
		 }
		 if( node->argstring[i] == '=' )
		 {
			if( !arg->name_length )
			  arg->name_length = (node->argstring + i++) - arg->name;
			state = UXML_ARG_VAL;
			while( isspace( node->argstring[i] ) )
			  i++;
			if( node->argstring[i] != '"' )
			  break;
			arg->start = node->argstring + i + 1;
			continue;
		 }
		 c = node->argstring[i];
		 if( c == '>' || c == '<' || !c || c=='"' || isspace(c) || iscntrl(c) )
		   break;
	  }
	  if( state == UXML_ARG_VAL )
	  {
		 if( node->argstring[i] == '"' )
		 {
			arg->length = (node->argstring + i) - arg->start;
			state = UXML_ARG_FIN;
			if( current == NULL )
			{
			   current = node->args = arg;
			}
			else
			{
			   current->next = arg;
			   current = arg;
			}
			arg = NULL;
			eatwsp = 1;
			continue;
		 }
	  }
   }
    /*uxml_dump_arguments( node );*/
   if( state != UXML_ARG_FIN )
   {
	  if( arg )
	    ffree( arg );
	  while( current )
	  {
		 arg = current;
		 current = current->next;
		 ffree( arg );
	  }
	  node->args = NULL;
	  return 0;
   }
   return 1;
}
int uxml_get(struct node *node, char *str, int opt)
{
   int type;
   while(*str)
   {
	  if(*str == '<')
	  {
		 str++;
		 if(*str == '!' || *str == '?')
		 {
			type = TAG_COMMENT;
			str++;
		 }
		 else if(*str == '/' && (opt == TAG_ANY || opt == TAG_END))
		 {
			type = TAG_END;
			str++;
		 }
		 else if(!(*str == '/') && (opt == TAG_ANY || opt == TAG_START))
		 {
			type = TAG_START;
		 }
		 else
		 {
			continue;
		 }
		 node->name = str;
		 while(*str && !(*str == '>' || *str == ' '))
		   str++;
		 node->name_length = str - node->name;
		 if(*str != '>')
		 {
			node->argstring = str;
			while(*str && *str != '>')
			  str++;
			node->arglength = str - node->argstring;
		 }
		 if(str[-1] == '/')
		 {
			node->name_length --;
			type |= TAG_END;
		 }
		 node->start = ++str;
		 node->type = type;
		 return type;
	  }
	  str++;
   }
   return -1;
}

/*
 * Functions below stolen from older apache I belive
 * only added uxml_ prefix so we don't get any name clashed
 */

void uxml_unescape_url(char *url)
{
   register int x,y;
   for (x=0, y=0; url[y]; ++x, ++y)
   {
	  if ((url[x] = url[y]) == '%' & url[x+1] != 0)
	  {
		 url[x] = uxml_x2c(&url[y+1]);
		 y+=2;
	  }
   }
   url[x] = '\0';
}
char uxml_x2c(char *what)
{
   register char digit;
   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
   return (digit);
}
void uxml_plustospace(char *str)
{
   register int x;
   for (x=0; str[x]; x++)
	 if (str[x] == '+')
	   str[x] = ' ';
}

