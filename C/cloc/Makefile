CC=clang
CFLAGS=-std=c11 -O -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined,signed-integer-overflow,unsigned-integer-overflow
CPPFLAGS=

OBJ=main.o

.PHONY: all
all: cloc

cloc: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.o cloc

PREFIX?=/opt/m4xdev
BINDIR=$(PREFIX)/bin

.PHONY: install
install: cloc
	install -d $(BINDIR)
	install cloc $(BINDIR)/ 

.PHONY: uninstall
uninstall: 
	rm -f $(BINDIR)/cloc