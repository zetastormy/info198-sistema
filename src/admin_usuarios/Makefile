BIN = ../../bin
SRC = admin_usuarios.cpp
OUT = $(BIN)/admin_usuarios

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -o $(OUT)
