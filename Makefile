# cfitsio build copied from astroem
CFITSIO 	= ./cfitsio-4.0.0
CFITSIO_CFLAGS	= $(EMFLAGS) -fno-common -D__x86_64__

guard:		FORCE
		@(echo "use 'make cfitsio' to build new cfitsio library")

cfitsio:	FORCE
		@(CDIR=`pwd`; \
		addem $(CFITSIO); \
		cd $(CFITSIO);       \
		echo "building $(CFITSIO) ..."; \
		FC=none emconfigure ./configure;   \
		sed -i -orig 's/ \-DCFITSIO_HAVE_CURL=1//;s/ \-DHAVE_NET_SERVICES=1//' Makefile;     \
		emmake make ZLIB_SOURCES="" CFLAGS="$(CFITSIO_CFLAGS)" clean libcfitsio.a "FITSIO_SRC=";      \
		cp -p libcfitsio.a $${CDIR}/lib;   \
	        cp -p *.h $${CDIR}/include;)

ftools:		FORCE
		@(cd tests && make ftools && make tar)

clean:		FORCE
		@(rm -rf *~ *.o foo* *.dSYM; \
		CDIR=`pwd`; \
		cd $${CDIR}/$(CFITSIO) && make clean; \
		cd $${CDIR}/tests && make clean;)

FORCE:
