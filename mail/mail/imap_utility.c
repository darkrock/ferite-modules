#include "imap_utility.h"

char *curhst = NIL;             /* currently connected host */
char *curusr = NIL;             /* current login user  */
char *global_pwd;


void set_error_string( FeriteScript *script,  FeriteObject *object, char* str ) {
	FeriteVariable *estr;
	estr = ferite_get_variable_from_hash(script, object->variables->variables, "_errstr");
 	if( estr == NULL )
 		return;	
	ferite_str_set( VAS(estr), str, 0,  FE_CHARSET_DEFAULT );
}


int create_address(FeriteScript *script, FeriteVariable *object, ADDRESS *addr) {
	FeriteVariable *v;
	if( addr->mailbox ) {
		v = fe_new_str( "mailbox", addr->mailbox, 0, FE_CHARSET_DEFAULT );
		if( v == NULL ) {
			//set_error_string( script, self,"Internal ferite error" );
			return -1;
		}
		ferite_object_set_var(script, VAO(object), "mailbox", v );
	}
	
	if( addr->host ) {
		v = fe_new_str("host", addr->host, 0, FE_CHARSET_DEFAULT );
		if( v == NULL ) {
			//set_error_string( script, self, "Internal error" );
			return -1;
		}
		ferite_object_set_var(script, VAO(object), "host", v );
	}
	
	if( addr->personal ) {
		v = fe_new_str("name", addr->personal, 0, FE_CHARSET_DEFAULT );
		if( v == NULL ) {
			//set_error_string( script, self, "Internal error" );
			return -1;
		}
		ferite_object_set_var(script, VAO(object), "name", v );
	}

	return 0;
}


FeriteVariable *create_address_list(FeriteScript *script, ADDRESS *root) {
        FeriteNamespaceBucket *nsb;
	FeriteVariable *address_list, *item_list, *item;
	ADDRESS *curr;
	int error;
	
	nsb = ferite_find_namespace( script, script->mainns, "Mail.AddressList", FENS_CLS );
	if( nsb == NULL )
		return NULL;
	address_list = ferite_build_object( script, (FeriteClass *)nsb->data );
	if( address_list == NULL )
		return NULL;
	
	//Get the array (member of address_list) and populate it with
	//the entries in the linked list current
	item_list = ferite_get_variable_from_hash( script, VAO(address_list)->variables->variables, "list" );
	
	nsb = ferite_find_namespace( script, script->mainns, "mail.address", FENS_CLS );
	if( nsb == NULL ) {
		return NULL;
	}

	curr=root;	
	while( curr ) {
		item =  ferite_build_object( script, (FeriteClass *)nsb->data );
		if( item == NULL )
			return NULL;
		error = create_address( script, item, curr);
		if(error) {
			return NULL;
		}
		ferite_uarray_add( script, VAUA(item_list), item, NULL , FE_ARRAY_ADD_AT_END);
		curr = curr->next;
	}
	return address_list;
}


FeriteVariable *create_ferite_header_object( FeriteScript *script, ENVELOPE *env )
{
    FeriteVariable *header, *v;
    FeriteNamespaceBucket *nsb;
    #define BUFSIZE 1025
    char buf[BUFSIZE];
    int i;

    ADDRESS *address_source[5] = { env->from, env->reply_to, env->cc, env->sender, env->to };
    char *address_target[5] = { "from", "reply_to" , "cc", "sender" , "to" };
    int n_address_source = 5;

    char *source[4] = { env->subject, env->date, env->message_id, env->in_reply_to };
    char *target[4] = { "subject", "date", "ID", "in_reply_to" };
    int  n_source = 4;

    nsb = ferite_find_namespace( script, script->mainns, "Mail.MailHeader", FENS_CLS );
    if( nsb == NULL)
	    return NULL;
    
    header = ferite_build_object( script, (FeriteClass *)nsb->data );
    if( header == NULL )
	    return NULL;


    //Parse each address structure in address_source[] and write the result to the 
    //corresonding object member in address_target[] 
    for(i=0; i < n_address_source; i++) {
	    v = create_address_list( script, address_source[i] );
	    if( v == NULL ) {
		    //set_error_string( script, self, "Couldn't create address list" );
		    return NULL;
	    }
	    ferite_object_set_var(script, VAO(header), address_target[i], v );
    }
    
    
    

    //Copy  each header-field in source[] to the
    //corresonding object member in address_target[] 
    for(i=0; i< n_source; i++) {
	    if( source[i] ) {
		    v = fe_new_str(target[i], source[i] , 0, FE_CHARSET_DEFAULT );
		    if( v == NULL ) {
			    //set_error_string( script, self, "Couldn't create mail header" );
			    return NULL;
		    }
		    ferite_object_set_var(script, VAO(header), target[i], v );
	    }
    }
    return header;
}

FeriteVariable *create_ferite_content_object( FeriteScript *script, MAILSTREAM *stream, BODY *body, int msgno, char *sec )
{
    FeriteVariable *object,*v;
    FeriteNamespaceBucket *nsb;
    char *object_name;
    
    object_name = ( body->type == TYPEMULTIPART ) ? "Mail.Multipart" : "Mail.Part" ;

    
    nsb = ferite_find_namespace( script, script->mainns, object_name , FENS_CLS );
    if( nsb == NULL )
	   return NULL;
    object = ferite_build_object( script, (FeriteClass *)nsb->data );
    if( object == NULL )
        return NULL;

    v  = fe_new_lng("type",body->type);
    ferite_object_set_var(script, VAO(object), "type", v);
    //printf("setting type %d\n", body->type);
     if( body->subtype ) {
	     v = fe_new_str( "subtype", body->subtype, 0, FE_CHARSET_DEFAULT );
	     ferite_object_set_var( script, VAO(object), "subtype", v ); 
     }

    if( body->type == TYPEMULTIPART ) {
	PART *part;
	int i = 0;
	char sec2[200];
	FeriteVariable *ret, *parts;

	

	parts = ferite_get_variable_from_hash(script,VAO(object)->variables->variables,"parts");
	part = body->nested.part;
	while( part ) {
	    i++;
	    if( sec ) {
		snprintf(sec2,200,"%s.%d",sec,i);
	    } else {
		snprintf(sec2,200,"%d",i);
	    }

	    ret = create_ferite_content_object( script, stream, &part->body, msgno, sec2 );
	    ferite_uarray_add( script, VAUA(parts), ret , NULL, FE_ARRAY_ADD_AT_END );
	    part = part->next;
	}
	v = fe_new_lng("nparts", i);
	ferite_object_set_var(script, VAO(object), "nparts", v );
    }
    else
    {
	long len,len2;
	char *buf,*buf2;
	FeriteVariable *v;
	PARAMETER *param;

	if( sec == NULL )
	    sec = "1";
	buf = mail_fetchbody( stream, msgno, sec, &len );


	switch(body->encoding){ 
	case ENCQUOTEDPRINTABLE: buf2 = rfc822_qprint(buf ,len,&len2);break;
	case ENCBASE64: buf2=rfc822_base64(buf,len,&len2);break;
	default: buf2=buf;len2=len;
	}

	
	//v = fe_new_lng("encoding", body->encoding );
	//ferite_object_set_var(script, VAO(object), "encoding", v );
	//body->contents.text.data = malloc( len + 1 );	
        //memcpy( body->contents.text.data, buf, len );
	//body->contents.text.data[len] = '\0';
	//body->contents.text.size = len;

	//v = fe_new_str("subtype", body->subtype, 0, FE_CHARSET_DEFAULT );
	//ferite_object_set_var(script, VAO(object), "subtype", v );

	v = fe_new_str("content", buf2, len2, FE_CHARSET_DEFAULT );
	ferite_object_set_var(script, VAO(object), "content", v );
	v = fe_new_lng("encoding", body->encoding );
	ferite_object_set_var(script, VAO(object), "encoding", v );

	



	if( body->disposition.type && strcasecmp(body->disposition.type, "attachment") == 0) {
		param = body->disposition.parameter;
		while(param){
			if( param->attribute && strcasecmp(param->attribute,"filename") == 0){
				v = fe_new_str("filename", param->value, 0, FE_CHARSET_DEFAULT );
				ferite_object_set_var(script, VAO(object), "filename", v );
				break;
			}
		param = param->next;
		}
	}
			

    }
    return object;
}

FeriteVariable *create_ferite_mail_object( FeriteScript *script, FeriteVariable *header, FeriteVariable *content )
{
    FeriteVariable *mailobject;
    FeriteNamespaceBucket *nsb;
    
    nsb = ferite_find_namespace( script, script->mainns, "Mail.Mailobj", FENS_CLS );
    if( nsb == NULL )
	    return NULL;
    mailobject = ferite_build_object( script, (FeriteClass *)nsb->data );
    if( mailobject == NULL )
	    return NULL;
    ferite_object_set_var(script, VAO(mailobject), "header", header );
    ferite_object_set_var(script, VAO(mailobject), "content", content );
    return mailobject;
}



BODY*  create_imap_content_leaf( FeriteScript *script, FeriteVariable *leaf ){

	
	BODY *body;
	FeriteVariable *v;

	
	body =  mail_newbody();

	                
	// fixme NULL/0 ?
	v = ferite_get_variable_from_hash(script,VAO(leaf)->variables->variables,"encoding");
	RETURN_IF_NULL(v);
	body->encoding=VAI(v);


	v = ferite_get_variable_from_hash( script, VAO(leaf)->variables->variables, "content");
	RETURN_IF_NULL(v);
	
	//fixme memcopy? Encode?
	body->contents.text.data =strdup(VAS(v)->data);
	body->contents.text.size =strlen(VAS(v)->data); //fixme 
        

	v = ferite_get_variable_from_hash( script, VAO(leaf)->variables->variables, "type" );
	RETURN_IF_NULL(v);
	body->type=VAI(v);
                
	v = ferite_get_variable_from_hash(script,VAO(leaf)->variables->variables,"subtype");
	RETURN_IF_NULL(v);
	body->subtype=cpystr( VAS(v)->data );
                
                
                
	v  = ferite_get_variable_from_hash(script,VAO(leaf)->variables->variables,"filename");
	RETURN_IF_NULL(v);
	if( strlen(VAS(v)->data)) {
		PARAMETER *param = mail_newbody_parameter();
		param->attribute = cpystr("filename");
		param->value=cpystr( VAS(v)->data );
		
		body->disposition.parameter = param;
		body->disposition.type = cpystr("attachment");
	}
	
	v = ferite_get_variable_from_hash( script, VAO(leaf)->variables->variables, "filepath" );
	RETURN_IF_NULL(v);
	if( strlen(VAS(v)->data) ) {
		off_t size;
		int fd;
		char *buf;
		struct stat statinfo;
		
		fd = open(VAS(v)->data,O_RDONLY);
		if( fd == -1 ) {
			//set_error_string( script, self, "Couldn't open file" );
			return NULL;
		}		
		size=lseek(fd,0,SEEK_END);
		lseek(fd,0,SEEK_SET);
		buf=malloc(size*sizeof(char)+1);
		if(buf){
			read(fd,buf,size);
			body->contents.text.data=buf;
			body->contents.text.size=size;
			close(fd);
		}
		else{
			ferite_error(script, 0, "Out of memory\n");
			close(fd);
			return NULL;
		}
	}
		   
	v = ferite_get_variable_from_hash(script,VAO(leaf)->variables->variables, "charset");
	RETURN_IF_NULL(v);
	if( strlen(VAS(v)->data) ) {
		PARAMETER *param = mail_newbody_parameter();
		if( body->parameter )
			param->next = body->parameter;
		body->parameter=param;
		
		body->parameter->attribute=cpystr("CHARSET");
		body->parameter->value=cpystr(VAS(v)->data);
	}

	return body;
}


BODY *create_imap_content_object(FeriteScript* script, FeriteVariable* fe_parent)
{
        FeriteVariable *parts_list,*fe_child,*v;
	BODY *root,*new_body;
	PART *new_part,*last_part =  NULL;
	int i=0;
        
	//Get array of child objects 
        parts_list = ferite_get_variable_from_hash( script, VAO(fe_parent)->variables->variables, "parts");
	//This is true if the top level is not a multipart
	if( parts_list == NULL )
		return( create_imap_content_leaf( script, fe_parent ));
        
	
	root = mail_newbody();
        root->type=TYPEMULTIPART;
	v = ferite_get_variable_from_hash( script, VAO(fe_parent)->variables->variables, "subtype");
	RETURN_IF_NULL(v);
	if( VAS(v)->data )
		root->subtype = cpystr( VAS(v)->data );

        //Loop trough child array
        i=0;
	while( i < VAUA(parts_list)->size) {
		new_part = mail_newbody_part();
		fe_child = ferite_uarray_get_index( script, VAUA(parts_list) , i );
		RETURN_IF_NULL(fe_child);

		// Is it a multipart or a leaf?
                if( ferite_get_variable_from_hash( script, VAO(fe_child)->variables->variables, "parts"))
                        new_body = create_imap_content_object( script , fe_child );
		else 
			new_body = create_imap_content_leaf( script, fe_child );
		RETURN_IF_NULL(new_body);		
		
		new_part->body = *new_body; //Fixme, this sucks 

		if( last_part == NULL )
			root->nested.part = last_part = new_part;
		else
			last_part = last_part->next = new_part;
		i++;
	}
	return root;
}

ADDRESS *create_imap_address( FeriteScript* script, FeriteVariable* fe_object){
	ADDRESS *root=NULL, *newaddr;
	FeriteVariable *fe_list,*v,*fe_addr;
	int i;
	
	RETURN_IF_NULL(fe_object && VAO(fe_object));
	
	fe_list = ferite_get_variable_from_hash( script, VAO(fe_object)->variables->variables, "list" );
	RETURN_IF_NULL(fe_list);
	
	for( i=0; i<  VAUA(fe_list)->size; i++ ){
		fe_addr  = ferite_uarray_get_index( script, VAUA(fe_list) , i );
		RETURN_IF_NULL(fe_addr);
		if(root == NULL)
			root = newaddr = mail_newaddr();
		else
			newaddr = newaddr->next =  mail_newaddr();
		
		v =  ferite_get_variable_from_hash( script, VAO(fe_addr)->variables->variables , "mailbox");
		RETURN_IF_NULL(v);
		if(VAS(v)->data)
			newaddr->mailbox = cpystr(VAS(v)->data);

		
		v =  ferite_get_variable_from_hash( script, VAO(fe_addr)->variables->variables , "host");
		RETURN_IF_NULL(v);
		if(VAS(v)->data)
			newaddr->host = cpystr(VAS(v)->data);

		
		v =  ferite_get_variable_from_hash( script, VAO(fe_addr)->variables->variables , "name");
		RETURN_IF_NULL(v);
		if(VAS(v)->data)
			newaddr->personal = cpystr(VAS(v)->data);
	}
	return root;
}

ENVELOPE *create_imap_envelope( FeriteScript *script, FeriteVariable *header ){

	ENVELOPE *env = mail_newenvelope();
	FeriteVariable *v;
	ADDRESS *a;
	int i;
	
	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "to" );
	RETURN_IF_NULL(v);
	a = create_imap_address( script, v );
	if(a)
		env->to = a;
		
	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "from" );
	RETURN_IF_NULL(v);
	a = create_imap_address( script, v );
	if(a)
		env->from = a;
	
	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "cc" );
	RETURN_IF_NULL(v);
	a = create_imap_address( script, v );
	if(a)
		env->cc = a;
	
	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "from" );
	RETURN_IF_NULL(v);
	a = create_imap_address( script, v );
	if(a)
		env->return_path = a;
	
	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "sender" );
	RETURN_IF_NULL(v);
	a = create_imap_address( script, v );
	if(a)
		env->sender = a;

	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "subject" );
	RETURN_IF_NULL(v);
	env->subject = cpystr( VAS(v)->data );
	RETURN_IF_NULL( env->subject );

	v = ferite_get_variable_from_hash( script, VAO(header)->variables->variables, "in_reply_to" );
	RETURN_IF_NULL(v);
	env->in_reply_to = cpystr( VAS(v)->data );
	RETURN_IF_NULL( env->in_reply_to );

	//Ignore id and date since c-client generate them

	return env;
}



ENVELOPE *create_env_from_object( FeriteScript *script, FeriteVariable *header )
{
    return NULL;
}
BODY *create_body_from_object( FeriteScript *script, FeriteVariable *content )
{
    return NULL;
}


/* Interfaces to C-client */

void mm_searched (MAILSTREAM *stream,unsigned long number)
{
}


void mm_exists (MAILSTREAM *stream,unsigned long number)
{
}


void mm_expunged (MAILSTREAM *stream,unsigned long number)
{
}


void mm_flags (MAILSTREAM *stream,unsigned long number)
{
}


void mm_notify (MAILSTREAM *stream,char *string,long errflg)
{
	mm_log (string,errflg);
}


void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
  putchar (' ');
  if (delimiter) putchar (delimiter);
  else fputs ("NIL",stdout);
  putchar (' ');
  fputs (mailbox,stdout);
  if (attributes & LATT_NOINFERIORS) fputs (", no inferiors",stdout);
  if (attributes & LATT_NOSELECT) fputs (", no select",stdout);
  if (attributes & LATT_MARKED) fputs (", marked",stdout);
  if (attributes & LATT_UNMARKED) fputs (", unmarked",stdout);
  putchar ('\n');
}


void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
{
  putchar (' ');
  if (delimiter) putchar (delimiter);
  else fputs ("NIL",stdout);
  putchar (' ');
  fputs (mailbox,stdout);
  if (attributes & LATT_NOINFERIORS) fputs (", no inferiors",stdout);
  if (attributes & LATT_NOSELECT) fputs (", no select",stdout);
  if (attributes & LATT_MARKED) fputs (", marked",stdout);
  if (attributes & LATT_UNMARKED) fputs (", unmarked",stdout);
  putchar ('\n');
}


void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
  printf (" Mailbox %s",mailbox);
  if (status->flags & SA_MESSAGES) printf (", %lu messages",status->messages);
  if (status->flags & SA_RECENT) printf (", %lu recent",status->recent);
  if (status->flags & SA_UNSEEN) printf (", %lu unseen",status->unseen);
  if (status->flags & SA_UIDVALIDITY) printf (", %lu UID validity",
					      status->uidvalidity);
  if (status->flags & SA_UIDNEXT) printf (", %lu next UID",status->uidnext);
  printf ("\n");
}


void mm_log (char *string,long errflg)
{
	switch ((short) errflg) {
	case NIL:
		printf ("[%s]\n",string);
		break;
	case PARSE:
	case WARN:
		printf ("%%%s\n",string);
		break;
	case ERROR:
		printf ("?%s\n",string);
		break;
	}
}


void mm_dlog (char *string)
{
	puts (string);
}


void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
	if (curhst) 
		fs_give ((void **) &curhst);
	
	curhst = (char *) 
		fs_get (1+strlen (mb->host));
	strcpy (curhst,mb->host);
	if (*mb->user) 
		strcpy (user,mb->user);
	
	if (curusr) 
		fs_give ((void **) &curusr);
	

	curusr = cpystr (user);
	strcpy(pwd,global_pwd);
}



void mm_critical (MAILSTREAM *stream)
{
}


void mm_nocritical (MAILSTREAM *stream)
{
}



long mm_diskerror (MAILSTREAM *stream,long errcode,long serious) 
{ 
	kill (getpid (),SIGSTOP);
	return NIL;
}


void mm_fatal (char *string)
{
  printf ("?%s\n",string);
}


int _send_mail(BODY *body,ENVELOPE *env,char* server)
{
	int debug=0;
        char *hostlist[]={server,NIL};
	SENDSTREAM *stream = NIL;
	
	if (env->to) {
		puts ("Sending...");
		if (stream = smtp_open (hostlist,debug))
			if (!smtp_mail (stream,"MAIL",env,body))
				return 0;
	}
	
	else {
		puts ("Posting...");
		/* if (stream = nntp_open (newslist,debug)) { */
		/*       if (nntp_mail (stream,mail->envelope,mail->body)) puts ("[Ok]"); */
		/*       else printf ("[Failed - %s]\n",stream->reply); */
		/*     } */
	}
	
	if (!stream)
		return 0;
	
	smtp_close (stream);
	//mail_free_envelope (&env);
	//mail_free_body (&body);
}
 
