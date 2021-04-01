#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#define SZ_LINE 1024

typedef struct DataStruct{
  char *buf;
  int cur;
  int maxlen;
} *Data, DataRec;

size_t cb(void *contents, size_t size, size_t nmemb, void *userp){
  size_t realsize = size * nmemb;
  Data d = (Data)userp;
  fprintf(stderr, "realsize: %ld\n", realsize);
  if( realsize > d->maxlen - d->cur ){
    d->maxlen = d->cur + realsize;
    if( d->buf != NULL ){
      d->buf = realloc(d->buf, d->maxlen + 1);
    } else {
      d->buf = malloc(d->maxlen + 1);
    }
  }
  memcpy(&d->buf[d->cur], contents, realsize);
  d->cur += realsize;
  d->buf[d->cur] = '\0';
  return realsize;
}

int main(int argc, char *argv[]){
  char *url;
  char *range;
  char **ranges;
  char *defranges[2];
  char *emrange[1];
  int i;
  int rlen;
  CURL *curl;
  CURLcode res;
  struct curl_slist *hs=NULL;
  Data d;

  defranges[0] = "80-159";
  defranges[1] = "240-399";
  emrange[0] = "240-399";

  switch(argc){
  case 1:
    url = "https://js9.si.edu/js9/data/fits/m13.fits";
    ranges = defranges;
    rlen = 2;
    break;
  case 2:
    url = "https://js9.si.edu/js9/data/fits/m13.fits";
    ranges = &argv[1];
    rlen = 1;
    break;
  default:
    url = argv[1];
    ranges = &argv[2];
    rlen = argc - 2;
    break;
  }
  d = calloc(1, sizeof(DataRec));
  fprintf(stderr, "url: %s\n", url);
  curl = curl_easy_init();
  if( curl ){
    /* set up options */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)d);
    hs = curl_slist_append(hs, "Content-Type: application/octet-stream");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
    /* read the data in range(s) */
    for(i=0; i<rlen; i++){
      range = ranges[i];
      fprintf(stderr, "range: %s\n", range);
      d->cur = 0;
      curl_easy_setopt(curl, CURLOPT_RANGE, range);
      res = curl_easy_perform(curl);
      if( res != CURLE_OK ){
	fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
      } else {
	fprintf(stderr, "total: %d\n", d->cur);
	fwrite(d->buf, 1, d->cur, stdout);
	fflush(stdout);
      }
    }
    curl_slist_free_all(hs);
    curl_easy_cleanup(curl);
  }

  if( d->buf ){ free(d->buf); }
  (void)free(d);
  return 0;
}
