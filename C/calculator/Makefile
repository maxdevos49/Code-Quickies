CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Werror -pedantic

RELEASE_CFLAGS=-O3
DEBUG_CFLAGS=-g -O0 -DDEBUG -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow

TARGET=bin/calc
OBJ=\
	obj/main.o\
	# obj/lexer.o

DEP=$(OBJ:.o=.d)

.PHONY: all
all: CFLAGS+=$(RELEASE_CFLAGS)
all: $(TARGET)

.PHONY: debug
debug: CFLAGS+=$(DEBUG_CFLAGS)
debug: $(TARGET)

$(TARGET): bin obj $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@ -MMD -MP

-include $(DEP)

bin obj:
	mkdir -p $@

.PHONY:
clean:
	rm -rf bin obj

