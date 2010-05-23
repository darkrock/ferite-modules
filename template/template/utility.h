#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ferite.h"

struct node {
    char *name;
    char *start;
    int length;
    short name_length;
    short type;

    short iter;
    
    struct node *next;
    struct node *child;
};
struct tag_ctx {
    struct node *tree;
    FeriteVariable *mother;
    FeriteVariable *current;
    FeriteBuffer *buf;
    FeriteVariable *parts;
    char *path;
    char *filedata;
    int level;
    int debug;
};
#define TAG_NOMOREDATA		(1 << 0) 
#define TAG_SUBSTITUTE  	(1 << 1)
#define TAG_LOOP		(1 << 2)
#define TAG_CONDITION		(1 << 3)
#define TAG_START		(1 << 4)
#define TAG_END		        (1 << 5)
#define TAG_ERROR		(1 << 6)
#define TAG_ROOT		(1 << 7)
#define TAG_INCLUDE		(1 << 8)
#define TAG_INVERSED		(1 << 9)
#define TAG_UP_ONE_LEVEL	(1 << 10)

FeriteVariable *tag_expand( FeriteScript *script, struct node *node, FeriteVariable *o );
struct node *tag_parse( FeriteScript *script, struct tag_ctx *ctx, char * );
int tag_get_piece( struct tag_ctx *ctx, struct node *, char ** );
void tag_free(struct node *tree);
void tag_dump( FeriteBuffer *buf, struct node *tree, int level );
void tag_run( FeriteScript *script, struct tag_ctx *ctx, struct node *tree, 
	      FeriteVariable *current, FeriteVariable *previous, int level );
