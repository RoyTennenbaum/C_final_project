CC = gcc
CFLAGS = -std=c90 -Wall -pedantic -Wextra -fno-common
SRC = Source
HDR = Headers
OBJ = /tmp/obj_$(USER)

# All object files - REMOVED first-iteration.o, helper-functions.o, and memory-handle.o
OBJS = $(OBJ)/main.o $(OBJ)/dynamic-tables.o $(OBJ)/errors.o \
       $(OBJ)/pre-assembler.o $(OBJ)/second-iteration.o $(OBJ)/static-tables.o

# Main target
assembler: $(OBJ) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o assembler

# Create obj directory
$(OBJ):
	mkdir -p $(OBJ)

# Object file rules
$(OBJ)/main.o: $(SRC)/main.c $(HDR)/global.h $(HDR)/pre-assembler.h $(HDR)/dynamic-tables.h $(HDR)/static-tables.h
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o $(OBJ)/main.o

$(OBJ)/dynamic-tables.o: $(SRC)/dynamic-tables.c $(HDR)/dynamic-tables.h $(HDR)/global.h
	$(CC) $(CFLAGS) -c $(SRC)/dynamic-tables.c -o $(OBJ)/dynamic-tables.o

$(OBJ)/errors.o: $(SRC)/errors.c $(HDR)/errors.h $(HDR)/global.h
	$(CC) $(CFLAGS) -c $(SRC)/errors.c -o $(OBJ)/errors.o

$(OBJ)/first-iteration.o: $(SRC)/first-iteration.c $(HDR)/first-iteration.h $(HDR)/global.h $(HDR)/dynamic-tables.h $(HDR)/static-tables.h $(HDR)/word-types.h
	$(CC) $(CFLAGS) -c $(SRC)/first-iteration.c -o $(OBJ)/first-iteration.o

$(OBJ)/pre-assembler.o: $(SRC)/pre-assembler.c $(HDR)/pre-assembler.h $(HDR)/global.h $(HDR)/dynamic-tables.h $(HDR)/static-tables.h
	$(CC) $(CFLAGS) -c $(SRC)/pre-assembler.c -o $(OBJ)/pre-assembler.o

$(OBJ)/second-iteration.o: $(SRC)/second-iteration.c $(HDR)/second-iteration.h $(HDR)/global.h $(HDR)/dynamic-tables.h $(HDR)/static-tables.h $(HDR)/word-types.h
	$(CC) $(CFLAGS) -c $(SRC)/second-iteration.c -o $(OBJ)/second-iteration.o

$(OBJ)/static-tables.o: $(SRC)/static-tables.c $(HDR)/static-tables.h $(HDR)/global.h
	$(CC) $(CFLAGS) -c $(SRC)/static-tables.c -o $(OBJ)/static-tables.o

# Clean target
clean:
	rm -rf $(OBJ) assembler

# Utility targets
.PHONY: clean all

all: assembler