
INCDIR  = .. ../..
#LIBDIR  = ../../bin
LIBDIR  = /usr/src/CATS_ECU2/api_library/canapi
LIBNAME = can_api

CFLAGS  = -g -O2 $(addprefix -I, $(INCDIR)) -lpthread
CC      = gcc
CANLIB  = $(LIBDIR)/lib$(LIBNAME).a

all: sample1 sample2

sample1: sample1.c $(CANLIB)
	$(CC) $(CFLAGS) -o sample1 sample1.c -L $(LIBDIR) -l$(LIBNAME)

sample2: sample2.c $(CANLIB)
	$(CC) $(CFLAGS) -o sample2 sample2.c -L $(LIBDIR) -l$(LIBNAME)

rebuild:
	cd ..; make clean; make test;
	make clean
	make all

test:
	cd ..; make DEBUG=1 TEST=1;
	make TEST=1 all

$(CANLIB):
	cd ..; make test

clean:
	cd ..; make clean
	rm -f sample1 sample2

