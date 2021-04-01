#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#include "uio.h"

/*
 *
 * uionew -- create a new UIO structure
 *
 */
UIO uionew (void){
  UIO uio;
  uio = (UIO)calloc(1, sizeof(UIORec));
  return uio;
}

/*
 *
 * uiofree -- destroy a UIO structure
 *
 */
void uiofree (UIO uio){
  if( uio == NULL ){
    return;
  }
  if( uio->url ){
    free(uio->url);
  }
  if( uio->mode ){
    free(uio->mode);
  }
  free((void *)uio);
}

/*
 *
 * uioopen -- open reading (someday writing?)
 *
 */
UIO uioopen (char *url, char *mode){
  UIO uio;
  /* allocate a new struct */
  if( (uio = uionew()) == NULL ){
    return NULL;
  }
  /* fill in the record */
  uio->url = strdup(url);
  uio->mode = strdup(mode);
  uio->cur = 0;
  uio->len = EM_ASM_INT({
      if( typeof XMLHttpRequest !== "undefined" ){
	const url = UTF8ToString($0);
	const xhr = new XMLHttpRequest();
	// false for sync
	xhr.open('HEAD', url, false);
	// make the request and wait
	xhr.send();
	// get header value, if possible
	len = xhr.getResponseHeader('Content-Length');
	if( len ){
	  return len;
	}
	return -1;
      }
  }, url);
  /* return the news */
  return uio;
}

/*
 *
 * uioseek -- url seek of data
 *
 */
off_t uioseek (UIO uio, off_t offset, int whence) {
  switch(whence){
  case 0:
    uio->cur = offset;
    break;
  case 1:
    uio->cur = uio->cur + offset;
    break;
  case 2:
    break;
  default:
    break;
  }
  return uio->cur;
}

/*
 *
 * uiotell -- url tell of data
 *
 */
off_t uiotell (UIO uio){
  return uio->cur;
}

/*
 *
 * uioread -- url read of data
 *
 */
size_t uioread (UIO uio, char *buf, size_t n){
  int len;
  char range[SZ_LINE];
  /* sanity checks */
  if( (uio == NULL) || (buf == NULL) || (n < 0) ){
    return -1;
  }
  if( !strchr(uio->mode, 'r') && !strstr(uio->mode, "w+") ){
    fprintf(stderr,
	   "ERROR: illegal read operation on write-only data: %s\n",
	    uio->url);
    return -1;
  }
  snprintf(range, SZ_LINE-1, "%lld-%lld", uio->cur, uio->cur+n-1);
  /* get range of bytes */
  len = EM_ASM_INT({
      let i;
      let s;
      const url = UTF8ToString($0);
      const range = UTF8ToString($1);
      if( typeof XMLHttpRequest !== "undefined" ){
	const xhr = new XMLHttpRequest();
	// mime type for FITS file
	xhr.overrideMimeType('application/octet-stream');
	// response type
	xhr.responseType = "arraybuffer";
	// false for sync
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
	}
      } else {
	console.log('ERROR: XMLHttpRequest not supported: %s', url);
	return -1;
      }
      return 0;
    }, uio->url, range, buf);
  /* bump position pointer */
  uio->cur += len;
  /* return bytes */
  return len;
}

/*
 *
 * uioclose -- close url
 *
 */
int uioclose (UIO uio){
  if( uio != NULL ){
    uiofree(uio);
    return 0;
  }
  return 1;
}

