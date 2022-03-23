# Makefile - P3 File Systems
# Sean McAuliffe, V00913346
# April 1, 2022

# Compiler flags
CFLAGS = -Wall -std=c99

# Executable name
BIN = diskinfo

# Modules
OBJS = 

# External libraries
LIBS = -lreadline -lpthread

# Defines
DEFS = -D_DEFAULT_SOURCE

.PHONY all:
all: $(BIN)

# This default rule compiles the executable program
$(BIN): $(OBJS) $(BIN).c
	$(CC) $(CFLAGS) $(DEFS) $(OBJS) $(BIN).c $(LIBS) -o $(BIN)

# Compile all modules
%.o: %.c %.h
	$(CC) -c $(CFLAGS) $(DEFS) $< -o $@

.PHONY clean:
clean:
	rm -f *.out *.o *.exe $(BIN)
