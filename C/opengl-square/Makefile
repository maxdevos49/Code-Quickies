CC=clang
PKGS=glfw3
CFLAGS=	-std=c11 						\
		-Wall							\
		-Wextra							\
		-Werror							\
		-pedantic 						\
		-Ivendor/glad/include			\
		$(shell pkg-config --cflags $(PKGS))
LIBS=$(shell pkg-config --static --libs $(PKGS))

RELEASE_CFLAGS=-O3
DEBUG_CFLAGS=-g -O0 -DDEBUG -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow

OBJ=obj/main.o
DEP=$(OBJ:.o=.d)
TARGET=bin/square

.PHONY: all
all: CFLAGS+=$(RELEASE_CFLAGS)
all: $(TARGET)

.phony: debug
debug: CFLAGS+=$(DEBUG_CFLAGS)
debug: $(TARGET)

$(TARGET): bin obj $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@ -MMD -MP

bin obj:
	mkdir -p $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf bin obj
