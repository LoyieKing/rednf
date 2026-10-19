// Differential oracle for the reconstructed buddy allocator
// (recon/src/commlib/comm_buddy_alloc.cpp).
//
// The ORIG libtss_sdk.so.1.0.394 is dlopen()ed, its BuddyAlloc /
// MultiBuddyAlloc methods are resolved by mangled name, and an ORIG instance is
// constructed inside a raw 0x1dc / 0x6c byte buffer (the reconstruction's
// sizeof assertions guard the layout).  The rebuilt implementation - linked
// into this binary - runs the identical deterministic sequence.
//
// The oracle exports ZEN_Trace_LogMsg::debug_errorex/debug_debugEx itself and
// is linked with -rdynamic, so the PLT calls of the ORIG library bind to the
// stubs in this file.  Every message of both sides is therefore captured and
// compared, which also covers print_info() output.
//
// Compared after every step:
//   * every scalar member of BuddyAlloc / MultiBuddyAlloc,
//   * the 32 free_classify_list_array_ chains (desc offsets + desc fields),
//   * the 32 classify_bitset_ bit arrays (byte length + raw bytes),
//   * the desc_obj_pool_ / additional_desc_obj_pool_ chains,
//   * ptr_desc_map_ / ptr_buddy_map_ / ptr_system_map_ contents,
//   * alloc() return values as offsets into the owning buffer,
//   * get_next_order_and_power() for every size in [0, buffer_len_ + 64],
//   * the behaviour once the descriptor pool runs dry (forked child).
//
// Build:
//   sh c5build.sh recon/src/commlib/comm_buddy_alloc.cpp recon/build/buddy/comm_buddy_alloc.o
//   $C5/usr/bin/g++ -m32 -O2 -fno-stack-protector -std=gnu++98 -I recon -rdynamic \
//       recon/src/commlib/test_buddy_alloc_oracle.cpp \
//       recon/build/buddy/comm_buddy_alloc.o -ldl -o recon/build/buddy/oracle
#include <dlfcn.h>
#include <new>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <ext/hash_map>

// The reconstruction's header, opened for inspection (the ABI is unaffected).
#define private public
#include "src/commlib/comm_buddy_alloc.h"
#undef private

// Layout constants used by the snapshot code (values of the ORIG DWARF).
static const int kOrderSlots = 32;          // BUDDY_FREE_CLASSIFY_MAX_NUM
static const unsigned int kPoolBytes = 100000u * 32u;   // desc pool buffer

// ---------------------------------------------------------------------------
// intercepted log functions
// ---------------------------------------------------------------------------
static std::vector<std::string> g_orig_log;
static std::vector<std::string> g_new_log;
static std::vector<std::string> *g_log = &g_new_log;

namespace ZEN_Trace_LogMsg {

static void record(const char *fmt, va_list ap) {
    char buf[4096];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    if (g_log != NULL) {
        g_log->push_back(buf);
    }
}

void debug_errorex(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    record(fmt, ap);
    va_end(ap);
}

void debug_debugEx(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    record(fmt, ap);
    va_end(ap);
}

}  // namespace ZEN_Trace_LogMsg

// ---------------------------------------------------------------------------
// ORIG entry points
// ---------------------------------------------------------------------------
static const char *kSo =
        "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";

struct BuddyApi {
    int (*init)(void *, unsigned int *, unsigned int, bool);
    void *(*alloc)(void *, unsigned int);
    void (*free)(void *, void *);
    int (*fini)(void *);
    void (*print_info)(void *);
    void (*ctor)(void *);
    void (*dtor)(void *);
    int (*get_next_order_and_power)(void *, unsigned int, int *, unsigned int *);
};

struct MultiApi {
    int (*init)(void *, unsigned int *, int, bool);
    void *(*alloc)(void *, unsigned int);
    void (*free)(void *, void *);
    int (*fini)(void *);
    void (*print_info)(void *);
    void (*ctor)(void *);
    void (*dtor)(void *);
};

template <typename T>
static T sym(const char *mangled) {
    void *p = dlsym(RTLD_DEFAULT, mangled);
    if (p == NULL) {
        fprintf(stderr, "FATAL: dlsym(%s): %s\n", mangled, dlerror());
        exit(2);
    }
    return reinterpret_cast<T>(p);
}

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------
static int g_failures = 0;
static int g_checks = 0;
static const char *g_ctx = "";

static std::string num(long long v) {
    char b[32];
    snprintf(b, sizeof(b), "%lld", v);
    return std::string(b);
}

static std::string unum(unsigned long long v) {
    char b[32];
    snprintf(b, sizeof(b), "%llu", v);
    return std::string(b);
}

static void fail(const char *what, long long a, long long b) {
    if (g_failures < 40) {
        printf("  MISMATCH [%s] %s: orig=%lld recon=%lld\n", g_ctx, what, a, b);
    }
    ++g_failures;
}

static void chk(const char *what, long long a, long long b) {
    ++g_checks;
    if (a != b) {
        fail(what, a, b);
    }
}

// offset of p inside [base, base+len); -1 for NULL, -2 for "heap address"
static long off(const void *p, const void *base, unsigned int len) {
    if (p == NULL) {
        return -1;
    }
    const char *c = (const char *)p;
    const char *b = (const char *)base;
    if (b != NULL && c >= b && c < b + len) {
        return (long)(c - b);
    }
    return -2;
}

// Walks a linked chain, stopping when a node leaves [base, base+len).  The
// stop marker keeps the sequences of both sides comparable.
template <typename T>
static void collect_chain(T *head, const void *base, unsigned int len,
                          std::vector<T *>& out, int maxn = 300000) {
    int guard = 0;
    for (T *n = head; n != NULL; n = n->next_) {
        const char *c = (const char *)n;
        if (base == NULL || c < (const char *)base ||
            c >= (const char *)base + len || guard >= maxn) {
            out.push_back(NULL);
            return;
        }
        out.push_back(n);
        ++guard;
    }
}

// ---------------------------------------------------------------------------
// BuddyAlloc state snapshot / comparison
// ---------------------------------------------------------------------------
struct BuddySnap {
    std::vector<std::string> items;
};

// `live` is false right after the constructor: free_classify_list_array_,
// classify_bitset_ and find_order_and_power_array are deliberately left
// uninitialised by the ORIG, so only raw values are recorded then.
static void collect_buddy(BuddyAlloc *a, BuddySnap& s, bool live = true) {
    const void *buf = a->buffer_;
    const unsigned int blen = (unsigned int)a->buffer_len_;
    const void *pool = a->desc_obj_pool_buffer_;

    s.items.push_back("buffer_len=" + unum(a->buffer_len_));
    s.items.push_back("base_size=" + unum(a->base_size_));
    s.items.push_back("need_order=" + num(a->need_order_));
    s.items.push_back("self_alloc=" + num(a->is_buffer_self_alloc_));
    s.items.push_back("lack_sys=" + num(a->lack_use_system_));
    s.items.push_back("size_used=" + unum(a->size_used_));
    s.items.push_back("size_alloc=" + unum(a->size_allocated_));
    s.items.push_back("alloc_times=" + unum(a->alloc_times_));
    s.items.push_back("alloc_split=" + unum(a->alloc_split_series_));
    s.items.push_back("free_times=" + unum(a->free_times_));
    s.items.push_back("free_merge=" + unum(a->free_merge_series_));
    s.items.push_back("max_size=" + unum(a->max_size_can_alloc_));
    s.items.push_back("max_order=" + num(a->max_size_can_alloc_order_));
    s.items.push_back("sys_alloc=" + unum(a->use_system_alloc_times_));
    s.items.push_back("sys_free=" + unum(a->use_system_free_times_));
    s.items.push_back("buf_off=" + num(off(a->buffer_, a->buffer_, 1)));
    s.items.push_back("map_size=" + unum(a->ptr_desc_map_.size()));

    for (int i = 0; i < kOrderSlots; ++i) {
        s.items.push_back("farr[" + num(i) + "]=" +
                          unum(a->find_order_and_power_array[i]));
    }
    for (int i = 0; i < kOrderSlots; ++i) {
        if (!live) {
            s.items.push_back("fl[" + num(i) + "]_raw=" +
                              unum((unsigned long)(a->free_classify_list_array_[i])));
            continue;
        }
        std::vector<BuddyBufDesc *> chain;
        collect_chain(a->free_classify_list_array_[i], pool, kPoolBytes, chain);
        s.items.push_back("fl[" + num(i) + "]_n=" + unum(chain.size()));
        for (size_t k = 0; k < chain.size(); ++k) {
            BuddyBufDesc *d = chain[k];
            if (d == NULL) {
                s.items.push_back("fl[" + num(i) + "][" + unum(k) +
                                  "]=INVALID");
                break;
            }
            s.items.push_back(
                    "fl[" + num(i) + "][" + unum(k) + "]=" +
                    num(off(d, pool, kPoolBytes)) +
                    " ptr=" + num(off(d->ptr_, buf, blen)) +
                    " len=" + unum(d->ptr_len_) +
                    " used=" + unum(d->used_len_) + " ord=" + num(d->order_) +
                    " blk=" + num((long long)d->block_index_) +
                    " pre=" + num(off(d->pre_, pool, kPoolBytes)));
        }
    }
    for (int i = 0; i < kOrderSlots; ++i) {
        BuddyBitSet *b = a->classify_bitset_[i];
        // The ORIG never initialises these slots; only the fact that they are
        // untouched (null vs not) is comparable.
        if (!live || i >= a->need_order_) {
            s.items.push_back("bs[" + num(i) + "]_slot=" +
                              (b == NULL ? "null" : "garbage"));
            continue;
        }
        if (b == NULL) {
            s.items.push_back("bs[" + num(i) + "]=NULL");
            continue;
        }
        std::string h = "bs[" + num(i) + "] len=" + unum(b->bit_ptr_byte_len_) +
                        " data=";
        for (unsigned int k = 0; k < b->bit_ptr_byte_len_; ++k) {
            char t[8];
            snprintf(t, sizeof(t), "%02x", b->bit_ptr_[k]);
            h += t;
        }
        s.items.push_back(h);
    }
    {
        if (!live) {
            s.items.push_back("pool_head_raw=" +
                              unum((unsigned long)a->desc_obj_pool_));
            s.items.push_back("add_pool_raw=" +
                              unum((unsigned long)a->additional_desc_obj_pool_));
        } else {
            std::vector<BuddyBufDesc *> chain;
            collect_chain(a->desc_obj_pool_, pool, kPoolBytes, chain);
            s.items.push_back("desc_pool_n=" + unum(chain.size()));
            std::string h = "desc_pool_head=";
            if (!chain.empty() && chain[0] != NULL) {
                h += num(off(chain[0], pool, kPoolBytes));
            } else {
                h += "-";
            }
            s.items.push_back(h);
            s.items.push_back("pool_chain_end=" +
                              num(chain.empty() || chain[chain.size() - 1] == NULL
                                          ? -1
                                          : 0));
            // descriptors created for system allocations live on the heap, so
            // only their contents are comparable, not their addresses.
            int guard = 0;
            std::vector<std::string> add;
            for (BuddyBufDesc *d = a->additional_desc_obj_pool_;
                 d != NULL && guard < 4096; d = d->next_, ++guard) {
                add.push_back("add[" + unum(guard) + "] len=" +
                              unum(d->ptr_len_) + " used=" + unum(d->used_len_) +
                              " ord=" + num(d->order_) + " blk=" +
                              num((long long)d->block_index_) + " sda=" +
                              num(d->self_desc_additional_) + " spa=" +
                              num(d->self_ptr_additional_));
            }
            s.items.push_back("add_pool_n=" + unum(add.size()));
            for (size_t i = 0; i < add.size(); ++i) {
                s.items.push_back(add[i]);
            }
        }
    }
    {
        BUDDY_ALLOC_POINTER_DESC_HASHMAP& m = a->ptr_desc_map_;
        std::vector<std::string> entries;
        for (BUDDY_ALLOC_POINTER_DESC_HASHMAP::iterator it = m.begin();
             it != m.end(); ++it) {
            std::string e = "k" + num(off(it->first, buf, blen)) + "->o" +
                            num(off(it->second, pool, kPoolBytes));
            if (it->second != NULL) {
                e += " ord=" + num(it->second->order_) + " blk=" +
                     num((long long)it->second->block_index_) + " len=" +
                     unum(it->second->ptr_len_) +
                     " used=" + unum(it->second->used_len_) +
                     " pptr=" + num(off(it->second->ptr_, buf, blen));
            }
            entries.push_back(e);
        }
        std::sort(entries.begin(), entries.end());
        for (size_t i = 0; i < entries.size(); ++i) {
            s.items.push_back("map:" + entries[i]);
        }
    }
}

static void cmp_buddy(const char *ctx, BuddyAlloc *o, BuddyAlloc *r,
                      bool live = true) {
    g_ctx = ctx;
    BuddySnap so, sr;
    collect_buddy(o, so, live);
    collect_buddy(r, sr, live);
    if (so.items.size() != sr.items.size()) {
        fail("snapshot item count", (long long)so.items.size(),
             (long long)sr.items.size());
    }
    size_t n = std::min(so.items.size(), sr.items.size());
    for (size_t i = 0; i < n; ++i) {
        ++g_checks;
        if (so.items[i] != sr.items[i]) {
            if (g_failures < 40) {
                printf("  MISMATCH [%s] item %lu:\n    orig : %s\n    recon: %s\n",
                       ctx, (unsigned long)i, so.items[i].c_str(),
                       sr.items[i].c_str());
            }
            ++g_failures;
        }
    }
}

static void *aligned_alloc_16(unsigned int size) {
    void *p = malloc(size + 32);
    unsigned long v = ((unsigned long)p + 31) & ~15UL;
    return (void *)v;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    printf("sizeof(BuddyBitSet)=%u sizeof(BuddyBufDesc)=%u sizeof(BuddyAlloc)=%u"
           " sizeof(MultiBuddyAlloc)=%u\n",
           (unsigned)sizeof(BuddyBitSet), (unsigned)sizeof(BuddyBufDesc),
           (unsigned)sizeof(BuddyAlloc), (unsigned)sizeof(MultiBuddyAlloc));
    if (sizeof(BuddyAlloc) != 476 || sizeof(MultiBuddyAlloc) != 108 ||
        sizeof(BuddyBufDesc) != 32 || sizeof(BuddyBitSet) != 8) {
        printf("FATAL: recon class layout does not match the ORIG DWARF\n");
        return 2;
    }

    void *h = dlopen(kSo, RTLD_NOW | RTLD_LOCAL);
    if (h == NULL) {
        printf("FATAL: dlopen failed: %s\n", dlerror());
        return 2;
    }

    BuddyApi ba;
    ba.ctor = sym<void (*)(void *)>("_ZN10BuddyAllocC1Ev");
    ba.dtor = sym<void (*)(void *)>("_ZN10BuddyAllocD1Ev");
    ba.init = sym<int (*)(void *, unsigned int *, unsigned int, bool)>(
            "_ZN10BuddyAlloc4initEPjjb");
    ba.alloc = sym<void *(*)(void *, unsigned int)>("_ZN10BuddyAlloc5allocEj");
    ba.free = sym<void (*)(void *, void *)>("_ZN10BuddyAlloc4freeEPv");
    ba.fini = sym<int (*)(void *)>("_ZN10BuddyAlloc4finiEv");
    ba.print_info = sym<void (*)(void *)>("_ZN10BuddyAlloc10print_infoEv");
    ba.get_next_order_and_power =
            sym<int (*)(void *, unsigned int, int *, unsigned int *)>(
                    "_ZN10BuddyAlloc24get_next_order_and_powerEjPiPj");

    MultiApi ma;
    ma.ctor = sym<void (*)(void *)>("_ZN15MultiBuddyAllocC1Ev");
    ma.dtor = sym<void (*)(void *)>("_ZN15MultiBuddyAllocD1Ev");
    ma.init = sym<int (*)(void *, unsigned int *, int, bool)>(
            "_ZN15MultiBuddyAlloc4initEPjib");
    ma.alloc = sym<void *(*)(void *, unsigned int)>(
            "_ZN15MultiBuddyAlloc5allocEj");
    ma.free = sym<void (*)(void *, void *)>("_ZN15MultiBuddyAlloc4freeEPv");
    ma.fini = sym<int (*)(void *)>("_ZN15MultiBuddyAlloc4finiEv");
    ma.print_info = sym<void (*)(void *)>("_ZN15MultiBuddyAlloc10print_infoEv");

    char ctx[160];
    const char *o_txt = "print_orig.txt";
    const char *r_txt = "print_recon.txt";
    (void)o_txt;
    (void)r_txt;

    // -----------------------------------------------------------------
    // BuddyAlloc: init + deterministic alloc/free sequence
    // -----------------------------------------------------------------
    BuddyAlloc *o = (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
    BuddyAlloc *r = (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
    memset(o, 0x00, sizeof(BuddyAlloc));
    memset(r, 0x00, sizeof(BuddyAlloc));
    g_log = &g_orig_log;
    ba.ctor(o);
    g_log = &g_new_log;
    new (r) BuddyAlloc();
    cmp_buddy("after ctor", o, r, false);

    unsigned int o_size = 0x200000;
    unsigned int r_size = 0x200000;
    g_log = &g_orig_log;
    int o_rc = ba.init(o, &o_size, 0x10, true);
    g_log = &g_new_log;
    int r_rc = r->init(&r_size, 0x10, true);
    chk("init rc", o_rc, r_rc);
    chk("init out size", o_size, r_size);
    cmp_buddy("after init(0x200000,16,true)", o, r);
    printf("after init: buffer_len=%lu need_order=%d max_size=%lu base=%lu "
           "chain_order0_is_empty=%d\n",
           (unsigned long)o->buffer_len_, o->need_order_,
           (unsigned long)o->max_size_can_alloc_, (unsigned long)o->base_size_,
           o->free_classify_list_array_[0] == NULL);

    std::vector<unsigned int> sizes;
    static const unsigned int kSizes[] = {
        1, 15, 16, 17, 31, 32, 33, 64, 100, 1000, 4095, 4096, 4097, 65535,
        65536, 65537, 0x7ffff, 0x80000, 0x100000, 0x1fffff, 0x200000};
    for (unsigned i = 0; i < sizeof(kSizes) / sizeof(kSizes[0]); ++i) {
        sizes.push_back(kSizes[i]);
    }
    unsigned int seed = 12345;
    for (int i = 0; i < 400; ++i) {
        seed = seed * 1103515245u + 12345u;
        sizes.push_back(1 + (seed >> 8) % 0x4000);
    }

    std::vector<void *> o_raw, r_raw;
    for (size_t i = 0; i < sizes.size(); ++i) {
        unsigned int sz = sizes[i];
        void *op = ba.alloc(o, sz);
        void *rp = r->alloc(sz);
        snprintf(ctx, sizeof(ctx), "alloc(%u) #%lu", sz, (unsigned long)i);
        long oo = off(op, o->buffer_, (unsigned int)o->buffer_len_);
        long ro = off(rp, r->buffer_, (unsigned int)r->buffer_len_);
        chk(ctx, oo, ro);
        // keep buffer blocks as well as system (heap) allocations so that the
        // self_ptr_additional_ free path is exercised too
        if ((oo >= 0 && ro >= 0) || (oo == -2 && ro == -2)) {
            o_raw.push_back(op);
            r_raw.push_back(rp);
        }
        if (i % 41 == 0 || i + 1 == sizes.size()) {
            snprintf(ctx, sizeof(ctx), "state after alloc #%lu",
                     (unsigned long)i);
            cmp_buddy(ctx, o, r);
        }
    }
    cmp_buddy("after alloc phase", o, r);
    printf("alloc phase: %lu live blocks, size_used=%lu size_allocated=%lu "
           "alloc_times=%u alloc_split=%u\n",
           (unsigned long)o_raw.size(), (unsigned long)o->size_used_,
           (unsigned long)o->size_allocated_, o->alloc_times_,
           o->alloc_split_series_);

    for (size_t i = 0; i < o_raw.size(); i += 2) {
        ba.free(o, o_raw[i]);
        r->free(r_raw[i]);
    }
    cmp_buddy("after free phase 1 (every 2nd)", o, r);

    // print_info() only prints while allocations are live and both counters
    // are non-zero; compare the text that both sides hand to the log.
    {
        size_t n0 = g_orig_log.size();
        size_t n1 = g_new_log.size();
        g_log = &g_orig_log;
        ba.print_info(o);
        g_log = &g_new_log;
        r->print_info();
        size_t n_mo = g_orig_log.size() - n0;
        size_t n_mr = g_new_log.size() - n1;
        printf("print_info (live state) produced %lu orig / %lu recon "
               "message(s)\n", (unsigned long)n_mo, (unsigned long)n_mr);
        chk("print_info message count", (long long)n_mo, (long long)n_mr);
        for (size_t i = 0; i < n_mo && i < n_mr; ++i) {
            ++g_checks;
            if (g_orig_log[n0 + i] != g_new_log[n1 + i]) {
                printf("  PRINT MISMATCH:\n--- orig ---\n%s\n--- recon ---\n%s\n",
                       g_orig_log[n0 + i].c_str(), g_new_log[n1 + i].c_str());
                ++g_failures;
            } else if (i == 0) {
                printf("print_info text identical (%lu bytes)\n",
                       (unsigned long)g_orig_log[n0 + i].size());
            }
        }
    }
    for (size_t i = 1; i < o_raw.size(); i += 2) {
        ba.free(o, o_raw[i]);
        r->free(r_raw[i]);
    }
    cmp_buddy("after free phase 2 (all freed)", o, r);
    printf("all freed: size_used=%lu size_allocated=%lu free_times=%u "
           "free_merge=%u map_n=%lu max_size=%lu max_order=%d\n",
           (unsigned long)o->size_used_, (unsigned long)o->size_allocated_,
           o->free_times_, o->free_merge_series_,
           (unsigned long)o->ptr_desc_map_.size(),
           (unsigned long)o->max_size_can_alloc_, o->max_size_can_alloc_order_);

    {
        unsigned int sz2 = 1234;
        void *op2 = ba.alloc(o, sz2);
        void *rp2 = r->alloc(sz2);
        snprintf(ctx, sizeof(ctx), "re-alloc(%u)", sz2);
        chk(ctx, off(op2, o->buffer_, (unsigned int)o->buffer_len_),
            off(rp2, r->buffer_, (unsigned int)r->buffer_len_));
        cmp_buddy("after re-alloc", o, r);
        ba.free(o, op2);
        r->free(rp2);
        cmp_buddy("after re-free", o, r);
    }

    // -----------------------------------------------------------------
    // invalid inputs (messages captured through the interposed log)
    // -----------------------------------------------------------------
    {
        size_t n0 = g_orig_log.size();
        size_t n1 = g_new_log.size();
        g_log = &g_orig_log;
        void *p0 = ba.alloc(o, 0);
        g_log = &g_new_log;
        void *p1 = r->alloc(0);
        chk("alloc(0)", off(p0, o->buffer_, (unsigned int)o->buffer_len_),
            off(p1, r->buffer_, (unsigned int)r->buffer_len_));

        unsigned int too_big = (unsigned int)o->buffer_len_ + 1;
        g_log = &g_orig_log;
        p0 = ba.alloc(o, too_big);
        g_log = &g_new_log;
        p1 = r->alloc(too_big);
        chk("alloc(buffer_len+1)",
            off(p0, o->buffer_, (unsigned int)o->buffer_len_),
            off(p1, r->buffer_, (unsigned int)r->buffer_len_));

        g_log = &g_orig_log;
        ba.free(o, NULL);
        unsigned char junk[64];
        memset(junk, 7, sizeof(junk));
        ba.free(o, junk);
        g_log = &g_new_log;
        r->free(NULL);
        r->free(junk);

        size_t n_mo = g_orig_log.size() - n0;
        size_t n_mr = g_new_log.size() - n1;
        printf("invalid-input messages: orig %lu, recon %lu\n",
               (unsigned long)n_mo, (unsigned long)n_mr);
        chk("invalid-input message count", (long long)n_mo, (long long)n_mr);
        for (size_t i = 0; i < n_mo && i < n_mr; ++i) {
            ++g_checks;
            if (g_orig_log[n0 + i] != g_new_log[n1 + i]) {
                printf("  LOG MISMATCH:\n    orig : %s\n    recon: %s\n",
                       g_orig_log[n0 + i].c_str(), g_new_log[n1 + i].c_str());
                ++g_failures;
            }
        }
        cmp_buddy("after invalid inputs", o, r);
    }

    // -----------------------------------------------------------------
    // fini() quirk: because init_bitset() only fills classify_bitset_ up to
    // need_order_, fini() deletes whatever the remaining 32-need_order_ slots
    // contain.  With an untouched (garbage) tail the ORIG dies; with a zeroed
    // tail it survives.  Both variants are compared in forked children.
    // -----------------------------------------------------------------
    {
        for (int variant = 0; variant < 2; ++variant) {
            long codes[2] = {0, 0};
            int sigs[2] = {0, 0};
            for (int side = 0; side < 2; ++side) {
                pid_t pid = fork();
                if (pid == 0) {
                    BuddyAlloc *x =
                            (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
                    memset(x, variant == 0 ? 0x00 : 0x5A, sizeof(BuddyAlloc));
                    unsigned int sz = 0x10000;
                    int rc = 0;
                    if (side == 0) {
                        ba.ctor(x);
                        rc = ba.init(x, &sz, 16, true);
                        if (rc == 0) {
                            rc = ba.fini(x);
                        }
                    } else {
                        new (x) BuddyAlloc();
                        rc = x->init(&sz, 16, true);
                        if (rc == 0) {
                            rc = x->fini();
                        }
                    }
                    _exit((rc + 8) & 0x7f);
                }
                int st = 0;
                waitpid(pid, &st, 0);
                if (WIFSIGNALED(st)) {
                    sigs[side] = WTERMSIG(st);
                } else {
                    codes[side] = WEXITSTATUS(st);
                }
            }
            printf("fini after init(0x10000,16) [%s tail] "
                   "orig: exit=%ld sig=%d | recon: exit=%ld sig=%d\n",
                   variant == 0 ? "zeroed" : "garbage", codes[0], sigs[0],
                   codes[1], sigs[1]);
            ++g_checks;
            if (codes[0] != codes[1] || sigs[0] != sigs[1]) {
                printf("  MISMATCH: fini() outcome differs\n");
                ++g_failures;
            }
        }
    }

    // -----------------------------------------------------------------
    // get_next_order_and_power sweep
    // -----------------------------------------------------------------
    {
        long bad = 0;
        unsigned int lim = (unsigned int)o->buffer_len_ + 64;
        for (unsigned int sz = 0; sz <= lim; ++sz) {
            int oo = -7, ro = -7;
            unsigned int op = 0, rp = 0;
            g_log = &g_orig_log;
            int orc = ba.get_next_order_and_power(o, sz, &oo, &op);
            g_log = &g_new_log;
            int rrc = r->get_next_order_and_power(sz, &ro, &rp);
            ++g_checks;
            if (orc != rrc || oo != ro || op != rp) {
                if (bad < 10) {
                    printf("  MISMATCH get_next_order_and_power(%u): rc %d/%d "
                           "order %d/%d power %u/%u\n",
                           sz, orc, rrc, oo, ro, op, rp);
                }
                ++g_failures;
                ++bad;
            }
        }
        printf("get_next_order_and_power sweep: %u sizes, %ld mismatches\n",
               lim + 1, bad);
    }

    // -----------------------------------------------------------------
    // init error paths
    //
    // Every rejection path of init() calls fini() unconditionally, and fini()
    // walks classify_bitset_[0..31] - which init_bitset() only fills for
    // i < need_order_.  On a never initialised instance this dereferences the
    // uninitialised array, so the ORIG has to die as well; each case is run in
    // a forked child and only the outcome (exit code / signal) is compared.
    // -----------------------------------------------------------------
    {
        static const struct {
            const char *name;
            int mode;            // 0 = fresh instance, 1 = after a valid init
            unsigned int base;
            int null_size;
            unsigned int size;
        } kCases[] = {
            {"init(NULL) fresh", 0, 16, 1, 0},
            {"init(size=0) fresh", 0, 16, 0, 0},
            {"init(size=8) fresh", 0, 16, 0, 8},
            {"init(base=4) fresh", 0, 4, 0, 0x10000},
            {"init(size=0) after valid init", 1, 16, 0, 0},
        };
        for (unsigned c = 0; c < sizeof(kCases) / sizeof(kCases[0]); ++c) {
            long codes[2] = {0, 0};
            int sigs[2] = {0, 0};
            for (int side = 0; side < 2; ++side) {
                pid_t pid = fork();
                if (pid == 0) {
                    BuddyAlloc *x =
                            (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
                    memset(x, 0x33, sizeof(BuddyAlloc));
                    if (side == 0) {
                        ba.ctor(x);
                    } else {
                        new (x) BuddyAlloc();
                    }
                    if (kCases[c].mode == 1) {
                        unsigned int sz = 0x10000;
                        int rc0 = (side == 0) ? ba.init(x, &sz, 16, true)
                                              : x->init(&sz, 16, true);
                        if (rc0 != 0) {
                            _exit(90);
                        }
                    }
                    unsigned int sz = kCases[c].size;
                    unsigned int *sp = kCases[c].null_size ? NULL : &sz;
                    int rc = (side == 0) ? ba.init(x, sp, kCases[c].base, true)
                                         : x->init(sp, kCases[c].base, true);
                    _exit(rc & 0x7f);
                }
                int st = 0;
                waitpid(pid, &st, 0);
                if (WIFSIGNALED(st)) {
                    sigs[side] = WTERMSIG(st);
                } else {
                    codes[side] = WEXITSTATUS(st);
                }
            }
            printf("%-32s orig: exit=%ld sig=%d | recon: exit=%ld sig=%d\n",
                   kCases[c].name, codes[0], sigs[0], codes[1], sigs[1]);
            ++g_checks;
            if (codes[0] != codes[1] || sigs[0] != sigs[1]) {
                printf("  MISMATCH: init error path outcome differs\n");
                ++g_failures;
            }
        }
        // a clean instance still initialises normally
        BuddyAlloc *o2 = (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
        BuddyAlloc *r2 = (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
        memset(o2, 0x00, sizeof(BuddyAlloc));
        memset(r2, 0x00, sizeof(BuddyAlloc));
        ba.ctor(o2);
        new (r2) BuddyAlloc();
        unsigned int s3 = 0x10000, s4 = 0x10000;
        chk("init(base=4) rc", ba.init(o2, &s3, 4, true),
            r2->init(&s4, 4, true));
        chk("init(base=4) base", (long long)o2->base_size_,
            (long long)r2->base_size_);
        cmp_buddy("init(base=4)", o2, r2);
        chk("init(base=4) fini rc", ba.fini(o2), r2->fini());
        ba.dtor(o2);
        r2->BuddyAlloc::~BuddyAlloc();
    }

    // -----------------------------------------------------------------
    // descriptor pool exhaustion (forked: the ORIG walks off the list)
    // -----------------------------------------------------------------
    {
        long counts[2] = {-1, -1};
        int sigs[2] = {0, 0};
        for (int side = 0; side < 2; ++side) {
            pid_t pid = fork();
            if (pid == 0) {
                const char *path = side == 0
                        ? "drain_orig.txt"
                        : "drain_recon.txt";
                FILE *f = fopen(path, "w");
                fflush(NULL);
                BuddyAlloc *x = (BuddyAlloc *)aligned_alloc_16(sizeof(BuddyAlloc));
                unsigned int sz = 0x200000;
                if (side == 0) {
                    ba.ctor(x);
                    ba.init(x, &sz, 16, false);
                } else {
                    new (x) BuddyAlloc();
                    x->init(&sz, 16, false);
                }
                long n = 0;
                for (;;) {
                    void *p = (side == 0) ? ba.alloc(x, 16) : x->alloc(16);
                    if (p == NULL) {
                        break;
                    }
                    ++n;
                    if ((n % 256) == 0 && f != NULL) {
                        // keep the count on disk so that a crash still reports
                        // how far the allocator got
                        fseek(f, 0, SEEK_SET);
                        fprintf(f, "%ld\n", n);
                        fflush(f);
                    }
                    if (n > 500000) {
                        break;
                    }
                }
                if (f != NULL) {
                    fseek(f, 0, SEEK_SET);
                    fprintf(f, "%ld\n", n);
                    fclose(f);
                }
                _exit(0);
            }
            int st = 0;
            waitpid(pid, &st, 0);
            if (WIFSIGNALED(st)) {
                sigs[side] = WTERMSIG(st);
            }
            {
                // read the progress even after a crash
                const char *path = side == 0
                        ? "drain_orig.txt"
                        : "drain_recon.txt";
                FILE *f = fopen(path, "r");
                if (f != NULL) {
                    if (fscanf(f, "%ld", &counts[side]) != 1) {
                        counts[side] = -2;
                    }
                    fclose(f);
                } else {
                    counts[side] = -3;
                }
            }
        }
        printf("descriptor pool drain: orig count=%ld signal=%d ; recon count=%ld"
               " signal=%d\n", counts[0], sigs[0], counts[1], sigs[1]);
        ++g_checks;
        if (counts[0] != counts[1] || sigs[0] != sigs[1]) {
            printf("  MISMATCH: pool exhaustion behaviour differs\n");
            ++g_failures;
        }
    }

    // -----------------------------------------------------------------
    // MultiBuddyAlloc
    // -----------------------------------------------------------------
    MultiBuddyAlloc *mo =
            (MultiBuddyAlloc *)aligned_alloc_16(sizeof(MultiBuddyAlloc));
    MultiBuddyAlloc *mr =
            (MultiBuddyAlloc *)aligned_alloc_16(sizeof(MultiBuddyAlloc));
    memset(mo, 0x00, sizeof(MultiBuddyAlloc));
    memset(mr, 0x00, sizeof(MultiBuddyAlloc));
    ma.ctor(mo);
    new (mr) MultiBuddyAlloc();

    unsigned int ms1 = 0x200000;
    unsigned int ms2 = 0x200000;
    int mrc_o = ma.init(mo, &ms1, 4, true);
    int mrc_r = mr->init(&ms2, 4, true);
    chk("multi init rc", mrc_o, mrc_r);
    chk("multi init size", ms1, ms2);
    printf("multi after init: size=%u buffer_len=%lu steps=%d max_order=%d "
           "buddy_map_n=%lu sys_map_n=%lu\n",
           ms1, (unsigned long)mo->buffer_len_, mo->num_of_steps_,
           mo->buddy_max_order_, (unsigned long)mo->ptr_buddy_map_.size(),
           (unsigned long)mo->ptr_system_map_.size());

    std::vector<void *> mo_raw, mr_raw;
    unsigned int mseed = 999;
    for (int i = 0; i < 300; ++i) {
        mseed = mseed * 1103515245u + 12345u;
        unsigned int sz = 1 + (mseed >> 9) % 0x8000;
        void *p1 = ma.alloc(mo, sz);
        void *p2 = mr->alloc(sz);
        snprintf(ctx, sizeof(ctx), "multi alloc(%u) #%d", sz, i);
        long a1 = off(p1, mo->buffer_, (unsigned int)mo->buffer_len_);
        long a2 = off(p2, mr->buffer_, (unsigned int)mr->buffer_len_);
        chk(ctx, a1, a2);
        if (a1 >= 0 && a2 >= 0) {
            mo_raw.push_back(p1);
            mr_raw.push_back(p2);
        }
    }
    {
        g_ctx = "multi state";
        chk("multi buffer_len", (long long)mo->buffer_len_,
            (long long)mr->buffer_len_);
        chk("multi num_steps", mo->num_of_steps_, mr->num_of_steps_);
        chk("multi max_order", mo->buddy_max_order_, mr->buddy_max_order_);
        chk("multi alloc_times", mo->alloc_times_, mr->alloc_times_);
        chk("multi sys_alloc", mo->use_system_alloc_times_,
            mr->use_system_alloc_times_);
        chk("multi free_times", mo->free_times_, mr->free_times_);
        chk("multi sys_free", mo->use_system_free_times_,
            mr->use_system_free_times_);
        chk("multi ptr_buddy_map n", (long long)mo->ptr_buddy_map_.size(),
            (long long)mr->ptr_buddy_map_.size());
        chk("multi ptr_system_map n", (long long)mo->ptr_system_map_.size(),
            (long long)mr->ptr_system_map_.size());
        std::vector<std::string> eo, er;
        for (BUDDY_ALLOC_POINTER_POOL_HASHMAP::iterator it =
                     mo->ptr_buddy_map_.begin(); it != mo->ptr_buddy_map_.end();
             ++it) {
            int idx = -1;
            for (int i = 0; i < mo->num_of_steps_; ++i) {
                if (it->second == mo->buddy_pool_array_[i]) {
                    idx = i;
                }
            }
            eo.push_back(num(off(it->first, mo->buffer_,
                                 (unsigned)mo->buffer_len_)) + "->" + num(idx));
        }
        for (BUDDY_ALLOC_POINTER_POOL_HASHMAP::iterator it =
                     mr->ptr_buddy_map_.begin(); it != mr->ptr_buddy_map_.end();
             ++it) {
            int idx = -1;
            for (int i = 0; i < mr->num_of_steps_; ++i) {
                if (it->second == mr->buddy_pool_array_[i]) {
                    idx = i;
                }
            }
            er.push_back(num(off(it->first, mr->buffer_,
                                 (unsigned)mr->buffer_len_)) + "->" + num(idx));
        }
        std::sort(eo.begin(), eo.end());
        std::sort(er.begin(), er.end());
        ++g_checks;
        if (eo != er) {
            printf("  MISMATCH ptr_buddy_map contents (%lu vs %lu)\n",
                   (unsigned long)eo.size(), (unsigned long)er.size());
            ++g_failures;
        }
        std::vector<std::string> so3, sr3;
        for (BUDDY_ALLOC_POINTER_SIZE_HASHMAP::iterator it =
                     mo->ptr_system_map_.begin(); it != mo->ptr_system_map_.end();
             ++it) {
            so3.push_back(num(off(it->first, mo->buffer_,
                                  (unsigned)mo->buffer_len_)) + "->" +
                          unum(it->second));
        }
        for (BUDDY_ALLOC_POINTER_SIZE_HASHMAP::iterator it =
                     mr->ptr_system_map_.begin(); it != mr->ptr_system_map_.end();
             ++it) {
            sr3.push_back(num(off(it->first, mr->buffer_,
                                  (unsigned)mr->buffer_len_)) + "->" +
                          unum(it->second));
        }
        std::sort(so3.begin(), so3.end());
        std::sort(sr3.begin(), sr3.end());
        ++g_checks;
        if (so3 != sr3) {
            printf("  MISMATCH ptr_system_map contents (%lu vs %lu)\n",
                   (unsigned long)so3.size(), (unsigned long)sr3.size());
            ++g_failures;
        }
        for (int i = 0; i < mo->num_of_steps_; ++i) {
            snprintf(ctx, sizeof(ctx), "multi pool %d", i);
            cmp_buddy(ctx, mo->buddy_pool_array_[i], mr->buddy_pool_array_[i]);
        }
    }

    // system fallback + its free path + the "no associated ptr" error path
    {
        unsigned int big = (unsigned int)(0x10u << mo->buddy_max_order_) + 1;
        g_log = &g_orig_log;
        void *p1 = ma.alloc(mo, big);
        g_log = &g_new_log;
        void *p2 = mr->alloc(big);
        chk("multi system alloc classified heap",
            off(p1, mo->buffer_, (unsigned int)mo->buffer_len_) == -2,
            off(p2, mr->buffer_, (unsigned int)mr->buffer_len_) == -2);
        chk("multi sys_alloc_times", mo->use_system_alloc_times_,
            mr->use_system_alloc_times_);
        chk("multi sys_map n", (long long)mo->ptr_system_map_.size(),
            (long long)mr->ptr_system_map_.size());
        ma.free(mo, p1);
        mr->free(p2);
        chk("multi sys_free_times", mo->use_system_free_times_,
            mr->use_system_free_times_);
        chk("multi sys_map n after free",
            (long long)mo->ptr_system_map_.size(),
            (long long)mr->ptr_system_map_.size());
        // free() of an unknown pointer -> error message on both sides
        size_t n0 = g_orig_log.size();
        size_t n1 = g_new_log.size();
        unsigned char junk[32];
        memset(junk, 3, sizeof(junk));
        g_log = &g_orig_log;
        ma.free(mo, junk);
        ma.free(mo, NULL);
        g_log = &g_new_log;
        mr->free(junk);
        mr->free(NULL);
        size_t n_mo = g_orig_log.size() - n0;
        size_t n_mr = g_new_log.size() - n1;
        chk("multi free errors count", (long long)n_mo, (long long)n_mr);
        for (size_t i = 0; i < n_mo && i < n_mr; ++i) {
            ++g_checks;
            if (g_orig_log[n0 + i] != g_new_log[n1 + i]) {
                printf("  LOG MISMATCH:\n    orig : %s\n    recon: %s\n",
                       g_orig_log[n0 + i].c_str(), g_new_log[n1 + i].c_str());
                ++g_failures;
            }
        }
        chk("multi free_times after extra frees", mo->free_times_,
            mr->free_times_);
    }
    for (size_t i = 0; i < mo_raw.size(); i += 2) {
        ma.free(mo, mo_raw[i]);
        mr->free(mr_raw[i]);
    }
    {
        // live state: half of the blocks are still allocated
        size_t n0 = g_orig_log.size();
        size_t n1 = g_new_log.size();
        g_log = &g_orig_log;
        ma.print_info(mo);
        g_log = &g_new_log;
        mr->print_info();
        size_t n_mo = g_orig_log.size() - n0;
        size_t n_mr = g_new_log.size() - n1;
        printf("multi print_info produced %lu orig / %lu recon message(s)\n",
               (unsigned long)n_mo, (unsigned long)n_mr);
        chk("multi print_info count", (long long)n_mo, (long long)n_mr);
        for (size_t i = 0; i < n_mo && i < n_mr; ++i) {
            ++g_checks;
            if (g_orig_log[n0 + i] != g_new_log[n1 + i]) {
                printf("  PRINT MISMATCH:\n--- orig ---\n%s\n--- recon ---\n%s\n",
                       g_orig_log[n0 + i].c_str(), g_new_log[n1 + i].c_str());
                ++g_failures;
            } else if (i == 0) {
                printf("multi print_info text identical (%lu bytes)\n",
                       (unsigned long)g_orig_log[n0 + i].size());
            }
        }
    }
    for (size_t i = 1; i < mo_raw.size(); i += 2) {
        ma.free(mo, mo_raw[i]);
        mr->free(mr_raw[i]);
    }
    {
        g_ctx = "multi after frees";
        chk("multi free_times", mo->free_times_, mr->free_times_);
        chk("multi sys_free", mo->use_system_free_times_,
            mr->use_system_free_times_);
        chk("multi buddy_map n", (long long)mo->ptr_buddy_map_.size(),
            (long long)mr->ptr_buddy_map_.size());
        for (int i = 0; i < mo->num_of_steps_; ++i) {
            snprintf(ctx, sizeof(ctx), "multi pool %d after free", i);
            cmp_buddy(ctx, mo->buddy_pool_array_[i], mr->buddy_pool_array_[i]);
        }
    }
    // MultiBuddyAlloc::fini() deletes the pools it created; those come from
    // operator new and keep the uninitialised classify_bitset_ tail, so the
    // outcome is compared in forked children.
    {
        long codes[2] = {0, 0};
        int sigs[2] = {0, 0};
        for (int side = 0; side < 2; ++side) {
            pid_t pid = fork();
            if (pid == 0) {
                MultiBuddyAlloc *x = (MultiBuddyAlloc *)aligned_alloc_16(
                        sizeof(MultiBuddyAlloc));
                unsigned int sz = 0x200000;
                int rc = 0;
                if (side == 0) {
                    ma.ctor(x);
                    rc = ma.init(x, &sz, 4, true);
                    if (rc == 0) {
                        rc = ma.fini(x);
                    }
                } else {
                    new (x) MultiBuddyAlloc();
                    rc = x->init(&sz, 4, true);
                    if (rc == 0) {
                        rc = x->fini();
                    }
                }
                _exit((rc + 8) & 0x7f);
            }
            int st = 0;
            waitpid(pid, &st, 0);
            if (WIFSIGNALED(st)) {
                sigs[side] = WTERMSIG(st);
            } else {
                codes[side] = WEXITSTATUS(st);
            }
        }
        printf("multi fini after init(0x200000,4) orig: exit=%ld sig=%d | "
               "recon: exit=%ld sig=%d\n", codes[0], sigs[0], codes[1],
               sigs[1]);
        ++g_checks;
        if (codes[0] != codes[1] || sigs[0] != sigs[1]) {
            printf("  MISMATCH: multi fini() outcome differs\n");
            ++g_failures;
        }
    }

    chk("fini rc", ba.fini(o), r->fini());
    ba.dtor(o);
    r->BuddyAlloc::~BuddyAlloc();

    printf("\nchecks=%d mismatches=%d\n", g_checks, g_failures);
    printf("ORACLE RESULT: %s\n", g_failures == 0 ? "PASS" : "FAIL");
    return g_failures == 0 ? 0 : 1;
}
