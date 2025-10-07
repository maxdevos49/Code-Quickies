CC=clang
CFLAGS=\
	-std=c11					\
	-Wall						\
	-Wextra						\
	-Werror						\
	-pedantic
LIBS=

TARGET=bin/voxel-terrain
OBJ=\
	obj/main.o\

#
# Configure above
# ==============================================================================

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

DEP=$(OBJ:.o=.d)
-include $(DEP)

.PHONY: clean
clean:
	rm -rf bin obj


