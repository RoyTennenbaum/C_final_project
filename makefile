CC = gcc
CFLAGS = -ansi -Wall -pedantic -I$(HDR)

SRC = Source
HDR = Headers
OBJ = Temp

TARGET = assembler

# All source files
SRCS := $(wildcard $(SRC)/*.c)

# Corresponding object files in Temp/
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

# Default target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Create Temp/ directory if it doesn't exist
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure Temp/ exists
$(OBJ):
	mkdir -p $(OBJ)

# Clean target
.PHONY: clean
clean:
	rm -f $(OBJ)/*.o $(TARGET)