BIN = ../../bin
SRC = indice_invertido.cpp ../common/util.cpp
OUT = $(BIN)/indice_invertido

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
