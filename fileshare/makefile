CC = gcc
SRC_DIR = src
INC_DIR = inc
OBJFILES := $(patsubst %.c,%.o,$(wildcard src/*.c))
CFLAGS = -D_FORTIFY_SOURCE=2 -g3 -Wall -Werror -march=native -Wshadow -Os -Winit-self -g -masm=intel -Wformat=2 -fsanitize=undefined  -fsanitize=address
INCFLAGS = -I $(INC_DIR)
TARGET = fileshare.out

all: $(TARGET)

$(TARGET): $(OBJFILES)
	@echo "building program with options: "
	$(CC) $(CFLAGS) $(INC_FLAGS) $(OBJFILES) -o $(TARGET)
	@echo "program built"

%.o: %.c
	@echo "compiling $<"
	$(CC) -c $< -o $@

clean:
	@echo "cleaning"
	rm -r $(TARGET) $(wildcard src/*.o)
	@echo "cleaned"
