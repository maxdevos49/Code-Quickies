CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic $(pkg-config --cflags glfw3)

PKGS = glfw3
CFLAGS += $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --static --libs $(PKGS))

INCLUDES = -Ivendor/glad/include

RELEASE_CFLAGS = -O3
DEBUG_CFLAGS = -g -O0 -DDEBUG -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow

OBJ = 	obj/main.o\
		obj/readall.o\
		obj/shader.o
TARGET = bin/triangle

.PHONY: all
all: CFLAGS += $(RELEASE_CFLAGS) $(INCLUDES)
all: $(TARGET)

.PHONY: debug
debug: CFLAGS += $(DEBUG_CFLAGS) $(INCLUDES)
debug: $(TARGET)

$(TARGET): bin obj $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

bin obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf bin obj

# Check for pkg-config dependencies
.PHONY: check-libs
check-libs:
	@for pkg in $(PKGS); do \
		if ! pkg-config --exists $$pkg; then \
			echo "Error: Lib $$pkg not found. Please install it."; \
			exit 1; \
		fi; \
	done
