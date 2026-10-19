// Static data members of the Tencrypt cipher classes of
// libtss_sdk.so.1.0.394 (tag 394).
//
// The classes themselves are declared exactly once, in the ORIG headers
// reconstructed under tencrypt_new/inc (des2p.h, desbig.h, desmo.h, despc.h,
// destoo.h, fastdes.h, gost.h).  This TU and the cipher TUs must see one
// ORIG-identical class: the earlier local stub classes made CDes2p 1 byte
// instead of ORIG's 8 and conflicted with the real class in des2p.cpp
// (ODR/type-layout deviation).
//
// This header therefore only pulls those declarations in; the definitions (the
// ORIG .rodata/.data bytes) live in crypto_static_data.cpp, where every member
// is size-checked against the ORIG `nm -DCS` symbol size and byte-compared
// against the ORIG image (build/emission/cmp_crypto_bytes.py, 45/45).
#ifndef RECON_SRC_EMISSION_CRYPTO_STATIC_DATA_H_
#define RECON_SRC_EMISSION_CRYPTO_STATIC_DATA_H_

#include "../../../../tencrypt_new/inc/des2p.h"
#include "../../../../tencrypt_new/inc/desbig.h"
#include "../../../../tencrypt_new/inc/desmo.h"
#include "../../../../tencrypt_new/inc/despc.h"
#include "../../../../tencrypt_new/inc/destoo.h"
#include "../../../../tencrypt_new/inc/fastdes.h"
#include "../../../../tencrypt_new/inc/gost.h"

#endif // RECON_SRC_EMISSION_CRYPTO_STATIC_DATA_H_
