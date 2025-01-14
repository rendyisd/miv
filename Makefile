CC = gcc
EXEC = build/text-editor

SRC = src/main.c src/gap_buffer.c 
OBJ = $(SRC:src/%.c=build/%.o)
CFLAGS = -Wall -Wextra -Iinclude/
LDFLAGS = -lncurses

build/%.o: src/%.c
	mkdir -p build/
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

clean:
	rm -rf build/
