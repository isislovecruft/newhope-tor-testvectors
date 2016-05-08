
all: discard discard16x

CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -g -O3 -mavx2

discard: discard.c

discard16x: discard16x.c

.PHONY: clean

clean:
	-rm discard
	-rm discard16x
