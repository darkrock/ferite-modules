#ifndef ZLib_utility_H
#define ZLib_utility_H

int ferite_zlib_priv_compres(int level, char *inData, long inLength, char **outData, long *outLength);
int ferite_zlib_priv_deflate(int level, char *inData, long inLength, char **outData, long *outLength);
int ferite_zlib_priv_inflate(char *inData, long inLength, char **outData, long *outLength);
int ferite_zlib_priv_uncompr(char *inData, long inLength, char **outData, long *outLength);

#endif /* ZLib_utility_H */
