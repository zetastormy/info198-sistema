BIN = ../../bin
SRC = pgm.cpp ../common/util.cpp ../common/conteo.cpp ../common/calcula_funcion.cpp ../common/palindrome.cpp
OUT = $(BIN)/pgm

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
