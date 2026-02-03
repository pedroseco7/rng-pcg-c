#ifndef STRUCT_H
#define STRUCT_H
#include "var.h"

typedef struct {
    u64 state;
    u64 inc;    
} prng_state;

extern prng_state s_prng_state;

#endif // STRUCT_H