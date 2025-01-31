CC = gcc
EXEC = build/miv

SRC = src/main.c src/miv.c src/buffer.c src/file_io.c src/input.c
OBJ = $(SRC:src/%.c=build/%.o)
CFLAGS = -Wall -Wextra -Iinclude/

all: $(EXEC)

debug: CFLAGS += -g3 -O0
debug: $(EXEC)

build/%.o: src/%.c
	mkdir -p build/
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -rf build/
