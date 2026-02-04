#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../include/funcs.h"
#include "../include/var.h"

#define NUM_ITERATIONS 10 
#define NUM_WARMUP 2       

double get_time_secs() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double benchmark_prng(size_t num_samples) {
    volatile u32 dummy_sum = 0;
    double start = get_time_secs();
    
    for (size_t i = 0; i < num_samples; i++) {
        dummy_sum += prng_rand();
    }
    
    double end = get_time_secs();
    return end - start;
}

double benchmark_rand(size_t num_samples) {
    volatile u32 dummy_sum = 0;
    double start = get_time_secs();
    
    for (size_t i = 0; i < num_samples; i++) {
        dummy_sum += rand();
    }
    
    double end = get_time_secs();
    return end - start;
}

double calculate_mean(double *times, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += times[i];
    }
    return sum / count;
}

double calculate_stddev(double *times, int count, double mean) {
    double sum_sq = 0.0;
    for (int i = 0; i < count; i++) {
        double diff = times[i] - mean;
        sum_sq += diff * diff;
    }
    return sqrt(sum_sq / count);
}

int main() {
    size_t sample_sizes[] = {
        1000000,      
        10000000,     
        100000000,    
        500000000     
    };
    int num_sizes = sizeof(sample_sizes) / sizeof(sample_sizes[0]);
    
    prng_seed_auto();
    srand(time(NULL));
    
    printf("=== Benchmark: PCG vs LIBC rand() ===\n");
    printf("Iterações por tamanho: %d (+ %d warmup)\n", NUM_ITERATIONS, NUM_WARMUP);
    printf("%-15s %-20s %-20s %-15s %-15s\n", 
           "Amostras", "Algoritmo", "Tempo Médio (s)", "Milhões/seg", "Desvio Padrão");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int s = 0; s < num_sizes; s++) {
        size_t samples = sample_sizes[s];
        double times_prng[NUM_ITERATIONS];
        double times_rand[NUM_ITERATIONS];
        
        // Warmup + Benchmark PRNG
        for (int i = 0; i < NUM_WARMUP; i++) {
            benchmark_prng(samples);
        }
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            times_prng[i] = benchmark_prng(samples);
        }
        
        // Warmup + Benchmark RAND
        for (int i = 0; i < NUM_WARMUP; i++) {
            benchmark_rand(samples);
        }
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            times_rand[i] = benchmark_rand(samples);
        }
        
        // Calcular estatísticas para LIBC rand()
        double mean_rand = calculate_mean(times_rand, NUM_ITERATIONS);
        double stddev_rand = calculate_stddev(times_rand, NUM_ITERATIONS, mean_rand);
        double throughput_rand = (samples / mean_rand) / 1000000.0;
        
        // Calcular estatísticas para PCG prng_rand()
        double mean_prng = calculate_mean(times_prng, NUM_ITERATIONS);
        double stddev_prng = calculate_stddev(times_prng, NUM_ITERATIONS, mean_prng);
        double throughput_prng = (samples / mean_prng) / 1000000.0;
        
        // Imprimir resultados
        printf("%-15zu %-20s %-20.4f %-15.2f %-15.4f\n", 
               samples, "LIBC rand()", mean_rand, throughput_rand, stddev_rand);
        printf("%-15zu %-20s %-20.4f %-15.2f %-15.4f\n", 
               samples, "PCG prng_rand()", mean_prng, throughput_prng, stddev_prng);
        printf("%-15s %-20s %-20.2fx\n\n", 
               "", "Speedup PCG", mean_rand / mean_prng);
    }
    
    return 0;
}