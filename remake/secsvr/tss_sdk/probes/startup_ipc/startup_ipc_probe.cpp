// Isolated dlopen probe for the two 2026-10-19 startup/IPC repairs.
//
// It loads exactly one library per process (the frozen ORIG
// libtss_sdk.so.1.0.394.orig or a rebuilt libtss_sdk.so.1.0.394) and exercises
// real exported entry points; it never includes a recon header and never links
// the library.  All object offsets are the documented 394 layouts:
//
//   ZEN_Bus_MMAPPipe             sizeof 10292 (0x2834)
//     bus_mmap_name_   +0x0000
//     bus_head_        +0x1004 (size_of_sizet_ +0, number_of_pipe_ +4,
//                               size_of_pipe_[512] +8, size_of_room_[512] +0x808)
//     bus_pipe_pointer_+0x200c
//     mmap_addr_       +0x282c
//     sharemem_type_   +0x2830
//
//   ZEN_Server_Status            sizeof 76
//     stat_lock_       +0x04  (ZEN_Thread_Mutex 28 / ZEN_Null_Mutex 8)
//     stat_file_       +0x0c  (ZEN_ShareMem_Posix 16; mmap_addr_ at +0x10)
//     stat_file_head_  +0x1c
//     status_stat_sandy_ +0x20 (12-byte shm_vector)
//     status_copy_mandy_ +0x24
//     multi_thread_guard_ +0x48, initialized_ +0x49
//
// Bus sequence: fresh create -> push a frame -> cleanup -> resume with a
// deliberately corrupted *unvalidated* header tail -> verify the tail is
// normalised to zero and the frame survived -> 3-arg attach -> partial
// init failure -> restore mismatch -> missing/short file failures.
//
// ORIG tag394 0x1b4af9-0x1b4b19 (source line 258) executes one
// memcpy(dst=mmap_addr_, src=&bus_head_) after the restore check in BOTH
// paths, so the unvalidated tail must become zero.  A build that does
// mmap->local on restore reports bus.resume.tail_* = false.  The 3-arg attach
// overload is the opposite direction at 0x1b4d40-0x1b4d5f (line 350).
//
// Status sequence: fresh initialise with multi_thread -> 28-byte mutex,
// counter, guard switch to 8-byte null mutex and back -> cleanup -> resume
// -> raw shared counter survives -> mirror-header failure keeps sandy and
// clears copy -> sandy-header failure clears both -> shm_open failure.
//
// The probe interposes global operator new/delete.  Every allocation gets a
// 32-byte 0xA5 canary after the requested size, so a model that allocates
// 4 bytes for ZEN_Thread_Mutex while its real constructor writes 28 is
// detected even before the heap is corrupted.  Requested sizes 8, 12, 24,
// 28, 76 and 10292 are counted; live critical allocations are checked after
// each cleanup.
//
// Build/run commands: see build_and_run.sh in this directory.  The rebuilt
// library must be rebuilt after any source change before running the probe.

#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <new>

namespace probe_alloc {

struct Header {
    uint32_t magic;
    uint32_t size;
    uint32_t pad0;
    uint32_t pad1;
};

struct Rec {
    void *ptr;
    uint32_t size;
    bool freed;
};

const uint32_t kMagic = 0x54A5A5A5u;
const unsigned char kCanary = 0xA5;
const uint32_t kCanaryBytes = 32;
const int kMaxRec = 4096;

Rec g_recs[kMaxRec];
int g_nrec = 0;
bool g_track = false;
uint32_t g_canary_bad = 0;
uint32_t g_bad_magic = 0;
uint32_t g_untracked_delete = 0;
uint32_t g_overflow = 0;
uint32_t g_alloc_8 = 0;
uint32_t g_alloc_12 = 0;
uint32_t g_alloc_24 = 0;
uint32_t g_alloc_28 = 0;
uint32_t g_alloc_76 = 0;
uint32_t g_alloc_10292 = 0;

void begin() {
    // Records are per-sequence; diagnostics and requested-size counters stay
    // cumulative so a corruption in the bus phase cannot be hidden by the
    // status phase resetting a counter.
    g_nrec = 0;
    g_track = true;
}

void end() { g_track = false; }

void record_alloc(void *ptr, uint32_t size) {
    if (!g_track) return;
    switch (size) {
    case 8: ++g_alloc_8; break;
    case 12: ++g_alloc_12; break;
    case 24: ++g_alloc_24; break;
    case 28: ++g_alloc_28; break;
    case 76: ++g_alloc_76; break;
    case 10292: ++g_alloc_10292; break;
    default: break;
    }
    if (g_nrec >= kMaxRec) {
        ++g_overflow;
        return;
    }
    g_recs[g_nrec].ptr = ptr;
    g_recs[g_nrec].size = size;
    g_recs[g_nrec].freed = false;
    ++g_nrec;
}

void mark_free(void *ptr) {
    for (int i = g_nrec - 1; i >= 0; --i) {
        if (g_recs[i].ptr == ptr) {
            if (!g_recs[i].freed) {
                g_recs[i].freed = true;
            } else {
                ++g_untracked_delete;
            }
            return;
        }
    }
    ++g_untracked_delete;
}

int live(uint32_t size) {
    int n = 0;
    for (int i = 0; i < g_nrec; ++i) {
        if (!g_recs[i].freed && g_recs[i].size == size) ++n;
    }
    return n;
}

}  // namespace probe_alloc

void *operator new(size_t n) throw(std::bad_alloc) {
    using namespace probe_alloc;
    const size_t total = sizeof(Header) + n + kCanaryBytes;
    unsigned char *raw = (unsigned char *)malloc(total);
    if (!raw) throw std::bad_alloc();
    Header *h = (Header *)raw;
    h->magic = kMagic;
    h->size = (uint32_t)n;
    h->pad0 = 0;
    h->pad1 = 0;
    unsigned char *user = raw + sizeof(Header);
    memset(user + n, kCanary, kCanaryBytes);
    record_alloc(user, (uint32_t)n);
    return user;
}

void *operator new[](size_t n) throw(std::bad_alloc) {
    return operator new(n);
}

void *operator new(size_t n, const std::nothrow_t &) throw() {
    try {
        return operator new(n);
    } catch (...) {
        return 0;
    }
}

void *operator new[](size_t n, const std::nothrow_t &) throw() {
    try {
        return operator new(n);
    } catch (...) {
        return 0;
    }
}

void operator delete(void *p) throw() {
    using namespace probe_alloc;
    if (!p) return;
    unsigned char *raw = (unsigned char *)p - sizeof(Header);
    Header *h = (Header *)raw;
    if (h->magic != kMagic) {
        ++g_bad_magic;
        free(p);
        return;
    }
    const uint32_t n = h->size;
    const unsigned char *canary = (const unsigned char *)p + n;
    for (uint32_t i = 0; i < kCanaryBytes; ++i) {
        if (canary[i] != kCanary) {
            ++g_canary_bad;
            break;
        }
    }
    if (g_track) mark_free(p);
    h->magic = 0;
    free(raw);
}

void operator delete[](void *p) throw() {
    operator delete(p);
}

void operator delete(void *p, const std::nothrow_t &) throw() {
    operator delete(p);
}

void operator delete[](void *p, const std::nothrow_t &) throw() {
    operator delete(p);
}

namespace {

const int kMaxChecks = 200;
struct Check {
    const char *name;
    bool pass;
    char detail[160];
};

Check g_checks[kMaxChecks];
int g_nchecks = 0;
int g_failures = 0;

void add_check(bool pass, const char *name, const char *fmt, ...) {
    if (g_nchecks >= kMaxChecks) return;
    Check &c = g_checks[g_nchecks++];
    c.name = name;
    c.pass = pass;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(c.detail, sizeof(c.detail), fmt, ap);
    va_end(ap);
    if (!pass) ++g_failures;
}

uint32_t rd32(const void *base, size_t off) {
    uint32_t v = 0;
    memcpy(&v, (const char *)base + off, sizeof(v));
    return v;
}

uint64_t rd64(const void *base, size_t off) {
    uint64_t v = 0;
    memcpy(&v, (const char *)base + off, sizeof(v));
    return v;
}

void *rdptr(const void *base, size_t off) {
    void *v = 0;
    memcpy(&v, (const char *)base + off, sizeof(v));
    return v;
}

void wr32(void *base, size_t off, uint32_t v) {
    memcpy((char *)base + off, &v, sizeof(v));
}

bool read_file_u32(const char *path, off_t off, uint32_t *out) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return false;
    ssize_t n = pread(fd, out, sizeof(*out), off);
    close(fd);
    return n == (ssize_t)sizeof(*out);
}

bool write_file_u32(const char *path, off_t off, uint32_t v) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return false;
    ssize_t n = pwrite(fd, &v, sizeof(v), off);
    close(fd);
    return n == (ssize_t)sizeof(v);
}

bool create_file(const char *path, size_t size) {
    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) return false;
    char buf[64];
    memset(buf, 0x5a, sizeof(buf));
    size_t left = size;
    bool ok = true;
    while (left > 0) {
        size_t n = left < sizeof(buf) ? left : sizeof(buf);
        if (write(fd, buf, n) != (ssize_t)n) {
            ok = false;
            break;
        }
        left -= n;
    }
    close(fd);
    return ok;
}

void *must_sym(void *handle, const char *name, const char *what,
               char *error, size_t error_len) {
    dlerror();
    void *p = dlsym(handle, name);
    const char *e = dlerror();
    if (e || !p) {
        snprintf(error, error_len, "missing %s (%s)%s%s", what, name,
                 e ? ": " : "", e ? e : "");
        return 0;
    }
    return p;
}

// Bus function pointers (i386 cdecl: `this` is the first stack argument).
typedef void *(*Fn_bus_instance)();
typedef void (*Fn_bus_clean)();
typedef int (*Fn_bus_init7)(void *, const char *, unsigned int,
                            unsigned int *, unsigned int, bool, bool,
                            unsigned char);
typedef int (*Fn_bus_init3)(void *, const char *, unsigned int,
                            unsigned char);
typedef bool (*Fn_deque_push)(void *, const void *);
typedef bool (*Fn_deque_pop)(void *, void *);
typedef bool (*Fn_deque_empty)(void *);

struct Node {
    uint32_t size_of_node_;
    unsigned char chunkdata[64];
};

const uint32_t kBusObjectSize = 0x2834u;
const uint32_t kBusHeadOff = 0x1004u;
const uint32_t kBusPipesOff = 0x200cu;
const uint32_t kBusMmapOff = 0x282cu;
const uint32_t kBusRoomOverhead = 0x1cu;
const uint32_t kBusHeaderSize = 0x1008u;

int run_bus(void *lib, const char *dir, char *error, size_t error_len) {
    char path[512];
    char path_bad[512];
    char path_small[512];
    char path_missing[512];
    snprintf(path, sizeof(path), "%s/bus.mmap", dir);
    snprintf(path_bad, sizeof(path_bad), "%s/bus_bad.mmap", dir);
    snprintf(path_small, sizeof(path_small), "%s/bus_small.bin", dir);
    snprintf(path_missing, sizeof(path_missing), "%s/bus_missing.mmap", dir);
    unlink(path);
    unlink(path_bad);
    unlink(path_small);
    unlink(path_missing);

    Fn_bus_instance inst = (Fn_bus_instance)must_sym(
        lib, "_ZN16ZEN_Bus_MMAPPipe8instanceEv", "ZEN_Bus_MMAPPipe::instance",
        error, error_len);
    Fn_bus_clean clean = (Fn_bus_clean)must_sym(
        lib, "_ZN16ZEN_Bus_MMAPPipe14clean_instanceEv",
        "ZEN_Bus_MMAPPipe::clean_instance", error, error_len);
    Fn_bus_init7 init7 = (Fn_bus_init7)must_sym(
        lib, "_ZN16ZEN_Bus_MMAPPipe10initializeEPKcjPjjbbh",
        "ZEN_Bus_MMAPPipe::initialize(7)", error, error_len);
    Fn_bus_init3 init3 = (Fn_bus_init3)must_sym(
        lib, "_ZN16ZEN_Bus_MMAPPipe10initializeEPKcjh",
        "ZEN_Bus_MMAPPipe::initialize(3)", error, error_len);
    Fn_deque_push push = (Fn_deque_push)must_sym(
        lib, "_ZN7ZEN_LIB14shm_dequechunk8push_endEPKNS_15dequechunk_nodeE",
        "shm_dequechunk::push_end", error, error_len);
    Fn_deque_pop pop = (Fn_deque_pop)must_sym(
        lib, "_ZN7ZEN_LIB14shm_dequechunk9pop_frontEPNS_15dequechunk_nodeE",
        "shm_dequechunk::pop_front", error, error_len);
    Fn_deque_empty empty = (Fn_deque_empty)must_sym(
        lib, "_ZN7ZEN_LIB14shm_dequechunk5emptyEv",
        "shm_dequechunk::empty", error, error_len);
    if (!inst || !clean || !init7 || !init3 || !push || !pop || !empty)
        return -1;

    const uint32_t sizes[2] = {0x800000u, 0x800000u};
    const uint32_t sizes_bad[2] = {1024u, 16u};
    const uint32_t sizes_mismatch[2] = {0x400000u, 16u};
    const unsigned char payload[12] = {
        's', 't', 'a', 'r', 't', 'u', 'p', '-', 'i', 'p', 'c', '!'};

    // Drop any singleton created by the library's static initialisers before
    // tracking starts, so the first allocation is observed by the probe.
    clean();

    probe_alloc::begin();

    // ctor / singleton.
    void *bus = inst();
    add_check(bus != 0, "bus.instance.ptr", "obj=%p", bus);
    add_check(probe_alloc::live(kBusObjectSize) == 1,
              "bus.instance.alloc_10292",
              "live=%d allocs=%u",
              probe_alloc::live(kBusObjectSize),
              probe_alloc::g_alloc_10292);
    add_check(bus && rd32(bus, 0x1008) == 0, "bus.ctor.pipe_count_zero",
              "value=%u", bus ? rd32(bus, 0x1008) : 0);
    add_check(bus && ((unsigned char *)bus)[0x2830] == 0,
              "bus.ctor.mem_type_zero", "value=%u",
              bus ? ((unsigned char *)bus)[0x2830] : 0);

    // Unsupported mem_type and zero pipe-count failure branches.
    int ret = init7(bus, path, 2, (unsigned int *)sizes, 1024, false, false,
                    3);
    add_check(ret == -1, "bus.fresh.unsupported_type", "ret=%d", ret);
    ret = init7(bus, path, 0, (unsigned int *)sizes, 1024, false, false, 1);
    add_check(ret == -1, "bus.fresh.zero_pipe", "ret=%d", ret);

    // Reset object state before the fresh create.
    clean();
    bus = inst();

    // Fresh create: 2 x 8 MiB pipes.  ORIG header is 4,2,8388608,8388608.
    ret = init7(bus, path, 2, (unsigned int *)sizes, 1024, false, false, 1);
    add_check(ret == 0, "bus.fresh.ret", "ret=%d", ret);
    add_check(rd32(bus, kBusHeadOff + 0) == 4, "bus.fresh.size_of_sizet",
              "value=%u", rd32(bus, kBusHeadOff + 0));
    add_check(rd32(bus, kBusHeadOff + 4) == 2, "bus.fresh.number_of_pipe",
              "value=%u", rd32(bus, kBusHeadOff + 4));
    add_check(rd32(bus, kBusHeadOff + 8) == 0x800000u &&
                  rd32(bus, kBusHeadOff + 12) == 0x800000u,
              "bus.fresh.pipe_sizes",
              "%u,%u", rd32(bus, kBusHeadOff + 8),
              rd32(bus, kBusHeadOff + 12));
    const uint32_t room = 0x800000u + kBusRoomOverhead;
    add_check(rd32(bus, kBusHeadOff + 0x808) == room &&
                  rd32(bus, kBusHeadOff + 0x80c) == room,
              "bus.fresh.pipe_rooms", "%u,%u",
              rd32(bus, kBusHeadOff + 0x808),
              rd32(bus, kBusHeadOff + 0x80c));

    uint32_t fv = 0;
    add_check(read_file_u32(path, 0, &fv) && fv == 4,
              "bus.fresh.file_size_of_sizet", "value=%u", fv);
    add_check(read_file_u32(path, 4, &fv) && fv == 2,
              "bus.fresh.file_number_of_pipe", "value=%u", fv);
    add_check(read_file_u32(path, 8, &fv) && fv == 0x800000u,
              "bus.fresh.file_pipe0_size", "value=%u", fv);
    add_check(read_file_u32(path, 12, &fv) && fv == 0x800000u,
              "bus.fresh.file_pipe1_size", "value=%u", fv);

    void *mmap_base = rdptr(bus, kBusMmapOff);
    void *pipe0 = rdptr(bus, kBusPipesOff + 0);
    void *pipe1 = rdptr(bus, kBusPipesOff + 4);
    add_check(mmap_base != 0 && pipe0 != 0 && pipe1 != 0,
              "bus.fresh.pipe_pointers", "mmap=%p p0=%p p1=%p", mmap_base,
              pipe0, pipe1);
    if (mmap_base && pipe0 && pipe1) {
        void *want0 = (char *)mmap_base + kBusHeaderSize;
        void *want1 = (char *)want0 + room;
        add_check(rdptr(pipe0, 0) == want0 && rdptr(pipe0, 4) == want0,
                  "bus.fresh.pipe0_points_into_mmap", "base=%p head=%p want=%p",
                  rdptr(pipe0, 0), rdptr(pipe0, 4), want0);
        add_check(rdptr(pipe1, 0) == want1 && rdptr(pipe1, 4) == want1,
                  "bus.fresh.pipe1_points_into_mmap", "base=%p head=%p want=%p",
                  rdptr(pipe1, 0), rdptr(pipe1, 4), want1);
    }
    add_check(probe_alloc::live(24) == 2, "bus.fresh.two_dequechunks",
              "live=%d allocs=%u", probe_alloc::live(24),
              probe_alloc::g_alloc_24);

    // Push one frame into pipe 0 and verify the deque is non-empty.
    Node node;
    memset(&node, 0, sizeof(node));
    node.size_of_node_ = 16;
    memcpy(node.chunkdata, payload, sizeof(payload));
    add_check(pipe0 && push(pipe0, &node), "bus.fresh.push",
              "pipe0=%p", pipe0);
    add_check(pipe0 && !empty(pipe0), "bus.fresh.not_empty", "pipe0=%p",
              pipe0);

    // Cleanup must release the object and both dequechunk objects.
    clean();
    add_check(probe_alloc::live(kBusObjectSize) == 0, "bus.clean.object",
              "live=%d", probe_alloc::live(kBusObjectSize));
    add_check(probe_alloc::live(24) == 0, "bus.clean.dequechunks",
              "live=%d", probe_alloc::live(24));

    // Corrupt the unvalidated header tail (index 2 of 2 pipes).  ORIG
    // rewrites the mapping from the local header and clears it.
    add_check(write_file_u32(path, 0x10, 0xDEADBEEFu),
              "bus.resume.corrupt_tail_pipe", "path=%s", path);
    add_check(write_file_u32(path, 0x810, 0xCAFEBABEu),
              "bus.resume.corrupt_tail_room", "path=%s", path);

    bus = inst();
    ret = init7(bus, path, 2, (unsigned int *)sizes, 1024, true, false, 1);
    add_check(ret == 0, "bus.resume.ret", "ret=%d", ret);
    add_check(rd32(bus, kBusHeadOff + 0) == 4 &&
                  rd32(bus, kBusHeadOff + 4) == 2,
              "bus.resume.header", "sizet=%u pipes=%u",
              rd32(bus, kBusHeadOff + 0), rd32(bus, kBusHeadOff + 4));
    add_check(rd32(bus, kBusHeadOff + 0x10) == 0,
              "bus.resume.tail_size_of_pipe2",
              "value=%u (ORIG local->mmap clears it)",
              rd32(bus, kBusHeadOff + 0x10));
    add_check(rd32(bus, kBusHeadOff + 0x810) == 0,
              "bus.resume.tail_size_of_room2",
              "value=%u (ORIG local->mmap clears it)",
              rd32(bus, kBusHeadOff + 0x810));

    pipe0 = rdptr(bus, kBusPipesOff + 0);
    add_check(pipe0 != 0 && !empty(pipe0), "bus.resume.frame_persisted",
              "pipe0=%p", pipe0);
    Node out;
    memset(&out, 0, sizeof(out));
    add_check(pipe0 && pop(pipe0, &out), "bus.resume.pop", "pipe0=%p", pipe0);
    add_check(out.size_of_node_ == 16 &&
                  memcmp(out.chunkdata, payload, sizeof(payload)) == 0,
              "bus.resume.payload", "len=%u", out.size_of_node_);

    clean();
    add_check(read_file_u32(path, 0x10, &fv) && fv == 0,
              "bus.resume.file_tail_size_of_pipe2", "value=%u", fv);
    add_check(read_file_u32(path, 0x810, &fv) && fv == 0,
              "bus.resume.file_tail_size_of_room2", "value=%u", fv);

    // 3-arg attach overload: mmap -> local (ORIG 0x1b4d40).
    bus = inst();
    ret = init3(bus, path, 1024, 1);
    add_check(ret == 0, "bus.attach.ret", "ret=%d", ret);
    add_check(rd32(bus, kBusHeadOff + 0) == 4 &&
                  rd32(bus, kBusHeadOff + 4) == 2,
              "bus.attach.header", "sizet=%u pipes=%u",
              rd32(bus, kBusHeadOff + 0), rd32(bus, kBusHeadOff + 4));
    clean();

    // Partial init failure: pipe 0 (1024) succeeds, pipe 1 (16) is rejected by
    // shm_dequechunk::initialize; cleanup must still delete pipe 0.
    bus = inst();
    ret = init7(bus, path_bad, 2, (unsigned int *)sizes_bad, 1024, false,
                false, 1);
    add_check(ret == -1, "bus.partial.ret", "ret=%d", ret);
    pipe0 = rdptr(bus, kBusPipesOff + 0);
    pipe1 = rdptr(bus, kBusPipesOff + 4);
    add_check(pipe0 != 0 && pipe1 == 0, "bus.partial.pointers",
              "p0=%p p1=%p", pipe0, pipe1);
    add_check(probe_alloc::live(24) == 1, "bus.partial.live_pipe",
              "live=%d", probe_alloc::live(24));
    clean();
    add_check(probe_alloc::live(24) == 0, "bus.partial.clean",
              "live=%d", probe_alloc::live(24));

    // Restore the partially-created file: header geometry matches, pipe 0's
    // chunk header is valid, pipe 1's is still zero, so init_all_pipe fails
    // after allocating pipe 0.  Cleanup must delete that wrapper.
    bus = inst();
    ret = init7(bus, path_bad, 2, (unsigned int *)sizes_bad, 1024, true,
                false, 1);
    add_check(ret == -1, "bus.restore_partial.ret", "ret=%d", ret);
    add_check(rdptr(bus, kBusPipesOff + 0) != 0 &&
                  rdptr(bus, kBusPipesOff + 4) == 0,
              "bus.restore_partial.pointers", "p0=%p p1=%p",
              rdptr(bus, kBusPipesOff + 0), rdptr(bus, kBusPipesOff + 4));
    clean();
    add_check(probe_alloc::live(24) == 0, "bus.restore_partial.clean",
              "live=%d", probe_alloc::live(24));

    // Restore with mismatched geometry must fail before init_all_pipe.
    bus = inst();
    ret = init7(bus, path_bad, 2, (unsigned int *)sizes_mismatch, 1024, true,
                false, 1);
    add_check(ret == -1, "bus.restore_mismatch.ret", "ret=%d", ret);
    add_check(rdptr(bus, kBusPipesOff + 0) == 0 &&
                  rdptr(bus, kBusPipesOff + 4) == 0,
              "bus.restore_mismatch.no_pipes", "p0=%p p1=%p",
              rdptr(bus, kBusPipesOff + 0), rdptr(bus, kBusPipesOff + 4));
    clean();

    // Restore with a mismatched max_frame_len fails inside init_all_pipe.
    bus = inst();
    ret = init7(bus, path, 2, (unsigned int *)sizes, 2048, true, false, 1);
    add_check(ret == -1, "bus.restore_maxframe.ret", "ret=%d", ret);
    add_check(rdptr(bus, kBusPipesOff + 0) == 0,
              "bus.restore_maxframe.no_pipe0", "p0=%p",
              rdptr(bus, kBusPipesOff + 0));
    clean();

    // The 3-arg attach overload has the same chunk-header failure path.
    bus = inst();
    ret = init3(bus, path, 2048, 1);
    add_check(ret == -1, "bus.attach_maxframe.ret", "ret=%d", ret);
    add_check(rdptr(bus, kBusPipesOff + 0) == 0,
              "bus.attach_maxframe.no_pipe0", "p0=%p",
              rdptr(bus, kBusPipesOff + 0));
    clean();


    // 7-arg restore with a missing file falls back to the fresh path
    // (stat failure clears if_restore) and succeeds.
    unlink(path_missing);
    bus = inst();
    ret = init7(bus, path_missing, 2, (unsigned int *)sizes, 1024, true,
                false, 1);
    add_check(ret == 0, "bus.restore_missing_creates.ret", "ret=%d", ret);
    add_check(rd32(bus, kBusHeadOff + 0) == 4 &&
                  rd32(bus, kBusHeadOff + 4) == 2,
              "bus.restore_missing_creates.header", "sizet=%u pipes=%u",
              rd32(bus, kBusHeadOff + 0), rd32(bus, kBusHeadOff + 4));
    clean();
    unlink(path_missing);


    // 3-arg attach on a missing file must fail (no restore fallback).
    bus = inst();
    ret = init3(bus, path_missing, 1024, 1);
    add_check(ret == -1, "bus.attach_missing.ret", "ret=%d", ret);
    clean();

    add_check(create_file(path_small, 100), "bus.attach_short.create",
              "path=%s", path_small);
    bus = inst();
    ret = init3(bus, path_small, 1024, 1);
    add_check(ret == -1, "bus.attach_short.ret", "ret=%d", ret);
    clean();

    probe_alloc::end();
    unlink(path);
    unlink(path_bad);
    unlink(path_small);
    unlink(path_missing);
    return 0;
}

typedef void *(*Fn_status_instance)();
typedef void (*Fn_status_clean)();
typedef int (*Fn_status_init3)(void *, const char *, bool, bool);
typedef int (*Fn_status_init4)(void *, const char *, unsigned int,
                               const void *, bool);
typedef void (*Fn_status_guard)(void *, bool);
typedef int (*Fn_status_increase)(void *, unsigned int, unsigned int,
                                  unsigned int, long long);
typedef unsigned long long (*Fn_status_get)(void *, unsigned int, unsigned int,
                                            unsigned int);
typedef void (*Fn_status_copy)(void *);
typedef unsigned int (*Fn_status_num)(void *);

const uint32_t kStatusObjectSize = 76;
const uint32_t kStatusLockOff = 0x04;
const uint32_t kStatusFileMmapOff = 0x10;
const uint32_t kStatusFileHeadOff = 0x1c;
const uint32_t kStatusSandyOff = 0x20;
const uint32_t kStatusMandyOff = 0x24;
const uint32_t kStatusGuardOff = 0x48;
const uint32_t kStatusInitializedOff = 0x49;
const uint32_t kStatusShmSize = 0x300028;
const uint32_t kStatusMagicOff = 0x10;
const uint32_t kStatusMagic = 0x18000c;
const uint32_t kStatusCountMagic = 0x10000;
const uint32_t kStatusMirrorMagicOff = 0x18001c;

void fill_status_item(unsigned char item[92], uint32_t statics_id,
                      const char *name) {
    memset(item, 0, 92);
    wr32(item, 0, statics_id);
    wr32(item, 4, 0);
    wr32(item, 8, 0);
    wr32(item, 12, 11);  // STATICS_ABSOLUTE_VALUE
    uint64_t zero = 0;
    memcpy(item + 16, &zero, sizeof(zero));
    snprintf((char *)item + 24, 65, "%s", name);
}

bool status_shared_headers_ok(void *st, uint32_t magic_off,
                              uint32_t mirror_off) {
    void *base = rdptr(st, kStatusFileHeadOff);
    if (!base) return false;
    if (rdptr(st, kStatusFileMmapOff) != base) return false;
    if (rd32(base, magic_off) != kStatusMagic) return false;
    if (rd32(base, magic_off + 4) != kStatusCountMagic) return false;
    if (rd32(base, mirror_off) != kStatusMagic) return false;
    if (rd32(base, mirror_off + 4) != kStatusCountMagic) return false;
    return true;
}

int run_status(void *lib, char *error, size_t error_len) {
    Fn_status_instance inst = (Fn_status_instance)must_sym(
        lib, "_ZN17ZEN_Server_Status8instanceEv",
        "ZEN_Server_Status::instance", error, error_len);
    Fn_status_clean clean = (Fn_status_clean)must_sym(
        lib, "_ZN17ZEN_Server_Status14clean_instanceEv",
        "ZEN_Server_Status::clean_instance", error, error_len);
    Fn_status_init3 init3 = (Fn_status_init3)must_sym(
        lib, "_ZN17ZEN_Server_Status10initializeEPKcbb",
        "ZEN_Server_Status::initialize(3)", error, error_len);
    Fn_status_init4 init4 = (Fn_status_init4)must_sym(
        lib, "_ZN17ZEN_Server_Status10initializeEPKcjPK24ZEN_STATUS_ITEM_WITHNAMEb",
        "ZEN_Server_Status::initialize(4)", error, error_len);
    Fn_status_guard guard = (Fn_status_guard)must_sym(
        lib, "_ZN17ZEN_Server_Status25modify_multi_thread_guardEb",
        "ZEN_Server_Status::modify_multi_thread_guard", error, error_len);
    Fn_status_increase inc = (Fn_status_increase)must_sym(
        lib, "_ZN17ZEN_Server_Status18increase_by_statidEjjjx",
        "ZEN_Server_Status::increase_by_statid", error, error_len);
    Fn_status_get get = (Fn_status_get)must_sym(
        lib, "_ZN17ZEN_Server_Status11get_counterEjjj",
        "ZEN_Server_Status::get_counter", error, error_len);
    Fn_status_copy copy = (Fn_status_copy)must_sym(
        lib, "_ZN17ZEN_Server_Status17copy_stat_counterEv",
        "ZEN_Server_Status::copy_stat_counter", error, error_len);
    Fn_status_num num = (Fn_status_num)must_sym(
        lib, "_ZN17ZEN_Server_Status14num_of_counterEv",
        "ZEN_Server_Status::num_of_counter", error, error_len);
    if (!inst || !clean || !init3 || !init4 || !guard || !inc || !get ||
        !copy || !num)
        return -1;

    char shm_name[128];
    snprintf(shm_name, sizeof(shm_name), "/tss_startup_ipc_%d_status",
             (int)getpid());
    shm_unlink(shm_name);
    const char *bad_name = "tss_startup_ipc_no_leading_slash";

    unsigned char item[92];
    fill_status_item(item, 100, "probe");

    // Same reset for the status singleton.
    clean();

    probe_alloc::begin();

    // Fresh initialise with one configured item and multi_thread=true.
    void *st = inst();
    add_check(st != 0, "status.instance.ptr", "obj=%p", st);
    add_check(probe_alloc::live(kStatusObjectSize) == 1,
              "status.instance.alloc_76",
              "live=%d allocs=%u", probe_alloc::live(kStatusObjectSize),
              probe_alloc::g_alloc_76);

    int ret = init4(st, shm_name, 1, item, true);
    add_check(ret == 0, "status.fresh.ret", "ret=%d", ret);
    add_check(((unsigned char *)st)[kStatusGuardOff] == 1 &&
                  ((unsigned char *)st)[kStatusInitializedOff] == 1,
              "status.fresh.flags", "guard=%u initialized=%u",
              ((unsigned char *)st)[kStatusGuardOff],
              ((unsigned char *)st)[kStatusInitializedOff]);
    add_check(probe_alloc::live(28) == 1, "status.fresh.thread_mutex_28",
              "live=%d allocs=%u", probe_alloc::live(28),
              probe_alloc::g_alloc_28);
    add_check(probe_alloc::live(8) == 0, "status.fresh.no_null_mutex",
              "live=%d", probe_alloc::live(8));
    add_check(probe_alloc::live(12) == 2, "status.fresh.two_shm_vectors",
              "live=%d allocs=%u", probe_alloc::live(12),
              probe_alloc::g_alloc_12);
    add_check(rdptr(st, kStatusLockOff) != 0, "status.fresh.lock_ptr",
              "lock=%p", rdptr(st, kStatusLockOff));
    add_check(status_shared_headers_ok(st, kStatusMagicOff,
                                       kStatusMirrorMagicOff),
              "status.fresh.shared_magic", "base=%p",
              rdptr(st, kStatusFileHeadOff));

    ret = inc(st, 100, 0, 0, 5);
    add_check(ret == 0, "status.counter.increase_5.ret", "ret=%d", ret);
    add_check(get(st, 100, 0, 0) == 5, "status.counter.value_5", "value=%llu",
              (unsigned long long)get(st, 100, 0, 0));
    copy(st);
    add_check(num(st) == 1, "status.counter.num_after_copy", "num=%u",
              num(st));

    // Delete the 28-byte thread mutex and allocate the 8-byte null mutex.
    guard(st, false);
    add_check(probe_alloc::live(28) == 0, "status.guard_false.thread_freed",
              "live=%d", probe_alloc::live(28));
    add_check(probe_alloc::live(8) == 1, "status.guard_false.null_8",
              "live=%d allocs=%u", probe_alloc::live(8),
              probe_alloc::g_alloc_8);
    ret = inc(st, 100, 0, 0, 3);
    add_check(ret == 0 && get(st, 100, 0, 0) == 8,
              "status.guard_false.counter", "ret=%d value=%llu", ret,
              (unsigned long long)get(st, 100, 0, 0));

    // Switch back: null mutex freed, 28-byte thread mutex allocated.
    guard(st, true);
    add_check(probe_alloc::live(8) == 0, "status.guard_true.null_freed",
              "live=%d", probe_alloc::live(8));
    add_check(probe_alloc::live(28) == 1, "status.guard_true.thread_28",
              "live=%d allocs=%u", probe_alloc::live(28),
              probe_alloc::g_alloc_28);
    ret = inc(st, 100, 0, 0, 2);
    add_check(ret == 0 && get(st, 100, 0, 0) == 10,
              "status.guard_true.counter", "ret=%d value=%llu", ret,
              (unsigned long long)get(st, 100, 0, 0));

    // Destructor must free the object, both 12-byte vectors and the mutex.
    clean();
    add_check(probe_alloc::live(kStatusObjectSize) == 0, "status.clean.object",
              "live=%d", probe_alloc::live(kStatusObjectSize));
    add_check(probe_alloc::live(12) == 0, "status.clean.vectors",
              "live=%d", probe_alloc::live(12));
    add_check(probe_alloc::live(28) == 0, "status.clean.thread_mutex",
              "live=%d", probe_alloc::live(28));
    add_check(probe_alloc::live(8) == 0, "status.clean.null_mutex",
              "live=%d", probe_alloc::live(8));

    // Resume: shared header/counter must survive, multi_thread=false gives the
    // 8-byte null mutex.
    st = inst();
    ret = init3(st, shm_name, true, false);
    add_check(ret == 0, "status.resume.ret", "ret=%d", ret);
    add_check(probe_alloc::live(8) == 1, "status.resume.null_8",
              "live=%d allocs=%u", probe_alloc::live(8),
              probe_alloc::g_alloc_8);
    add_check(probe_alloc::live(28) == 0, "status.resume.no_thread_mutex",
              "live=%d", probe_alloc::live(28));
    add_check(status_shared_headers_ok(st, kStatusMagicOff,
                                       kStatusMirrorMagicOff),
              "status.resume.shared_magic", "base=%p",
              rdptr(st, kStatusFileHeadOff));

    void *sandy = rdptr(st, kStatusSandyOff);
    void *mandy = rdptr(st, kStatusMandyOff);
    add_check(sandy != 0 && mandy != 0, "status.resume.vectors",
              "sandy=%p mandy=%p", sandy, mandy);
    if (sandy) {
        void *head = rdptr(sandy, 4);
        void *data = rdptr(sandy, 8);
        add_check(head && data, "status.resume.vector_fields",
                  "head=%p data=%p", head, data);
        if (head && data) {
            add_check(rd32(head, 8) == 1, "status.resume.num_of_use",
                      "num=%u", rd32(head, 8));
            add_check(rd32(data, 0) == 100, "status.resume.statics_id",
                      "id=%u", rd32(data, 0));
            add_check(rd64(data, 16) == 10, "status.resume.counter",
                      "counter=%llu", (unsigned long long)rd64(data, 16));
        }
    }
    clean();

    // Mirror header failure: sandy is allocated, copy is cleared, cleanup
    // deletes the partial sandy vector.
    {
        int fd = shm_open(shm_name, O_RDWR, 0);
        add_check(fd >= 0, "status.mirror_failure.shm_open", "fd=%d", fd);
        if (fd >= 0) {
            void *map = mmap(0, kStatusShmSize, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
            add_check(map != MAP_FAILED, "status.mirror_failure.mmap",
                      "map=%p", map);
            if (map != MAP_FAILED) {
                wr32(map, kStatusMirrorMagicOff, 0);
                munmap(map, kStatusShmSize);
            }
            close(fd);
        }
    }
    st = inst();
    ret = init3(st, shm_name, true, false);
    add_check(ret == -1, "status.mirror_failure.ret", "ret=%d", ret);
    add_check(rdptr(st, kStatusSandyOff) != 0 &&
                  rdptr(st, kStatusMandyOff) == 0,
              "status.mirror_failure.pointers", "sandy=%p mandy=%p",
              rdptr(st, kStatusSandyOff), rdptr(st, kStatusMandyOff));
    clean();
    add_check(probe_alloc::live(12) == 0,
              "status.mirror_failure.clean_vectors", "live=%d",
              probe_alloc::live(12));

    // Recreate a valid header, then fail on the sandy header.
    st = inst();
    ret = init4(st, shm_name, 1, item, true);
    add_check(ret == 0, "status.sandy_failure.recreate", "ret=%d", ret);
    clean();
    {
        int fd = shm_open(shm_name, O_RDWR, 0);
        add_check(fd >= 0, "status.sandy_failure.shm_open", "fd=%d", fd);
        if (fd >= 0) {
            void *map = mmap(0, kStatusShmSize, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
            add_check(map != MAP_FAILED, "status.sandy_failure.mmap",
                      "map=%p", map);
            if (map != MAP_FAILED) {
                wr32(map, kStatusMagicOff, 0);
                munmap(map, kStatusShmSize);
            }
            close(fd);
        }
    }
    st = inst();
    ret = init3(st, shm_name, true, false);
    add_check(ret == -1, "status.sandy_failure.ret", "ret=%d", ret);
    add_check(rdptr(st, kStatusSandyOff) == 0 &&
                  rdptr(st, kStatusMandyOff) == 0,
              "status.sandy_failure.pointers", "sandy=%p mandy=%p",
              rdptr(st, kStatusSandyOff), rdptr(st, kStatusMandyOff));
    clean();

    // shm_open failure (name without the required leading slash).
    st = inst();
    ret = init3(st, bad_name, true, false);
    add_check(ret == -1, "status.open_failure.ret", "ret=%d", ret);
    clean();

    // Singleton identity and clean_instance.
    st = inst();
    add_check(st == inst(), "status.singleton.identity", "obj=%p", st);
    clean();
    void *st2 = inst();
    add_check(st2 != 0, "status.singleton.recreated", "obj=%p", st2);
    clean();

    probe_alloc::end();
    shm_unlink(shm_name);
    return 0;
}

void print_report(const char *library) {
    printf("{\n  \"library\": \"%s\",\n  \"checks\": [\n", library);
    for (int i = 0; i < g_nchecks; ++i) {
        printf("    {\"name\": \"%s\", \"pass\": %s, \"detail\": \"%s\"}%s\n",
               g_checks[i].name, g_checks[i].pass ? "true" : "false",
               g_checks[i].detail, i + 1 == g_nchecks ? "" : ",");
    }
    printf("  ],\n");
    printf("  \"failures\": %d,\n", g_failures);
    printf("  \"canary_bad\": %u,\n", probe_alloc::g_canary_bad);
    printf("  \"bad_magic\": %u,\n", probe_alloc::g_bad_magic);
    printf("  \"untracked_delete\": %u,\n",
           probe_alloc::g_untracked_delete);
    printf("  \"record_overflow\": %u,\n", probe_alloc::g_overflow);
    printf("  \"allocs\": {\"8\": %u, \"12\": %u, \"24\": %u, \"28\": %u, "
           "\"76\": %u, \"10292\": %u},\n",
           probe_alloc::g_alloc_8, probe_alloc::g_alloc_12,
           probe_alloc::g_alloc_24, probe_alloc::g_alloc_28,
           probe_alloc::g_alloc_76, probe_alloc::g_alloc_10292);
    printf("  \"result\": \"%s\"\n}\n", g_failures ? "FAIL" : "PASS");
}

}  // namespace

int main(int argc, char **argv) {
    const char *library = 0;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--lib") == 0 && i + 1 < argc) {
            library = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("usage: %s --lib PATH\n", argv[0]);
            return 0;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
            return 2;
        }
    }
    if (!library) {
        fprintf(stderr, "--lib PATH is required\n");
        return 2;
    }

    char dir[256];
    snprintf(dir, sizeof(dir), "/tmp/tss_startup_ipc_%d", (int)getpid());
    if (mkdir(dir, 0700) != 0 && errno != EEXIST) {
        fprintf(stderr, "mkdir %s failed: %s\n", dir, strerror(errno));
        return 2;
    }

    dlerror();
    void *lib = dlopen(library, RTLD_NOW | RTLD_LOCAL);
    const char *dl_error = dlerror();
    if (!lib) {
        printf("{\n  \"library\": \"%s\",\n  \"failures\": 1,\n"
               "  \"result\": \"FAIL\",\n  \"error\": \"%s\"\n}\n",
               library, dl_error ? dl_error : "dlopen failed");
        rmdir(dir);
        return 2;
    }

    char error[512];
    error[0] = '\0';
    int rc = 0;
    if (run_bus(lib, dir, error, sizeof(error)) != 0) {
        add_check(false, "bus.symbols", "%s", error);
        rc = 1;
    }
    error[0] = '\0';
    if (run_status(lib, error, sizeof(error)) != 0) {
        add_check(false, "status.symbols", "%s", error);
        rc = 1;
    }

    // Allocator guards are cumulative across both sequences: a 4-byte
    // ZEN_Thread_Mutex model makes the real 28-byte constructor overwrite the
    // 32-byte canary.
    add_check(probe_alloc::g_canary_bad == 0, "alloc.canary_32", "bad=%u",
              probe_alloc::g_canary_bad);
    add_check(probe_alloc::g_bad_magic == 0, "alloc.new_magic", "bad=%u",
              probe_alloc::g_bad_magic);
    add_check(probe_alloc::g_overflow == 0, "alloc.records", "overflow=%u",
              probe_alloc::g_overflow);

    print_report(library);
    dlclose(lib);
    rmdir(dir);
    return (g_failures || rc) ? 1 : 0;
}
