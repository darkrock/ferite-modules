
typedef struct _embfer_closure_callback {
    FeriteObject *object;
    FeriteScript *script;
    char         *tag;
} EmbferClosureCallback;

#define EMB_MAX_CLOSURES 32 /* If you change this, change the one in utils.c as well */

extern int emb_closure_count;
extern EmbferClosureCallback *emb_closurehold[EMB_MAX_CLOSURES];

void __embferclass_tag_handler( FeriteBuffer *output, const char *type, const char *contents );
