# 
# Makefile for project 1 
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS = -Wall -Wextra -std=gnu99 -g


## LIB = Library to link.
## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

LIB = 
HDR = memory.h list.h
SRC = $(HDR:.h=.c) main.c
OBJ = $(SRC:.c=.o)
EXE = swap

## Top level target is executable.
$(EXE):	$(OBJ) 
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies

$(OBJ): $(HDR)