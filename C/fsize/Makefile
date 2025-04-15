CC=clang
CFLAGS=-std=c11 -O -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow
CPPFLAGS=

OBJ=main.o

.PHONY: all
all: fsize

fsize: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o fsize