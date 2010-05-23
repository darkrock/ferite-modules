#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <imap/c-client.h>
#include "ferite.h"


#define RETURN_IF_NULL(v); if( v == NULL ) return(NULL);

BODY *create_imap_content_object(FeriteScript* script, FeriteVariable* fe_parent);
BODY *create_imap_content_leaf(FeriteScript* script, FeriteVariable* fe_parent);

void set_error_string( FeriteScript *script, FeriteObject *object, char* str );
FeriteVariable *create_address_list(FeriteScript *script, ADDRESS *root);
int create_adress(FeriteScript *script, FeriteVariable *object, ADDRESS *addr);
FeriteVariable *create_ferite_header_object( FeriteScript *script, ENVELOPE *env );
FeriteVariable *create_ferite_content_object( FeriteScript *script, MAILSTREAM *stream, BODY *body, int n, char *sec );
FeriteVariable *create_ferite_mail_object( FeriteScript *script, FeriteVariable *header, FeriteVariable *content );
ENVELOPE *create_env_from_object( FeriteScript *script, FeriteVariable *header );
BODY *create_body_from_object( FeriteScript *script, FeriteVariable *content );
ENVELOPE *create_imap_envelope( FeriteScript *script, FeriteVariable *header );



void mm_log (char *string,long errflg);
void mm (MAILSTREAM *stream,long debug);
void overview_header (MAILSTREAM *stream,unsigned long uid,OVERVIEW *ov);
void header (MAILSTREAM *stream,long msgno);
void display_body (BODY *body,char *pfx,long i);
void status (MAILSTREAM *stream);
void prompt (char *msg,char *txt);
void smtptest (long debug);

