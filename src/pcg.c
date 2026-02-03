#include "../include/funcs.h"

prng_state s_prng_state = { 
    0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL 
};

//Based on pcg RNG (https://www.pcg-random.org/)
//Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

u32 prng_rand_r(prng_state* rng) {
    u64 oldstate = rng->state;
    
    //1. LCG
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;

    //2. Permutação (XSH-RR)
    u32 xorshifted = (u32)(((oldstate >> 18u) ^ oldstate) >> 27u);
    u32 rot = oldstate >> 59u;
    
    // Rotação bitwise para misturar ainda mais
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void prng_seed_r(prng_state* rng, u64 initstate, u64 initseq){
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    prng_rand_r(rng);
    rng->state += initstate;
    prng_rand_r(rng);
}

void prng_seed(u64 initstate, u64 initseq){
    prng_seed_r(&s_prng_state, initstate, initseq);
}

u32 prng_rand(void){
    return prng_rand_r(&s_prng_state);
}