// Differential cipher matrix against the ORIG libtss_sdk.
//
// The existing recon/test_cryptdata.cpp only exercises CCryptData id 0..30
// with benc=true and a fixed 128-byte buffer.  That misses:
//   - decrypt direction,
//   - every non-block-aligned remainder (the TsLocal Seattos /
//     ProcessLastBytes tail),
//   - Cipher(), which is the path DPVMBaseEngine actually calls.
//
// This probe is intentionally a *separate process per library*: it dlopen()s
// exactly one library (RTLD_NOW|RTLD_LOCAL) and prints one deterministic line
// per case.  The driver runs it once for the ORIG and once for the rebuilt
// library and diffs the two outputs byte-for-byte.
//
// Build (WSL, 32-bit because the target library is ELF32 i386):
//   g++ -m32 -O2 -o /tmp/test_cipher_matrix \
//       recon/test_cipher_matrix.cpp -ldl
//
// Run:
//   /tmp/test_cipher_matrix <libtss_sdk.so> cipher  > /tmp/mx_orig.txt
//   /tmp/test_cipher_matrix <libtss_sdk.so> ccrypt  > /tmp/mx_orig_cc.txt
//   /tmp/test_cipher_matrix <libtss_sdk.so> direct  > /tmp/mx_orig_dir.txt
//   ...same for the rebuilt library, then diff the files.
//   Use `direct <TenName>` to isolate one algorithm (state/crash isolation).
//
// Cipher ids 0..20 are covered (0 and >19 are the reject boundary).  The
// CCryptData 27-entry dispatch (0..26) is covered.  The `direct` mode calls
// every block/stream wrapper used by CCryptData plus Mars.  The `captured`
// mode replays the exact flow1-S-89 item19 ciphertext (alg 13, 57 bytes,
// seed 0x0594e806) and checks the ORIG plaintext.
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

typedef int (*CipherFn)(int, unsigned char *, unsigned int, unsigned int,
                        bool);
typedef int (*EncryptDataFn)(void *, unsigned short, unsigned char *, bool,
                             unsigned char *, unsigned int);
typedef void (*TenFn)(unsigned char *, bool, unsigned char *, unsigned int);

// CCryptData ids 1..26 (id 0 is the no-op).
static const char *kDirect[] = {
    "TenAdder",     "TenBlowFish16", "TenBlowFish24", "TenBlowFish32",
    "TenBlowFish56", "TenBlowFish8", "TenCast",      "TenD3des161",
    "TenD3des162",  "TenDes2P",     "TenDesBig",    "TenDesMo",
    "TenDesNew",    "TenDesPC",     "TenDesToo",    "TenFastDes",
    "TenFeal8",     "TenGene",      "TenGost",      "TenGramary",
    "TenIdea",      "TenMars",      "TenRc6",       "TenShift",
    "TenSubtracter", "TenTean"
};

// Multiples of every block size, plus every remainder 0..15 and the
// 57-byte captured-item19 length.
static const unsigned int kLengths[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,
    13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
    26,  27,  28,  29,  30,  31,  32,  33,  40,  41,  47,  48,  49,
    56,  57,  63,  64,  65,  72,  73,  79,  80,  95,  96,  97,  127,
    128, 129, 191, 192, 193, 255, 256, 257, 511, 512, 513, 1023,
    1024, 1025, 2048, 2049, 2220, 2221, 2222, 4096, 4097
};

static const unsigned int kSeeds[] = {
    0x00000000u, 0x0594e806u, 0xdeadbeefu, 0xffffffffu, 0x12345678u
};

static const char *kCapturedKey =
    "2952db0322dcd196c709ef4615da901a07a71b17cf225923ebff3de2e800cf60";

static int nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

static void make_key(unsigned char *key, unsigned int which)
{
    unsigned int i;
    if (which == 2) {
        size_t n = strlen(kCapturedKey) / 2;
        for (i = 0; i < 128; ++i)
            key[i] = (unsigned char)((nibble(kCapturedKey[(i % n) * 2]) << 4) |
                                     nibble(kCapturedKey[(i % n) * 2 + 1]));
        return;
    }
    for (i = 0; i < 128; ++i) {
        if (which == 0)
            key[i] = (unsigned char)((i * 7u + 3u) & 0xffu);
        else
            key[i] = (unsigned char)((i * 13u + 0x5au) & 0xffu);
    }
}

static void make_input(unsigned char *buf, unsigned int len, unsigned int id,
                       unsigned int dir, unsigned int keyid)
{
    unsigned int i;
    for (i = 0; i < len; ++i)
        buf[i] = (unsigned char)((i * 37u + id * 11u + dir * 3u + keyid * 5u +
                                  1u) & 0xffu);
}

// Two independent 64-bit hashes; the driver compares both.  A mismatch is
// reported with the line index, and the case can be re-run in a debugger.
static void hash_buf(const unsigned char *p, size_t n, unsigned long long &h1,
                     unsigned long long &h2)
{
    size_t i;
    unsigned long long a = 1469598103934665603ULL;
    unsigned long long b = 1099511628211ULL;
    for (i = 0; i < n; ++i) {
        a ^= (unsigned long long)p[i];
        a *= 1099511628211ULL;
        b += (unsigned long long)p[i] + 0x9e3779b97f4a7c15ULL;
        b ^= (b >> 29);
        b *= 0xbf58476d1ce4e5b9ULL;
    }
    h1 = a;
    h2 = b;
}

static unsigned char *make_buffer(unsigned int len)
{
    unsigned char *p = (unsigned char *)malloc((size_t)len + 64);
    if (p == 0) {
        fprintf(stderr, "out of memory\n");
        exit(2);
    }
    memset(p, 0x5a, 32);
    memset(p + 32, 0xa5, (size_t)len + 32);
    return p;
}

static void print_case(const char *kind, const char *name, unsigned int len,
                       unsigned int dir, unsigned int keyid, int ret,
                       const unsigned char *buf, unsigned int pre_ok,
                       unsigned int post_ok)
{
    unsigned long long h1, h2;
    hash_buf(buf, (size_t)len + 64, h1, h2);
    printf("%s|%s|%u|%u|%u|%d|%016llx|%016llx|%u|%u\n", kind, name, len,
           dir, keyid, ret, h1, h2, pre_ok, post_ok);
}

static std::string mangled(const char *name)
{
    char prefix[32];
    sprintf(prefix, "_Z%u", (unsigned int)strlen(name));
    return std::string(prefix) + name + "PhbS_j";
}

static int run_cipher(void *h, const char *only)
{
    (void)only;
    CipherFn fn = (CipherFn)dlsym(h, "_Z6CipheriPhjjb");
    if (fn == 0) {
        fprintf(stderr, "missing Cipher\n");
        return 2;
    }
    unsigned int id, li, si, d;
    for (id = 0; id <= 20; ++id) {
        for (li = 0; li < sizeof(kLengths) / sizeof(kLengths[0]); ++li) {
            unsigned int len = kLengths[li];
            for (si = 0; si < sizeof(kSeeds) / sizeof(kSeeds[0]); ++si) {
                for (d = 0; d <= 1; ++d) {
                    unsigned char *p = make_buffer(len);
                    make_input(p + 32, len, id, d, si);
                    int ret = fn((int)id, p + 32, len, kSeeds[si], d != 0);
                    unsigned int pre = 1, post = 1;
                    unsigned int i;
                    for (i = 0; i < 32; ++i) {
                        if (p[i] != 0x5a) pre = 0;
                        if (p[32 + len + i] != 0xa5) post = 0;
                    }
                    char name[32];
                    sprintf(name, "cipher%u", id);
                    print_case("C", name, len, d, si, ret, p, pre, post);
                    free(p);
                }
            }
        }
    }
    return 0;
}

static void hex_decode(const char *s, unsigned char *out, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; ++i)
        out[i] = (unsigned char)((nibble(s[i * 2]) << 4) |
                                 nibble(s[i * 2 + 1]));
}

// Exact flow1-S-89 item19 (2026-10-19T05:33:59.700Z): the first BUY response
// after C-88, alg 13, key seed 0x0594e806, 57-byte fix payload (inner op14
// UPDATE_ITEM_LIST 47B + identity/seq/pad1).  This is captured ciphertext,
// not a source-text default.  ORIG plaintext ends in 9 bytes produced by
// ITsLocal::Seattos, which the pre-fix TenMars wrapper left as ciphertext.
static int run_captured(void *h, const char *only)
{
    (void)only;
    static const char *kCiphertext57 =
        "ef4474a5f457ae636a65596ea8d8aa9add0f94460093b706b339d8f4ef9deb70"
        "52f917a7975ff4c677c4a72dc593666e4b927cb8abedfd118a";
    static const char *kPlain57 =
        "5beab0b10000000001f7000e002f0000"
        "00ca54a28fca54a28f00010001000100"
        "00000001000000000000000000000000"
        "00000000002e0990cf";
    CipherFn fn = (CipherFn)dlsym(h, "_Z6CipheriPhjjb");
    unsigned char buf[57];
    unsigned char expect[57];
    unsigned int i;
    int ret;
    int match = 1;
    if (fn == 0) {
        fprintf(stderr, "missing Cipher\n");
        return 2;
    }
    hex_decode(kCiphertext57, buf, 57);
    hex_decode(kPlain57, expect, 57);
    ret = fn(13, buf, 57, 0x0594e806u, false);
    for (i = 0; i < 57; ++i)
        if (buf[i] != expect[i]) match = 0;
    printf("captured item19 alg=13 len=57 ret=%d match=%d plain=", ret,
           match);
    for (i = 0; i < 57; ++i) printf("%02x", buf[i]);
    printf("\n");
    return match ? 0 : 1;
}

static int run_ccrypt(void *h, const char *only)
{
    (void)only;
    EncryptDataFn fn =
        (EncryptDataFn)dlsym(h, "_ZN10CCryptData11EncryptDataEtPhbS0_j");
    if (fn == 0) {
        fprintf(stderr, "missing CCryptData::EncryptData\n");
        return 2;
    }
    unsigned int id, li, ki, d;
    for (id = 0; id <= 26; ++id) {
        for (li = 0; li < sizeof(kLengths) / sizeof(kLengths[0]); ++li) {
            unsigned int len = kLengths[li];
            for (ki = 0; ki < 3; ++ki) {
                unsigned char key[128];
                make_key(key, ki);
                for (d = 0; d <= 1; ++d) {
                    unsigned char *p = make_buffer(len);
                    make_input(p + 32, len, id, d, ki);
                    int ret = fn(0, (unsigned short)id, key, d != 0, p + 32,
                                 len);
                    unsigned int pre = 1, post = 1;
                    unsigned int i;
                    for (i = 0; i < 32; ++i) {
                        if (p[i] != 0x5a) pre = 0;
                        if (p[32 + len + i] != 0xa5) post = 0;
                    }
                    char name[32];
                    sprintf(name, "ccrypt%u", id);
                    print_case("D", name, len, d, ki, ret, p, pre, post);
                    free(p);
                }
            }
        }
    }
    return 0;
}

static int run_direct(void *h, const char *only)
{
    unsigned int n = sizeof(kDirect) / sizeof(kDirect[0]);
    unsigned int ni, li, ki, d;
    for (ni = 0; ni < n; ++ni) {
        const char *name = kDirect[ni];
        if (only != 0 && only[0] != 0 && strcmp(only, name) != 0)
            continue;
        std::string sym = mangled(name);
        TenFn fn = (TenFn)dlsym(h, sym.c_str());
        if (fn == 0) {
            fprintf(stderr, "missing %s (%s)\n", name, sym.c_str());
            return 2;
        }
        for (li = 0; li < sizeof(kLengths) / sizeof(kLengths[0]); ++li) {
            unsigned int len = kLengths[li];
            for (ki = 0; ki < 3; ++ki) {
                unsigned char key[128];
                make_key(key, ki);
                for (d = 0; d <= 1; ++d) {
                    unsigned char *p = make_buffer(len);
                    make_input(p + 32, len, ni, d, ki);
                    fn(key, d != 0, p + 32, len);
                    unsigned int pre = 1, post = 1;
                    unsigned int i;
                    for (i = 0; i < 32; ++i) {
                        if (p[i] != 0x5a) pre = 0;
                        if (p[32 + len + i] != 0xa5) post = 0;
                    }
                    print_case("T", name, len, d, ki, -1, p, pre, post);
                    free(p);
                }
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr,
                "usage: %s <libtss_sdk.so> "
                "cipher|ccrypt|direct|captured [TenName]\n",
                argv[0]);
        return 2;
    }
    srand(12345);
    void *h = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (h == 0) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return 2;
    }
    const char *mode = argv[2];
    const char *only = argc > 3 ? argv[3] : 0;
    int ret;
    if (strcmp(mode, "cipher") == 0)
        ret = run_cipher(h, only);
    else if (strcmp(mode, "ccrypt") == 0)
        ret = run_ccrypt(h, only);
    else if (strcmp(mode, "captured") == 0)
        ret = run_captured(h, only);
    else if (strcmp(mode, "direct") == 0)
        ret = run_direct(h, only);
    else {
        fprintf(stderr, "unknown mode %s\n", mode);
        ret = 2;
    }
    dlclose(h);
    return ret;
}
