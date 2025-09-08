BIN = ../../bin
SRC = admin_usuarios.cpp ../common/util.cpp ../common/conteo.cpp ../common/calcula_funcion.cpp
OUT = $(BIN)/admin_usuarios

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
