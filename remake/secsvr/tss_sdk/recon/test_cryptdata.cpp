// Behavior check: reconstructed CCryptData dispatch vs original libtss_sdk.
// Calls the original method with a null `this`; EncryptData does not touch this.
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include "inc/VMCore/cryptdata.h"

typedef BOOL (*EncFn)(void*, unsigned short, unsigned char*, bool, unsigned char*, unsigned int);

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1]
        : "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
    void* h = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!h) { std::printf("dlopen: %s\n", dlerror()); return 2; }
    EncFn orig = (EncFn)dlsym(h, "_ZN10CCryptData11EncryptDataEtPhbS0_j");
    if (!orig) { std::printf("dlsym missing\n"); return 2; }
    CCryptData mine;
    int fail = 0;
    for (unsigned id = 0; id <= 30; ++id) {
        unsigned char key[64], a[128], b[128];
        for (int i = 0; i < 64; ++i) key[i] = (unsigned char)(i * 7 + id);
        for (int i = 0; i < 128; ++i) a[i] = b[i] = (unsigned char)(i * 13 + id * 3);
        BOOL ra = orig(0, (unsigned short)id, key, true, a, sizeof(a));
        BOOL rb = mine.EncryptData((unsigned short)id, key, true, b, sizeof(b));
        bool same = (ra == rb) && std::memcmp(a, b, sizeof(a)) == 0;
        std::printf("id=%2u ret=%d/%d buf=%s\n", id, (int)ra, (int)rb, same ? "same" : "DIFF");
        if (!same) ++fail;
    }
    dlclose(h);
    return fail ? 1 : 0;
}
