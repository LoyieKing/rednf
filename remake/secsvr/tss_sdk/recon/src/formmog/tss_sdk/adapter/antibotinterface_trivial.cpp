// Reconstructed from libtss_sdk.so.1.0.394 (adapter/antibotinterface.cpp).
// Six instruction-verified slots: P2PAddClient/P2PDeleteClient/CheckTimer
// return 0, SetLogPath/ShowInfo are empty, ForwardData returns 0.
// The complete class declaration (base + derived, 18 slots) is shared through
// iantibotinterface.h; these definitions override the corresponding slots.

#include "iantibotinterface.h"

// 0x000885a0: xor eax,eax; ret
int AntibotInterface::P2PAddClient(unsigned int, int, unsigned int) {
    return 0;
}

// 0x000885b0: xor eax,eax; ret
int AntibotInterface::P2PDeleteClient(unsigned int) {
    return 0;
}

// 0x000885c0: xor eax,eax; ret
int AntibotInterface::CheckTimer(int, int, unsigned int) {
    return 0;
}

// 0x000885d0: ret
void AntibotInterface::SetLogPath(char *) {
}

// 0x000885e0: ret
void AntibotInterface::ShowInfo() {
}

// 0x000885f0: xor eax,eax; ret
int AntibotInterface::ForwardData(int, int, unsigned int, unsigned int,
                                  unsigned int, const unsigned char *) {
    return 0;
}
