TARGET=main
SRC=main.c
CC=cc

CFLAGS = -std=c99 -Wall -O2

# MACOS
INCLUDES = -I$(shell brew --prefix raylib)/include
LDFLAGS = -L$(shell brew --prefix raylib)/lib -lraylib \
          -framework OpenGL \
          -framework Cocoa \
          -framework IOKit 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
