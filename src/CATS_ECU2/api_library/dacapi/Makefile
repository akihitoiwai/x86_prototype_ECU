# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= dac_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libdac_api.a $(OBJS)
	ranlib libdac_api.a
	cp -f libdac_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/dac_api.h
	rm -f /usr/local/lib/CATS_ECU2/libdac_api.a
