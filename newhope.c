#include <stdio.h>
#include "poly.h"
#include "randombytes.h"
#include "error_correction.h"
#include "fips202.h"

static void printbytes(unsigned char *x, unsigned int len)
{
  unsigned int i;
  printf("{");
  for(i=0;i<len;i++) 
  {
    printf("0x%02x", x[i]);
    if(i!=len - 1) 
      printf(", ");
    else
      printf("}");
    if(!((i+1)%16))printf("\n ");
  }
  printf("\n");
}

static void encode_a(unsigned char *r, const poly *pk, const unsigned char *seed)
{
  int i;
  poly_tobytes(r, pk);

  for(i=0;i<NEWHOPE_SEEDBYTES;i++)
    r[POLY_BYTES+i] = seed[i];
}

static void decode_a(poly *pk, unsigned char *seed, const unsigned char *r)
{
  int i;
  poly_frombytes(pk, r);
  for(i=0;i<NEWHOPE_SEEDBYTES;i++)
    seed[i] = r[POLY_BYTES+i];
}


static void gen_a(poly *a, const unsigned char *seed)
{
    poly_uniform(a,seed);
}


// API FUNCTIONS 

void newhope_keygen(unsigned char *send, unsigned char *sk)
{
  poly a, e, s, r, pk;
  unsigned char seed[NEWHOPE_SEEDBYTES];
  unsigned char noiseseed[32];


  randombytes(seed, NEWHOPE_SEEDBYTES);

  // TESTVECTORS
  printf("randombytes(32) output:\n");
  printbytes(seed, 32);
  /*
  for(i=0;i<16;i++)
    printf("0x%02x,",seed[i]);
  printf("\n ");
  for(i=16;i<31;i++)
    printf("0x%02x,",seed[i]);
  printf("0x%02x}\n",seed[i]);
  */

  shake256_256(seed, seed, NEWHOPE_SEEDBYTES);
  // TESTVECTORS
  printf("seed = H(randombytes(32)) output:\n");
  printbytes(seed, 32);
  /*
  for(i=0;i<16;i++)
    printf("0x%02x,",seed[i]);
  printf("\n ");
  for(i=16;i<31;i++)
    printf("0x%02x,",seed[i]);
  printf("0x%02x}\n",seed[i]);
  */
  
  printf("============= NEWHOPE_KEYGEN =============\n\n");

  randombytes(noiseseed, 32);

  gen_a(&a, seed);
  // TESTVECTORS
  printf("â = gen_a(seed):\n");
  poly_print(&a);
  printf("\n");

  printf("4096 random bytes used in s = poly_getnoise():\n");
  poly_getnoise(&s,noiseseed,0);
  // TESTVECTORS
  printf("s = poly_getnoise():\n");
  poly_print(&s);
  printf("\n");
 
  printf("4096 random bytes used in e = poly_getnoise():\n");
  poly_getnoise(&e,noiseseed,1);
  // TESTVECTORS
  printf("e = poly_getnoise():\n");
  poly_print(&e);
  printf("\n");

  poly_ntt(&s);
  // TESTVECTORS
  printf("ŝ = ntt(s):\n");
  poly_print(&s);
  printf("\n");
  
  poly_ntt(&e);
  // TESTVECTORS
  printf("ê = ntt(e):\n");
  poly_print(&e);
  printf("\n");
 
  poly_pointwise(&r,&s,&a);
  // TESTVECTORS
  printf("pointwise(â, ŝ):\n");
  poly_print(&r);
  printf("\n");

  poly_add(&pk,&e,&r);
  // TESTVECTORS
  printf("b̂ = pointwise(â, ŝ) + ê:\n");
  poly_print(&pk);
  printf("\n");
  
  poly_tobytes(sk, &s);
  // TESTVECTORS
  printf("sp = tobytes(ŝ):\n");
  printbytes(sk, POLY_BYTES);
  /*
  printf("{");
  for(i=0;i<POLY_BYTES;i++) 
  {
    printf("0x%02x", sk[i]);
    if(i!=POLY_BYTES - 1) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */

  encode_a(send, &pk, seed);
  // TESTVECTORS
  printf("bp = tobytes(b̂):\n");
  printbytes(send, POLY_BYTES);
  
  // TESTVECTORS
  printf("NEWHOPE_MSGA:\n");
  printbytes(send, NEWHOPE_SENDABYTES);
  /*
  printf("{");
  for(i=0;i<NEWHOPE_SENDABYTES;i++) 
  {
    printf("0x%02x", send[i]);
    if(i!=NEWHOPE_SENDABYTES - 1) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */
}


void newhope_sharedb(unsigned char *sharedkey, unsigned char *send, const unsigned char *received)
{
  poly sp, ep, v, a, pka, epp, u;
  unsigned char seed[NEWHOPE_SEEDBYTES];
  unsigned char noiseseed[32];
  
  printf("============= NEWHOPE_SHAREDB =============\n\n");
  
  randombytes(noiseseed, 32);

  decode_a(&pka, seed, received);
  gen_a(&a, seed);

  printf("4096 random bytes used in s' = poly_getnoise():\n");
  poly_getnoise(&sp,noiseseed,0);
  // TESTVECTORS
  printf("s' = poly_getnoise():\n");
  poly_print(&sp);
  printf("\n");

  printf("4096 random bytes used in e' = poly_getnoise():\n");
  poly_getnoise(&ep,noiseseed,1);
  // TESTVECTORS
  printf("e' = poly_getnoise():\n");
  poly_print(&ep);
  printf("\n");
  
  printf("4096 random bytes used in e\" = poly_getnoise():\n");
  poly_getnoise(&epp,noiseseed,2);
  // TESTVECTORS
  printf("e\" = poly_getnoise():\n");
  poly_print(&epp);
  printf("\n");

  printf("b̂ = poly_frombytes(bp):\n");
  printf("see above\n\n");
  printf("â = gen_a(seed):\n");
  printf("see above\n\n");

  poly_ntt(&sp);
  // TESTVECTORS
  printf("ŝ' = poly_ntt(s'):\n");
  poly_print(&sp);
  printf("\n");

  poly_ntt(&ep);
  // TESTVECTORS
  printf("ê' = poly_ntt(e'):\n");
  poly_print(&ep);
  printf("\n");

  poly_pointwise(&u, &a, &sp);
  // TESTVECTORS
  printf("poly_pointwise(â, ŝ'):\n");
  poly_print(&u);
  printf("\n");

  poly_add(&u, &u, &ep);
   // TESTVECTORS
  printf("û = poly_pointwise(â, ŝ') + ê':\n");
  poly_print(&u);
  printf("\n");
  
  poly_pointwise(&v, &pka, &sp);
  // TESTVECTORS
  printf("poly_pointwise(b̂, ŝ'):\n");
  poly_print(&v);
  printf("\n");

  poly_invntt(&v);
  // TESTVECTORS
  printf("poly_invntt(poly_pointwise(b̂, ŝ')):\n");
  poly_print(&v);
  printf("\n");

  poly_add(&v, &v, &epp);
  // TESTVECTORS
  printf("v = poly_invntt(poly_pointwise(b̂, ŝ')) + e\":\n");
  poly_print(&v);
  printf("\n");

  helprec(send+POLY_BYTES, &v, noiseseed, 3);
  // TESTVECTORS
  printf("r = helprec(v):\n");
  printbytes(send+POLY_BYTES,NEWHOPE_RECBYTES);
  /*
  printf("{");
  for(i=0;i<NEWHOPE_RECBYTES;i++) 
  {
    printf("0x%02x", send[POLY_BYTES+i]);
    if(i!=NEWHOPE_RECBYTES - 1) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */

  poly_tobytes(send,&u);
  // TESTVECTORS
  printf("up = poly_tobytes(u):\n");
  printbytes(send, POLY_BYTES);
  /*
  printf("{");
  for(i=0;i<POLY_BYTES;i++) 
  {
    printf("0x%02x", send[i]);
    if(i!=POLY_BYTES - 1) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */
  // TESTVECTORS
  printf("NEWHOPE_MSGB:\n");
  printbytes(send, NEWHOPE_SENDBBYTES);

  rec(sharedkey, &v, send+POLY_BYTES);
  // TESTVECTORS
  printf("k = rec(v, r):\n");
  printbytes(sharedkey, 32);
  /*
  printf("{");
  for(i=0;i<32;i++) 
  {
    printf("0x%02x", sharedkey[i]);
    if(i!=31) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */
}


void newhope_shareda(unsigned char *sharedkey, const unsigned char *sk, const unsigned char *received)
{
  poly v,u,s;
  
  printf("============= NEWHOPE_SHAREDA =============\n\n");

  poly_frombytes(&u, received);
  // TESTVECTORS
  printf("û = poly_frombytes(up):");
  printf("see above\n\n");

  poly_frombytes(&s, sk);
  // TESTVECTORS
  printf("ŝ = poly_frombytes(sp):");
  printf("see above\n\n");

  poly_pointwise(&v,&s,&u);
  // TESTVECTORS
  printf("poly_pointwise(û, ŝ):\n");
  poly_print(&v);
  printf("\n");

  poly_invntt(&v);
  // TESTVECTORS
  printf("v' = poly_invntt(poly_pointwise(û, ŝ)):\n");
  poly_print(&v);
  printf("\n");
 
  rec(sharedkey, &v, received+POLY_BYTES);
  // TESTVECTORS
  printf("k = rec(v', r):\n");
  printbytes(sharedkey, 32);
  /*
  printf("{");
  for(i=0;i<32;i++) 
  {
    printf("0x%02x", sharedkey[i]);
    if(i!=31) 
      printf(", ");
    else
      printf("}\n");
    if(!((i+1)%16))printf("\n ");
  }
  */
}
