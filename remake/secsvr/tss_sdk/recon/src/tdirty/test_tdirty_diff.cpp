// Differential harness: compares reconstructed tdirty C API against ORIG.
// Build: see run_tdirty_diff.sh in this directory.
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>

typedef int (*fn_init_ex)(char *, int);
typedef int (*fn_check)(int, char *, int *);
typedef int (*fn_check_ex)(int, char *, int *, int *, int *);
typedef int (*fn_replace)(int, char *, int *);
typedef int (*fn_map)(char *, char *, char *, int);

static void *hOrig;
static void *hRecon;

template <typename T>
static T sym(void *h, const char *n) {
    void *p = dlsym(h, n);
    if (!p) {
        fprintf(stderr, "missing symbol %s: %s\n", n, dlerror());
    }
    return (T)p;
}

static int gFail = 0;
static int gPass = 0;
static int gDetected = 0;
static int gReplaced = 0;
static int gPinyDetected = 0;
static int gUtf8Detected = 0;

static void report(const char *name, int a, int b, const char *detail) {
    if (a == b) {
        ++gPass;
    } else {
        ++gFail;
        printf("FAIL %-42s orig=%d recon=%d %s\n", name, a, b,
               detail ? detail : "");
    }
}

static void reportStr(const char *name, const char *a, const char *b) {
    if (strcmp(a, b) == 0) {
        ++gPass;
    } else {
        ++gFail;
        printf("FAIL %-42s orig='%s' recon='%s'\n", name, a, b);
    }
}

struct Api {
    fn_init_ex initEx;
    fn_check check;
    fn_check_ex checkEx;
    fn_replace replace;
    fn_map map;
    fn_check pinyin;
    fn_check utf8;
    fn_replace utf8Replace;
};

static Api load(void *h) {
    Api a;
    a.initEx = sym<fn_init_ex>(h, "tInitializeDirtyCheckEx");
    a.check = sym<fn_check>(h, "tDirtyCheck");
    a.checkEx = sym<fn_check_ex>(h, "tDirtyCheckEx");
    a.replace = sym<fn_replace>(h, "tDirtyReplace");
    a.map = sym<fn_map>(h, "tMakeMapCode");
    a.pinyin = sym<fn_check>(h, "tDirtyCheckPinyin");
    a.utf8 = sym<fn_check>(h, "utf8DirtyCheck");
    a.utf8Replace = sym<fn_replace>(h, "utf8DirtyReplace");
    return a;
}

static const char kDirtyTable[] =
    "1 \xc9\xb5\xb1\xc6|\xc9\xb5 1\n"          // 傻逼|傻
    "1 \xb2\xd9\xc4\xe3\xc2\xe8|\xc4\xe3 1\n"  // 操你妈|你
    "1 \xc9\xb5" "ab|\xc9\xb5 1\n"              // 傻ab|傻 (mixed)
    "2 fuck 2\n"                               // english
    "3 f.ck 3\n"                               // regex
    "4 shabi 4\n";                             // pinyin regex

static void runCheck(Api &a, int door, const char *text, const char *label) {
    char b1[1024], b2[1024];
    strcpy(b1, text);
    strcpy(b2, text);
    int l1 = -77, l2 = -77;
    int r1 = a.check(door, b1, &l1);
    int r2 = a.check(door, b2, &l2);
    char d[128];
    snprintf(d, sizeof(d), "level %d/%d text='%s'", l1, l2, text);
    report(label, r1, r2, d);
    report((std::string(label) + "/level").c_str(), l1, l2, text);

    int e1 = -1, e2 = -1, q1 = -1, q2 = -1;
    strcpy(b1, text);
    strcpy(b2, text);
    int x1 = a.checkEx(door, b1, &l1, &e1, &q1);
    int x2 = a.checkEx(door, b2, &l2, &e2, &q2);
    snprintf(d, sizeof(d), "ret %d/%d level %d/%d begin %d/%d end %d/%d",
             x1, x2, l1, l2, e1, e2, q1, q2);
    report((std::string(label) + "/ex").c_str(), x1, x2, d);
    report((std::string(label) + "/begin").c_str(), e1, e2, text);
    report((std::string(label) + "/end").c_str(), q1, q2, text);
}

static void runReplace(Api &a, int door, const char *text, const char *label) {
    char b1[1024], b2[1024];
    strcpy(b1, text);
    strcpy(b2, text);
    int l1 = -77, l2 = -77;
    int r1 = a.replace(door, b1, &l1);
    int r2 = a.replace(door, b2, &l2);
    char d[128];
    snprintf(d, sizeof(d), "ret %d/%d level %d/%d", r1, r2, l1, l2);
    report(label, r1, r2, d);
    report((std::string(label) + "/level").c_str(), l1, l2, d);
    reportStr((std::string(label) + "/text").c_str(), b1, b2);
}

int main(int argc, char **argv) {
    const char *origPath = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";
    const char *reconPath = "./libtdirty_recon.so";
    if (argc > 1) origPath = argv[1];
    if (argc > 2) reconPath = argv[2];

    hOrig = dlopen(origPath, RTLD_NOW | RTLD_GLOBAL);
    if (!hOrig) {
        printf("SKIP cannot load ORIG: %s\n", dlerror());
        return 2;
    }
    if (getenv("ORIG_ONLY") != 0) {
        hRecon = hOrig;
    } else {
        hRecon = dlopen(reconPath, RTLD_NOW | RTLD_LOCAL);
        if (!hRecon) {
            printf("SKIP cannot load recon: %s\n", dlerror());
            return 2;
        }
    }

    Api o = load(hOrig);
    Api r = load(hRecon);
    if (!o.initEx || !r.initEx)
        return 2;

    {
        const char *simple = "2 fuck 2\n";
        char sb1[64], sb2[64];
        strcpy(sb1, simple);
        strcpy(sb2, simple);
        int s1 = o.initEx(sb1, (int)strlen(simple));
        int s2 = r.initEx(sb2, (int)strlen(simple));
        int sl1 = -1, sl2 = -1;
        int sr1 = o.check(1, (char *)"fuck", &sl1);
        int sr2 = r.check(1, (char *)"fuck", &sl2);
        printf("simple_init=%d/%d simple_check=%d/%d level=%d/%d\n", s1, s2,
               sr1, sr2, sl1, sl2);
    }

    {
        const char *one = "1 \xc9\xb5\xb1\xc6|\xc9\xb5 1\n";
        char ob1[64], ob2[64];
        strcpy(ob1, one);
        strcpy(ob2, one);
        int s1 = o.initEx(ob1, (int)strlen(one));
        int s2 = r.initEx(ob2, (int)strlen(one));
        int sl1 = -1, sl2 = -1;
        int sr1 = o.check(1, (char *)"\xc9\xb5\xb1\xc6", &sl1);
        int sr2 = r.check(1, (char *)"\xc9\xb5\xb1\xc6", &sl2);
        printf("gbk_init=%d/%d gbk_check=%d/%d level=%d/%d\n", s1, s2,
               sr1, sr2, sl1, sl2);
    }

    char t1[sizeof(kDirtyTable)];
    char t2[sizeof(kDirtyTable)];
    memcpy(t1, kDirtyTable, sizeof(kDirtyTable));
    memcpy(t2, kDirtyTable, sizeof(kDirtyTable));
    int i1 = o.initEx(t1, (int)strlen(kDirtyTable));
    int i2 = r.initEx(t2, (int)strlen(kDirtyTable));
    printf("init orig=%d recon=%d table_len=%u\n", i1, i2,
           (unsigned)strlen(kDirtyTable));
    for (unsigned k = 0; k < strlen(kDirtyTable); ++k)
        printf("%02x ", (unsigned char)kDirtyTable[k]);
    printf("\n");
    report("InitializeDirtyCheckEx", i1, i2, kDirtyTable);

    const char *texts[] = {
        "hello world",
        "\xc4\xe3\xba\xc3\xca\xc0\xbd\xe7",              // 你好世界
        "\xd5\xe2\xca\xc7\xc9\xb5\xb1\xc6\xb2\xe2\xca\xd4", // 这是傻逼测试
        "\xb2\xd9\xc4\xe3\xc2\xe8",                      // 操你妈
        "x\xc9\xb5" "aby",                              // x傻aby
        "you FUCK off",
        "a fuck b",
        "fuck",
        "f.ck",
        "shabi",
        "SHABI",
        "",
        "x",
    };
    // Real comparisons: invoke both sides explicitly per case.
    for (unsigned i = 0; i < sizeof(texts) / sizeof(texts[0]); ++i) {
        char b1[1024], b2[1024];
        strcpy(b1, texts[i]);
        strcpy(b2, texts[i]);
        int l1 = -77, l2 = -77;
        int r1 = o.check(1, b1, &l1);
        int r2 = r.check(1, b2, &l2);
        if (r1 != 0)
            ++gDetected;
        char d[256], label[64];
        snprintf(label, sizeof(label), "check[%u]", i);
        snprintf(d, sizeof(d), "level %d/%d", l1, l2);
        if (i < 8)
            printf("check[%u] orig=%d/%d recon=%d/%d '%s'\n", i, r1, l1, r2,
                   l2, texts[i]);
        report(label, r1, r2, d);
        snprintf(label, sizeof(label), "check[%u]/level", i);
        report(label, l1, l2, texts[i]);

        int e1 = -1, e2 = -1, q1 = -1, q2 = -1;
        strcpy(b1, texts[i]);
        strcpy(b2, texts[i]);
        int x1 = o.checkEx(1, b1, &l1, &e1, &q1);
        int x2 = r.checkEx(1, b2, &l2, &e2, &q2);
        snprintf(label, sizeof(label), "checkEx[%u]", i);
        snprintf(d, sizeof(d), "ret %d/%d level %d/%d begin %d/%d end %d/%d",
                 x1, x2, l1, l2, e1, e2, q1, q2);
        report(label, x1, x2, d);
        snprintf(label, sizeof(label), "checkEx[%u]/begin", i);
        report(label, e1, e2, texts[i]);
        snprintf(label, sizeof(label), "checkEx[%u]/end", i);
        report(label, q1, q2, texts[i]);

        strcpy(b1, texts[i]);
        strcpy(b2, texts[i]);
        int p1 = o.replace(1, b1, &l1);
        int p2 = r.replace(1, b2, &l2);
        if (strcmp(b1, texts[i]) != 0)
            ++gReplaced;
        snprintf(label, sizeof(label), "replace[%u]", i);
        snprintf(d, sizeof(d), "ret %d/%d level %d/%d", p1, p2, l1, l2);
        report(label, p1, p2, d);
        snprintf(label, sizeof(label), "replace[%u]/text", i);
        reportStr(label, b1, b2);
    }

    // MakeMapCode comparison over GBK characters and boundaries.
    const char *chars[] = {
        "a", "A1", "\xc4\xe3", "\xba\xc3", "\xc9\xb5", "\xb1\xc6",
        "\xd5\xe2\xca\xc7", "\xa3\xc1", "\xa3\xb1", "\xa6\xa2", "\xa7\xa1",
        "\xb2\xe2\xca\xd4", "abc\xc4\xe3", "\xc4\xe3" "xyz",
    };
    const int maxes[] = {1, 2, 3, 4, 8, 1024};
    for (unsigned i = 0; i < sizeof(chars) / sizeof(chars[0]); ++i) {
        for (unsigned j = 0; j < sizeof(maxes) / sizeof(maxes[0]); ++j) {
            char s1[1024], p1[1024], s2[1024], p2[1024];
            memset(s1, 0x7f, sizeof(s1));
            memset(p1, 0x7f, sizeof(p1));
            memset(s2, 0x7f, sizeof(s2));
            memset(p2, 0x7f, sizeof(p2));
            int m1 = o.map((char *)chars[i], s1, p1, maxes[j]);
            int m2 = r.map((char *)chars[i], s2, p2, maxes[j]);
            char label[96], d[128];
            snprintf(label, sizeof(label), "map[%u][max=%d]", i, maxes[j]);
            snprintf(d, sizeof(d), "spell='%s/%s' pinyin='%s/%s'",
                     s1, s2, p1, p2);
            report(label, m1, m2, d);
            snprintf(label, sizeof(label), "map[%u][max=%d]/spell", i, maxes[j]);
            reportStr(label, s1, s2);
            snprintf(label, sizeof(label), "map[%u][max=%d]/pinyin", i, maxes[j]);
            reportStr(label, p1, p2);
        }
    }

    // Boundary: NULL/empty and unknown handle.
    int l1 = 7, l2 = 7;
    report("check(NULL)", o.check(1, 0, &l1), r.check(1, 0, &l2), "");
    report("check(empty)", o.check(1, (char *)"", &l1),
           r.check(1, (char *)"", &l2), "");

    // Pinyin table selector (type 4 records).
    const char *piny[] = {"\xc9\xb5\xb1\xc6", "\xc9\xb5\xb1\xc6 abc", "nope"};
    for (unsigned i = 0; i < sizeof(piny) / sizeof(piny[0]); ++i) {
        int l1 = -1, l2 = -1;
        int r1 = o.pinyin(1, (char *)piny[i], &l1);
        int r2 = r.pinyin(1, (char *)piny[i], &l2);
        if (r1 > 0)
            ++gPinyDetected;
        char label[64], d[128];
        snprintf(label, sizeof(label), "pinyin[%u]", i);
        snprintf(d, sizeof(d), "level %d/%d", l1, l2);
        report(label, r1, r2, d);
        report((std::string(label) + "/level").c_str(), l1, l2, piny[i]);
    }

    // UTF-8 selector.
    const char *u8[] = {
        "fuck",
        "\xe5\x82\xbb\xe9\x80\xbc",          // 傻逼 in UTF-8
        "\xe6\x93\x8d\xe4\xbd\xa0\xe5\xa6\x88", // 操你妈 in UTF-8
        "hello world",
    };
    for (unsigned i = 0; i < sizeof(u8) / sizeof(u8[0]); ++i) {
        char b1[256], b2[256];
        strcpy(b1, u8[i]);
        strcpy(b2, u8[i]);
        int l1 = -1, l2 = -1;
        int r1 = o.utf8(1, b1, &l1);
        int r2 = r.utf8(1, b2, &l2);
        if (r1 > 0)
            ++gUtf8Detected;
        if (i < 2)
            printf("utf8[%u] orig=%d/%d recon=%d/%d\n", i, r1, l1, r2, l2);
        char label[64], d[128];
        snprintf(label, sizeof(label), "utf8[%u]", i);
        snprintf(d, sizeof(d), "level %d/%d", l1, l2);
        report(label, r1, r2, d);
        report((std::string(label) + "/level").c_str(), l1, l2, u8[i]);

        strcpy(b1, u8[i]);
        strcpy(b2, u8[i]);
        int q1 = o.utf8Replace(1, b1, &l1);
        int q2 = r.utf8Replace(1, b2, &l2);
        snprintf(label, sizeof(label), "utf8Replace[%u]", i);
        snprintf(d, sizeof(d), "ret %d/%d level %d/%d", q1, q2, l1, l2);
        report(label, q1, q2, d);
        snprintf(label, sizeof(label), "utf8Replace[%u]/text", i);
        reportStr(label, b1, b2);
    }

    printf("PASS=%d FAIL=%d dirty=%d replaced=%d pinyin=%d utf8=%d\n", gPass,
           gFail, gDetected, gReplaced, gPinyDetected, gUtf8Detected);
    return gFail == 0 ? 0 : 1;
}
