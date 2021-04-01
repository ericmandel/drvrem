all:	imstat listhead liststruc tablist

imstat:	FORCE
	gcc -o imstat imstat.c -I ~/include -L ~/lib -lcfitsio -lcurl

listhead:	FORCE
	gcc -o listhead listhead.c -I ~/include -L ~/lib -lcfitsio -lcurl

liststruc:	FORCE
	gcc -o liststruc liststruc.c -I ~/include -L ~/lib -lcfitsio -lcurl

tablist:FORCE
	gcc -o tablist tablist.c -I ~/include -L ~/lib -lcfitsio -lcurl

clean:	FORCE
	@(rm -rf *~ *.o *.a imstat liststruc listhead tablist)

FORCE:



