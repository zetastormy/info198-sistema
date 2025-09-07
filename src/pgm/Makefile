BIN = ../../bin
SRC = pgm.cpp
OUT = $(BIN)/pgm

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -o $(OUT)
