CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
INC_DIR = external
BIN = ObjPicoCAD2Import

C_JSON_SRC = $(INC_DIR)/cJSON.c
C_JSON_OBJ = $(INC_DIR)/cJSON.o

all: $(BIN)
$(BIN): main.o obj.o pico.o $(C_JSON_OBJ)
	$(CC) $^ -o $@

main.o: main.c
	$(CC) -g $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

pico.o: pico.c
	$(CC) -g -c $(CFLAGS) $^ -o $@

obj.o: obj.c
	$(CC) -I$(INC_DIR) $(CFLAGS) -c $^ -o $@

$(C_JSON_OBJ): $(C_JSON_SRC)
	$(CC) -I$(INC_DIR) -c $^ -o $@

clean:
	rm $(BIN) *.o $(C_JSON_OBJ)
