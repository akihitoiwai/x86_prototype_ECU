# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = main.c

#OBJS = $(SRCS:.c=.o) ../api_library/cats_api.o
OBJS = $(SRCS:.c=.o)

TARGET	= sample_app

all : $(OBJS)
	#$(CC) $(CFLGAS) -lpthread -o $(TARGET) $(OBJS)
	#$(CC) $(CFLGAS) $(OBJS) -L ../api_library/ -l cats_api -lpthread  -o $(TARGET)
	$(CC) $(CFLGAS) $(OBJS) -L /usr/src/CATS_ECU2/api_library/dacapi -l dac_api -lpthread -o $(TARGET)

	#cp -f $(SRCS) ../cf_files

clean:
	rm -f $(TARGET) *.o *~ core .depend
