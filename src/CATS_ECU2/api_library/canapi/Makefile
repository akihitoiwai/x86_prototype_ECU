# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= can_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libcan_api.a $(OBJS)
	ranlib libcan_api.a
	cp -f libcan_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/can_api.h
	rm -f /usr/local/lib/CATS_ECU2/libcan_api.a
