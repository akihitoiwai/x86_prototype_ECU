# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = main.c

#OBJS = $(SRCS:.c=.o) /usr/src/CATS_ECU/Utility/api_library/cats_api.o
OBJS = $(SRCS:.c=.o)

TARGET	= sample_app

all : $(OBJS)
	#$(CC) $(CFLGAS) -lpthread -o $(TARGET) $(OBJS)
	$(CC) $(CFLGAS) $(OBJS) -L /usr/src/CATS_ECU2/api_library/pwmapi/ -l pwm_api -lpthread  -o $(TARGET)

clean:
	rm -f $(TARGET) *.o *~ core .depend
