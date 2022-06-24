OUT = bf
CC = audr32-cwcc
CFLAGS = -o $(OUT)
VM = ~/projects/audr32/vm/bin/vm
VMFLAGS = -rom 

.PHONY: clean

all: $(OUT)

$(OUT): bf.c
	$(CC) $(CFLAGS) $^

run: $(OUT)
	$(VM) $(VMFLAGS) $(OUT)

clean:
	rm $(OUT)
