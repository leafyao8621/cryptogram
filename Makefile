CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
SRC = $(wildcard src/app/*.c) $(wildcard src/parser/*.c) $(wildcard src/util/*.c) $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = cryptogram

%.o: %.c
	$(CC) $(CFLAGS) -O3 -c $< -o $@ -Iinclude

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) -lcontainers

.PHONY: clean valgrind
clean:
	@rm $(OBJ)
	@rm $(BIN)
valgrind: $(BIN)
	@valgrind -s --leak-check=full ./$(BIN)
