BIN = ../../bin
SRC = multi.cpp
OUT = $(BIN)/multi

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -o $(OUT)
