# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = main.c

OBJS = $(SRCS:.c=.o)

TARGET	= sample_app

all : $(OBJS)
	$(CC) $(CFLGAS) $(OBJS) -L /usr/src/CATS_ECU2/api_library/gpioapi \
    -l gpio_api -lpthread   -o $(TARGET)

clean:
	rm -f $(TARGET) *.o *~ core .depend
