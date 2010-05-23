#include "zlib_header.h"
#include "utility.h"

#include <zlib.h>
#include <stdlib.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

int
ferite_zlib_priv_compres(int level,
			 char *inData, long inLength,
			 char **outData, long *outLength)
{
  int retVal;

  *outLength = inLength + inLength / 1024 + 16;
  *outData   = malloc(*outLength);
  if (*outData == 0) {
    /* out of memory */
    retVal = Z_MEM_ERROR;
  } else {
    retVal = compress2((unsigned char *)(*outData), (unsigned long *)outLength,
		       (unsigned char *)inData, (unsigned long)inLength,
		       (level >= 0 && level <= 9)
		       ? (unsigned long)level : Z_DEFAULT_COMPRESSION);
  }

  /* if we have an error, free resources */
  if (retVal != Z_OK) {
    free(*outData);
  }


  return retVal;
}

int
ferite_zlib_priv_deflate(int level,
			 char *inData, long inLength,
			 char **outData, long *outLength)
{
  int      retVal;
  z_stream z;

  /* allocate deflate state */
  z.data_type = Z_ASCII;
  z.zalloc    = Z_NULL;
  z.zfree     = Z_NULL;
  z.opaque    = Z_NULL;

  /* point to the input string */
  z.next_in  = (unsigned char *)inData;
  z.avail_in = inLength;

  /* get space for the output string */
  z.avail_out = z.avail_in + (z.avail_in / 1024) + 16; /* guarentee memory */
  *outData = malloc(z.avail_out);
  if (*outData == 0) {
    /* out of memory */
    return Z_MEM_ERROR;
  }
  z.next_out = (unsigned char *)(*outData);

  /* php/ext/zlib/zlib.c says that the following will compress without
   * the zlib internal headers and that is good enough for me
   */
  retVal = deflateInit2(&z,
			(level >= 0 && level <= 9) ? level : Z_DEFAULT_COMPRESSION,
			Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, 0);
  if (retVal == Z_OK) {
    /* deflate succeeded, so clean up */
    retVal = deflate(&z, Z_FINISH);

    if (retVal == Z_STREAM_END) {
      retVal = deflateEnd(&z);
    } else {
      deflateEnd(&z);
      if (retVal == Z_OK) {
	retVal = Z_BUF_ERROR;
      }
    }
  }
  *outLength = z.total_out;

  /* if we have an error, free resources */
  if (retVal != Z_OK) {
    free(*outData);
  }

  return retVal;
}

int
ferite_zlib_priv_inflate(char *inData, long inLength, char **outData, long *outLength)
{
  int  multiple;
  int  retVal;
  z_stream z;

  *outData = 0;

  /* from PHP, use multiple to expand our buffer until it is large enough*/
  for (multiple = 2; multiple < 16; ++multiple) {
    *outLength = inLength * (1 << multiple);

    /* get a better, bigger, buffer */
    char *newBuffer = realloc(*outData, *outLength);
    if (newBuffer == 0) {
      /* out of memory */
      retVal = Z_MEM_ERROR;
      break;
    }
    *outData = newBuffer;

    z.zalloc    = Z_NULL;
    z.zfree     = Z_NULL;
    z.next_in   = (unsigned char *)inData;
    z.avail_in  = inLength;
    z.next_out  = (unsigned char *)(*outData);
    z.avail_out = *outLength;

    retVal = inflateInit2(&z, -MAX_WBITS);
    if (retVal == Z_OK) {
      retVal = inflate(&z, Z_FINISH);
      if (retVal == Z_STREAM_END) {
	retVal = inflateEnd(&z);
	if (retVal != Z_BUF_ERROR) {
	  break;
	}
      } else {
	inflateEnd(&z);
	if (retVal == Z_OK) {
	  retVal = Z_BUF_ERROR;
	}
      }
    }
  }
  *outLength = z.total_out;

  /* if we have an error, free resources */
  if (retVal != Z_OK && *outData) {
    free(*outData);
  }

  return retVal;
}

int
ferite_zlib_priv_uncompr(char *inData, long inLength, char **outData, long *outLength)
{
  int  multiple;
  int  retVal;

  *outData = 0;

  /* from PHP, use multiple to expand our buffer until it is large enough*/
  for (multiple = 2; multiple < 16; ++multiple) {
    *outLength = inLength * (1 << multiple);

    /* get a better, bigger, buffer */
    char *newBuffer = realloc(*outData, *outLength);
    if (newBuffer == 0) {
      /* out of memory */
      retVal = Z_MEM_ERROR;
      break;
    }
    *outData = newBuffer;

    retVal = uncompress((unsigned char *)(*outData),
			(unsigned long)outLength,
			(unsigned char *)inData,
			(unsigned long)inLength);
    if (retVal != Z_BUF_ERROR) {
      break;
    }
  }

  /* if we have an error, free resources */
  if (retVal != Z_OK && *outData) {
    free(*outData);
  }

  return retVal;
}

#if 0


















/* Compress from file source to file dest until EOF on source.
 * def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 * allocated for processing, Z_STREAM_ERROR if an invalid compression
 * level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
 * version of the library linked do not match, or Z_ERRNO if there is
 * an error reading or writing the files.
 */
int
def(FILE *source, FILE *dest, int level)
{
  int ret, flush;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

  /* allocate deflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, level);
  if (ret != Z_OK) {
    return ret;
  }

  /* compress until end of file */
  do {
    strm.avail_in = fread(in, 1, CHUNK, source);
    if (ferror(source)) {
      (void)deflateEnd(&strm);
      return Z_ERRNO;
    }
    flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
    strm.next_in = in;

    /* run deflate() on input until output buffer not full, finish
     * compression if all of source has been read in
     */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, flush);    /* no bad return value */
#if 0
      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
#else
      if (ret == Z_STREAM_ERROR) {
	fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
      }
#endif
      have = CHUNK - strm.avail_out;
      if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
	(void)deflateEnd(&strm);
	return Z_ERRNO;
      }
    } while (strm.avail_out == 0);
#if 0
    assert(strm.avail_in == 0);     /* all input will be used */
#else
    if (strm.avail_in != 0) {
      fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
    }
#endif

    /* done when last data in file processed */
  } while (flush != Z_FINISH);
#if 0
  assert(ret == Z_STREAM_END);        /* stream will be complete */
#else
  if (ret != Z_STREAM_END) {
    fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
  }
#endif

  /* clean up and return */
  (void)deflateEnd(&strm);
  return Z_OK;
}

/* Decompress from file source to file dest until stream ends or EOF.
 * inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
 * allocated for processing, Z_DATA_ERROR if the deflate data is
 * invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
 * the version of the library linked do not match, or Z_ERRNO if there
 * is an error reading or writing the files.
 */
int
inf(FILE *source, FILE *dest)
{
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK) {
    return ret;
  }

  /* decompress until deflate stream ends or end of file */
  do {
    strm.avail_in = fread(in, 1, CHUNK, source);
    if (ferror(source)) {
      (void)inflateEnd(&strm);
      return Z_ERRNO;
    }
    if (strm.avail_in == 0) {
      break;
    }
    strm.next_in = in;

    /* run inflate() on input until output buffer not full */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
#if 0
      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
#else
      if (ret != Z_STREAM_ERROR) {
	fprintf(stderr, "%s %d\n", __FILE__, __LINE__);
      }
#endif
      switch (ret) {
      case Z_NEED_DICT:
	ret = Z_DATA_ERROR;     /* and fall through */
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
	(void)inflateEnd(&strm);
	return ret;
      }
      have = CHUNK - strm.avail_out;
      if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
	(void)inflateEnd(&strm);
	return Z_ERRNO;
      }
    } while (strm.avail_out == 0);

    /* done when inflate() says it's done */
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  (void)inflateEnd(&strm);
  return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void
zerr(int ret)
{
  fputs("zpipe: ", stderr);
  switch (ret) {
  case Z_ERRNO:
    if (ferror(stdin)) {
      fputs("error reading stdin\n", stderr);
    }
    if (ferror(stdout)) {
      fputs("error writing stdout\n", stderr);
    }
    break;
  case Z_STREAM_ERROR:
    fputs("invalid compression level\n", stderr);
    break;
  case Z_DATA_ERROR:
    fputs("invalid or incomplete deflate data\n", stderr);
    break;
  case Z_MEM_ERROR:
    fputs("out of memory\n", stderr);
    break;
  case Z_VERSION_ERROR:
    fputs("zlib version mismatch!\n", stderr);
  }
}

/* compress or decompress from stdin to stdout */
int
testMain(int argc, char **argv)
{
  int ret;

  /* avoid end-of-line conversions */
  SET_BINARY_MODE(stdin);
  SET_BINARY_MODE(stdout);

  /* do compression if no arguments */
  if (argc == 1) {
    ret = def(stdin, stdout, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
      zerr(ret);
    }
    return ret;
  }

  /* do decompression if -d specified */
  else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
    ret = inf(stdin, stdout);
    if (ret != Z_OK) {
      zerr(ret);
    }
    return ret;
  }

  /* otherwise, report usage */
  else {
    fputs("zpipe usage: zpipe [-d] < source > dest\n", stderr);
    return 1;
  }
}
#endif
