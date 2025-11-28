BIN = ../../bin
SRC = cache.cpp
OUT = $(BIN)/cache

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
