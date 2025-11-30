BIN = ../../../bin
SRC = search_engine.cpp
OUT = $(BIN)/search_engine

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
