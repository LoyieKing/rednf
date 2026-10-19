// Reconstructed from libtss_sdk.so.1.0.394
// CU src/TencBase.cpp, class CTsLocalImp (declaration at TencBase.cpp:1256,
// destructor 1269, LocalGetModuleHandle 1273, LocalGetProcAddress 1278,
// ProcessLastBytes 1283, Seattos 1336).
#ifndef TSS_SDK_RECON_SRC_VMCORE_TS_LOCAL_IMP_H_
#define TSS_SDK_RECON_SRC_VMCORE_TS_LOCAL_IMP_H_

#include "src/VMCore/tencrypt_ts_local.h"

// DWARF: class CTsLocalImp, sizeof 12, inherits ITsLocal.
//   +0x04 void *m_pvGetModuleHandle
//   +0x08 void *m_pvGetProcAddress
class CTsLocalImp : public ITsLocal {
public:
    CTsLocalImp();          // ORIG 0x00217f20 (C1) / 0x00218140 (C2)
    virtual ~CTsLocalImp(); // ORIG 0x00218120 (D1), 0x00218170 (D2),
                            //      0x00218190 (D0)

    virtual void ProcessLastBytes(unsigned char *pbyKey, bool bEncrypt,
                                  unsigned int dwBufLen, unsigned int dwKeySize,
                                  unsigned char *pbyBuf);
    virtual void *LocalGetModuleHandle();
    virtual void *LocalGetProcAddress();
    virtual void Seattos(unsigned char *key, bool benc, unsigned char *buf,
                         unsigned int buflen);

private:
    void *m_pvGetModuleHandle;
    void *m_pvGetProcAddress;
};

#endif // TSS_SDK_RECON_SRC_VMCORE_TS_LOCAL_IMP_H_
