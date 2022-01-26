# EMCC compiler
EMCC		= emcc
# debugging:
# EMFLAGS		= -g -O0
EMFLAGS		= -O3
EMPORTS	=	-s USE_ZLIB=1 -s USE_BZIP2=1
EMOPTS =	-s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 $(EMPORTS) \
		-s BUILD_AS_WORKER -s EXPORTED_RUNTIME_METHODS='["cwrap"]'
# debugging:
# EMDOPTS =	-s ASSERTIONS=1  -s SAFE_HEAP=1
EMINCS = 	-I./include
EMCC =		emcc $(EMFLAGS) $(EMOPTS) $(EMDOPTS) $(EMINCS)

# flags to generate all emcc objects (i.e. so default rule for $(CC) runs emcc)
CC =		emcc $(EMFLAGS) -I../include

all:	ftools

# replacement technique: make a library using emar:
# https://github.com/emscripten-core/emscripten/issues/12382
SRCS =  	liststruc.c listhead.c imstat.c tablist.c
OBJS =		$(SRCS:%.c=%.o)

INCS = 	-I ./include
LIBS =	libftools.a ../lib/libcfitsio.a

libftools.a:	$(OBJS)
		emar rcs $@ $?

ftools:	libftools.a
	@($(EMCC) \
	-s EXPORTED_FUNCTIONS="['_liststruc', '_listhead', '_imstat', '_tablist', '_ffclos', '_ffghdn', '_ffghdt', '_ffgipr', '_ffgky', '_ffgncl', '_ffgnrw', '_ffkeyn', '_ffmrhd', '_ffopentest', '_ffrprt']" \
	--pre-js pre_ftools.js --post-js post_ftools.js \
	-o ftools.js $(INCS) $(LIBS); \
	rm -f libftools.a)

tar:	FORCE
	@(tar cf - ftools[._]* | gzip -c > ../ftools.tar.gz)

clean:	FORCE
	@(rm -rf libftools.a \
	*~ *.o foo* *.dSYM */*.o */*~; \
	CDIR=`pwd`; \
	cd $${CDIR}/fitsem && make clean; \
	cd $${CDIR}/tcurl && make clean; \
	cd $${CDIR}/urlopen && make clean; \
	cd $${CDIR}/ftools && make clean;)

FORCE:
