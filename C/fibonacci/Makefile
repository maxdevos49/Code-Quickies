CC=clang
CFLAGS=-g

OBJ=main.o

.PHONY: all
all: fibonacci

fibonacci: $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o fibonacci


		