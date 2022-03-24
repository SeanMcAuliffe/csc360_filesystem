# Makefile - P3 File Systems
# Sean McAuliffe, V00913346
# April 1, 2022

# Compiler
CC = cc

# Compiler flags
CFLAGS = -Wall -std=c99

# Executable name
BIN = diskinfo disklist

# External libraries
LIBS = 

# Defines
DEFS =

.PHONY all:
all: diskinfo disklist

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) $(DEFS) diskinfo.c $(LIBS) -o diskinfo

disklist: disklist.o
	$(CC) $(CFLAGS) $(DEFS) disklist.c $(LIBS) -o disklist

# Compile all modules
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $(DEFS) $< -o $@

.PHONY clean:
clean:
	rm -f *.out *.o *.exe $(BIN)
