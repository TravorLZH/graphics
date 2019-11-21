CPPFLAGS:=$(shell pkg-config --cflags glfw3)
LDLIBS+=-lglfw -framework OpenGL

TARGET=hello-glfw
SRC=hello-glfw.c
OBJ=$(SRC:.c=.o)

all:	$(TARGET)

$(TARGET):	$(OBJ)

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)
