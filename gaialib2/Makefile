###############################################################################
# program name and version

PROGNAME        = gaia2read
PROGVERSION     = 1.0


###############################################################################
# deployment directory
BINDIR          = ../../bin

###############################################################################
# compiler

CC              = gcc
MAKE            = make

CFLAGS   = -W -Wall -ansi -pedantic -std=c99 -g

###############################################################################

# targets

###############################################################################
# default: target program (release)

.PHONY: all

all: $(PROGNAME)

###############################################################################
# deploy target

.PHONY: install

install: all
	@echo "installing $(PROGNAME) to $(BINDIR)"
	@cp -f $(PROGNAME) $(BINDIR)

.PHONY: clean

clean:
	rm -f *.o $(PROGNAME)

gaia2read: gaia2read.o gaia2ret.o gaia2cat.o gaiastar.o astromath.o astrio.o astrometry.o mmath.o myargs.o pmotion.o point.o sllist.o utils.o gaiaPrint.o
	$(CC) $(CFLAGS) -o gaia2read gaia2read.o gaia2ret.o gaia2cat.o gaiastar.o astromath.o astrio.o astrometry.o mmath.o myargs.o pmotion.o point.o sllist.o utils.o gaiaPrint.o -lm

gaia2read.o: gaia2read.c gaia2ret.h myargs.h astrio.h astrometry.h utils.h gaiaPrint.h
	$(CC) $(CFLAGS) -c gaia2read.c

gaia2ret.o: gaia2ret.c gaia2ret.h gaia2cat.h astrometry.h mmath.h utils.h gaia2idsort.h gaiastar.h sllist.h astromath.h pmotion.h
	$(CC) $(CFLAGS) -c gaia2ret.c

gaia2cat.o: gaia2cat.c gaiastar.h sllist.h gaia2cat.h gaia2ret.h utils.h
	$(CC) $(CFLAGS) -c gaia2cat.c

gaiastar.o: gaiastar.c gaiastar.h pmotion.h
	$(CC) $(CFLAGS) -c gaiastar.c

gaiaPrint.o: gaiaPrint.c gaiaPrint.h gaiastar.h gaia2ret.h
	$(CC) $(CFLAGS) -c gaiaPrint.c

astromath.o: astromath.c astromath.h mmath.h
	$(CC) $(CFLAGS) -c astromath.c

astrio.o: astrio.c astrio.h astromath.h mmath.h
	$(CC) $(CFLAGS) -c astrio.c

astrometry.o: astrometry.c astrometry.h astromath.h mmath.c
	$(CC) $(CFLAGS) -c astrometry.c

mmath.o: mmath.c mmath.h
	$(CC) $(CFLAGS) -c mmath.c

myargs.o: myargs.c myargs.h utils.h
	$(CC) $(CFLAGS) -c myargs.c

pmotion.o: pmotion.c pmotion.h astromath.h
	$(CC) $(CFLAGS) -c pmotion.c

point.o: point.c point.h astrometry.h mmath.h
	$(CC) $(CFLAGS) -c point.c

sllist.o: sllist.c sllist.h
	$(CC) $(CFLAGS) -c sllist.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c
