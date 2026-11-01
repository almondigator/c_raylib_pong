TARGET=main
SRC=main.c
CC=cc

CFLAGS = -std=c99 -Wall -O2

OS := MACOS

# MACOS
ifeq ($(OS), MACOS)
	INCLUDES = -I$(shell brew --prefix raylib)/include
	LDFLAGS = -L$(shell brew --prefix raylib)/lib -lraylib \
		  -framework OpenGL \
		  -framework Cocoa \
		  -framework IOKit 
else ifeq ($(OS), LINUX)
	INCLUDES = $(shell pkg-config --cflags raylib)
	LDFLAGS = $(shell pkg-config --libs raylib) 
endif

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LDFLAGS)
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)
