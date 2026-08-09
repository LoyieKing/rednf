/* ts_local.cpp -- TsLocal 共享层单例（由二进制反汇编还原）
   CTsLocalImp 继承 ITsLocal（vtable 布局 [0]ProcessLastBytes
   [1]LocalGetModuleHandle [2]LocalGetProcAddress [3]Seattos [4]D1 [5]D0），
   对象 = vptr + m_pModuleHandle(+4) + m_pProcAddress(+8)，sizeof 12。
   CreateTsLocal：g_sTsLocal 单例惰性 new；FreeTsLocal：vtable[5] 删除并置空。
   ProcessLastBytes：rem = buflen & 7，按 rem 分发 BOX3D1..7(key, buflen, benc,
   buf+processed)；rem==0 直接返回。 */

#include "include/TenCrypt.h"
#include "inc/seattos.h"

#include <string.h>

extern void BOX3D1(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D2(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D3(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D4(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D5(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D6(unsigned char *, unsigned int, bool, unsigned char *);
extern void BOX3D7(unsigned char *, unsigned int, bool, unsigned char *);

class CTsLocalImp : public ITsLocal {
public:
    void *m_pModuleHandle;
    void *m_pProcAddress;

    CTsLocalImp();
    virtual ~CTsLocalImp();
    void ProcessLastBytes(unsigned char *key, bool benc, unsigned int buflen,
                          unsigned int block, unsigned char *buf);
    void *LocalGetModuleHandle();
    void *LocalGetProcAddress();
    void Seattos(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen);
};

// mangled: _ZN8ITsLocalD1Ev / _ZN8ITsLocalD0Ev
ITsLocal::~ITsLocal() {
}

static CTsLocalImp *g_sTsLocal = 0;

// mangled: _ZN11CTsLocalImpC1Ev / _ZN11CTsLocalImpC2Ev
CTsLocalImp::CTsLocalImp() : m_pModuleHandle(0), m_pProcAddress(0) {
}

// mangled: _ZN11CTsLocalImpD1Ev / _ZN11CTsLocalImpD2Ev / D0
CTsLocalImp::~CTsLocalImp() {
}

// mangled: _ZN11CTsLocalImp20LocalGetModuleHandleEv
void *CTsLocalImp::LocalGetModuleHandle() {
    return m_pModuleHandle;
}

// mangled: _ZN11CTsLocalImp19LocalGetProcAddressEv
void *CTsLocalImp::LocalGetProcAddress() {
    return m_pProcAddress;
}

// mangled: _ZN11CTsLocalImp7SeattosEPhbS0_j
void CTsLocalImp::Seattos(unsigned char *key, bool benc, unsigned char *buf,
                          unsigned int buflen) {
    CSeattos seattos;

    seattos.Seattos(key, benc, buf, buflen);
}

// mangled: _ZN11CTsLocalImp16ProcessLastBytesEPhbjjS0_
void CTsLocalImp::ProcessLastBytes(unsigned char *key, bool benc,
                                   unsigned int buflen, unsigned int block,
                                   unsigned char *buf) {
    unsigned int rem = buflen & 7;
    unsigned char *p = buf + (buflen & ~7U);

    switch (rem) {
    case 1: BOX3D1(key, block, benc, p); break;
    case 2: BOX3D2(key, block, benc, p); break;
    case 3: BOX3D3(key, block, benc, p); break;
    case 4: BOX3D4(key, block, benc, p); break;
    case 5: BOX3D5(key, block, benc, p); break;
    case 6: BOX3D6(key, block, benc, p); break;
    case 7: BOX3D7(key, block, benc, p); break;
    }
}

// mangled: _Z13CreateTsLocalv
ITsLocal *CreateTsLocal() {
    if (!g_sTsLocal) {
        g_sTsLocal = new CTsLocalImp;
    }
    return g_sTsLocal;
}

// mangled: _Z11FreeTsLocalv
void FreeTsLocal() {
    if (g_sTsLocal) {
        delete g_sTsLocal;
        g_sTsLocal = 0;
    }
}
