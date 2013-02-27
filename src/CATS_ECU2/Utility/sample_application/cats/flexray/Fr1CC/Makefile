# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = fr1cc.c

OBJS = $(SRCS:.c=.o)

TARGET	= FlexRayTest

all : $(OBJS)
#	$(CC) $(CFLGAS) -lpthread -o $(TARGET) $(OBJS)
	$(CC) $(CFLGAS) $(OBJS) -lpthread  -o $(TARGET)

clean:
	rm -f $(TARGET) *.o *~ core .depend
