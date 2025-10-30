BIN = ../../bin
SRC = indice_invertido_paralelo.cpp ../common/util.cpp
OUT = $(BIN)/indice_invertido_paralelo
LIBRARIES = -pthread

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) $(BIN)
	g++ $(SRC) -I ../../include -o $(OUT) $(LIBRARIES)

clean:
	rm -f $(OUT)
