CC = gcc
FLAGS = -std=gnu99 -O2 -Wall -Wextra
EXE = image_processing

.PHONY: build run clean

build: $(EXE)
$(EXE): main.o bmplib.o stack.o
	$(CC) main.o bmplib.o stack.o -o image_processing $(FLAGS)

main.o: main.c bmplib.h
	$(CC) main.c -c -o main.o $(FLAGS)

bmplib.o: bmplib.c bmplib.h bmpheaders.h stack.h
	$(CC) bmplib.c -c -o bmplib.o $(FLAGS)

stack.o: stack.c stack.h
	$(CC) stack.c -c -o stack.o $(FLAGS)

run: $(EXE)
	./$(EXE)

clean:
	rm -r $(EXE) main.o bmplib.o stack.o
