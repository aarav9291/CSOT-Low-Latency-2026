// ============================================================================
//  cache_sim.stub.cpp — STARTING POINT for the Week-2 cache-sim challenge.
//
//  Copy this to `cache_sim.cpp`, then make it correct, then make it fast:
//      cp samples/cache_sim.stub.cpp cache_sim.cpp
//      cmake -B build -DCSOT_CACHE_SIM_SRC=cache_sim.cpp && cmake --build build -j
//      ./build/cache_sim_runner data/tiny.trace      # compare to data/tiny.stats.json
//
//  This stub COMPILES and RUNS but is INTENTIONALLY NOT A CORRECT SIMULATOR.
//  It counts reads/writes and treats every access as an L1 miss + L2 miss so
//  you can see the harness work end-to-end. Your job is to implement the real
//  two-level hierarchy from CACHE_SPEC.md so the seven counters match the
//  reference exactly — and then to make run() as fast as you can.
//
//  Everything must live in this ONE translation unit. The judge builds exactly
//  this file against its own main(); no extra .cpp, no custom CMake.
// ============================================================================

#include "cache_sim.hpp"
#include <cstdint>

namespace {
class StubCacheSim final : public csot::CacheSim {
private:
    bool l1_valid[64][8];
    bool l1_dirty[64][8];
    uint64_t l1_tag[64][8];
    uint8_t l1_lru[64][8];
    bool l2_valid[512][8];
    bool l2_dirty[512][8];
    uint64_t l2_tag[512][8];
    uint8_t l2_lru[512][8];
public:
    void on_init() override {
        // Allocate ALL state you will ever need here (tag arrays, LRU bits,
        // dirty bits, …). After on_init() returns, run() must not touch the
        // heap. See 04-zero-allocation.md.
        //
        // TODO: size and zero your L1 (64 sets x 8 ways) and L2 (512 sets x
        //       8 ways) structures. Geometry constants are in CACHE_SPEC.md §3.
        for (int s = 0; s < 64; s++) {
            for (int w = 0; w < 8; w++) {
                l1_valid[s][w] = false;
                l1_dirty[s][w] = false;
                l1_tag[s][w] = 0;
                l1_lru[s][w] = w;
            }
        }
        for (int s = 0; s < 512; s++) {
            for (int w = 0; w < 8; w++) {
                l2_valid[s][w] = false;
                l2_dirty[s][w] = false;
                l2_tag[s][w] = 0;
                l2_lru[s][w] = w;
            }
        }
    }

    csot::CacheStats run(const csot::MemAccess* acc, std::size_t n) override {
        csot::CacheStats s{};

        for (std::size_t i = 0; i < n; ++i) {
            const csot::MemAccess& a = acc[i];

            if (a.is_write) {
                ++s.writes;
            } else {
                ++s.reads;
            }

            // TODO: replace this placeholder with the real CACHE_SPEC.md §5
            //       per-access algorithm:
            //         1. probe L1 (set = (addr >> 6) & 63, tag = addr >> 12)
            //         2. on L1 miss, probe L2 (set = (addr >> 6) & 511)
            //         3. write-allocate fills, true LRU per set
            //         4. write-back dirty victims; count dirty L2->memory
            //            evictions in dirty_writebacks
            //
            // The placeholder below is WRONG on purpose (it never hits).
            ++s.l1_misses;
            ++s.l2_misses;
            (void)a.address;
        }

        return s;
    }
};

}  // namespace

extern "C" csot::CacheSim* create_cache_sim() {
    return new StubCacheSim();
}
