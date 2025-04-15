CC=clang
CFLAGS=-std=c11 -O -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow
CPPFLAGS=

TARGET=bin/hashtable
OBJ=obj/main.o

.PHONY: all
all: $(TARGET)

$(TARGET): bin obj $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

bin obj:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf bin obj

# PREFIX?=/opt/m4xdev
# LIBDIR=$(PREFIX)/lib

# .PHONY: install
# install: $(TARGET)
# 	install -d $(LIBDIR)
# 	install $(TARGET) $(BINDIR)/

# .PHONY: uninstall
# uninstall:
# 	rm -f $(BINDIR)/$(TARGET)
