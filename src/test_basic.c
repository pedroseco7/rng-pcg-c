#include <stdio.h>
#include "../include/funcs.h"

int main(){

    prng_seed_auto();

    for (int i = 0; i < 10; i++){
        printf("%f\n", prng_randf());
    }

    return 0;
}
