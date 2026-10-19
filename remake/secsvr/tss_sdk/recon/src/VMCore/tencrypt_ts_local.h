// Reconstructed from libtss_sdk.so.1.0.394
// Declarations owned by CU src/TenCrypt.cpp (class ITsLocal/CSeattos) and the
// singleton API at TenCrypt.cpp:1006/1015.
//
// The ORIG emits ITsLocal's destructor as two *weak* copies
// (_ZN8ITsLocalD1Ev @0x001b6890/vtable slot 4, _ZN8ITsLocalD0Ev @0x002181c0,
// vtable slot 5), i.e. the destructor is header-inline in the original build.
// It is kept inline here for the same vague-linkage emission.
//
// NOTE: recon/include/TenCrypt.h declares the same interface with an
// out-of-line destructor; that header is owned by another unit.  No
// translation unit includes both.
#ifndef TSS_SDK_RECON_SRC_VMCORE_TENCRYPT_TS_LOCAL_H_
#define TSS_SDK_RECON_SRC_VMCORE_TENCRYPT_TS_LOCAL_H_

#include "inc/common/Types.h"

// ITsLocal (DWARF: struct, sizeof 4, vtable
// [0] ProcessLastBytes [1] LocalGetModuleHandle [2] LocalGetProcAddress
// [3] Seattos [4] ~ITsLocal D1 [5] ~ITsLocal D0).
struct ITsLocal {
    virtual void ProcessLastBytes(unsigned char *pbyKey, bool bEncrypt,
                                  unsigned int dwBufLen, unsigned int dwKeySize,
                                  unsigned char *pbyBuf) = 0;
    virtual void *LocalGetModuleHandle() = 0;
    virtual void *LocalGetProcAddress() = 0;
    virtual void Seattos(unsigned char *key, bool benc, unsigned char *buf,
                         unsigned int buflen) = 0;
    virtual ~ITsLocal() {}
};

// CSeattos (DWARF: class, sizeof 16; methods are defined in the Seattos CU).
class CSeattos {
public:
    unsigned char *seattosbbit; // +0x00
    unsigned char *seattosIP;   // +0x04
    void *m_pSys;               // +0x08
    void *m_pLocal;             // +0x0c

    CSeattos();
    ~CSeattos();
    void Init(void *pSys, void *pLocal);
    void UnInit();
    void Seattos(unsigned char *pbyData, bool bEncrypt, unsigned char *pbyKey,
                 unsigned int dwLen);
    void SetseattosKey(unsigned char *pbyKey, unsigned char *pbyOut);
};

// Post-processing helpers used by CTsLocalImp::ProcessLastBytes; the seven
// bodies live in the BOX3D CU and are exported by the ORIG.
void BOX3D1(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D2(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D3(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D4(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D5(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D6(unsigned char *, unsigned int, bool, unsigned char *);
void BOX3D7(unsigned char *, unsigned int, bool, unsigned char *);

// ORIG 0x001b6890 / 0x001b6850.
ITsLocal *CreateTsLocal();
void FreeTsLocal();

#endif // TSS_SDK_RECON_SRC_VMCORE_TENCRYPT_TS_LOCAL_H_
