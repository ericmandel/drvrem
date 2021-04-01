# EMCC compiler
EMCC		= emcc
EMFLAGS		= -O3
EMPORTS	=	-s USE_ZLIB=1 -s USE_BZIP2=1
EMOPTS =	-s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 $(EMPORTS) \
		-s BUILD_AS_WORKER -s EXPORTED_RUNTIME_METHODS='["cwrap"]'
EMINCS = 	-I./include
EMCC =		emcc $(EMFLAGS) $(EMOPTS) $(EMINCS)

# flags to generate all emcc objects (i.e. so default rule for $(CC) runs emcc)
CC =		emcc $(EMFLAGS) -I./include

all:	fitsem

fitsem:	FORCE
	emcc -g \
	-s BUILD_AS_WORKER=1 \
	-s EXPORTED_FUNCTIONS="['_fitsem']" \
	-s EXPORTED_RUNTIME_METHODS='["cwrap"]' \
	--pre-js pre_fitsem.js --post-js post_fitsem.js \
	-o fitsem.js fitsem.c uio.c

clean:	FORCE
	@(rm -rf fitsem.js fitsem.wasm \
	*~ *.o foo* *.dSYM)

FORCE:
