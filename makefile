CC = gcc
CFLAGS = -ansi -Wall -pedantic
SRC = Source
HDR = Headers
OBJ = obj

assembler: $(OBJ)/main.o $(OBJ)/list-data-structure.o
	$(CC) $(CFLAGS) $^ -o assembler
$(OBJ)/main.o: $(SRC)/main.c $(HDR)/list-data-structure.h
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o $(OBJ)/main.o
$(OBJ)/list-data-structure.o: $(SRC)/list-data-structure.c $(HDR)/list-data-structure.h
	$(CC) $(CFLAGS) -c $(SRC)/list-data-structure.c -o $(OBJ)/list-data-structure.o

clean:
	rm -rf $(OBJ) assembler