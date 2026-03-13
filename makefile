
BIN=bin
OBJS=../midi_writer_cpp/lib/libmidiwriter.so
DIRS=$(BIN)
APPS=$(BIN)/dnamusic
CPL=g++

# Common flags for all modes
CFLAGS=-ffast-math -Wwrite-strings -fextended-identifiers -std=c++17

# Default CFLAGS - release mode
CFLAGS+=-O3

# Debug CFLAGS - allows gdb, valgrind
# CFLAGS+=-g

# For gprof
# example command line:
# gprof bin/aromadock gmon.out > aromadock.output
# CFLAGS+=-g -pg

# For code coverage instrumentation, switch to these CFLAGS (slower performance):
# CFLAGS+=-g -fprofile-arcs -ftest-coverage


all: $(DIRS) $(APPS)


# Dirs

$(BIN):
	if [ ! -f $(BIN) ]; then mkdir -p $(BIN); fi


# Apps

$(BIN)/dnamusic: src/dnamusic.cpp $(OBJS)
	$(CPL) src/dnamusic.cpp $(OBJS) -o $(BIN)/dnamusic $(CFLAGS)

