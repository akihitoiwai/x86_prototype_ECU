
#KDIR := /usr/src/linux-headers-2.6.26-2-686/
#INCLUDES = -I/usr/src/linux-headers-2.6.26-2-686/include
KDIR := /usr/src/linux-headers-2.6.26-2-486/
#INCLUDES = -I/usr/src/linux-headers-2.6.26-2-486/include



KERNMINOR = $(shell uname -r | cut -d. -f3 | cut -d- -f1)

KERNTYPEMINOR18 = $(shell if test $(KERNMINOR) -lt 19; then echo "-DKERNELMINOR18"; fi)

#MOD_DIR	=	/usr/src/CATS_ECU2

TARGET	=	cats_module

SOURCES	=	cats_sja1000.c\
			cats_can.c\
			cats_flexray.c\
			cats_adc.c\
			cats_dac.c\
			cats_pwm.c\
			cats_gpio.c\
			cats_powacc.c\
			cats_callback.c\
			cats_testmodule.c\
			cats_fpga.c

CFLAGS += $(KERNTYPEMINOR18)

OBJECTS	=	$(SOURCES:.c=.o)

obj-m += $(TARGET).o
$(TARGET)-objs := $(OBJECTS)

all:
	clear
	make -C $(KDIR) SUBDIRS=`pwd` modules

clean:
	make -C $(KDIR) SUBDIRS=`pwd` clean
	rm -f $(TARGET) *.o *.ko .*.cmd *~ core


