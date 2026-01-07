# High-Performance Settlement Engine (C++)

This project is a high-performance payment settlement engine I built to optimize how inter-bank debts are reconciled.

The main goal was to solve the scaling issues inherent in naive settlement algorithms. I started with a standard solution that ran in quadratic time, which struggled with large datasets, and refactored it into a linear-time system using **Max-Heaps** and **Bitsets**. The final version handles 5,000+ banks with sub-millisecond latency.

## The Challenge

In a network of N banks, checking every bank against every other bank to find payment matches results in $O(N^2)$ complexity.
* For 10 banks, this is negligible.
* For 5,000 banks, the processing time spikes to **57.7ms**, which is unacceptable for high-frequency financial systems.

### Example Scenario
To visualize the complexity, consider a simple cluster of 6 banks (RBI, AXIS, SBI, ICICI, Bank of Baroda, HDFC) with specific payment modes (Google Pay, BharatPe, Paytm).

**Initial Debt Network:**
A standard day of transactions creates a complex web of debts:

| Debtor Bank           | Creditor Bank          | Amount (Rs) |
|-----------------------|------------------------|-------------|
| HDFC Bank             | RBI                    | 100         |
| HDFC Bank             | AXIS Bank              | 300         |
| HDFC Bank             | SBI Bank               | 100         |
| HDFC Bank             | ICICI Bank             | 100         |
| Bank of Baroda        | RBI                    | 300         |
| Bank of Baroda        | SBI Bank               | 100         |
| RBI                   | AXIS Bank              | 400         |
| AXIS Bank             | SBI Bank               | 200         |
| SBI Bank              | ICICI Bank             | 500         |

These obligations form a directed graph where edges represent financial liabilities:

![Initial Graph](https://github.com/rajvijay1504/High-Performance-Settlement-Engine/assets/120421407/5725ff12-387a-4839-bb3a-9d5dc253d784)

**The Goal:**
The system must resolve these debts using the minimum number of transactions, respecting that each bank only supports specific payment modes (e.g., HDFC only supports Paytm, while RBI supports all).

**Optimized Result:**
After processing through the engine, the complex graph is simplified into minimized transactions:

![Optimized Result](https://github.com/rajvijay1504/High-Performance-Settlement-Engine/assets/120421407/b6f7ec1d-18d4-4c7c-8cb1-62e4ad28d5b6)

## My Approach

I decoupled the settlement logic into a standalone header (`SettlementEngine.h`) to allow for better testing and benchmarking.

1.  **Algorithmic Optimization**: Instead of nested loops, I used a Max-Heap (`priority_queue`) to greedily match the largest debtor with the largest creditor. This brought complexity down to **$O(N \log N)$**.
2.  **Payment Verification**: I replaced string-based mode checks with `std::bitset`. This allows for **$O(1)$** bitwise intersection to verify if two banks share a valid payment corridor.
3.  **Sharding Simulation**: I implemented a hashing function to simulate how banks would be partitioned across different nodes in a distributed system.

## Project Structure

* **`src/SettlementEngine.h`**: The core logic file. Contains the optimized solver and data structures.
* **`tests/perf_benchmark.cpp`**: The Google Benchmark harness used to measure Big-O complexity and CPU cycles.
* **`tests/real_tests.cpp`**: Unit tests using the **Google Test (GTest)** framework. It covers edge cases like circular debts and disconnected graphs.
* **`legacy/version1.cpp`**: My original $O(N^2)$ implementation. I kept this in the repo to serve as a baseline for performance comparisons.
* **`google_benchmark_results.txt`**: The raw output from the benchmarking tool proving the performance claims.

## Performance Results

I benchmarked both versions on an 8-Core Apple M-Series chip. The results demonstrate a clear shift from quadratic to linear scaling as the dataset size increases.

| Metric | Legacy System ($O(N^2)$) | Optimized Engine ($O(N \log N)$) | Improvement |
| :--- | :--- | :--- | :--- |
| **Complexity (Big-O)** | $2.31 N^2$ | **26.78 N** (Linear scaling) | **Algorithmic Shift** |
| **Latency (5,000 Banks)** | ~57.7 ms | **~0.13 ms** | **99.7% Reduction** |
| **Items per Second** | N/A | **37.4 Million** | **High Throughput** |
| **Speedup Factor** | 1x | **432x Faster** | **Orders of Magnitude** |

> *Source: Data verified via Google Benchmark; logs are available in `google_benchmark_results.txt`.*

I use **CMake** for the build system to handle the dependencies (GTest and Google Benchmark).

**Prerequisites:**
You need `cmake`, `googletest`, and `google-benchmark` installed. On Mac, you can use Homebrew:
`brew install cmake googletest google-benchmark`

**Compiling:**
```bash
mkdir build
cd build
cmake ..
make
```

**Running Tests:**
```bash
./unit_tests
```

**Running Benchmarks:**
```bash
./perf_test
```
