# APIソースのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS	= pwm_api.c
OBJS 	= $(SRCS:.c=.o)

all : $(OBJS)
	ar rc libpwm_api.a $(OBJS)
	ranlib libpwm_api.a
	cp -f libpwm_api.a *.h /usr/local/lib/CATS_ECU2

clean:
	rm -f $(TARGET) *.o *.a *~ core .depend
	rm -f /usr/local/lib/CATS_ECU2/pwm_api.h
	rm -f /usr/local/lib/CATS_ECU2/libpwm_api.a
