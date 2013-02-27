# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= test_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libtest_api.a $(OBJS)
	ranlib libtest_api.a
	cp -f libtest_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/test_api.h
	rm -f /usr/local/lib/CATS_ECU2/libtest_api.a
