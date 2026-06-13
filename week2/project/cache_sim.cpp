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
namespace geo {
inline constexpr std::uint64_t LINE_BITS = 6;   
inline constexpr std::uint64_t L1_SETS = 64;
inline constexpr std::uint64_t L2_SETS = 512;
inline constexpr std::uint64_t WAYS = 8;
inline constexpr std::uint64_t L1_MASK = L1_SETS - 1;   
inline constexpr std::uint64_t L2_MASK = L2_SETS - 1;   
}
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
void lruupdatel1(int set,int way){
    int j = 0;
    while (l1_lru[set][j] != way) j++;
    for (int i=j;i>0;i--) l1_lru[set][i] = l1_lru[set][i-1];
    l1_lru[set][0] = way;
}
void lruupdatel2(int set,int way){
    int j = 0;
    while (l2_lru[set][j] != way) j++;
    for (int i=j;i>0;i--) l2_lru[set][i] = l2_lru[set][i-1];
    l2_lru[set][0] = way;
}
int searchl1(uint64_t tag,int set){
    for (int i=0;i<8;i++){
        if (l1_valid[set][i] && l1_tag[set][i] == tag) return i;
    }
    return -1;
}
int searchl2(uint64_t tag,int set){
    for (int i=0;i<8;i++){
        if (l2_valid[set][i] && l2_tag[set][i] == tag) return i;
    }
    return -1;
}
int victiml2(int set){
    for (int i=0;i<8;i++){
        if (!l2_valid[set][i]) return i;
    }
    return l2_lru[set][7];
}
int victiml1(int set){
    for (int i = 0; i < 8; i++){
        if (!l1_valid[set][i]) return i;
    }
    return l1_lru[set][7];
}
public:
    void on_init() override {
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
                s.writes++;
            } 
            else {
                s.reads++;
            }
            uint64_t b = a.address >> LINE_BITS;
            int s1 = b & L1_MASK;
            uint64_t t1 = b >> LINE_BITS;
            int way = searchl1(t1,s1);
            //l1 hit
            if (way != -1){
                s.l1_hits++;
                lruupdatel1(s1,way);
                if (a.is_write) l1_dirty[s1][way] = true;
                continue;
            }
            //l1 miss
            s.l1_misses++;
            int s2 = b & L2_MASK;
            uint64_t t2 = b >> 9;
            int way2 = searchl2(t2,s2);
            //l2 hit
            if (way2 != -1){
                s.l2_hits++;
                lruupdatel2(s2,way2);
            }
            //l2 miss
            else {
                s.l2_misses++;
                int victim = victiml2(s2);
                if (l2_valid[s2][victim] && l2_dirty[s2][victim]) s.dirty_writebacks++;
                l2_tag[s2][victim] = t2;
                l2_valid[s2][victim] = true;
                l2_dirty[s2][victim] = false;
                lruupdatel2(s2, victim);
            }
            int victim = victiml1(s1);
            if (l1_valid[s1][victim] && l1_dirty[s1][victim]){
                uint64_t b_v = (l1_tag[s1][victim] << 6) | s1;
                int s2v = b_v & L2_MASK;
                uint64_t t2v = b_v >> 9;
                int way = searchl2(t2v,s2v);
                if (way!=-1) l2_dirty[s2v][way] = true;
                else{
                    int victim2 = victiml2(s2v);
                    if (l2_valid[s2v][victim2] && l2_dirty[s2v][victim2]) s.dirty_writebacks++;
                    l2_valid[s2v][victim2] = true;
                    l2_dirty[s2v][victim2] = true;
                    l2_tag[s2v][victim2] = t2v;
                    lruupdatel2(s2v,victim2);
                }
            }
            l1_tag[s1][victim] = t1;
            l1_valid[s1][victim] = true;
            if (a.is_write) l1_dirty[s1][victim] = true;
            else l1_dirty[s1][victim] = false;
            lruupdatel1(s1,victim);
        }

        return s;
    }
};

}  // namespace

extern "C" csot::CacheSim* create_cache_sim() {
    return new StubCacheSim();
}
