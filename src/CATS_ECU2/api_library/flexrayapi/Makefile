# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= Fr.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libflexray_api.a $(OBJS)
	ranlib libflexray_api.a
	cp -f libflexray_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/Fr.h
	rm -f /usr/local/lib/CATS_ECU2/Fr_GeneralTypes.h
	rm -f /usr/local/lib/CATS_ECU2/flexray_api.h
	rm -f /usr/local/lib/CATS_ECU2/libflexray_api.a
