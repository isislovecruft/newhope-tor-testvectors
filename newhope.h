#ifndef NEWHOPE_H
#define NEWHOPE_H

#include "randombytes.h"
#include "crypto_stream_chacha20.h"
#include "error_correction.h"
#include <math.h>
#include <stdio.h>

void newhope_keygen(unsigned char *send, unsigned char *sk);
void newhope_sharedb(unsigned char *sharedkey, unsigned char *send, const unsigned char *received);
void newhope_shareda(unsigned char *sharedkey, const unsigned char *ska, const unsigned char *received);

#endif
