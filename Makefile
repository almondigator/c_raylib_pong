TARGET=main
SRC=main.c
CC=cc

CFLAGS = -std=c99 -Wall -O2

UNAME := $(shell uname -s)

# MACOS
ifeq ($(UNAME), Darwin)
	INCLUDES = -I$(shell brew --prefix raylib)/include
	LDFLAGS = -L$(shell brew --prefix raylib)/lib -lraylib \
		  -framework OpenGL \
		  -framework Cocoa \
		  -framework IOKit 
else ifeq ($(UNAME), Linux)
	INCLUDES = $(shell pkg-config --cflags raylib)
	LDFLAGS = $(shell pkg-config --libs raylib) -lm
endif

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LDFLAGS)
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)
