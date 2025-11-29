BIN = ../../bin
SRC = cache_service.cpp
OUT = $(BIN)/cache_service

all: $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC) | $(BIN)
	g++ $(SRC) -I../include -o $(OUT)
