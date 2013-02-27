# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= adc_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libadc_api.a $(OBJS)
	ranlib libadc_api.a
	cp -f libadc_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/adc_api.h
	rm -f /usr/local/lib/CATS_ECU2/libadc_api.a
