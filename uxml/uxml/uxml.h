#include <ferite.h>
enum {  TAG_ANY = 1 ,
        TAG_START = 2,
        TAG_END = 4,
        TAG_VAR = 8,
        TAG_COND = 16,
        TAG_ROOT = 32,
	TAG_COMMENT = 64
};
#define UXML_ARG_KEY    1
#define UXML_ARG_VAL    2
#define UXML_ARG_DELIM  3
#define UXML_ARG_FIN    4

#define UXML_REFS	10
#define UXML_REAL	11

struct uxml_ctx {
	char *ptr;
	struct node *tree;
	FeriteBuffer *buf;
	int mother;
};
struct uxml_arg {
        char *name;
        char *start;
        short name_length;
        short length;
        struct uxml_arg *next;
};

struct node {
	unsigned int id;
	char *name;
	char *start;
	int length;
	short name_length;
	short type;
	char *argstring;
	short arglength;

	struct node *next;
	struct node *child;
	struct uxml_arg *args;
};


struct node *uxml_parse( FeriteScript *script, char *str );
int uxml_get(struct node *node, char *str, int opt);
int uxml_parse_arguments( FeriteScript *script, struct node *node );

struct node *uxml_path_search( FeriteString *path, struct node *tree );
FeriteVariable *uxml_path_to_string( FeriteScript *script, FeriteString *path, struct node *tree );
FeriteVariable *uxml_path_to_array( FeriteScript *script, FeriteString *path, struct node *tree );
FeriteVariable *uxml_path_to_long( FeriteScript *script, FeriteString *path, struct node *tree );
FeriteVariable *uxml_path_to_double( FeriteScript *script, FeriteString *path, struct node *tree );
FeriteVariable *uxml_make_child_object( FeriteScript *script, struct uxml_ctx *ctx, struct node *node );
int uxml_create_ferite_object_tree( FeriteScript *script, struct uxml_ctx *ctx, struct node *tree, FeriteVariable *mother, int mode );
void uxml_add_leaf( struct uxml_ctx *ctx, struct node *tree, FeriteString *name, FeriteString *val );

void uxml_unescape_url(char *);
char uxml_x2c(char *);
void uxml_plustospace(char *);

