# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= gpio_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libgpio_api.a $(OBJS)
	ranlib libgpio_api.a
	cp -f libgpio_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/gpio_api.h
	rm -f /usr/local/lib/CATS_ECU2/libgpio_api.a
