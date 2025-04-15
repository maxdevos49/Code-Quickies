CC=clang
CFLAGS=-std=c11 -O -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined,signed-integer-overflow
CPPFLAGS=


OBJ=main.o

.PHONY: all
all: permutation

permutation: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: all
clean:
	rm -rf *.o permutation