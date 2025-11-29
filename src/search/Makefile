BIN = ../../bin
SRC = search.cpp ../common/util.cpp
OUT = $(BIN)/search

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
