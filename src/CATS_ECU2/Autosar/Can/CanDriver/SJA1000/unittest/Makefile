#-----------------------------------------------
#
#	CAN sample application Makefile
#
#-----------------------------------------------
TARGET		=	cats_can
PSOURCES	=
CSOURCES	=	main.c\
			../cats_canapi.c\
			../sja1000ctrl.c

STDLIBS		=
OPTLIBS		=

INC_SYS		=	.. ../../..
PINC_APP	=
CINC_APP	=
LIBDIR		=

INC_S	=	$(addprefix -I, $(INC_SYS))
CINC_A	=	$(addprefix -I, $(CINC_APP))
PINC_A	=	$(addprefix -I, $(PINC_APP))
CINCS	=	$(INC_S) $(CINC_A)
PINCS	=	$(INC_S) $(PINC_A)
LIBS	=	$(addprefix -L, $(LIBDIR))
LIBN	=	$(addprefix -l, $(STDLIBS))
LIBN	+=	$(addprefix -l, $(OPTLIBS))

#
# Compiler, Linker Options, Resource_compiler
#
CP	=	g++
CC	=	gcc
LK	=	g++
RC	=

POPT	=	-O2
COPT	=	-O2
LOPT	=

COF	=	-o
LOF	=	-o

PFLAGS	=
CFLAGS	=	-DDEBUG -D_DEBUG -g
#CFLAGS	=	-DNDEBUG
PFLAGS	=	
LFLAGS	=

# -Wunused -Werror
CWARN	=	-Wimplicit -Wreturn-type -Wswitch\
		-Wformat -Wshadow -Wuninitialized

OBJECTS	=	$(PSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

.SUFFIXES :
.SUFFIXES : .hpp .h .c .cpp .o

$(TARGET): $(OBJECTS) Makefile
	$(LK) $(LOF)$(TARGET) $(LFLAGS) $(LIBS) $(OBJECTS) $(LIBN)

.c.o:
	$(CC) $(COPT) $(CFLAGS) $(CINCS) $(CWARN) -c $< $(COF)$@

.cpp.o:
	$(CC) $(POPT) $(PFLAGS) $(PINCS) -c $< $(COF)$@

run:
	./$(TARGET)

all:
	make clean
	make depend
	make $(TARGET)

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)

tarball:
	chmod -x *.[hc] Makefile *.txt Doxyfile
	rm -f $(TARGET)_src.tgz
	tar cfvz $(TARGET)_src.tgz *.[hc] Makefile *.txt Doxyfile
	chmod 444 $(TARGET)_src.tgz

depend:
	makedepend -Y -- $(CFLAGS) $(INC_A) -- $(CSOURCES) $(PSOURCES)

# DO NOT DELETE

../cats_canapi.o: ../cats_canapi.h ../kernel_driver/cats_ioctl.h
../cats_canapi.o: ../kernel_driver/cats_sja1000.h ../sja1000ctrl.h
../sja1000ctrl.o: ../kernel_driver/cats_sja1000.h ../sja1000ctrl.h
../sja1000ctrl.o: ../kernel_driver/cats_ioctl.h
