#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <ferite.h>
#include <math.h>


unsigned char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char pr2six[256] = {
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int base64decode_len(const char *bufcoded) {
    int nbytesdecoded;
    register const char *bufin;
    register int nprbytes;
    bufin = bufcoded;
    while (pr2six[*bufin++] <= 63);
    nprbytes = (bufin - (const char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
    return nbytesdecoded + 1;
}

int base64decode(char *bufplain, const char *bufcoded) {
    int nbytesdecoded;
    register const char* bufin;
    register char *bufout;
    register int nprbytes;

    bufin = bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (char *) bufplain;
    bufin = (const char *) bufcoded;

    while (nprbytes > 4) {
        *bufout++ = (pr2six[(unsigned char) *bufin] << 2 | pr2six[(unsigned char)bufin[1]] >> 4);
        *bufout++ = (pr2six[(unsigned char)bufin[1]] << 4 | pr2six[(unsigned char)bufin[2]] >> 2);
        *bufout++ = (pr2six[(unsigned char)bufin[2]] << 6 | pr2six[(unsigned char)bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1)
        *bufout++ = (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    if (nprbytes > 2)
        *bufout++ = (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    if (nprbytes > 3)
        *bufout++ = (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);

    *bufout++ = '\0';
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}


char* base64encode(unsigned char *ip, int insize)
{
    int  bits = 0;
    int in = 0, out = 0, rem = 0,i,index;
    unsigned char *op,*start,*test;
    if( (start = op = fmalloc_ngc(4*(insize/3) + 4*((insize%3) > 0) + 1 )) == NULL )
	    return NULL;
    while ( in++ < insize ) {
	    rem++;
	    bits += *ip++ << 8*(3-rem);
	    if ( rem == 3 ) {
		    for(;rem>=0;rem--) {
		      index = (bits >> 6*rem) & 0x3f;
		      *op++ =  alphabet[index];
		      
		    }
		    bits = rem = 0;
	    }
    }
    if ( rem )
	    for(i= 3 ; i>=0 ;i--)
		    if( (i > 1 ) || ( i==1 && rem ==2))
			    *op++ = alphabet[(bits >> 6*i) & 0x3f];
		    else
			    *op++ = '=';
    
    *op = '\0';
   
    return start;
}

