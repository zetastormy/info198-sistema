BIN = bin

$(BIN):
	mkdir -p $(BIN)

all: $(BIN)
	g++ src/main.cpp -o $(BIN)/prog

clean:
	rm -rf $(BIN)
