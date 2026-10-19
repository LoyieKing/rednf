// Reconstructed from libtss_sdk.so.1.0.394
// CU src/TenCrypt.cpp — reference-count-free TsLocal singleton.
//
// CreateTsLocal (ORIG 0x001b6890, 0x67 bytes)
//     ITsLocal *g_sTsLocal;                       // .bss, local symbol 0x3267cc
//     if (g_sTsLocal == 0)
//         g_sTsLocal = new CTsLocalImp();         // operator new(12) + C1
//     return g_sTsLocal;
//
// FreeTsLocal (ORIG 0x001b6850, 0x34 bytes)
//     if (g_sTsLocal != 0) {
//         delete g_sTsLocal;                      // vtable slot +0x14 (D0)
//         g_sTsLocal = 0;
//     }
//
// There is no reference counter: the disassembly shows one pointer-sized
// global and nothing else; CreateTsLocal does not increment anything and
// FreeTsLocal unconditionally destroys and clears it.
#include "src/VMCore/ts_local_imp.h"

static ITsLocal *g_sTsLocal;

ITsLocal *CreateTsLocal()
{
    if (g_sTsLocal == 0) {
        g_sTsLocal = new CTsLocalImp();
    }
    return g_sTsLocal;
}

void FreeTsLocal()
{
    if (g_sTsLocal != 0) {
        delete g_sTsLocal;
        g_sTsLocal = 0;
    }
}
