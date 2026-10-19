// Differential oracle for the TsLocal units reconstructed in
//   recon/src/VMCore/tencrypt_ts_local.cpp  (CreateTsLocal/FreeTsLocal)
//   recon/src/VMCore/ts_local_imp.cpp       (CTsLocalImp)
// against ORIG libtss_sdk.so.1.0.394.
//
// The BOX3D1..BOX3D7 and CSeattos bodies live in other (already reconstructed)
// CUs.  This harness supplies recording stubs for them and links the test
// executable with -rdynamic, so the ORIG's PLT calls to the same mangled names
// bind to these stubs.  That makes the *dispatch* of
// CTsLocalImp::ProcessLastBytes and the argument forwarding of
// CTsLocalImp::Seattos observable and directly comparable between ORIG and the
// reconstruction.  When the ORIG turns out to be linked -Bsymbolic the
// interposition probe fails and only the non-interposed checks run (reported).
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "src/VMCore/ts_local_imp.h"

#define ORIG_PATH "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394"

static int g_checks = 0;
static int g_fails = 0;

static void check(bool ok, const char *what, long a = 0, long b = 0)
{
    ++g_checks;
    if (!ok) {
        ++g_fails;
        printf("FAIL %s (a=%ld b=%ld)\n", what, a, b);
    }
}

// ---------------------------------------------------------------- recording
struct BoxRec {
    int id;
    unsigned char *a1;
    unsigned int a2;
    int a3;
    unsigned char *a4;
};

static BoxRec g_box[64];
static int g_nbox;

typedef void (*BoxFn)(unsigned char *, unsigned int, bool, unsigned char *);
static BoxFn g_realBox[8]; // [1..7] = real ORIG bodies (trampoline targets)
static int g_forward;      // matching id -> forward to the ORIG body

static void boxrec(int id, unsigned char *a1, unsigned int a2, bool a3,
                   unsigned char *a4)
{
    if (g_nbox < 64) {
        g_box[g_nbox].id = id;
        g_box[g_nbox].a1 = a1;
        g_box[g_nbox].a2 = a2;
        g_box[g_nbox].a3 = a3 ? 1 : 0;
        g_box[g_nbox].a4 = a4;
    }
    ++g_nbox;
    if (g_forward == id && g_realBox[id]) {
        g_realBox[id](a1, a2, a3, a4);
    } else if (a4) {
        a4[0] = (unsigned char)(a4[0] ^ (0x11 * id));
    }
}

void BOX3D1(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(1, a1, a2, a3, a4);
}
void BOX3D2(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(2, a1, a2, a3, a4);
}
void BOX3D3(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(3, a1, a2, a3, a4);
}
void BOX3D4(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(4, a1, a2, a3, a4);
}
void BOX3D5(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(5, a1, a2, a3, a4);
}
void BOX3D6(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(6, a1, a2, a3, a4);
}
void BOX3D7(unsigned char *a1, unsigned int a2, bool a3, unsigned char *a4)
{
    boxrec(7, a1, a2, a3, a4);
}

struct SeaRec {
    int kind; // 0 ctor, 1 dtor, 2 Seattos
    void *self;
    unsigned char *data;
    int enc;
    unsigned char *key;
    unsigned int len;
};

static SeaRec g_sea[64];
static int g_nsea;

static void searec(int kind, void *self, unsigned char *data, bool enc,
                   unsigned char *key, unsigned int len)
{
    if (g_nsea < 64) {
        g_sea[g_nsea].kind = kind;
        g_sea[g_nsea].self = self;
        g_sea[g_nsea].data = data;
        g_sea[g_nsea].enc = enc ? 1 : 0;
        g_sea[g_nsea].key = key;
        g_sea[g_nsea].len = len;
    }
    ++g_nsea;
}

CSeattos::CSeattos()
{
    searec(0, this, 0, false, 0, 0);
}
CSeattos::~CSeattos()
{
    searec(1, this, 0, false, 0, 0);
}
void CSeattos::Seattos(unsigned char *data, bool enc, unsigned char *key,
                       unsigned int len)
{
    searec(2, this, data, enc, key, len);
}

// allocation accounting
static int g_newCalls;
static unsigned int g_newSize;
static int g_delCalls;

void *operator new(size_t n)
{
    ++g_newCalls;
    g_newSize = (unsigned int)n;
    return malloc(n ? n : 1);
}
void operator delete(void *p)
{
    ++g_delCalls;
    free(p);
}

// ------------------------------------------------------------- ORIG symbols
typedef void (*ProcLast_t)(void *, unsigned char *, bool, unsigned int,
                           unsigned int, unsigned char *);
typedef void *(*Getter_t)(void *);
typedef void (*Seattos_t)(void *, unsigned char *, bool, unsigned char *,
                          unsigned int);
typedef void (*VoidThis_t)(void *);
typedef ITsLocal *(*Create_t)();
typedef void (*Free_t)();

int main()
{
    void *h = dlopen(ORIG_PATH, RTLD_NOW | RTLD_LOCAL);
    if (!h) {
        printf("CANNOT DLOPEN %s: %s\n", ORIG_PATH, dlerror());
        return 2;
    }

    ProcLast_t oProc = (ProcLast_t)dlsym(
        h, "_ZN11CTsLocalImp16ProcessLastBytesEPhbjjS0_");
    Getter_t oMh = (Getter_t)dlsym(h, "_ZN11CTsLocalImp20LocalGetModuleHandleEv");
    Getter_t oPa = (Getter_t)dlsym(h, "_ZN11CTsLocalImp19LocalGetProcAddressEv");
    Seattos_t oSea = (Seattos_t)dlsym(h, "_ZN11CTsLocalImp7SeattosEPhbS0_j");
    VoidThis_t oC1 = (VoidThis_t)dlsym(h, "_ZN11CTsLocalImpC1Ev");
    VoidThis_t oC2 = (VoidThis_t)dlsym(h, "_ZN11CTsLocalImpC2Ev");
    VoidThis_t oD0 = (VoidThis_t)dlsym(h, "_ZN11CTsLocalImpD0Ev");
    VoidThis_t oD1 = (VoidThis_t)dlsym(h, "_ZN11CTsLocalImpD1Ev");
    VoidThis_t oD2 = (VoidThis_t)dlsym(h, "_ZN11CTsLocalImpD2Ev");
    Create_t oCreate = (Create_t)dlsym(h, "_Z13CreateTsLocalv");
    Free_t oFree = (Free_t)dlsym(h, "_Z11FreeTsLocalv");
    void *oVtIt = dlsym(h, "_ZTV8ITsLocal");
    void *oVtImp = dlsym(h, "_ZTV11CTsLocalImp");
    void *oTiIt = dlsym(h, "_ZTI8ITsLocal");
    void *oTiImp = dlsym(h, "_ZTI11CTsLocalImp");
    void *oTsIt = dlsym(h, "_ZTS8ITsLocal");
    void *oTsImp = dlsym(h, "_ZTS11CTsLocalImp");
    void *oD0It = dlsym(h, "_ZN8ITsLocalD0Ev");
    void *oD1It = dlsym(h, "_ZN8ITsLocalD1Ev");
    g_realBox[1] = (BoxFn)dlsym(h, "_Z6BOX3D1PhjbS_");
    g_realBox[2] = (BoxFn)dlsym(h, "_Z6BOX3D2PhjbS_");
    g_realBox[3] = (BoxFn)dlsym(h, "_Z6BOX3D3PhjbS_");
    g_realBox[4] = (BoxFn)dlsym(h, "_Z6BOX3D4PhjbS_");
    g_realBox[5] = (BoxFn)dlsym(h, "_Z6BOX3D5PhjbS_");
    g_realBox[6] = (BoxFn)dlsym(h, "_Z6BOX3D6PhjbS_");
    g_realBox[7] = (BoxFn)dlsym(h, "_Z6BOX3D7PhjbS_");
    check(g_realBox[1] && g_realBox[7], "orig BOX3D bodies resolve");


    check(oProc && oMh && oPa && oSea && oC1 && oC2 && oD0 && oD1 && oD2 &&
              oCreate && oFree,
          "orig methods resolve");
    check(oVtIt && oVtImp && oTiIt && oTiImp && oTsIt && oTsImp && oD0It && oD1It,
          "orig vtable/typeinfo resolve");

    ITsLocal *mine = CreateTsLocal();
    check(mine != 0, "CreateTsLocal non-null");

    // ---- ITsLocal / CTsLocalImp vtables of this build
    void *myVtIt = dlsym(RTLD_DEFAULT, "_ZTV8ITsLocal");
    void *myVtImp = dlsym(RTLD_DEFAULT, "_ZTV11CTsLocalImp");
    check(myVtIt && myVtImp, "own vtables visible via -rdynamic");
    void *pure = dlsym(RTLD_DEFAULT, "__cxa_pure_virtual");
    check(pure != 0, "__cxa_pure_virtual visible");
    if (myVtIt && pure) {
        void **slots = (void **)myVtIt;
        check(slots[2] == pure && slots[3] == pure && slots[4] == pure &&
                  slots[5] == pure,
              "ITsLocal vtable slots 0..3 are __cxa_pure_virtual");
        check(slots[6] != pure && slots[7] != pure,
              "ITsLocal vtable D1/D0 are emitted");
    }
    if (oVtIt) {
        void **slots = (void **)oVtIt;
        check(slots[2] == slots[3] && slots[3] == slots[4] &&
                  slots[4] == slots[5],
              "orig ITsLocal vtable slots 0..3 identical (pure)");
        check(slots[2] != slots[6] && slots[2] != slots[7],
              "orig ITsLocal D1/D0 distinct");
    }
    if (myVtImp) {
        void **slots = (void **)myVtImp;
        check(slots[8] != 0 && slots[8] != slots[9] && slots[9] != 0,
              "CTsLocalImp vtable D1/D0 slots present");
        check(*(void **)mine == (void *)((char *)myVtImp + 8),
              "CTsLocalImp vptr points at own vtable+8");
    }
    check(myVtImp != oVtImp, "own vtable is not the orig vtable");

    // ---- getters: parity on a raw storage blob (both read +4 / +8)
    void *raw[3];
    raw[0] = (void *)0;
    raw[1] = (void *)0x12345678;
    raw[2] = (void *)0x9abcdef0;
    check(oMh(raw) == (void *)0x12345678, "orig LocalGetModuleHandle reads +4");
    check(oPa(raw) == (void *)0x9abcdef0, "orig LocalGetProcAddress reads +8");
    check(reinterpret_cast<CTsLocalImp *>(raw)
                  ->CTsLocalImp::LocalGetModuleHandle() == (void *)0x12345678,
          "own LocalGetModuleHandle reads +4");
    check(reinterpret_cast<CTsLocalImp *>(raw)
                  ->CTsLocalImp::LocalGetProcAddress() == (void *)0x9abcdef0,
          "own LocalGetProcAddress reads +8");

    // ---- constructors on raw storage
    unsigned char obuf[16];
    memset(obuf, 0xaa, sizeof(obuf));
    oC1(obuf);
    check(*(void **)obuf != 0, "orig ctor sets vptr");
    check(*(void **)(obuf + 4) == 0 && *(void **)(obuf + 8) == 0,
          "orig ctor zeroes +4/+8");
    memset(obuf, 0xaa, sizeof(obuf));
    oC2(obuf);
    check(*(void **)obuf != 0, "orig base-ctor sets vptr");
    check(*(void **)(obuf + 4) == 0 && *(void **)(obuf + 8) == 0,
          "orig base-ctor zeroes +4/+8");
    check(oC1 != oC2, "orig ctor and base-ctor are distinct symbols");
    {
        CTsLocalImp *q = new CTsLocalImp();
        check(*(void **)q == *(void **)mine, "own ctor vptr matches singleton");
        check(*(void **)((char *)q + 4) == 0 && *(void **)((char *)q + 8) == 0,
              "own ctor zeroes +4/+8");
        check(g_newSize == 12, "own ctor allocation size is 12", g_newSize);
        delete q;
    }

    // ---- ProcessLastBytes dispatch
    unsigned char *bufA = (unsigned char *)malloc(0x40000);
    unsigned char *bufB = (unsigned char *)malloc(0x40000);
    unsigned char *bufA2 = (unsigned char *)malloc(0x40000);
    unsigned char *bufB2 = (unsigned char *)malloc(0x40000);
    memset(bufA, 0, 0x40000);
    memset(bufB, 0, 0x40000);

    // (a) probe: are the ORIG's internal BOX3D calls interposable?
    g_forward = 0;
    g_nbox = 0;
    oProc(0, bufA, true, 9, 0, bufB); // residue 1 -> BOX3D1 if interposed
    bool interposed = (g_nbox > 0);
    printf("interposition: %s (%d BOX3D calls on probe)\n",
           interposed ? "active" : "INACTIVE (orig -Bsymbolic?)", g_nbox);

    // (b) own dispatch: exact argument tuple for every residue and base
    {
        const unsigned bases[4] = { 0, 8, 16, 0x1000 };
        for (int bi = 0; bi < 4; ++bi) {
            for (unsigned r = 0; r < 8; ++r) {
                unsigned len = bases[bi] + r;

                g_forward = 0;
                memset(bufA, 0, 0x40000);
                memset(bufB, 0, 0x40000);
                g_nbox = 0;
                mine->ProcessLastBytes(bufA, true, len, 5, bufB);
                check(g_nbox == (r == 0 ? 0 : 1), "own dispatch count", g_nbox);
                if (r != 0) {
                    check(g_box[0].id == (int)r, "own variant index == residue",
                          g_box[0].id, (long)r);
                    check(g_box[0].a1 == bufA, "own arg1 == pbyKey");
                    check(g_box[0].a2 == 5, "own arg2 == dwKeySize", g_box[0].a2);
                    check(g_box[0].a3 == 1, "own arg3 == bEncrypt", g_box[0].a3);
                    check(g_box[0].a4 == bufB + bases[bi],
                          "own arg4 == pbyBuf + (dwBufLen & ~7)",
                          (long)(g_box[0].a4 - bufB), (long)bases[bi]);
                }
                g_nbox = 0;
                mine->ProcessLastBytes(bufA, false, len, 7, bufB);
                if (r != 0)
                    check(g_nbox == 1 && g_box[0].a3 == 0,
                          "own bEncrypt false forwarded", g_nbox, g_box[0].a3);
            }
        }
        // a large (but in-bounds) length exercises the &~7 base masking
        memset(bufB, 0, 0x40000);
        g_nbox = 0;
        mine->ProcessLastBytes(bufA, true, 0x3fffd, 3, bufB);
        check(g_nbox == 1 && g_box[0].id == 5, "large length residue 5", g_nbox,
              g_nbox ? g_box[0].id : -1);
        check(g_box[0].a4 == bufB + 0x3fff8, "large length base mask",
              (long)(g_box[0].a4 - bufB), 0x3fff8);
    }

    // (c) behaviour parity: ORIG's ProcessLastBytes and ours run on identical
    //     inputs, both ending in the *real* BOX3D bodies (ours via the
    //     trampolines); any difference in variant selection, buffer offset or
    //     argument order changes the resulting bytes.
    check(g_realBox[1] && g_realBox[7], "real BOX3D bodies available");
    {
        const unsigned bases[3] = { 0, 8, 0x1000 };
        for (int bi = 0; bi < 3; ++bi) {
            for (unsigned r = 1; r < 8; ++r) {
                unsigned len = bases[bi] + r;

                memset(bufA, 0, 0x40000);
                memset(bufB, 0, 0x40000);
                bufB[bases[bi]] = (unsigned char)(0x5a + r);
                bufB[bases[bi] + 1] = (unsigned char)(0x13 * r);
                memcpy(bufA2, bufA, 0x40000);
                memcpy(bufB2, bufB, 0x40000);

                // ORIG path; its BOX3D call is interposed, so the argument
                // tuple it passes is recorded as well
                BoxRec ob;
                int onb;
                g_forward = (int)r;
                g_nbox = 0;
                oProc(0, bufA, true, len, 5, bufB);
                onb = g_nbox;
                memcpy(&ob, g_box, sizeof(ob));

                g_nbox = 0;
                mine->ProcessLastBytes(bufA2, true, len, 5, bufB2);
                g_forward = 0;

                check(onb == 1 && g_nbox == 1, "parity path dispatched once",
                      onb, g_nbox);
                check(ob.id == (int)r && ob.id == g_box[0].id,
                      "orig variant index == residue", ob.id, (long)r);
                check(ob.a1 == bufA && g_box[0].a1 == bufA2,
                      "orig/own arg1 == pbyKey", (long)ob.a1,
                      (long)g_box[0].a1);
                check(ob.a2 == g_box[0].a2 && ob.a3 == g_box[0].a3,
                      "orig/own arg2/arg3 parity", (long)ob.a2,
                      (long)g_box[0].a2);
                check(ob.a4 - bufB == g_box[0].a4 - bufB2,
                      "orig/own arg4 offset parity", (long)(ob.a4 - bufB),
                      (long)(g_box[0].a4 - bufB2));
                check(ob.a4 == bufB + bases[bi],
                      "orig arg4 == pbyBuf + (dwBufLen & ~7)",
                      (long)(ob.a4 - bufB), (long)bases[bi]);
                check(memcmp(bufA, bufA2, 0x40000) == 0, "pbyKey buffer parity",
                      (long)r, (long)bases[bi]);
                check(memcmp(bufB, bufB2, 0x40000) == 0, "pbyBuf buffer parity",
                      (long)r, (long)bases[bi]);
            }
        }
    }

    // ---- Seattos forwarding
    {
        unsigned char key[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        unsigned char dat[8] = { 9, 8, 7, 6, 5, 4, 3, 2 };
        SeaRec a[4], b[4];

        g_nsea = 0;
        oSea(0, key, true, dat, 4);
        int na = g_nsea > 4 ? 4 : g_nsea;
        memcpy(a, g_sea, sizeof(SeaRec) * 4);

        g_nsea = 0;
        mine->Seattos(key, true, dat, 4);
        int nb = g_nsea > 4 ? 4 : g_nsea;
        memcpy(b, g_sea, sizeof(SeaRec) * 4);

        check(na == 3 && a[0].kind == 0 && a[1].kind == 2 && a[2].kind == 1,
              "orig Seattos sequence ctor/Seattos/dtor", na);
        check(nb == 3 && b[0].kind == 0 && b[1].kind == 2 && b[2].kind == 1,
              "own Seattos sequence ctor/Seattos/dtor", nb);
        if (na == 3 && nb == 3) {
            check(a[0].self == a[1].self && a[1].self == a[2].self,
                  "orig local CSeattos on stack all same this");
            check(b[0].self == b[1].self && b[1].self == b[2].self,
                  "own local CSeattos on stack all same this");
            // CSeattos::Seattos(pbyData, bEncrypt, pbyKey, dwLen) receives
            // (key, benc, buf, buflen) from CTsLocalImp::Seattos.
            check(a[1].data == key && b[1].data == key, "Seattos data==key",
                  (long)a[1].data, (long)b[1].data);
            check(a[1].key == dat && b[1].key == dat, "Seattos key==buf",
                  (long)a[1].key, (long)b[1].key);
            check(a[1].enc == 1 && b[1].enc == 1, "Seattos bEncrypt forwarded");
            check(a[1].len == 4 && b[1].len == 4, "Seattos buflen forwarded");
        }
    }

    // ---- CreateTsLocal / FreeTsLocal
    {
        g_newCalls = 0;
        g_delCalls = 0;
        ITsLocal *r1 = CreateTsLocal();
        check(r1 != 0, "second CreateTsLocal non-null");
        check(r1 == mine, "CreateTsLocal is a singleton", (long)r1, (long)mine);
        check(g_newCalls == 0, "singleton create allocates nothing",
              g_newCalls);
        FreeTsLocal();
        check(g_delCalls == 1, "FreeTsLocal destroys once", g_delCalls);
        FreeTsLocal();
        check(g_delCalls == 1, "FreeTsLocal on cleared singleton is a no-op",
              g_delCalls);
        FreeTsLocal();
        check(g_delCalls == 1, "FreeTsLocal stays a no-op", g_delCalls);

        g_newCalls = 0;
        g_newSize = 0;
        ITsLocal *r2 = CreateTsLocal();
        check(r2 != 0, "CreateTsLocal recreates", (long)r2);
        check(g_newCalls == 1, "recreate allocates once", g_newCalls);
        check(g_newSize == 12, "recreate allocation size 12", g_newSize);
        if (myVtImp)
            check(*(void **)r2 == (void *)((char *)myVtImp + 8),
                  "recreated vptr correct");
        FreeTsLocal();
        check(g_delCalls == 2, "recreate frees once", g_delCalls);
    }

    // ---- orig singleton behaviour (its Create/Free route through the same
    //      interposed ctor/dtor/operators, so only shape is checked)
    {
        ITsLocal *o1 = oCreate();
        ITsLocal *o2 = oCreate();
        check(o1 != 0 && o1 == o2, "orig CreateTsLocal singleton",
              (long)o1, (long)o2);
        check(g_newSize == 12, "orig singleton allocation size 12", g_newSize);
        oFree();
        oFree();
        ITsLocal *o3 = oCreate();
        check(o3 != 0, "orig CreateTsLocal after Free", (long)o3);
        oFree();
    }

    free(bufA);
    free(bufB);

    printf("%s: %d checks, %d failures\n",
           g_fails ? "FAILED" : "PASSED", g_checks, g_fails);
    return g_fails ? 1 : 0;
}
