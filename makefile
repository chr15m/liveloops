# Directories:
LIBS = `allegro-config --libs`
INCLUDES = `allegro-config --cflags`

# Compiler:
CC = gcc

# Compiler options: Compile only, speed optimizations, all warnings, warnings are errors
CCOPTS = -c -O2 -Wall $(INCLUDES) # -Werror

# Linker: gcc
LINK = gcc

# Targets:

liveloops :       liveloops.o funcs.o
		$(LINK) liveloops.o funcs.o -o liveloops $(LIBS)

liveloops.o :         liveloops.c
		$(CC) liveloops.c -o liveloops.o $(CCOPTS)

funcs.o :         funcs.c
		$(CC) funcs.c -o funcs.o $(CCOPTS)
