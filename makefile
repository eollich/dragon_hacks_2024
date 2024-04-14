CC=clang++
SRC=src
OBJ=obj
BIN=bin
INCLUDE=include
LIB=lib
CCFLAGS=-I$(INCLUDE) -std=c++17 $(shell pkg-config --cflags libmongocxx)
LDFLAGS=$(shell pkg-config --libs libmongocxx)


SOURCES= mongo populate_restaurants
OBJECTS=$(foreach source,$(SOURCES),$(OBJ)/$(source).o)

all: main


main: $(OBJECTS) $(OBJ)/main.o
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cc
	[ -d $(OBJ) ] || mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) $^ -c -o $@

clean:
	rm -rf obj
	rm -rf bin
