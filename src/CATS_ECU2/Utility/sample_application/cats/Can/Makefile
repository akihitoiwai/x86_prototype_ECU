#-----------------------------------------------
#
#	CAN sample application Makefile
#
#-----------------------------------------------
TARGET	= can_app
PSOURCES = main.cpp

INCDIR = /usr/src/CATS_ECU2/api_library/canapi/
LIBDIR = /usr/src/CATS_ECU2/api_library/canapi/
LIBNAME = can_api

CFLAGS	= -DDEBUG -g
#CFLAGS = -DNDEBUG

#OBJECTS = $(PSOURCES:.cpp=.o)

all:
	$(CC) $(CFLAGS) main.cpp -I $(INCDIR) -L $(LIBDIR) -l $(LIBNAME) \
	 -l stdc++ -lpthread -o can_app
# 注意：main.cppはスタンダードライブラリを使用するため -l stdcc++が必要
#	make $(TARGET)

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)


