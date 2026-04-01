CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
INC_DIR = external
BIN = ObjPicoCAD2Import

C_JSON_SRC = $(INC_DIR)/cJSON.c
C_JSON_OBJ = $(INC_DIR)/cJSON.o

PICO_OBJ = pico.o pico_image.o

all: $(BIN)
$(BIN): main.o obj.o $(PICO_OBJ) $(C_JSON_OBJ)
	$(CC) $^ -limagequant -lm -o $@

main.o: main.c
	$(CC) -g $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

pico.o: pico.c
	$(CC) -g $(CFLAGS) -c pico.c -o $@
pico_image.o: pico_image.c
	$(CC) -g -I$(INC_DIR) -c pico_image.c -o $@

obj.o: obj.c
	$(CC) -I$(INC_DIR) $(CFLAGS) -c $^ -o $@

$(C_JSON_OBJ): $(C_JSON_SRC)
	$(CC) -I$(INC_DIR) -c $^ -o $@

clean:
	rm -f $(BIN) *.o $(C_JSON_OBJ)
