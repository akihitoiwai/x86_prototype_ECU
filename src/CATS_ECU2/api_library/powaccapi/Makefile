# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= powacc_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libpowacc_api.a $(OBJS)
	ranlib libpowacc_api.a
	cp -f libpowacc_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/powacc_api.h
	rm -f /usr/local/lib/CATS_ECU2/libpowacc_api.a
