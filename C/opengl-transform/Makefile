CC=clang
PKGS=glfw3
LIBS=
CFLAGS=\
	-std=c11				\
	-Wall					\
	-Wextra					\
	-pedantic				\
	-I/opt/homebrew/Cellar/cglm/0.9.6/include\
	-Ivendor/glad/include

TARGET=bin/transform
OBJ=\
	obj/main.o

#
# Configure above
# ==============================================================================

CFLAGS+=$(shell pkg-config --cflags $(PKGS))
LIBS+=$(shell pkg-config --static --libs $(PKGS))
DEP=$(OBJ:.o=.d)

.PHONY: release
release: CFLAGS+= -O3 -Werror
release: $(TARGET)

.PHONY: debug
debug: CFLAGS+= -g -O0 -DDEBUG -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow
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
