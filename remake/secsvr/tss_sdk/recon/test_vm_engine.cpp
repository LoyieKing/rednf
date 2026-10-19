// Differential oracle for the reconstructed VMCore units:
//   src/VMCore/vm.cpp                 (CVM)
//   src/VMCore/upvm.cpp               (CUPVM)
//   src/VMCore/server_crypt_code_mgr.cpp
//
// The original libtss_sdk.so.1.0.394 is dlopen()ed; its exported CVM/CUPVM
// methods are called through dlsym() and compared against the reconstructed
// classes linked into this binary.  CommonProc helpers (rol/ror/xora/anda/ora/
// nota/CallDecrypt/DecodeShort) and CCryptData are leaf dependencies that are
// owned by other reconstruction units; this test forwards them to the ORIG so
// that only the reconstructed units under test differ.
//
// Build (WSL, from tools/tss_sdk_recon):
//   sh c5build.sh recon/src/VMCore/vm.cpp build/tmp/vm_engine/vm.o
//   sh c5build.sh recon/src/VMCore/upvm.cpp build/tmp/vm_engine/upvm.o
//   sh c5build.sh recon/src/VMCore/server_crypt_code_mgr.cpp \
//        build/tmp/vm_engine/server_crypt_code_mgr.o
//   g++ -m32 -O2 -fno-stack-protector -I recon -o build/tmp/vm_engine/vm_diff \
//       recon/test_vm_engine.cpp build/tmp/vm_engine/{vm,upvm,server_crypt_code_mgr}.o -ldl
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "src/VMCore/vm.h"

// ---------------------------------------------------------------------------
// ORIG handles and leaf forwarders
// ---------------------------------------------------------------------------
static void *g_h;

static void *need(const char *name)
{
    void *p = dlsym(g_h, name);
    if (p == 0) {
        fprintf(stderr, "FATAL: missing ORIG symbol %s\n", name);
        exit(2);
    }
    return p;
}

typedef void (*RollFn)(unsigned char *, int);
typedef void (*XaoFn)(unsigned char *, int, unsigned int);
typedef void (*NotaFn)(unsigned char *, int);
typedef BOOL (*CallDecryptFn)(int, unsigned char *, UINT32, unsigned char *,
                              UINT32, bool);
typedef int (*DecodeShortFn)(char **, unsigned short *);
typedef void (*CtorFn)(void *);
typedef void (*DtorFn)(void *);
typedef BOOL (*EncFn)(void *, unsigned short, unsigned char *, bool,
                      unsigned char *, unsigned int);

static RollFn f_rol;
static RollFn f_ror;
static XaoFn f_xora;
static XaoFn f_anda;
static XaoFn f_ora;
static NotaFn f_nota;
static CallDecryptFn f_calldecrypt;
static DecodeShortFn f_decodeshort;
static CtorFn f_cc_ctor;
static DtorFn f_cc_dtor;
static EncFn f_cc_enc;

void rol(unsigned char *data, int time) { f_rol(data, time); }
void ror(unsigned char *data, int time) { f_ror(data, time); }
void xora(unsigned char *start, int count, unsigned int seed)
{ f_xora(start, count, seed); }
void anda(unsigned char *start, int count, unsigned int seed)
{ f_anda(start, count, seed); }
void ora(unsigned char *start, int count, unsigned int seed)
{ f_ora(start, count, seed); }
void nota(unsigned char *start, int count) { f_nota(start, count); }
BOOL CallDecrypt(int nCode, PBYTE pbyBuffer, UINT32 dwBufLen, PBYTE pbyKey,
                 UINT32 dwKeyLen, bool bIsEncode)
{
    return f_calldecrypt(nCode, pbyBuffer, dwBufLen, pbyKey, dwKeyLen,
                         bIsEncode);
}
int DecodeShort(char **pstrDecode, unsigned short *pusDest)
{
    return f_decodeshort(pstrDecode, pusDest);
}

CCryptData::CCryptData() { f_cc_ctor(this); }
CCryptData::~CCryptData() { f_cc_dtor(this); }
BOOL CCryptData::EncryptData(unsigned short ushCryptID,
                             unsigned char *pbyCryptKey, bool bEnCrypt,
                             unsigned char *pbySrcData,
                             unsigned int unSrcDataLen)
{
    return f_cc_enc(this, ushCryptID, pbyCryptKey, bEnCrypt, pbySrcData,
                    unSrcDataLen);
}

// ---------------------------------------------------------------------------
// ORIG method table (i386 member-function ABI: this is the first argument)
// ---------------------------------------------------------------------------
typedef BOOL (*fn_Init)(CVM *);
typedef void (*fn_void)(CVM *);
typedef BOOL (*fn_Code)(CVM *, BYTE *, PBYTE &, WORD &);
typedef void (*fn_Header)(CVM *, PBYTE, PWORD, PWORD, PWORD);
typedef void (*fn_DataBuf)(CVM *, PBYTE &, UINT32 &);
typedef BOOL (*fn_Key)(CVM *, PVOID, UINT32);
typedef BOOL (*fn_KeyM)(CVM *, PVOID, UINT32, UINT32);
typedef BOOL (*fn_Run)(CVM *);
typedef BOOL (*fn_Load)(CVM *, PVOID, SIZE_T, PVOID, SIZE_T);
typedef int (*fn_Err)(CVM *);
typedef void (*fn_SetErr)(CVM *, int);

static struct {
    CVMBase *(*GetVM)();
    CVMBase *(*GetUPVM)();
    fn_Init init;
    fn_void release;
    fn_Code getDecode;
    fn_Code getEncode;
    fn_Header getHeader;
    fn_Header setHeader;
    fn_DataBuf getDataBuffer;
    fn_Key setKeyTree;
    fn_KeyM setKeyMatrix;
    fn_Key setKeyLink;
    fn_Run run;
    fn_Load load;
    fn_Err getLastError;
    fn_SetErr setError;
    fn_Init upvmInit;
    fn_void upvmRelease;
    fn_Run upvmRun;
} G;

static int g_fail;
static int g_checks;
static const char *g_case = "";

static void fail(const char *fmt, ...);

static void fail(const char *fmt, ...)
{
    va_list ap;
    ++g_fail;
    fprintf(stderr, "FAIL [%s] ", g_case);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

// ---------------------------------------------------------------------------
// state comparison
// ---------------------------------------------------------------------------
static void cmpState(CVM *a, CVM *b, const char *when, int cmpArena)
{
    if (a->m_iErrCode != b->m_iErrCode)
        fail("%s: m_iErrCode %d/%d", when, a->m_iErrCode, b->m_iErrCode);
    if (a->m_dwTreeLength != b->m_dwTreeLength)
        fail("%s: m_dwTreeLength %u/%u", when, a->m_dwTreeLength,
             b->m_dwTreeLength);
    if (a->m_dwLinkLength != b->m_dwLinkLength)
        fail("%s: m_dwLinkLength %u/%u", when, a->m_dwLinkLength,
             b->m_dwLinkLength);
    if (a->m_pTree != b->m_pTree)
        fail("%s: m_pTree %p/%p", when, a->m_pTree, b->m_pTree);
    if (a->m_pMatrix != b->m_pMatrix)
        fail("%s: m_pMatrix %p/%p", when, a->m_pMatrix, b->m_pMatrix);
    if (a->m_pLink != b->m_pLink)
        fail("%s: m_pLink %p/%p", when, a->m_pLink, b->m_pLink);
    if (a->m_MatrixX != b->m_MatrixX)
        fail("%s: m_MatrixX %u/%u", when, a->m_MatrixX, b->m_MatrixX);
    if (a->m_MatrixY != b->m_MatrixY)
        fail("%s: m_MatrixY %u/%u", when, a->m_MatrixY, b->m_MatrixY);
    if (memcmp(a->m_Reg, b->m_Reg, sizeof(a->m_Reg)) != 0) {
        int i;
        for (i = 0; i < 13; ++i)
            if (a->m_Reg[i] != b->m_Reg[i])
                fail("%s: m_Reg[%d] %08x/%08x", when, i, a->m_Reg[i],
                     b->m_Reg[i]);
    }
    if (a->m_ReserveReg.KMP != b->m_ReserveReg.KMP)
        fail("%s: KMP %p/%p", when, a->m_ReserveReg.KMP,
             b->m_ReserveReg.KMP);
    if (a->m_ReserveReg.KLP != b->m_ReserveReg.KLP)
        fail("%s: KLP %p/%p", when, a->m_ReserveReg.KLP,
             b->m_ReserveReg.KLP);
    if (a->m_ReserveReg.KTP != b->m_ReserveReg.KTP)
        fail("%s: KTP %p/%p", when, a->m_ReserveReg.KTP,
             b->m_ReserveReg.KTP);
    if (a->m_ReserveReg.CL != b->m_ReserveReg.CL)
        fail("%s: CL %u/%u", when, a->m_ReserveReg.CL,
             b->m_ReserveReg.CL);
    if ((a->m_ReserveReg.CBR == 0) != (b->m_ReserveReg.CBR == 0))
        fail("%s: CBR nullness %p/%p", when, a->m_ReserveReg.CBR,
             b->m_ReserveReg.CBR);
    else if (a->m_ReserveReg.CBR != 0 &&
             (a->m_ReserveReg.CBR - (PBYTE)a->m_lpMem) !=
             (b->m_ReserveReg.CBR - (PBYTE)b->m_lpMem))
        fail("%s: CBR offset %ld/%ld", when,
             (long)(a->m_ReserveReg.CBR - (PBYTE)a->m_lpMem),
             (long)(b->m_ReserveReg.CBR - (PBYTE)b->m_lpMem));
    if ((a->m_ReserveReg.DP == 0) != (b->m_ReserveReg.DP == 0))
        fail("%s: DP nullness %p/%p", when, a->m_ReserveReg.DP,
             b->m_ReserveReg.DP);
    else if (a->m_ReserveReg.DP != 0 &&
             (a->m_ReserveReg.DP - (PBYTE)a->m_lpMem) !=
             (b->m_ReserveReg.DP - (PBYTE)b->m_lpMem))
        fail("%s: DP offset %ld/%ld", when,
             (long)(a->m_ReserveReg.DP - (PBYTE)a->m_lpMem),
             (long)(b->m_ReserveReg.DP - (PBYTE)b->m_lpMem));
    if (cmpArena && memcmp(a->m_lpMem, b->m_lpMem, 0x10400) != 0) {
        int i;
        for (i = 0; i < 0x10400; ++i)
            if (((PBYTE)a->m_lpMem)[i] != ((PBYTE)b->m_lpMem)[i]) {
                fail("%s: arena[%#x] %02x/%02x", when, i,
                     ((PBYTE)a->m_lpMem)[i], ((PBYTE)b->m_lpMem)[i]);
                break;
            }
    }
}

// ---------------------------------------------------------------------------
// keys used by key-table instructions
// ---------------------------------------------------------------------------
static UINT32 g_link[8] = { 0x11111111u, 0x22222222u, 0x33333333u,
                            0x44444444u, 0x55555555u, 0x66666666u,
                            0x77777777u, 0x88888888u };
static UINT32 g_matrix[8] = { 0xa0a0a0a0u, 0xb1b1b1b1u, 0xc2c2c2c2u,
                              0xd3d3d3d3u, 0xe4e4e4e4u, 0xf5f5f5f5u,
                              0x06060606u, 0x17171717u };
static UINT32 g_tree[8] = { 0xdeadbeefu, 0xcafebabeu, 0x12345678u,
                            0x9abcdef0u, 0x0fedcba9u, 0x87654321u,
                            0x5a5a5a5au, 0xa5a5a5a5u };

#define KEY_LINK   1
#define KEY_MATRIX 2
#define KEY_TREE   4

// ---------------------------------------------------------------------------
// one Load+Run differential case
// ---------------------------------------------------------------------------
static void doCase(bool upvm, const char *name, const BYTE *prog, int progLen,
                   const BYTE *data, int dataLen, const UINT32 *regs,
                   int setRegs, int keyMode, UINT32 *hostWord,
                   int expectLoad)
{
    BYTE code[0x800];
    BYTE din[0x400];
    int i;

    g_case = name;
    ++g_checks;

    memset(code, 0, sizeof(code));
    memset(din, 0, sizeof(din));
    // DATAHEADER: Len (unused by Load), CodeLen, DataLen, big endian.
    code[0] = 0;
    code[1] = 0;
    code[2] = (BYTE)(progLen >> 8);
    code[3] = (BYTE)progLen;
    code[4] = 0;
    code[5] = 0;
    if (progLen > 0) memcpy(code + 6, prog, progLen);
    if (dataLen > 0) memcpy(din, data, dataLen);

    CVM *a = upvm ? (CVM *)G.GetUPVM() : (CVM *)G.GetVM();
    CVM *b = upvm ? (CVM *)new CUPVM() : new CVM();
    if (a == 0 || b == 0) {
        fail("allocation failed");
        return;
    }

    BOOL ia = upvm ? G.upvmInit(a) : G.init(a);
    BOOL ib = b->Init();
    if (ia != ib) fail("Init %d/%d", ia, ib);

    if (keyMode & KEY_LINK) {
        BOOL ra = G.setKeyLink(a, g_link, 4);
        BOOL rb = b->SetKeyLink(g_link, 4);
        if (ra != rb) fail("SetKeyLink %d/%d", ra, rb);
    }
    if (keyMode & KEY_MATRIX) {
        BOOL ra = G.setKeyMatrix(a, g_matrix, 2, 4);
        BOOL rb = b->SetKeyMatrix(g_matrix, 2, 4);
        if (ra != rb) fail("SetKeyMatrix %d/%d", ra, rb);
    }
    if (keyMode & KEY_TREE) {
        BOOL ra = G.setKeyTree(a, g_tree, 6);
        BOOL rb = b->SetKeyTree(g_tree, 6);
        if (ra != rb) fail("SetKeyTree %d/%d", ra, rb);
    }

    BOOL la = G.load(a, code, 6 + progLen, din, dataLen);
    BOOL lb = b->Load(code, 6 + progLen, din, dataLen);
    if (la != lb) fail("Load %d/%d", la, lb);
    if (la != (BOOL)expectLoad) fail("Load return %d, expected %d", la,
                                     expectLoad);

    // The arena beyond the copied code+data is malloc garbage.  Zero it in
    // both objects so that Run output is deterministic and comparable.
    if (la && lb) {
        memset((PBYTE)a->m_lpMem + a->m_ReserveReg.CL + dataLen, 0,
               0x10400 - a->m_ReserveReg.CL - dataLen);
        memset((PBYTE)b->m_lpMem + b->m_ReserveReg.CL + dataLen, 0,
               0x10400 - b->m_ReserveReg.CL - dataLen);
    }
    cmpState(a, b, "after Load", la && lb);

    if (!expectLoad) {
        G.release(a);
        b->Release();
        return;
    }

    if (setRegs) {
        for (i = 0; i < 13; ++i) {
            a->m_Reg[i] = regs[i];
            b->m_Reg[i] = regs[i];
        }
    }

    if (hostWord != 0) *hostWord = 0xAAAAAAAAu;
    BOOL ra = upvm ? G.upvmRun(a) : G.run(a);
    UINT32 ha = hostWord ? *hostWord : 0;
    if (hostWord != 0) *hostWord = 0xAAAAAAAAu;
    BOOL rb = b->Run();
    UINT32 hb = hostWord ? *hostWord : 0;

    if (ra != rb) fail("Run %d/%d", ra, rb);
    if (hostWord != 0 && ha != hb)
        fail("host word %08x/%08x", ha, hb);
    cmpState(a, b, "after Run", 1);

    if (upvm) G.upvmRelease(a); else G.release(a);
    b->Release();
}

// ---------------------------------------------------------------------------
// header/code-pointer accessors
// ---------------------------------------------------------------------------
static void accessorTests()
{
    int it;
    g_case = "GetHeader/SetHeader";
    ++g_checks;

    for (it = 0; it < 400; ++it) {
        BYTE h1[6], h2[6];
        WORD a1 = 0, b1 = 0, c1 = 0, a2 = 0, b2 = 0, c2 = 0;
        int i;
        for (i = 0; i < 6; ++i) h1[i] = h2[i] = (BYTE)(rand() & 0xff);
        CVM *m = new CVM();
        G.getHeader(m, 0, (PWORD)&a1, (PWORD)&b1, (PWORD)&c1);
        m->GetHeader(0, (PWORD)&a2, (PWORD)&b2, (PWORD)&c2);
        if (a1 != a2 || b1 != b2 || c1 != c2)
            fail("GetHeader(null) outputs differ");
        G.getHeader(m, h1, (PWORD)&a1, (PWORD)&b1, (PWORD)&c1);
        m->GetHeader(h2, (PWORD)&a2, (PWORD)&b2, (PWORD)&c2);
        if (a1 != a2 || b1 != b2 || c1 != c2)
            fail("GetHeader %04x/%04x %04x/%04x %04x/%04x", a1, a2, b1, b2,
                 c1, c2);
        // Null out-param forms must not be written.
        G.getHeader(m, h1, 0, (PWORD)&b1, 0);
        m->GetHeader(h2, 0, (PWORD)&b2, 0);
        if (b1 != b2) fail("GetHeader null-args differ");
        G.setHeader(m, h1, (PWORD)&a1, (PWORD)&b1, (PWORD)&c1);
        m->SetHeader(h2, (PWORD)&a2, (PWORD)&b2, (PWORD)&c2);
        if (memcmp(h1, h2, 6) != 0) fail("SetHeader bytes differ");
        // Round trip through the other implementation.
        BYTE h3[6];
        memcpy(h3, h1, 6);
        G.getHeader(m, h3, (PWORD)&a2, (PWORD)&b2, (PWORD)&c2);
        if (a2 != a1 || b2 != b1 || c2 != c1)
            fail("SetHeader/GetHeader round trip differs");
        delete m;
    }

    g_case = "GetDecodeCode/GetEncodeCode";
    ++g_checks;
    for (it = 0; it < 400; ++it) {
        BYTE buf[0x200];
        int i;
        for (i = 0; i < (int)sizeof(buf); ++i) buf[i] = (BYTE)(rand() & 0xff);
        WORD w1 = (WORD)(rand() & 0x3ff);
        WORD inner2 = (WORD)(rand() & 0x3ff);
        // inner header 1 at +0xb; its CodeLen lives at +0xd
        buf[0xd] = (BYTE)(w1 >> 8);
        buf[0xe] = (BYTE)w1;
        int p2 = 0x11 + w1;
        if (p2 + 6 < (int)sizeof(buf)) {
            buf[p2 + 2] = (BYTE)(inner2 >> 8);
            buf[p2 + 3] = (BYTE)inner2;
        }
        CVM *m = new CVM();
        PBYTE pa = 0, pb = 0;
        WORD la = 0, lb = 0;
        BOOL ra = G.getDecode(m, buf, pa, la);
        BOOL rb = m->GetDecodeCode(buf, pb, lb);
        if (ra != rb) fail("GetDecodeCode ret %d/%d", ra, rb);
        if (pa != pb) fail("GetDecodeCode ptr off %ld/%ld",
                           (long)(pa - buf), (long)(pb - buf));
        if (la != lb) fail("GetDecodeCode len %u/%u", la, lb);
        PBYTE qa = 0, qb = 0;
        WORD ma = 0, mb = 0;
        ra = G.getEncode(m, buf, qa, ma);
        rb = m->GetEncodeCode(buf, qb, mb);
        if (ra != rb) fail("GetEncodeCode ret %d/%d", ra, rb);
        if (qa != qb) fail("GetEncodeCode ptr off %ld/%ld",
                           (long)(qa - buf), (long)(qb - buf));
        if (ma != mb) fail("GetEncodeCode len %u/%u", ma, mb);
        // Null buffer.
        ra = G.getDecode(m, 0, pa, la);
        rb = m->GetDecodeCode(0, pb, lb);
        if (ra != rb || ra != FALSE) fail("GetDecodeCode(null) %d/%d", ra, rb);
        ra = G.getEncode(m, 0, qa, ma);
        rb = m->GetEncodeCode(0, qb, mb);
        if (ra != rb || ra != FALSE) fail("GetEncodeCode(null) %d/%d", ra, rb);
        delete m;
    }
}

// ---------------------------------------------------------------------------
// error helpers, key setters, GetDataBuffer
// ---------------------------------------------------------------------------
static void simpleTests()
{
    g_case = "error state / key setters / data buffer";
    ++g_checks;
    CVM *a = (CVM *)G.GetVM();
    CVM *b = new CVM();

    if (G.getLastError(a) != b->GetLastError())
        fail("initial GetLastError");
    G.setError(a, 7);
    b->SetError(7);
    if (G.getLastError(a) != b->GetLastError()) fail("SetError(7)");
    G.setError(a, -3);
    b->SetError(-3);
    if (G.getLastError(a) != b->GetLastError()) fail("SetError(-3)");

    if (G.setKeyLink(a, 0, 5) != b->SetKeyLink(0, 5))
        fail("SetKeyLink(null) ret");
    if (G.getLastError(a) != b->GetLastError()) fail("SetKeyLink(null) err");
    if (G.setKeyTree(a, 0, 5) != b->SetKeyTree(0, 5))
        fail("SetKeyTree(null) ret");
    if (G.setKeyMatrix(a, 0, 5, 6) != b->SetKeyMatrix(0, 5, 6))
        fail("SetKeyMatrix(null) ret");
    if (G.setKeyLink(a, g_link, 3) != b->SetKeyLink(g_link, 3))
        fail("SetKeyLink(ptr) ret");
    if (G.setKeyTree(a, g_tree, 7) != b->SetKeyTree(g_tree, 7))
        fail("SetKeyTree(ptr) ret");
    if (G.setKeyMatrix(a, g_matrix, 3, 5) != b->SetKeyMatrix(g_matrix, 3, 5))
        fail("SetKeyMatrix(ptr) ret");
    if (G.getLastError(a) != b->GetLastError()) fail("key setters err");
    cmpState(a, b, "key setters", 0);

    PBYTE da = 0, db = 0;
    UINT32 na = 0, nb = 0;
    G.getDataBuffer(a, da, na);
    b->GetDataBuffer(db, nb);
    if (da != db || na != nb) fail("GetDataBuffer default");

    // Load a known image, then GetDataBuffer must expose DP/reg2.
    BYTE code[16], data[8];
    memset(code, 0, sizeof(code));
    memset(data, 0x5a, sizeof(data));
    code[2] = 0;
    code[3] = 4;
    code[4] = 0;
    code[5] = 8;
    BOOL la = G.load(a, code, 10, data, 8);
    BOOL lb = b->Load(code, 10, data, 8);
    if (la != lb) fail("Load in simpleTests");
    G.getDataBuffer(a, da, na);
    b->GetDataBuffer(db, nb);
    if ((da - (PBYTE)a->m_lpMem) != (db - (PBYTE)b->m_lpMem))
        fail("GetDataBuffer ptr offset");
    if (na != nb) fail("GetDataBuffer len");
    memset((PBYTE)a->m_lpMem + a->m_ReserveReg.CL + 8, 0,
           0x10400 - a->m_ReserveReg.CL - 8);
    memset((PBYTE)b->m_lpMem + b->m_ReserveReg.CL + 8, 0,
           0x10400 - b->m_ReserveReg.CL - 8);
    cmpState(a, b, "after Load", 1);

    G.release(a);
    b->Release();
}

// ---------------------------------------------------------------------------
// Load edge cases
// ---------------------------------------------------------------------------
static void loadTests()
{
    static BYTE bigCode[0x20000];
    static BYTE bigData[0x800];
    int it;

    g_case = "Load edge cases";
    ++g_checks;
    memset(bigCode, 0xcc, sizeof(bigCode));
    memset(bigData, 0x3c, sizeof(bigData));

    for (it = 0; it < 600; ++it) {
        // Random header words.
        WORD len = (WORD)(rand() & 0xffff);
        WORD clen = (WORD)(rand() & 0xffff);
        WORD dlen = (WORD)(rand() & 0xffff);
        bigCode[0] = (BYTE)(len >> 8);
        bigCode[1] = (BYTE)len;
        bigCode[2] = (BYTE)(clen >> 8);
        bigCode[3] = (BYTE)clen;
        bigCode[4] = (BYTE)(dlen >> 8);
        bigCode[5] = (BYTE)dlen;
        SIZE_T size = (SIZE_T)(rand() % 0x800);
        SIZE_T dsize = (SIZE_T)(rand() % 0x300);

        CVM *a = (CVM *)G.GetVM();
        CVM *b = new CVM();
        BOOL ra = G.load(a, bigCode, size, bigData, dsize);
        BOOL rb = b->Load(bigCode, size, bigData, dsize);
        if (ra != rb) fail("Load ret %d/%d (len=%u clen=%u dlen=%u size=%u)",
                           ra, rb, len, clen, dlen, (unsigned)size);
        if (ra && rb) {
            memset((PBYTE)a->m_lpMem + a->m_ReserveReg.CL + dsize, 0,
                   0x10400 - a->m_ReserveReg.CL - dsize);
            memset((PBYTE)b->m_lpMem + b->m_ReserveReg.CL + dsize, 0,
                   0x10400 - b->m_ReserveReg.CL - dsize);
        }
        cmpState(a, b, "Load edge", ra && rb);
        G.release(a);
        b->Release();
    }

    // Null arguments.
    {
        CVM *a = (CVM *)G.GetVM();
        CVM *b = new CVM();
        BOOL ra = G.load(a, 0, 10, bigData, 4);
        BOOL rb = b->Load(0, 10, bigData, 4);
        if (ra != rb || ra != FALSE) fail("Load null code");
        if (G.getLastError(a) != b->GetLastError()) fail("Load null code err");
        ra = G.load(a, bigCode, 10, 0, 4);
        rb = b->Load(bigCode, 10, 0, 4);
        if (ra != rb || ra != FALSE) fail("Load null data");
        if (G.getLastError(a) != b->GetLastError()) fail("Load null data err");
        G.release(a);
        b->Release();
    }

    // Size below 6 with a large CodeLen: the original wraps the subtraction.
    {
        memset(bigCode, 0, 16);
        bigCode[2] = 0;
        bigCode[3] = 4;   // CodeLen = 4
        CVM *a = (CVM *)G.GetVM();
        CVM *b = new CVM();
        BOOL ra = G.load(a, bigCode, 3, bigData, 4);
        BOOL rb = b->Load(bigCode, 3, bigData, 4);
        if (ra != rb) fail("Load size<6 ret %d/%d", ra, rb);
        memset((PBYTE)a->m_lpMem + a->m_ReserveReg.CL + 4, 0,
               0x10400 - a->m_ReserveReg.CL - 4);
        memset((PBYTE)b->m_lpMem + b->m_ReserveReg.CL + 4, 0,
               0x10400 - b->m_ReserveReg.CL - 4);
        cmpState(a, b, "size<6", 1);
        G.release(a);
        b->Release();
    }

    // Negative header words: the sign test happens after the copies.  A large
    // CodeBufferSize lets the CodeLen sign bit reach that test.
    {
        memset(bigCode, 0x20, 0x9000);
        bigCode[2] = 0x80;   // CodeLen = 0x8000
        bigCode[3] = 0x00;
        bigCode[4] = 0x00;   // DataLen = 0
        bigCode[5] = 0x00;
        CVM *a = (CVM *)G.GetVM();
        CVM *b = new CVM();
        BOOL ra = G.load(a, bigCode, 0x9000, bigData, 4);
        BOOL rb = b->Load(bigCode, 0x9000, bigData, 4);
        if (ra != rb || ra != FALSE) fail("Load neg CodeLen %d/%d", ra, rb);
        if (G.getLastError(a) != 4 || b->GetLastError() != 4)
            fail("Load neg CodeLen err %d/%d", G.getLastError(a),
                 b->GetLastError());
        G.release(a);
        b->Release();
    }
    {
        memset(bigCode, 0x20, 0x100);
        bigCode[2] = 0x00;   // CodeLen = 0
        bigCode[3] = 0x00;
        bigCode[4] = 0x80;   // DataLen = 0x8000
        bigCode[5] = 0x00;
        CVM *a = (CVM *)G.GetVM();
        CVM *b = new CVM();
        BOOL ra = G.load(a, bigCode, 0x100, bigData, 4);
        BOOL rb = b->Load(bigCode, 0x100, bigData, 4);
        if (ra != rb || ra != FALSE) fail("Load neg DataLen %d/%d", ra, rb);
        if (G.getLastError(a) != 4 || b->GetLastError() != 4)
            fail("Load neg DataLen err %d/%d", G.getLastError(a),
                 b->GetLastError());
        G.release(a);
        b->Release();
    }
}

// ---------------------------------------------------------------------------
// vtable layout / dispatch check
// ---------------------------------------------------------------------------
static void vtableTests()
{
    g_case = "vtable layout";
    ++g_checks;
    CVM *ref = (CVM *)G.GetVM();
    CVM *sub = (CVM *)G.GetUPVM();
    CVM *mref = new CVM();
    CVM *msub = new CUPVM();
    void **va = *(void ***)ref;
    void **vb = *(void ***)sub;
    void **ma = *(void ***)mref;
    void **mb = *(void ***)msub;

    // Slots 2..9 and 11..13 are inherited by CUPVM; 0, 1 and 10 are
    // overridden.  Check that both the ORIG and the reconstruction have the
    // same sharing pattern.
    int i;
    for (i = 0; i < 16; ++i) {
        int inherited = (i >= 2 && i <= 9) || (i >= 11 && i <= 13);
        if (inherited) {
            if (va[i] != vb[i]) fail("ORIG slot %d not shared", i);
            if (ma[i] != mb[i]) fail("recon slot %d not shared", i);
        } else {
            if (va[i] == vb[i]) fail("ORIG slot %d not overridden", i);
            if (ma[i] == mb[i]) fail("recon slot %d not overridden", i);
        }
    }

    // Call the slots that have simple, comparable behaviour.
    fn_Init fi = (fn_Init)va[0];
    fn_Init mfi = (fn_Init)ma[0];
    if (fi(ref) != mfi(mref) || fi(ref) != TRUE) fail("slot0 Init");
    fn_Err fe = (fn_Err)va[12];
    fn_SetErr fse = (fn_SetErr)va[13];
    fn_Err mfe = (fn_Err)ma[12];
    fn_SetErr mfse = (fn_SetErr)ma[13];
    fse(ref, 42);
    mfse(mref, 42);
    if (fe(ref) != mfe(mref) || fe(ref) != 42) fail("slot12/13 error");

    fn_Key fk = (fn_Key)va[7];
    fn_Key mfk = (fn_Key)ma[7];
    if (fk(ref, g_tree, 6) != mfk(mref, g_tree, 6)) fail("slot7 SetKeyTree");
    fn_KeyM fkm = (fn_KeyM)va[8];
    fn_KeyM mfkm = (fn_KeyM)ma[8];
    if (fkm(ref, g_matrix, 2, 3) != mfkm(mref, g_matrix, 2, 3))
        fail("slot8 SetKeyMatrix");
    fn_Key fkl = (fn_Key)va[9];
    fn_Key mfkl = (fn_Key)ma[9];
    if (fkl(ref, g_link, 4) != mfkl(mref, g_link, 4)) fail("slot9 SetKeyLink");

    BYTE code[16], data[8];
    memset(code, 0, sizeof(code));
    memset(data, 0x11, sizeof(data));
    code[3] = 4;
    code[5] = 8;
    fn_Load fl = (fn_Load)va[11];
    fn_Load mfl = (fn_Load)ma[11];
    if (fl(ref, code, 10, data, 8) != mfl(mref, code, 10, data, 8))
        fail("slot11 Load");
    fn_Run fr = (fn_Run)va[10];
    fn_Run mfr = (fn_Run)ma[10];
    // code[6..9] is zero opcodes -> instruction 0 raises InvalidInstruction.
    if (fr(ref) != mfr(mref)) fail("slot10 Run");

    // Slot 1 Release on fresh objects (also verifies the deleting dtor path).
    CVM *x = (CVM *)G.GetVM();
    CVM *y = new CVM();
    ((fn_void)va[1])(x);
    ((fn_void)ma[1])(y);
    CVM *xu = (CVM *)G.GetUPVM();
    CVM *yu = new CUPVM();
    ((fn_void)vb[1])(xu);
    ((fn_void)mb[1])(yu);
    // Release is guarded by an explicit `this` test in the original; calling
    // it through the vtable with a null object must not dereference the vptr.
    ((fn_void)va[1])(0);
    ((fn_void)ma[1])(0);
    ((fn_void)vb[1])(0);
    ((fn_void)mb[1])(0);

    G.release(ref);
    G.release(sub);
    delete msub;
    delete mref;
}

// ---------------------------------------------------------------------------
// opcode test programs
// ---------------------------------------------------------------------------
static BYTE g_data[0x200];

static void initData()
{
    int i;
    for (i = 0; i < (int)sizeof(g_data); ++i)
        g_data[i] = (BYTE)(i * 7 + 3);
}

static void opcodeTests(bool upvm)
{
    UINT32 r[13];
    BYTE p[64];
    int i;

    initData();
    for (i = 0; i < 13; ++i) r[i] = 0;

    // --- halt / nop ------------------------------------------------------
    { BYTE q[1]; q[0] = 0x22;
      doCase(upvm, upvm ? "U:halt" : "C:halt", q, 1, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[1]; q[0] = 0x23;
      doCase(upvm, upvm ? "U:nop" : "C:nop", q, 1, g_data, 0x40, r, 1, 0, 0, 1); }
    // CodeLen == 0: Run returns TRUE without executing anything.
    { BYTE q[1]; q[0] = 0;
      doCase(upvm, upvm ? "U:empty" : "C:empty", q, 0, g_data, 0x10, r, 1,
             0, 0, 1); }
    // m_Reg[0] beyond CL is a silent FALSE (no SetError), tested through an
    // unchecked conditional-jump target.
    if (!upvm) {
        r[2] = 0;
        r[3] = 0;
        r[4] = 0x100;
        { BYTE q[6] = { 0x1c, 0x02, 0x03, 0x04, 0x22, 0x23 };
          doCase(0, "C:pc-overrun", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
        // Unchecked call target beyond CL.
        r[1] = 0x8000;
        { BYTE q[3] = { 0x3c, 0xff, 0x00 };
          doCase(0, "C:call-overrun", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        // Jump target exactly at CL (allowed; the byte at CL is data).
        { BYTE q[8] = { 0x1a, 0x08, 0x00, 0x22, 0x23, 0x23, 0x23, 0x23 };
          doCase(0, "C:1a-at-CL", q, 8, g_data, 0x40, r, 1, 0, 0, 1); }
    }

    // --- copies ----------------------------------------------------------
    { BYTE q[6] = { 0x01, 0x10, 0x00, 0x20, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:01" : "C:01", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x02, 0x10, 0x00, 0x20, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:02" : "C:02", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x03, 0x10, 0x00, 0x20, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:03" : "C:03", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    // 0x02/0x03 with the 0xffff sentinel.
    { BYTE q[6] = { 0x02, 0xff, 0xff, 0x20, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:02-ffff" : "C:02-ffff", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x03, 0x10, 0x00, 0xfe, 0xff, 0x22 };
      doCase(upvm, upvm ? "U:03-bounds" : "C:03-bounds", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }

    // --- stores/loads, immediate address ---------------------------------
    { BYTE q[7] = { 0x04, 0x10, 0x00, 0x05, 0x22, 0, 0 };
      for (i = 0; i < 13; ++i) r[i] = 0x1000 + i;
      doCase(upvm, upvm ? "U:04" : "C:04", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x05, 0x10, 0x00, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:05" : "C:05", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x06, 0x10, 0x00, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:06" : "C:06", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x07, 0x05, 0x10, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:07" : "C:07", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x08, 0x05, 0x10, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:08" : "C:08", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x09, 0x05, 0x10, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:09" : "C:09", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x09, 0x05, 0xfe, 0xff, 0x22 };
      doCase(upvm, upvm ? "U:09-ffff" : "C:09-ffff", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }

    // --- register/indirect moves -----------------------------------------
    { BYTE q[4] = { 0x0a, 0x02, 0x03, 0x22 };
      for (i = 0; i < 13; ++i) r[i] = 0x2000 + i;
      doCase(upvm, upvm ? "U:0a" : "C:0a", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0a, 0x0d, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:0a-badreg" : "C:0a-badreg", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    for (i = 0; i < 13; ++i) r[i] = 0;
    r[3] = 0x20;
    { BYTE q[4] = { 0x0b, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:0b" : "C:0b", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0c, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:0c" : "C:0c", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0d, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:0d" : "C:0d", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0e, 0x03, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:0e" : "C:0e", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0f, 0x03, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:0f" : "C:0f", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x10, 0x03, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:10" : "C:10", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    r[3] = 0x10000;   // out of 16-bit range
    { BYTE q[4] = { 0x0b, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:0b-range" : "C:0b-range", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    r[3] = 0;

    // --- stack -----------------------------------------------------------
    r[1] = 0x8000;
    for (i = 0; i < 13; ++i) r[i] = (i == 1) ? 0x8000 : (0x3000 + i);
    { BYTE q[3] = { 0x11, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:11" : "C:11", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x12, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:12" : "C:12", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x13, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:13" : "C:13", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    r[1] = 0x8008;
    { BYTE q[3] = { 0x14, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:14" : "C:14", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x15, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:15" : "C:15", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x16, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:16" : "C:16", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    r[1] = 0x7fff;   // underflow
    { BYTE q[3] = { 0x14, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:14-under" : "C:14-under", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    r[1] = 0x8000;

    // --- immediates ------------------------------------------------------
    for (i = 0; i < 13; ++i) r[i] = 0;
    { BYTE q[4] = { 0x17, 0x04, 0x5a, 0x22 };
      doCase(upvm, upvm ? "U:17" : "C:17", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x18, 0x04, 0x34, 0x12, 0x22 };
      doCase(upvm, upvm ? "U:18" : "C:18", q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x19, 0x04, 0x78, 0x56, 0x34, 0x12, 0x22 };
      doCase(upvm, upvm ? "U:19" : "C:19", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }

    if (!upvm) {
        // --- control flow (CVM only) -------------------------------------
        r[0] = 0;
        { BYTE q[12] = { 0x1a, 0x08, 0x00, 0x23, 0x23, 0x23, 0x23, 0x23,
                         0x22, 0x23, 0x23, 0x23 };
          doCase(0, "C:1a", q, 12, g_data, 0x40, r, 1, 0, 0, 1); }
        { BYTE q[3] = { 0x1a, 0xff, 0x00 };   // target beyond CL
          doCase(0, "C:1a-bounds", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        r[4] = 8;
        { BYTE q[12] = { 0x1b, 0x04, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
                         0x22, 0x23, 0x23, 0x23 };
          doCase(0, "C:1b", q, 12, g_data, 0x40, r, 1, 0, 0, 1); }
        r[4] = 0x100;
        { BYTE q[3] = { 0x1b, 0x04, 0x22 };
          doCase(0, "C:1b-bounds", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        r[4] = 0;

        // Conditional jumps: taken and not taken, plus the regA<=CL quirk.
        {
            BYTE ops[6] = { 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21 };
            int k;
            for (k = 0; k < 6; ++k) {
                int t;
                for (t = 0; t < 4; ++t) {
                    UINT32 va = (t & 1) ? 5 : 3;
                    UINT32 vb = (t & 2) ? 5 : 3;
                    BYTE q[16];
                    char nm[32];
                    sprintf(nm, "C:%02x-%d", ops[k], t);
                    memset(q, 0x23, sizeof(q));
                    q[0] = ops[k];
                    q[1] = 0x02;      // regA = r2
                    q[2] = 0x03;      // regB = r3
                    q[3] = 0x04;      // target register = r4
                    q[4] = 0x22;      // fall-through halt
                    q[8] = 0x19;      // taken path: mov r7, 0x11223344
                    q[9] = 0x07;
                    q[10] = 0x44;
                    q[11] = 0x33;
                    q[12] = 0x22;
                    q[13] = 0x11;
                    q[14] = 0x22;     // halt
                    r[2] = va;
                    r[3] = vb;
                    r[4] = 8;
                    r[7] = 0;
                    doCase(0, nm, q, 16, g_data, 0x40, r, 1, 0, 0, 1);
                }
            }
            // regA > CL -> RunTimeError even though the branch condition is
            // satisfiable (original quirk).
            r[2] = 0x200;
            r[3] = 0x200;
            r[4] = 8;
            { BYTE q[6] = { 0x1c, 0x02, 0x03, 0x04, 0x22, 0x23 };
              doCase(0, "C:1c-quirk", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
            r[2] = 0;
        }

        // loop (0x24)
        {
            int k;
            for (k = 0; k < 4; ++k) {
                BYTE q[9] = { 0x24, 0x07, 0x00, 0x22, 0x23, 0x23, 0x23, 0x22,
                              0x23 };
                char nm[24];
                sprintf(nm, "C:24-%d", k);
                r[3] = (UINT32)k;
                doCase(0, nm, q, 9, g_data, 0x40, r, 1, 0, 0, 1);
            }
            r[3] = 0;
        }

        // call/ret
        { BYTE q[10] = { 0x3c, 0x08, 0x00, 0x22, 0x23, 0x23, 0x23, 0x23,
                         0x23, 0x3d };
          for (i = 0; i < 13; ++i) r[i] = 0;
          r[1] = 0x8000;
          doCase(0, "C:3c/3d", q, 10, g_data, 0x40, r, 1, 0, 0, 1); }
        { BYTE q[6] = { 0x06, 0x00, 0x80, 0x05, 0x3d, 0x22 };
          for (i = 0; i < 13; ++i) r[i] = 0;
          r[1] = 0x8002;
          r[5] = 0x0000ffff;
          doCase(0, "C:3d-bounds", q, 6, g_data, 0x40, r, 1, 0, 0, 1); }

        // host memory access
        {
            UINT32 host = 0x12345678;
            BYTE q[8] = { 0x40, 0x04, 0, 0, 0, 0, 0x22, 0x23 };
            *(UINT32 *)(q + 2) = (UINT32)(size_t)&host;
            doCase(0, "C:40", q, 7, g_data, 0x40, r, 1, 0, 0, 1);

            UINT32 host2 = 0x9abcdef0;
            BYTE q2[8] = { 0x42, 0, 0, 0, 0, 0x04, 0x22, 0x23 };
            *(UINT32 *)(q2 + 1) = (UINT32)(size_t)&host2;
            doCase(0, "C:42", q2, 7, g_data, 0x40, r, 1, 0, &host2, 1);


            UINT32 host3 = 0x0badf00d;
            r[2] = (UINT32)(size_t)&host3;
            { BYTE q3[4] = { 0x41, 0x04, 0x02, 0x22 };
              doCase(0, "C:41", q3, 4, g_data, 0x40, r, 1, 0, 0, 1); }
            host3 = 0xcafebabe;
            r[2] = (UINT32)(size_t)&host3;
            r[5] = 0x11223344;
            { BYTE q4[4] = { 0x43, 0x02, 0x05, 0x22 };
              doCase(0, "C:43", q4, 4, g_data, 0x40, r, 1, 0, &host3, 1); }
            r[2] = 0;
            r[5] = 0;
        }

        // CallDecrypt (0x3e / 0x3f): TenDes needs an 8-byte key.
        {
            for (i = 0; i < 13; ++i) r[i] = 0;
            r[2] = 0x20;   // data length
            r[3] = 0;      // key at DP
            r[4] = 8;      // key length
            { BYTE q[4] = { 0x3e, 0x00, 0x00, 0x22 };
              doCase(0, "C:3e-des", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
            { BYTE q[3] = { 0x3f, 0x05, 0x22 };
              r[5] = 0;
              doCase(0, "C:3f-des", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
            // Unsupported algorithm id -> CallDecrypt returns FALSE.
            { BYTE q[4] = { 0x3e, 0x0e, 0x00, 0x22 };
              doCase(0, "C:3e-bad-id", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
            // Length above 0xffff -> FALSE.
            r[2] = 0x10000;
            { BYTE q[4] = { 0x3e, 0x00, 0x00, 0x22 };
              doCase(0, "C:3e-len", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
            r[2] = 0x20;
        }
    } else {
        // UPVM call-outs (same ids, bIsEncode = false).
        for (i = 0; i < 13; ++i) r[i] = 0;
        r[2] = 0x20;
        r[3] = 0;
        r[4] = 8;
        { BYTE q[4] = { 0x3e, 0x00, 0x00, 0x22 };
          doCase(1, "U:3e-des", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
        { BYTE q[4] = { 0x3f, 0x00, 0x00, 0x22 };
          doCase(1, "U:3f-des", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    }

    // --- bulk transforms --------------------------------------------------
    for (i = 0; i < 13; ++i) r[i] = 0;
    r[2] = 0x20;
    r[5] = 0x12345678;
    { BYTE q[3] = { 0x44, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:44" : "C:44", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x45, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:45" : "C:45", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x46, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:46" : "C:46", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[2] = { 0x47, 0x22 };
      doCase(upvm, upvm ? "U:47" : "C:47", q, 2, g_data, 0x40, r, 1, 0, 0, 1); }
    r[2] = 0;

    // --- arithmetic -------------------------------------------------------
    for (i = 0; i < 13; ++i) r[i] = 0x1000 + i * 0x101;
    { BYTE q[4] = { 0x25, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:25" : "C:25", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x26, 0x02, 0x78, 0x56, 0x34, 0x12, 0x22 };
      doCase(upvm, upvm ? "U:26" : "C:26", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x27, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:27" : "C:27", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x28, 0x02, 0x78, 0x56, 0x34, 0x12, 0x22 };
      doCase(upvm, upvm ? "U:28" : "C:28", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x29, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:29" : "C:29", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x2a, 0x02, 0x78, 0x56, 0x34, 0x12, 0x22 };
      doCase(upvm, upvm ? "U:2a" : "C:2a", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    r[3] = 7;
    { BYTE q[4] = { 0x2b, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:2b" : "C:2b", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    r[3] = 0x1000 + 3 * 0x101;
    { BYTE q[7] = { 0x2c, 0x02, 0x09, 0x00, 0x00, 0x00, 0x22 };
      doCase(upvm, upvm ? "U:2c" : "C:2c", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    for (i = 0; i < 13; ++i) r[i] = 0x1000 + i * 0x101;
    { BYTE q[4] = { 0x2d, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:2d" : "C:2d", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x2e, 0x02, 0x0f, 0x0f, 0x0f, 0x0f, 0x22 };
      doCase(upvm, upvm ? "U:2e" : "C:2e", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x2f, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:2f" : "C:2f", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x30, 0x02, 0xf0, 0xf0, 0xf0, 0xf0, 0x22 };
      doCase(upvm, upvm ? "U:30" : "C:30", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x31, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:31" : "C:31", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[7] = { 0x32, 0x02, 0xaa, 0xaa, 0xaa, 0xaa, 0x22 };
      doCase(upvm, upvm ? "U:32" : "C:32", q, 7, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x33, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:33" : "C:33", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x34, 0x02, 0x05, 0x22 };
      doCase(upvm, upvm ? "U:34" : "C:34", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    r[4] = 13;
    { BYTE q[3] = { 0x35, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:35" : "C:35", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x36, 0x02, 0x07, 0x22 };
      doCase(upvm, upvm ? "U:36" : "C:36", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x37, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:37" : "C:37", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x38, 0x02, 0x03, 0x22 };
      doCase(upvm, upvm ? "U:38" : "C:38", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x39, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:39" : "C:39", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x3a, 0x02, 0x04, 0x22 };
      doCase(upvm, upvm ? "U:3a" : "C:3a", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[3] = { 0x3b, 0x02, 0x22 };
      doCase(upvm, upvm ? "U:3b" : "C:3b", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }

    // --- invalid opcodes --------------------------------------------------
    {
        BYTE inv[8] = { 0x00, 0x51, 0x58, 0x5b, 0x5e, 0xff, 0x22, 0x23 };
        for (i = 0; i < 6; ++i) {
            char nm[32];
            sprintf(nm, "%s:invalid-%02x", upvm ? "U" : "C", inv[i]);
            doCase(upvm, nm, inv + i, 2, g_data, 0x40, r, 1, 0, 0, 1);
        }
        if (upvm) {
            unsigned char uinv[6] = { 0x1a, 0x24, 0x3c, 0x40, 0x48, 0x51 };
            for (i = 0; i < 6; ++i) {
                char nm[32];
                sprintf(nm, "U:unimpl-%02x", uinv[i]);
                doCase(upvm, nm, uinv + i, 2, g_data, 0x40, r, 1, 0, 0, 1);
            }
        }
    }

    // --- key tables (CVM only) -------------------------------------------
    if (!upvm) {
        for (i = 0; i < 13; ++i) r[i] = 0;
        { BYTE q[3] = { 0x48, 0x02, 0x22 };
          doCase(0, "C:48", q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        { BYTE q[3] = { 0x48, 0x09, 0x22 };
          doCase(0, "C:48-bounds", q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        { BYTE q[3] = { 0x48, 0x02, 0x22 };
          doCase(0, "C:48-nokey", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        r[2] = 1;
        { BYTE q[3] = { 0x49, 0x02, 0x22 };
          doCase(0, "C:49", q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        r[2] = 9;
        { BYTE q[3] = { 0x49, 0x02, 0x22 };
          doCase(0, "C:49-bounds", q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        r[2] = 0xc;   // bad register index
        { BYTE q[3] = { 0x49, 0x0d, 0x22 };
          doCase(0, "C:49-badreg", q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        r[2] = 0;
        { BYTE q[3] = { 0x4a, 0x05, 0x22 };
          doCase(0, "C:4a", q, 3, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        { BYTE q[3] = { 0x4a, 0x08, 0x22 };
          doCase(0, "C:4a-bounds", q, 3, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        r[2] = 3;
        { BYTE q[3] = { 0x4b, 0x02, 0x22 };
          doCase(0, "C:4b", q, 3, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        { BYTE q[3] = { 0x4c, 0x03, 0x22 };
          doCase(0, "C:4c", q, 3, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
        { BYTE q[3] = { 0x4c, 0x07, 0x22 };
          doCase(0, "C:4c-bounds", q, 3, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
        r[2] = 4;
        { BYTE q[3] = { 0x4d, 0x02, 0x22 };
          doCase(0, "C:4d", q, 3, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
        // stores: reg5 = index, reg6 = value
        r[5] = 1;
        r[6] = 0xfeedface;
        { BYTE q[2] = { 0x4e, 0x22 };
          doCase(0, "C:4e", q, 2, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        { BYTE q[2] = { 0x4f, 0x22 };
          doCase(0, "C:4f", q, 2, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        { BYTE q[2] = { 0x50, 0x22 };
          doCase(0, "C:50", q, 2, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
        r[5] = 9;
        { BYTE q[2] = { 0x4e, 0x22 };
          doCase(0, "C:4e-bounds", q, 2, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        r[5] = 0;
        r[6] = 0;

        // --- block crypto -------------------------------------------------
        for (i = 0; i < 13; ++i) r[i] = 0;
        r[2] = 0x20;   // data length
        r[3] = 0x10;   // key at DP+0x10
        r[4] = 16;     // key length
        r[5] = 1;      // encode
        { BYTE q[4] = { 0x5c, 0x00, 0x00, 0x22 };
          doCase(0, "C:5c-id0", q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
        r[6] = 0x1a;
        { BYTE q[3] = { 0x5d, 0x06, 0x22 };
          doCase(0, "C:5d-id26", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        r[6] = 0x1b;   // out of range -> FALSE -> BinCodeError
        { BYTE q[3] = { 0x5d, 0x06, 0x22 };
          doCase(0, "C:5d-bad-id", q, 3, g_data, 0x40, r, 1, 0, 0, 1); }
        r[6] = 0;
        // no manager: create an object and clear m_pCodeMgr before Run.
        {
            g_case = "C:5c-nomgr";
            ++g_checks;
            BYTE code[16], din[0x40];
            memset(code, 0, sizeof(code));
            memset(din, 0, sizeof(din));
            code[3] = 4;
            code[6] = 0x5c;
            code[7] = 0;
            code[8] = 0;
            CVM *a = (CVM *)G.GetVM();
            CVM *b = new CVM();
            G.load(a, code, 10, din, 0x40);
            b->Load(code, 10, din, 0x40);
            a->m_pCodeMgr = 0;
            b->m_pCodeMgr = 0;
            BOOL ra = G.run(a);
            BOOL rb = b->Run();
            if (ra != rb) fail("Run %d/%d", ra, rb);
            cmpState(a, b, "no manager", 0);
            G.release(a);
            b->Release();
        }
    }
}

// ---------------------------------------------------------------------------
// random fuzzing over terminating, non-host, non-crypto instructions
// ---------------------------------------------------------------------------
static int safeOpcode(bool upvm, int op)
{
    if (op >= 0x01 && op <= 0x19) return 1;   // no public jump/call opcodes here
    if (op == 0x22 || op == 0x23) return 1;
    if (op >= 0x25 && op <= 0x2a) return 1;   // no div
    if (op >= 0x2d && op <= 0x3b) return 1;
    // 0x44..0x47 consume m_Reg[2] as a byte count for the bulk helpers; a
    // random register value would write outside the arena, so those opcodes
    // are covered by directed cases instead.
    if (!upvm && op >= 0x48 && op <= 0x50) return 1;
    return 0;
}

static void fuzz(bool upvm, int iterations, unsigned seed)
{
    int it;
    char nm[64];
    srand(seed);
    g_case = upvm ? "U:fuzz" : "C:fuzz";
    for (it = 0; it < iterations; ++it) {
        BYTE prog[0x100];
        BYTE data[0x100];
        UINT32 regs[13];
        int i;
        int n = 1 + (rand() % 0x3c);
        int dataLen = rand() % 0x80;
        for (i = 0; i < n; ++i) {
            int op = rand() & 0xff;
            while (!safeOpcode(upvm, op)) op = rand() & 0xff;
            prog[i] = (BYTE)op;
        }
        // Pad so operand reads stay inside the buffer even at the end.
        {
            BYTE p2[0x100];
            memcpy(p2, prog, n);
            for (i = n; i < (int)sizeof(p2); ++i) p2[i] = 0;
            memcpy(prog, p2, sizeof(p2));
        }
        for (i = 0; i < (int)sizeof(data); ++i) data[i] = (BYTE)(rand() & 0xff);
        for (i = 0; i < 13; ++i) regs[i] = (UINT32)rand() ^ ((UINT32)rand() << 16);
        regs[0] = 0;
        regs[2] = (UINT32)dataLen;             // keeps xora/anda/ora/nota in range
        regs[1] = 0x7ff0 + (rand() % 0x30);    // stack window, some invalid
        if (!upvm) {
            regs[5] = rand() % 0x10;
            regs[6] = (UINT32)rand();
        }
        sprintf(nm, "%s:fuzz-%d", upvm ? "U" : "C", it);
        doCase(upvm, nm, prog, n, data, dataLen, regs, 1,
               KEY_LINK | KEY_MATRIX | KEY_TREE, 0, 1);
    }
}

// ---------------------------------------------------------------------------
// exact boundary values for every checked range
// ---------------------------------------------------------------------------
static void boundaryTests(bool upvm)
{
    UINT32 r[13];
    int i;
    const char *u = upvm ? "U" : "C";
    char nm[48];

    initData();
    for (i = 0; i < 13; ++i) r[i] = 0;
    r[5] = 0x11223344;
    r[6] = 0x55667788;

    // word/dword copies: 0xffff sentinel / 0xfffc limit
    { BYTE q[6] = { 0x02, 0xfe, 0xff, 0xfd, 0xff, 0x22 };
      sprintf(nm, "%s:b02-ok", u);
      doCase(upvm, nm, q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x03, 0xfc, 0xff, 0xfb, 0xff, 0x22 };
      sprintf(nm, "%s:b03-ok", u);
      doCase(upvm, nm, q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x03, 0xfd, 0xff, 0xfb, 0xff, 0x22 };
      sprintf(nm, "%s:b03-dst-limit", u);
      doCase(upvm, nm, q, 6, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[6] = { 0x03, 0xfc, 0xff, 0xfd, 0xff, 0x22 };
      sprintf(nm, "%s:b03-src-limit", u);
      doCase(upvm, nm, q, 6, g_data, 0x40, r, 1, 0, 0, 1); }

    // immediate stores/loads at their limits
    { BYTE q[5] = { 0x05, 0xff, 0xff, 0x05, 0x22 };
      sprintf(nm, "%s:b05-limit", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x05, 0xfe, 0xff, 0x05, 0x22 };
      sprintf(nm, "%s:b05-ok", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x06, 0xfc, 0xff, 0x05, 0x22 };
      sprintf(nm, "%s:b06-ok", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x06, 0xfd, 0xff, 0x05, 0x22 };
      sprintf(nm, "%s:b06-limit", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x08, 0x05, 0xff, 0xff, 0x22 };
      sprintf(nm, "%s:b08-limit", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x09, 0x05, 0xfc, 0xff, 0x22 };
      sprintf(nm, "%s:b09-ok", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[5] = { 0x09, 0x05, 0xfd, 0xff, 0x22 };
      sprintf(nm, "%s:b09-limit", u);
      doCase(upvm, nm, q, 5, g_data, 0x40, r, 1, 0, 0, 1); }

    // indirect access through a register address
    { BYTE q[4] = { 0x0b, 0x05, 0x02, 0x22 };
      UINT32 a[13];
      for (i = 0; i < 13; ++i) a[i] = r[i];
      a[2] = 0xffff;
      sprintf(nm, "%s:b0b-ok", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1);
      a[2] = 0x10000;
      sprintf(nm, "%s:b0b-limit", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0c, 0x05, 0x02, 0x22 };
      UINT32 a[13];
      for (i = 0; i < 13; ++i) a[i] = r[i];
      a[2] = 0xfffe;
      sprintf(nm, "%s:b0c-ok", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1);
      a[2] = 0xffff;
      sprintf(nm, "%s:b0c-limit", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0d, 0x05, 0x02, 0x22 };
      UINT32 a[13];
      for (i = 0; i < 13; ++i) a[i] = r[i];
      a[2] = 0xfffc;
      sprintf(nm, "%s:b0d-ok", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1);
      a[2] = 0xfffd;
      sprintf(nm, "%s:b0d-limit", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, a, 1, 0, 0, 1); }

    // register index 0xc is valid, 0xd is not
    { BYTE q[4] = { 0x0a, 0x0c, 0x05, 0x22 };
      sprintf(nm, "%s:reg-c", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, r, 1, 0, 0, 1); }
    { BYTE q[4] = { 0x0a, 0x0d, 0x05, 0x22 };
      sprintf(nm, "%s:reg-d", u);
      doCase(upvm, nm, q, 4, g_data, 0x40, r, 1, 0, 0, 1); }

    // stack pointer boundaries
    { BYTE q[3] = { 0x11, 0x05, 0x22 };
      UINT32 sp[] = { 0x7fff, 0x8000, 0xffff };
      int k;
      for (k = 0; k < 3; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b11-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }
    { BYTE q[3] = { 0x12, 0x05, 0x22 };
      UINT32 sp[] = { 0x7fff, 0x8000, 0xfffe, 0xffff };
      int k;
      for (k = 0; k < 4; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b12-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }
    { BYTE q[3] = { 0x13, 0x05, 0x22 };
      UINT32 sp[] = { 0x7fff, 0x8000, 0xfffc, 0xfffd };
      int k;
      for (k = 0; k < 4; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b13-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }
    { BYTE q[3] = { 0x14, 0x05, 0x22 };
      UINT32 sp[] = { 0x8000, 0x8001, 0xffff };
      int k;
      for (k = 0; k < 3; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b14-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }
    { BYTE q[3] = { 0x15, 0x05, 0x22 };
      UINT32 sp[] = { 0x8001, 0x8002 };
      int k;
      for (k = 0; k < 2; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b15-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }
    { BYTE q[3] = { 0x16, 0x05, 0x22 };
      UINT32 sp[] = { 0x8003, 0x8004 };
      int k;
      for (k = 0; k < 2; ++k) {
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = r[i];
          a[1] = sp[k];
          sprintf(nm, "%s:b16-%x", u, sp[k]);
          doCase(upvm, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1);
      } }

    if (!upvm) {
        // conditional-jump quirk boundary: regA == CL is allowed
        { BYTE q[6] = { 0x1c, 0x02, 0x03, 0x04, 0x22, 0x23 };
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = 0;
          a[2] = 6;      // == CL
          a[3] = 6;
          a[4] = 4;      // fall-through halt
          sprintf(nm, "C:b1c-atCL");
          doCase(0, nm, q, 6, g_data, 0x40, a, 1, 0, 0, 1);
          a[2] = 7;      // > CL
          sprintf(nm, "C:b1c-overCL");
          doCase(0, nm, q, 6, g_data, 0x40, a, 1, 0, 0, 1); }
        // loop target is not bounds-checked
        { BYTE q[3] = { 0x24, 0xff, 0x00 };
          UINT32 a[13];
          for (i = 0; i < 13; ++i) a[i] = 0;
          a[3] = 1;
          sprintf(nm, "C:b24-target");
          doCase(0, nm, q, 3, g_data, 0x40, a, 1, 0, 0, 1); }
        // key-table last valid index
        { BYTE q[3] = { 0x48, 0x03, 0x22 };
          for (i = 0; i < 13; ++i) r[i] = 0;
          sprintf(nm, "C:b48-last");
          doCase(0, nm, q, 3, g_data, 0x40, r, 1, KEY_LINK, 0, 1); }
        { BYTE q[3] = { 0x4a, 0x07, 0x22 };
          sprintf(nm, "C:b4a-last");
          doCase(0, nm, q, 3, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        { BYTE q[3] = { 0x4c, 0x05, 0x22 };
          sprintf(nm, "C:b4c-last");
          doCase(0, nm, q, 3, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
        { BYTE q[3] = { 0x4a, 0x08, 0x22 };
          sprintf(nm, "C:b4a-over");
          doCase(0, nm, q, 3, g_data, 0x40, r, 1, KEY_MATRIX, 0, 1); }
        { BYTE q[3] = { 0x4c, 0x06, 0x22 };
          sprintf(nm, "C:b4c-over");
          doCase(0, nm, q, 3, g_data, 0x40, r, 1, KEY_TREE, 0, 1); }
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1]
        : "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394";

    g_h = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (g_h == 0) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return 2;
    }

    G.GetVM = (CVMBase *(*)())need("_Z5GetVMv");
    G.GetUPVM = (CVMBase *(*)())need("_Z7GetUPVMv");
    G.init = (fn_Init)need("_ZN3CVM4InitEv");
    G.release = (fn_void)need("_ZN3CVM7ReleaseEv");
    G.getDecode = (fn_Code)need("_ZN3CVM13GetDecodeCodeEPhRS0_Rt");
    G.getEncode = (fn_Code)need("_ZN3CVM13GetEncodeCodeEPhRS0_Rt");
    G.getHeader = (fn_Header)need("_ZN3CVM9GetHeaderEPhPtS1_S1_");
    G.setHeader = (fn_Header)need("_ZN3CVM9SetHeaderEPhPtS1_S1_");
    G.getDataBuffer = (fn_DataBuf)need("_ZN3CVM13GetDataBufferERPhRj");
    G.setKeyTree = (fn_Key)need("_ZN3CVM10SetKeyTreeEPvj");
    G.setKeyMatrix = (fn_KeyM)need("_ZN3CVM12SetKeyMatrixEPvjj");
    G.setKeyLink = (fn_Key)need("_ZN3CVM10SetKeyLinkEPvj");
    G.run = (fn_Run)need("_ZN3CVM3RunEv");
    G.load = (fn_Load)need("_ZN3CVM4LoadEPvmS0_m");
    G.getLastError = (fn_Err)need("_ZN3CVM12GetLastErrorEv");
    G.setError = (fn_SetErr)need("_ZN3CVM8SetErrorEi");
    G.upvmInit = (fn_Init)need("_ZN5CUPVM4InitEv");
    G.upvmRelease = (fn_void)need("_ZN5CUPVM7ReleaseEv");
    G.upvmRun = (fn_Run)need("_ZN5CUPVM3RunEv");

    f_rol = (RollFn)need("_Z3rolPhi");
    f_ror = (RollFn)need("_Z3rorPhi");
    f_xora = (XaoFn)need("_Z4xoraPhij");
    f_anda = (XaoFn)need("_Z4andaPhij");
    f_ora = (XaoFn)need("_Z3oraPhij");
    f_nota = (NotaFn)need("_Z4notaPhi");
    f_calldecrypt = (CallDecryptFn)need("_Z11CallDecryptiPhjS_jb");
    f_decodeshort = (DecodeShortFn)need("_Z11DecodeShortPPcPt");
    f_cc_ctor = (CtorFn)need("_ZN10CCryptDataC1Ev");
    f_cc_dtor = (DtorFn)need("_ZN10CCryptDataD1Ev");
    f_cc_enc = (EncFn)need("_ZN10CCryptData11EncryptDataEtPhbS0_j");

    accessorTests();
    simpleTests();
    loadTests();
    vtableTests();
    opcodeTests(false);
    opcodeTests(true);
    boundaryTests(false);
    boundaryTests(true);
    fuzz(false, 3000, 0x1234);
    fuzz(true, 2000, 0x5678);
    fuzz(false, 1500, 0x9abc);
    fuzz(true, 1500, 0xdef0);
    fuzz(false, 1000, 0x2468);
    fuzz(true, 1000, 0x1357);

    printf("%s: %d checks, %d failures\n", g_fail ? "FAILED" : "PASSED",
           g_checks, g_fail);
    dlclose(g_h);
    return g_fail ? 1 : 0;
}
