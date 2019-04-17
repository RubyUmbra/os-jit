.PHONY: all clean

all: jit

jit: main.c
	cc -o $@ $^

clean:
	rm -rfv jit

