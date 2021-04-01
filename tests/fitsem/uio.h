#ifndef SZ_LINE
#define SZ_LINE 4096
#endif

typedef struct {
  char *url;
  char *mode;
  off_t len;
  off_t cur;
} *UIO, UIORec;

UIO uioopen(char *name, char *mode);
off_t uioseek(UIO uio, off_t offset, int whence);
off_t uiotell(UIO uio);
size_t uioread(UIO uio, char *buf, size_t n);
int uioclose(UIO uio);
