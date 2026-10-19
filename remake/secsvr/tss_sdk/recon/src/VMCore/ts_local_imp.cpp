// Reconstructed from libtss_sdk.so.1.0.394
// CU src/TencBase.cpp — CTsLocalImp method bodies.
//
// ORIG addresses:
//   0x00217f20 C1 / 0x00218140 C2   ctor (vptr + two zeroed pointers)
//   0x00218120 D1 / 0x00218170 D2 / 0x00218190 D0
//   0x00217f50 LocalGetModuleHandle   (return m_pvGetModuleHandle)
//   0x00217f60 LocalGetProcAddress    (return m_pvGetProcAddress)
//   0x00217f70 ProcessLastBytes
//   0x002180a0 Seattos
#include "src/VMCore/ts_local_imp.h"

CTsLocalImp::CTsLocalImp()
{
    m_pvGetModuleHandle = 0;
    m_pvGetProcAddress = 0;
}

CTsLocalImp::~CTsLocalImp()
{
}

void *CTsLocalImp::LocalGetModuleHandle()
{
    return m_pvGetModuleHandle;
}

void *CTsLocalImp::LocalGetProcAddress()
{
    return m_pvGetProcAddress;
}

// The low three bits of dwBufLen select one of the seven BOX3D post-processing
// variants; the remaining bits are a byte offset into pbyBuf.  Variant 0 is a
// no-op (the original's jump-table slot 0 returns immediately).
void CTsLocalImp::ProcessLastBytes(unsigned char *pbyKey, bool bEncrypt,
                                   unsigned int dwBufLen,
                                   unsigned int dwKeySize,
                                   unsigned char *pbyBuf)
{
    unsigned int dwBase = dwBufLen & ~7u;

    switch (dwBufLen - dwBase) {
    case 1:
        BOX3D1(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 2:
        BOX3D2(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 3:
        BOX3D3(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 4:
        BOX3D4(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 5:
        BOX3D5(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 6:
        BOX3D6(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    case 7:
        BOX3D7(pbyKey, dwKeySize, bEncrypt, pbyBuf + dwBase);
        break;
    default:
        break;
    }
}

// A stack CSeattos is constructed for the duration of the call; its
// constructor/destructor and Seattos body live in the Seattos CU.
void CTsLocalImp::Seattos(unsigned char *key, bool benc, unsigned char *buf,
                          unsigned int buflen)
{
    CSeattos seattos;
    seattos.Seattos(key, benc, buf, buflen);
}
