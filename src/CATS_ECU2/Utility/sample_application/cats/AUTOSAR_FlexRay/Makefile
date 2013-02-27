# サンプルアプリのMAKEFILE
# ビルド		: 	make
# ビルドクリーン	: 	make clean

SRCS = FlexRayTest.c /usr/src/CATS_ECU2/Autosar/FlexRay/FlexRayInterface/FrIf.c

#OBJS = $(SRCS:.c=.o) ../api_library/cats_api.o
OBJS = $(SRCS:.c=.o)

TARGET	= FlexRayTest

all : $(OBJS)
	#$(CC) $(CFLGAS) -lpthread -o $(TARGET) $(OBJS)
	$(CC) $(CFLGAS) $(OBJS) -L /usr/src/CATS_ECU2/api_library/flexrayapi/ -l flexray_api -o $(TARGET)
	#cp -f $(SRCS) ../cf_files

clean:
	rm -f $(TARGET) *.o *‾ core .depend
