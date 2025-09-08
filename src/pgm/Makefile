BIN = ../../bin
SRC = pgm.cpp ../common/util.cpp
OUT = $(BIN)/pgm

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
