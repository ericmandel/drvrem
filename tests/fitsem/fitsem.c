#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <emscripten.h>
#include "uio.h"

#define min(a,b) (a<b?a:b)

#define SZ_BUF 288000

int fitsem(char *url, char *mode, int start, int len){
  int i, get;
  int got = 0;
  int total = 0;
  time_t s1, s2;
  char *buf;
  UIO uio;

  fprintf(stderr, "url:%s mode:%s start:%d len:%d\n", url, mode, start, len);
  uio = uioopen(url, "r");
  if( !strcmp(mode, "all") && uio->len >= 0 ){
    buf = calloc(1, SZ_BUF);
    fprintf(stderr, "read: %lld bytes\n", uio->len);
    s1 = time(NULL);
    while( total < uio->len ){
      get = min(uio->len - uio->cur, SZ_BUF);
      got = uioread(uio, buf, get);
      total += got;
    }
    s2 = time(NULL);
    fprintf(stderr, "elapsed: %lu seconds\n", s2 - s1);
  } else if( !strcmp(mode, "section") && len ){
    buf = calloc(1, len);
    uioseek(uio, start, 0);
    total = uioread(uio, buf, len);
    uioclose(uio);
    for(i=0; i<total; i++){
      if( start + i < 2880 ){
	fprintf(stderr, "%c", buf[i]);
      } else {
	fprintf(stderr, "%hhx ", buf[i]);
      }
    }
    fprintf(stderr, "\n");
  } else {
    fprintf(stderr, "why is there nothing to do?\n");
    return 0;
  }
  if( buf ){
    free(buf);
  }
  fprintf(stderr, "total: %d\n", total);
  return total;
}
