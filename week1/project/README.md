# CSoT'26 Low Latency Track — Week 1



## Build Instructions

Configure and build the project:

```bash
cmake -B build
cmake --build build -j
```

Run the replay engine:

```bash
./build/quant_runner data/tiny.csv
./build/quant_runner data/synthetic_small.csv
./build/quant_runner data/synthetic_large.csv
```

Generate and test the public dataset:

```bash
python3 data/gen.py --seed 42 --rows 50000 --out data/public.csv
./build/quant_runner data/public.csv
```

---

## Hardware

| Property    | Value                          |
| ----------- | ------------------------------ |
| Machine     | MacBook Pro (Apple Silicon M5) |
| CPU         | Apple M5 (ARM64)               |
| RAM         | 16 GB Unified Memory           |
| Host OS     | macOS Tahoe 26                 |
| VM Software | UTM                            |
| VM OS       | Ubuntu 26.04 LTS               |
| VM Kernel   | Linux 7.0.0-22-generic         |                      |


---

## Strategy Implementation

Implemented the reference strategy from `STRATEGY_SPEC.md`.

Per-symbol state consists of:

* 64-tick rolling mid-price window
* Current position
* Rolling sum of mid-prices
* Rolling sum of squared mid-prices

The original implementation recomputed mean and variance by scanning all 64 values every tick. This was later optimized using rolling statistics:

* Running sum
* Running sum of squares
* O(1) mean computation
* O(1) variance computation

This eliminated the O(64) work previously performed on every tick.

---

## Latency Results

### tiny.csv

| Metric       | Result   |
| ------------ | -------- |
| Median (p50) | <= 64 ns |
| p99          | <= 64 ns |
| p999         | <= 128 ns |

### synthetic_small.csv

| Metric       | Result   |
| ------------ | -------- |
| Median (p50) | <= 64 ns |
| p99          | <= 64 ns |
| p999         | <= 128 ns |

---
## perf stat Output (synthetic_small.csv)

Command:

```bash
perf stat ./build/quant_runner data/synthetic_small.csv
```

Output:

```text
count = 10000
p50  <= 64 ns
p90  <= 64 ns
p99  <= 64 ns
p999 <= 128 ns

Performance counter stats for './build/quant_runner data/synthetic_small.csv':

                 0      context-switches                 #      0.0 cs/sec
                 0      cpu-migrations                   #      0.0 migrations/sec
               448      page-faults                      #  95452.8 faults/sec
              4.69 msec task-clock                       #      0.5 CPUs utilized
   <not supported>      cpu-cycles                       #      nan GHz

       0.005065459 seconds time elapsed

       0.005106000 seconds user
       0.000000000 seconds sys
```


---

## One Thing That Surprised Me


I was surprised by how small the latency numbers were. Before starting the project, a latency of a few hundred nanoseconds did not mean much to me. After implementing the strategy and measuring its performance on millions of ticks, seeing a median latency below 64 ns made the scale of low-latency systems much more concrete. It changed my intuition for what "fast" actually means in performance-critical software.


