gaia2read: gaia2read.o gaia2ret.o gaia2cat.o gaiastar.o astromath.o astrio.o astrometry.o mmath.o myargs.o pmotion.o point.o sllist.o utils.o gaiaPrint.o
	gcc -O -Wall -W -pedantic -std=c99 -o gaia2read gaia2read.o gaia2ret.o gaia2cat.o gaiastar.o astromath.o astrio.o astrometry.o mmath.o myargs.o pmotion.o point.o sllist.o utils.o gaiaPrint.o -lm

gaia2read.o: gaia2read.c gaia2ret.h myargs.h astrio.h astrometry.h utils.h gaiaPrint.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c gaia2read.c

gaia2ret.o: gaia2ret.c gaia2ret.h gaia2cat.h astrometry.h mmath.h utils.h gaia2idsort.h gaiastar.h sllist.h astromath.h pmotion.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c gaia2ret.c

gaia2cat.o: gaia2cat.c gaiastar.h sllist.h gaia2cat.h gaia2ret.h utils.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c gaia2cat.c

gaiastar.o: gaiastar.c gaiastar.h pmotion.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c gaiastar.c

gaiaPrint.o: gaiaPrint.c gaiaPrint.h gaiastar.h gaia2ret.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c gaiaPrint.c

astromath.o: astromath.c astromath.h mmath.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c astromath.c

astrio.o: astrio.c astrio.h astromath.h mmath.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c astrio.c

astrometry.o: astrometry.c astrometry.h astromath.h mmath.c
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c astrometry.c

mmath.o: mmath.c mmath.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c mmath.c

myargs.o: myargs.c myargs.h utils.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c myargs.c

pmotion.o: pmotion.c pmotion.h astromath.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c pmotion.c

point.o: point.c point.h astrometry.h mmath.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c point.c

sllist.o: sllist.c sllist.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c sllist.c

utils.o: utils.c utils.h
	gcc -O -Wall -W -pedantic -ansi -std=c99 -c utils.c






