CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-pthread
OBJ=cruzamento.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

cruzamento: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) cruzamento
