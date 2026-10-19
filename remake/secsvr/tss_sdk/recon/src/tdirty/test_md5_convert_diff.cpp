// Differential harness for tdirty md5.c + convert.c against ORIG.
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void (*fn_init)(void *);
typedef void (*fn_update)(void *, unsigned char *, unsigned int);
typedef void (*fn_final)(unsigned char *, void *);
typedef void (*fn_hashbuf)(char *, char *, unsigned int);
typedef void (*fn_hex)(unsigned char *, char *);
typedef void (*fn_file)(char *, char *);
typedef int (*fn_conv)(char *, int, const char *, int);

static int gPass, gFail;

static void rep(const char *what, const char *a, const char *b) {
    if (strcmp(a, b) == 0) ++gPass;
    else { ++gFail; printf("FAIL %s\n  orig=%s\n  recon=%s\n", what, a, b); }
}
static void repi(const char *what, int a, int b) {
    if (a == b) ++gPass;
    else { ++gFail; printf("FAIL %s orig=%d recon=%d\n", what, a, b); }
}

struct Api {
    fn_init init; fn_update update; fn_final final;
    fn_hashbuf hash; fn_hex hex; fn_file file;
    fn_conv g2u; fn_conv u2g;
};

static Api load(void *h) {
    Api a;
    a.init = (fn_init)dlsym(h, "MD5Init");
    a.update = (fn_update)dlsym(h, "MD5Update");
    a.final = (fn_final)dlsym(h, "MD5Final");
    a.hash = (fn_hashbuf)dlsym(h, "Md5HashBuffer");
    a.hex = (fn_hex)dlsym(h, "Md5HexString");
    a.file = (fn_file)dlsym(h, "Md5File");
    a.g2u = (fn_conv)dlsym(h, "GB2312ToUTF8");
    a.u2g = (fn_conv)dlsym(h, "UTF8ToGB2312");
    return a;
}

static void digestOf(Api &a, const char *data, unsigned len, char *outHex) {
    unsigned char d[16];
    char ctx[256];
    memset(ctx, 0xcc, sizeof(ctx));
    a.init(ctx);
    a.update(ctx, (unsigned char *)data, len);
    a.final(d, ctx);
    memset(outHex, 0, 64);
    a.hex(d, outHex);
}

int main(int argc, char **argv) {
    const char *orig = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
    const char *recon = "./libtdirty_recon.so";
    if (argc > 1) orig = argv[1];
    if (argc > 2) recon = argv[2];
    void *ho = dlopen(orig, RTLD_NOW | RTLD_GLOBAL);
    if (!ho) { printf("SKIP orig: %s\n", dlerror()); return 2; }
    void *hr = dlopen(recon, RTLD_NOW | RTLD_LOCAL);
    if (!hr) { printf("SKIP recon: %s\n", dlerror()); return 2; }
    Api o = load(ho), r = load(hr);

    const char *texts[] = {"", "a", "abc", "message digest",
                           "12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
    for (unsigned i = 0; i < sizeof(texts)/sizeof(texts[0]); ++i) {
        char h1[64], h2[64];
        digestOf(o, texts[i], (unsigned)strlen(texts[i]), h1);
        digestOf(r, texts[i], (unsigned)strlen(texts[i]), h2);
        char label[64]; snprintf(label, sizeof(label), "md5[%u]", i);
        rep(label, h1, h2);
    }
    // multi-block / update-splitting boundary
    char big[1000]; for (int i = 0; i < 1000; ++i) big[i] = (char)(i * 7);
    char h1[64], h2[64];
    digestOf(o, big, 1000, h1); digestOf(r, big, 1000, h2);
    rep("md5[1000]", h1, h2);

    // Md5File
    const char *path = "/tmp/tdirty_md5_probe.bin";
    FILE *f = fopen(path, "wb");
    if (f) { fwrite(big, 1, 333, f); fclose(f); }
    char d1[16], d2[16], x1[64], x2[64];
    o.file((char *)d1, (char *)path); r.file((char *)d2, (char *)path);
    memset(x1, 0, sizeof(x1)); memset(x2, 0, sizeof(x2));
    o.hex((unsigned char *)d1, x1); r.hex((unsigned char *)d2, x2);
    rep("Md5File", x1, x2);

    // convert.c: round-trip and parity (iconv availability may make both -1)
    const char *gbk = "\xc9\xb5\xb1\xc6";
    const char *utf8 = "\xe5\x82\xbb\xe9\x80\xbc";
    char b1[256], b2[256];
    memset(b1, 0x5a, sizeof(b1)); memset(b2, 0x5a, sizeof(b2));
    int c1 = o.g2u(b1, 256, gbk, (int)strlen(gbk));
    int c2 = r.g2u(b2, 256, gbk, (int)strlen(gbk));
    repi("GB2312ToUTF8 ret", c1, c2);
    if (c1 > 0 && c1 == c2) { b1[c1] = 0; b2[c2] = 0; rep("GB2312ToUTF8 bytes", b1, b2); }
    memset(b1, 0x5a, sizeof(b1)); memset(b2, 0x5a, sizeof(b2));
    c1 = o.u2g(b1, 256, utf8, (int)strlen(utf8));
    c2 = r.u2g(b2, 256, utf8, (int)strlen(utf8));
    repi("UTF8ToGB2312 ret", c1, c2);
    if (c1 > 0 && c1 == c2) { b1[c1] = 0; b2[c2] = 0; rep("UTF8ToGB2312 bytes", b1, b2); }

    printf("PASS=%d FAIL=%d\n", gPass, gFail);
    return gFail ? 1 : 0;
}
