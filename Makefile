all: bin/irgdump

clean:
	rm -rf bin

bin/irgdump: irgdump.c |bin
	$(CC) -o $@ $<

bin:
	mkdir -p bin
