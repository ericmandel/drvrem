/*  This file, drvrem.c, contains driver routines for accessing web files
    directly from within the emscripten environment.
*/

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.                                           */

#if __EMSCRIPTEN__

#include <stdlib.h>
#include <string.h>
#include "fitsio2.h"
#include <emscripten.h>

#define CACHE_MAX 2048000

#ifndef min
#define min(a,b) (a)<(b)?(a):(b)
#endif
#ifndef max
#define max(a,b) (a)>(b)?(a):(b)
#endif

typedef struct    /* structure containing em file structure */ 
{
  char *url;		 /* url of file */
  char *cache;           /* io cache buffer */
  LONGLONG cachesize;    /* current io cache size */
  LONGLONG cachemax;	 /* max size of cache */
  LONGLONG startpos;     /* start buffer position */
  LONGLONG endpos;       /* end buffer position */
  LONGLONG curpos;       /* current file position */
  LONGLONG filesize;     /* file size (via HEAD request) */
} emdriver;

static emdriver emTable[NMAXFILES];  /* allocate em file handle tables */

/*--------------------------------------------------------------------------*/
int em_init(void)
{
    int ii;

    for (ii = 0; ii < NMAXFILES; ii++) /* initialize all empty slots in table */
    {
       emTable[ii].filesize = -1;
       emTable[ii].cache = NULL;
       emTable[ii].cachesize = 0;
       emTable[ii].startpos = 0;
       emTable[ii].endpos = 0;
       emTable[ii].curpos = 0;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
int em_shutdown(void)
{
  return 0;
}

/*--------------------------------------------------------------------------*/
int em_setoptions(int options)
{
  /* do something with the options argument, to stop compiler warning */
  options = 0;
  return options;
}

/*--------------------------------------------------------------------------*/
int em_getoptions(int *options)
{
  *options = 0;
  return 0;
}

/*--------------------------------------------------------------------------*/
int em_getversion(int *version)
{
    *version = 10;
    return 0;
}

/*--------------------------------------------------------------------------*/
int em_open(char *filename, int rwmode, char *prefix, int *handle)
{
  char *s;
  int ii, got;
  *handle = -1;
  for(ii=0; ii<NMAXFILES; ii++){  /* find empty slot in table */
    if( !emTable[ii].url ){
      *handle = ii;
      break;
    }
  }
  if( *handle == -1 ){
    return TOO_MANY_FILES;    /* too many files opened */
  }
  emTable[*handle].url = calloc(1, strlen(filename) + 9);
  strcpy(emTable[*handle].url, prefix);
  strcat(emTable[*handle].url, filename);
  emTable[*handle].curpos = 0;
  if( (s=(char *)getenv("CFITSIO_EM_CACHE_MAX")) != NULL ){
    emTable[*handle].cachemax = atol(s);
  } else {
    emTable[*handle].cachemax = CACHE_MAX;
  }
  got = EM_ASM_INT({
      if( typeof XMLHttpRequest !== "undefined" ){
	const url = UTF8ToString($0);
	const xhr = new XMLHttpRequest();
	// false for sync, which is needed because cfitsio is synchronous
	xhr.open('HEAD', url, false);
	// make the request and wait
	xhr.send();
	// error check
	if( xhr.status !== 200 ){
	  // FILE_NOT_OPENED
	  return -104;
	}
	// get header value, if possible
	len = xhr.getResponseHeader('Content-Length');
	if( len ){
	  return len;
	} else {
	  // READ_ERROR
	  return -108;
	}
      } else {
	// FILE_NOT_OPENED
	return -104;
      }
  }, emTable[*handle].url);
  if( got < 0 ){
    return abs(got);
  }
  emTable[*handle].filesize = got;
  return 0;
}

int em_open_http(char *filename, int rwmode, int *handle)
{
  return em_open(filename, rwmode, "http://", handle);
}

int em_open_https(char *filename, int rwmode, int *handle)
{
  return em_open(filename, rwmode, "https://", handle);
}

/*--------------------------------------------------------------------------*/
int em_close(int handle)
/*
  close the file and free the memory.
*/
{
  if( emTable[handle].url ){
    free( emTable[handle].url );
    emTable[handle].url = NULL;
  }
  if( emTable[handle].cache ){
    free( emTable[handle].cache );
    emTable[handle].cache = NULL;
  }
  emTable[handle].cachesize = 0;
  emTable[handle].startpos = 0;
  emTable[handle].endpos = 0;
  emTable[handle].curpos = 0;
  emTable[handle].filesize = -1;
  return 0;
}

/*--------------------------------------------------------------------------*/
int em_size(int handle, LONGLONG *filesize)
/*
  return the size of the file; only called when the file is first opened
*/
{
    *filesize = emTable[handle].filesize;
    return 0;
}

/*--------------------------------------------------------------------------*/
int em_seek(int handle, LONGLONG offset)
{
  if( offset >  emTable[handle].filesize ){
    return END_OF_FILE;
  }
  emTable[handle].curpos = offset;
  return 0;
}

/*--------------------------------------------------------------------------*/
int em_read(int hdl, void *buffer, long nbytes)
/*
  read bytes from the current position in the file
*/
{
  char range[1024];
  long get, got;
  /* check for EOF */
  if( emTable[hdl].curpos + nbytes > emTable[hdl].filesize ){
    return END_OF_FILE;
  }
  /* can we use the cache? */
  if( emTable[hdl].cache != NULL                                  &&
      (emTable[hdl].curpos >= emTable[hdl].startpos)        &&
      (emTable[hdl].curpos + nbytes <= emTable[hdl].endpos) ){
    memcpy(buffer,
	   &emTable[hdl].cache[emTable[hdl].curpos - emTable[hdl].startpos],
	   nbytes);
    emTable[hdl].curpos += nbytes;
    return 0;
  }
  /* retrieve a new cache of data */
  if( nbytes > emTable[hdl].cachemax ){
    get = nbytes;
  } else {
    get = min(emTable[hdl].cachemax,
	      emTable[hdl].filesize - emTable[hdl].curpos);
  }
  /* might have to allocate or re-allocate the cache buffer */
  if( !emTable[hdl].cache ){
    emTable[hdl].cache = malloc(get);
    emTable[hdl].cachesize = get;
  } else if( emTable[hdl].cachesize < get ){
    emTable[hdl].cache = realloc(emTable[hdl].cache, get);
    emTable[hdl].cachesize = get;
  }
  /* use range to retrieve bytes for this cache */
  snprintf(range, 1024, "%lld-%lld",
	   emTable[hdl].curpos, emTable[hdl].curpos + get - 1);
  got = EM_ASM_INT({
      // in-line javascript to make the xhr call
      let i;
      let s;
      const url = UTF8ToString($0);
      const range = UTF8ToString($1);
      const xhr = new XMLHttpRequest();
      // mime type for FITS file
      xhr.overrideMimeType('application/octet-stream');
      // response type
      xhr.responseType = 'arraybuffer';
      // false for sync, which is needed because cfitsio is synchronous
      xhr.open('GET', url, false);
      // range request
      xhr.setRequestHeader('Range', `bytes=${range}`);
      // make the request and wait
      xhr.send();
      // synchronous call waits until completion and then ...
      // 206 is Partial Content success status
      if( xhr.status === 206 ){
	s = new Uint8Array(xhr.response);
	Module.HEAP8.set(s, $2);
	return s.length;
      } else {
	// READ_ERROR
	return -108;
      }
    }, emTable[hdl].url, range, emTable[hdl].cache);
  /* check for error */
  if( got < 0 ){
    return abs(got);
  }
  /* reset position pointers for newly cached data */
  emTable[hdl].startpos = emTable[hdl].curpos;
  emTable[hdl].endpos = emTable[hdl].startpos + got;
  /* copy the data to output buffer */
  memcpy(buffer, emTable[hdl].cache, nbytes);
  /* update current position for next read */
  emTable[hdl].curpos += nbytes;
  return 0;
}

/*--------------------------------------------------------------------------*/
int em_write(int hdl, void *buffer, long nbytes)
{
  return READONLY_FILE;
}

/* avoid compiler warning (below) if not compiling with EMSCRIPTEN */
int have_emscripten = 1;

#else

/* avoid compiler warning if not compiling with EMSCRIPTEN */
int have_emscripten = 0;

#endif
