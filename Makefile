BIN = bin
SRC = src/main.cpp
OUT = $(BIN)/prog

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -o $(OUT)

clean:
	rm -rf $(BIN)
