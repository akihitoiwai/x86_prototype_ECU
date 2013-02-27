# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = main.c

OBJS = $(SRCS:.c=.o)

TARGET	= sample_app

all : $(OBJS)
	#$(CC) $(CFLGAS) $(OBJS) -L ../api_library/ -l cats_api -lpthread  -o $(TARGET)
	$(CC) $(CFLGAS) $(OBJS) -L /usr/src/CATS_ECU2/api_library/testapi -l test_api -lpthread -o $(TARGET)

clean:
	rm -f $(TARGET) *.o *~ core .depend
