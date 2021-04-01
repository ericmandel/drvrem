# EMCC compiler
EMCC		= emcc
EMFLAGS		= -O3
EMPORTS	=	-s USE_ZLIB=1 -s USE_BZIP2=1
EMOPTS =	-s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 $(EMPORTS) \
		-s BUILD_AS_WORKER -s EXPORTED_RUNTIME_METHODS='["cwrap"]'
EMINCS = 	-I./include
EMCC =		emcc $(EMFLAGS) $(EMOPTS) $(EMINCS)

# flags to generate all emcc objects (i.e. so default rule for $(CC) runs emcc)
CC =		emcc $(EMFLAGS) -I../include

all:	ftools

# zlib
ZEXTRA =	./zextra
ZSRCS =		$(ZEXTRA)/zcompress.c \
		$(ZEXTRA)/zuncompress.c
ZOBJS = 	$(ZSRCS:%.c=%.o)

# replacement technique: make a library using emar:
# https://github.com/emscripten-core/emscripten/issues/12382
SRCS =  	liststruc.c listhead.c imstat.c tablist.c
OBJS =		$(SRCS:%.c=%.o)

INCS = 	-I ./include
LIBS =	libftools.a ../lib/libcfitsio.a

libftools.a:	$(OBJS) $(ZOBJS)
		emar rcs $@ $?

ftools:	libftools.a
	$(EMCC) \
	-s EXPORTED_FUNCTIONS="['_liststruc', '_listhead', '_imstat', '_tablist', '_ffclos', '_ffghdn', '_ffghdt', '_ffgipr', '_ffgky', '_ffgncl', '_ffgnrw', '_ffkeyn', '_ffmrhd', '_ffopentest', '_ffrprt']" \
	--preload-file data \
	--pre-js pre_ftools.js --post-js post_ftools.js \
	-o ftools.js $(INCS) $(LIBS)

clean:	FORCE
	@(rm -rf libftools.a \
	*~ *.o foo* *.dSYM */*.o */*~; \
	CDIR=`pwd`; \
	cd $${CDIR}/fitsem && make clean; \
	cd $${CDIR}/tcurl && make clean; \
	cd $${CDIR}/urlopen && make clean; \
	cd $${CDIR}/ftools && make clean;)

FORCE:
