#include <ferite.h>
#include "utility.h"


int resolve_category(int which)
{

       int category = LC_ALL;

       switch(which) {
       case 1: category = LC_ALL; break;
       case 2: category = LC_MESSAGES; break;
       case 3: category = LC_CTYPE; break;
       case 4: category = LC_COLLATE; break;
       case 5: category = LC_MONETARY; break;
       case 6: category = LC_NUMERIC; break;
       case 7: category = LC_TIME; break;
       default:
	 //ferite error?
	 category = 0; break;

       }	
       return category;
}

