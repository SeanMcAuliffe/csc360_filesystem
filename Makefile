# Makefile - P3 File Systems
# Sean McAuliffe, V00913346
# April 1, 2022

# Compiler
CC = cc

# Compiler flags
CFLAGS = -Wall -std=gnu99

# Executable name
BIN = diskinfo disklist diskget

# External libraries
LIBS = 

# Defines
DEFS =

.PHONY all:
all: diskinfo disklist diskget

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) $(DEFS) diskinfo.c $(LIBS) -o diskinfo

disklist: disklist.o
	$(CC) $(CFLAGS) $(DEFS) disklist.c $(LIBS) -o disklist

diskget: diskget.o
	$(CC) $(CFLAGS) $(DEFS) diskget.c $(LIBS) -o diskget

# Compile all modules
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $(DEFS) $< -o $@

.PHONY clean:
clean:
	rm -f *.out *.o *.exe $(BIN)
