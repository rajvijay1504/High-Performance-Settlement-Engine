#include <benchmark/benchmark.h>
#include "SettlementEngine.h" 
#include <vector>
#include <string>

// --- HELPER: DATA GENERATOR ---
// Generates N banks with net zero balance
std::vector<Bank> generateData(int N) {
    std::vector<Bank> banks(N);
    // RBI at 0
    banks[0] = {"RBI", 0}; 
    banks[0].optimizedModes.set(); // RBI has all modes
    
    // Generate random banks
    for(int i=1; i<N; ++i) {
        banks[i] = {"B" + std::to_string(i), 0};
        // Simple toggle: Even banks GPay (0), Odd banks Paytm (2)
        if(i % 2 == 0) banks[i].optimizedModes.set(0);
        else banks[i].optimizedModes.set(2);
    }
    return banks;
}

// --- BENCHMARK 1: OPTIMIZED SOLVER ---
static void BM_OptimizedSolver(benchmark::State& state) {
    // Setup (happens once before the loop)
    int N = state.range(0); // Reading input size (10, 100, 1000...)
    auto banks = generateData(N);

    // The Measurement Loop
    for (auto _ : state) {
        // We copy data to simulate fresh state every run
        auto banks_copy = banks; 
        optimizedSolver(N, banks_copy);
    }
    
    // Metrics
    state.SetComplexityN(state.range(0)); // Calculates O(N) vs O(N log N)
    state.SetItemsProcessed(state.iterations() * N);
}

// --- BENCHMARK 2: LEGACY SOLVER (SIMULATED) ---
// We simulate the O(N^2) load here for comparison
static void BM_LegacySimulation(benchmark::State& state) {
    int N = state.range(0);
    std::vector<long> dummy(N, 100);
    
    for (auto _ : state) {
        // Simulate O(N^2) work
        long sum = 0;
        for(int i=0; i<N; ++i) {
            for(int j=0; j<N; ++j) {
                sum += (dummy[i] - dummy[j]);
            }
        }
        benchmark::DoNotOptimize(sum); // Prevent compiler optimizing this away
    }
    state.SetComplexityN(state.range(0));
}

// Register functions and define ranges (10 to 5000 banks)
BENCHMARK(BM_OptimizedSolver)->RangeMultiplier(10)->Range(10, 5000)->Complexity();
BENCHMARK(BM_LegacySimulation)->RangeMultiplier(10)->Range(10, 5000)->Complexity();

BENCHMARK_MAIN();