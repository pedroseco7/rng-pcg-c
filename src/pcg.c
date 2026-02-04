#include "../include/funcs.h"
#include <time.h> 
#include <stdio.h>   

//SO detection
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <bcrypt.h>
    #include <process.h> 
    #define IS_WINDOWS 1
#else
    #include <unistd.h>  
    #include <fcntl.h>   
    #define IS_WINDOWS 0
#endif

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

f32 prng_randf_r(prng_state* rng) {
    return (f32)prng_rand_r(rng) / (f32)UINT32_MAX;
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

f32 prng_randf(void){
    return prng_randf_r(&s_prng_state);
}

void prng_seed_auto(void){
    u64 seed_state = 0;
    u64 seed_seq = 0;
    int success = 0;

    #if IS_WINDOWS
    NTSTATUS status = BCryptGenRandom(
        NULL, 
        (PUCHAR)&seed_state, 
        sizeof(seed_state), 
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );
    
    NTSTATUS status2 = BCryptGenRandom(
        NULL, 
        (PUCHAR)&seed_seq, 
        sizeof(seed_seq), 
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );

    if (status == 0 && status2 == 0) {
        success = 1;
    }

    #else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) {
        ssize_t r1 = read(fd, &seed_state, sizeof(seed_state));
        ssize_t r2 = read(fd, &seed_seq, sizeof(seed_seq));
        if (r1 == sizeof(seed_state) && r2 == sizeof(seed_seq)) {
            success = 1;
        }
        close(fd);
    }
    #endif

    //fallback para tempo + PID se a leitura de entropia falhar
    if (!success) {
        u64 pid = 0;
        #if IS_WINDOWS
            pid = (u64)_getpid();
        #else
            pid = (u64)getpid();
        #endif

        seed_state = (u64)time(NULL) ^ pid;
        seed_seq = (u64)((uintptr_t)&seed_state); 
    }

    prng_seed(seed_state, seed_seq);

}
