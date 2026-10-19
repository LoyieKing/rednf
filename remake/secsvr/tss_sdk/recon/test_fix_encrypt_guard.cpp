// Focused differential for DPVMBaseEngine::fix_encrypt's output-capacity
// guard.  ORIG 0x000bd4df..0x000bd4e6 reserves buf_len + 0x11a (17-byte
// DPInterData + 265-byte DPAlgorithmData).  The old reconstruction used
// buf_len + 17, so it returned success for out_len in [buf_len+17,
// buf_len+281] and wrote an under-sized DPAlgorithmData.
//
// Usage (32-bit, separate process per library):
//   g++ -m32 -O2 -o /tmp/test_fix_encrypt_guard \
//       recon/test_fix_encrypt_guard.cpp -ldl
//   /tmp/test_fix_encrypt_guard <libtss_sdk.so> > /tmp/guard_orig.txt
//   /tmp/test_fix_encrypt_guard <libtss_sdk.so.rebuilt> > /tmp/guard_rebuilt.txt
//   diff -u /tmp/guard_orig.txt /tmp/guard_rebuilt.txt
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*FixEncFn)(void *, void *, unsigned char *, unsigned int,
                        unsigned char *, unsigned int *);

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

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <libtss_sdk.so>\n", argv[0]);
        return 2;
    }
    void *h = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (h == 0) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return 2;
    }
    FixEncFn fn = (FixEncFn)dlsym(
        h, "_ZN9tss_dpsdk14DPVMBaseEngine11fix_encryptEPN12tss_sdk_impl8"
           "UserInfoEPhjS4_Rj");
    if (fn == 0) {
        fprintf(stderr, "missing fix_encrypt\n");
        return 2;
    }

    unsigned int buf_len;
    for (buf_len = 0; buf_len <= 1; ++buf_len) {
        unsigned int k;
        for (k = 0; k <= 300; ++k) {
            unsigned char engine[68];
            unsigned char buf[2];
            unsigned char out[512 + 64];
            unsigned int out_len = buf_len + k;
            unsigned int i;
            unsigned long long h1, h2;
            memset(engine, 0, sizeof(engine));
            for (i = 0; i < sizeof(buf); ++i) buf[i] = (unsigned char)(i + 1);
            memset(out, 0x5a, sizeof(out));
            srand(1);
            int ret = fn(engine, 0, buf, buf_len, out, &out_len);
            hash_buf(out, sizeof(out), h1, h2);
            int canary = 1;
            for (i = 0; i < 64; ++i)
                if (out[512 + i] != 0x5a) canary = 0;
            printf("buf_len=%u k=%u ret=%d out_len=%u h1=%016llx h2=%016llx "
                   "canary=%d\n",
                   buf_len, k, ret, out_len, h1, h2, canary);
        }
    }
    dlclose(h);
    return 0;
}
