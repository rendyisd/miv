CC = gcc
EXEC = build/text-editor

SRC = src/main.c src/gap_buffer.c 
OBJ = $(SRC:src/%.c=build/%.o)
INC = -Iinclude/

build/%.o: src/%.c
	mkdir -p build/
	$(CC) $(INC) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) $(INC) -o $@ $^

clean:
	rm -rf build/
