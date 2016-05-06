
#include "../newhope.h"
#include "../poly.h"
#include "../randombytes.h"
#include "../crypto_stream_chacha20.h"
#include "../error_correction.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

int main()
{
  unsigned char key_a[32], key_b[32];
  unsigned char senda[NEWHOPE_SENDABYTES];
  unsigned char sendb[NEWHOPE_SENDBBYTES];
  unsigned char sk_a[POLY_BYTES];

  //Alice generates a public key
  newhope_keygen(senda, sk_a);

  //Bob derives a secret key and creates a response
  newhope_sharedb(key_b, sendb, senda);
  
  //Alice uses Bobs response to get her secre key
  newhope_shareda(key_a, sk_a, sendb);

  if(memcmp(key_a, key_b, 32))
    printf("ERROR keys\n");

  return 0;
}
