
.SUFFIXES: .o .c

BINDIR = ../bin
OBJDIR = ./obj
ROOTDIR = ..
CANROOT = .
CC = gcc
AR = ar

#CFLAGS = -I $(ROOTDIR) -I $(CANROOT) -I $(CANROOT)/CanDriver/SJA1000/kernel_driver
CFLAGS = -I $(ROOTDIR) -I $(CANROOT) -I $(CANROOT)/usr/src/CATS_ECU2/kernel_driver
ifdef DEBUG
CFLAGS += -g -D _DEBUG
else
CFLAGS += -O2
endif
ifdef TEST
CFLAGS += -D TEST
endif

#TARGET = $(BINDIR)/libcan_api.a
TARGET = /usr/src/CATS_ECU2/api_library/canapi/libcan_api.a

SRC1DIR = $(CANROOT)/CanDriver/SJA1000
SRC1 = Can_SJA1000.c 

SRC2DIR = $(CANROOT)/CanInterface
SRC2 = CanIf.c CanIf_sub.c

##SRC3DIR = $(SRC1DIR)/usr/src/CATS_ECU/api_library
#SRC3DIR = /usr/src/CATS_ECU2/api_library
##SRC3 = can_api.c
#SRC3 = can_api.c

SRC4DIR = $(ROOTDIR)
SRC4 = Dem.c

SRC6DIR = $(ROOTDIR)/Test/util
SRC6 = TimeRecorder.c


OBJ1 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/1, $(SRC1)))
OBJ2 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/2, $(SRC2)))
OBJ3 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/3, $(SRC3)))
OBJ4 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/4, $(SRC4)))
OBJ6 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/6, $(SRC6)))

OBJS = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ6)
SRCS = $(addprefix $(SRC1DIR)/, $(SRC1))
SRCS += $(addprefix $(SRC2DIR)/, $(SRC2))
SRCS += $(addprefix $(SRC3DIR)/, $(SRC3))
SRCS += $(addprefix $(SRC4DIR)/, $(SRC4))
SRCS += $(addprefix $(SRC6DIR)/, $(SRC6))

ifdef TEST
SRC5DIR = $(CANROOT)/CanDriver/test
SRC5 = Can_test.c
OBJ5 = $(patsubst %.c,%.o,$(addprefix $(OBJDIR)/5, $(SRC5)))
OBJS += $(OBJ5)
SRCS += $(addprefix $(SRC5DIR)/, $(SRC5))
endif

all : $(OBJDIR) $(BINDIR) $(TARGET)

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

$(TARGET) : $(OBJS)
	$(AR) -cr $(TARGET) $(OBJS)

$(OBJDIR)/1%.o : $(SRC1DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/2%.o : $(SRC2DIR)/%.c
	$(CC) -c -D SJA1000 $(CFLAGS) -o $@ $<

$(OBJDIR)/3%.o : $(SRC3DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/4%.o : $(SRC4DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/5%.o : $(SRC5DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/6%.o : $(SRC6DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f obj/* bin/*

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

test:
	make TEST=1

debug:
	make TEST=1 DEBUG=1

# DO NOT DELETE
