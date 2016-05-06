CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -O3 -fomit-frame-pointer -msse2avx -march=corei7-avx

HEADERS = crypto_stream_chacha20.h params.h poly.h randombytes.h ntt.h error_correction.h reduce.h
CFILES  = crypto_stream_chacha20.c poly.c ntt.c precomp.c error_correction.c newhope.c reduce.c fips202.c

test/testvectors_tor: $(HEADERS) $(CFILES) test/testvectors_tor.c randombytes.c
	$(CC) $(CFLAGS) $(CFILES) randombytes.c test/testvectors_tor.c -o $@

.PHONY: clean

clean:
	-rm test/testvectors_tor
