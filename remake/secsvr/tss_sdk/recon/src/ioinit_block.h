// Per-TU `std::ios_base::Init __ioinit` object for TUs whose ORIG static-init
// block contains only the iostream initializer (e.g. zenlib and protocol
// translation units; they have no COMM_STAT_ITEM_WITH_NAME table).
//
// GCC 4.1's <iostream> declared `static ios_base::Init __ioinit;` in every TU
// that included it; GCC 13 initializes the iostream globals inside libstdc++
// with an init-priority initializer and no longer emits the TU-local object.
// Reproduce the ORIG TU-local static (same name/type/size 1, dynamic init +
// __cxa_atexit) explicitly. Include from a .cpp only: internal linkage.
#ifndef TSS_SDK_RECON_IOINIT_BLOCK_H_
#define TSS_SDK_RECON_IOINIT_BLOCK_H_

#include <ios>

static std::ios_base::Init __ioinit;

#endif  // TSS_SDK_RECON_IOINIT_BLOCK_H_
