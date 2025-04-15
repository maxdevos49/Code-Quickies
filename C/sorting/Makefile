CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic
RELEASE_CFLAGS=-O3 -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow
DEBUG_CFLAGS=-g -O0 -DDEBUG -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow

OBJ=obj/main.o
TARGET=bin/sorting

.PHONY: all
all: CFLAGS+=$(RELEASE_CFLAGS)
all: $(TARGET)

.PHONY: debug
debug: CFLAGS+=$(DEBUG_CFLAGS)
debug: $(TARGET)

$(TARGET): bin obj $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

bin obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf bin obj