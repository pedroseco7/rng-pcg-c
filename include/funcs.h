#ifndef FUNCS_H
#define FUNCS_H
#include "structs.h"
#include "var.h"

void prng_seed_r(prng_state* rng, u64 initstate, u64 initseq);
void prng_seed(u64 initstate, u64 initseq); //global version

u32 prng_rand_r(prng_state* rng);
u32 prng_rand(void); //global version

#endif // FUNCS_H