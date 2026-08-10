/* cast.cpp -- CAST-128 块密码（Steve Reid 公版，类封装）
   S 盒表从二进制 .rodata 0x081b8c20 提取（8×256 dword，标准 CAST S1-S8）。 */

#include "inc/cast.h"
#include "include/TenCrypt.h"

typedef unsigned char u8;
typedef unsigned int u32;

#define U8a(x) ((u8)((x) >> 24))
#define U8b(x) ((u8)(((x) >> 16) & 255))
#define U8c(x) ((u8)(((x) >> 8) & 255))
#define U8d(x) ((u8)((x) & 255))

#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

const unsigned int CCast::FUCKING_CAST_SBOX[8][256] = {
#include "cast_sbox.inc"
};

#define S1 CCast::FUCKING_CAST_SBOX[0]
#define S2 CCast::FUCKING_CAST_SBOX[1]
#define S3 CCast::FUCKING_CAST_SBOX[2]
#define S4 CCast::FUCKING_CAST_SBOX[3]
#define S5 CCast::FUCKING_CAST_SBOX[4]
#define S6 CCast::FUCKING_CAST_SBOX[5]
#define S7 CCast::FUCKING_CAST_SBOX[6]
#define S8 CCast::FUCKING_CAST_SBOX[7]

#define F1(l, r, i)                                                            \
    t = ROL(key->xkey[i] + r, key->xkey[i + 16]);                              \
    l ^= ((S1[U8a(t)] ^ S2[U8b(t)]) - S3[U8c(t)]) + S4[U8d(t)]
#define F2(l, r, i)                                                            \
    t = ROL(key->xkey[i] ^ r, key->xkey[i + 16]);                              \
    l ^= ((S1[U8a(t)] - S2[U8b(t)]) + S3[U8c(t)]) ^ S4[U8d(t)]
#define F3(l, r, i)                                                            \
    t = ROL(key->xkey[i] - r, key->xkey[i + 16]);                              \
    l ^= ((S1[U8a(t)] + S2[U8b(t)]) ^ S3[U8c(t)]) - S4[U8d(t)]

// mangled: _ZN5CCast12cast_encryptEP8cast_keyPhS1_
void CCast::cast_encrypt(cast_key *key, unsigned char *inblock, unsigned char *outblock) {
    u32 t, l, r;

    l = ((u32)inblock[0] << 24) | ((u32)inblock[1] << 16) |
        ((u32)inblock[2] << 8) | (u32)inblock[3];
    r = ((u32)inblock[4] << 24) | ((u32)inblock[5] << 16) |
        ((u32)inblock[6] << 8) | (u32)inblock[7];

    F1(l, r, 0);
    F2(r, l, 1);
    F3(l, r, 2);
    F1(r, l, 3);
    F2(l, r, 4);
    F3(r, l, 5);
    F1(l, r, 6);
    F2(r, l, 7);
    F3(l, r, 8);
    F1(r, l, 9);
    F2(l, r, 10);
    F3(r, l, 11);
    if (key->rounds > 12) {
        F1(l, r, 12);
        F2(r, l, 13);
        F3(l, r, 14);
        F1(r, l, 15);
    }
    outblock[0] = (u8)(r >> 24);
    outblock[1] = (u8)(r >> 16);
    outblock[2] = (u8)(r >> 8);
    outblock[3] = (u8)r;
    outblock[4] = (u8)(l >> 24);
    outblock[5] = (u8)(l >> 16);
    outblock[6] = (u8)(l >> 8);
    outblock[7] = (u8)l;
}

// mangled: _ZN5CCast12cast_decryptEP8cast_keyPhS1_
void CCast::cast_decrypt(cast_key *key, unsigned char *inblock, unsigned char *outblock) {
    u32 t, l, r;

    r = ((u32)inblock[0] << 24) | ((u32)inblock[1] << 16) |
        ((u32)inblock[2] << 8) | (u32)inblock[3];
    l = ((u32)inblock[4] << 24) | ((u32)inblock[5] << 16) |
        ((u32)inblock[6] << 8) | (u32)inblock[7];

    if (key->rounds > 12) {
        F1(r, l, 15);
        F3(l, r, 14);
        F2(r, l, 13);
        F1(l, r, 12);
    }
    F3(r, l, 11);
    F2(l, r, 10);
    F1(r, l, 9);
    F3(l, r, 8);
    F2(r, l, 7);
    F1(l, r, 6);
    F3(r, l, 5);
    F2(l, r, 4);
    F1(r, l, 3);
    F3(l, r, 2);
    F2(r, l, 1);
    F1(l, r, 0);

    outblock[0] = (u8)(l >> 24);
    outblock[1] = (u8)(l >> 16);
    outblock[2] = (u8)(l >> 8);
    outblock[3] = (u8)l;
    outblock[4] = (u8)(r >> 24);
    outblock[5] = (u8)(r >> 16);
    outblock[6] = (u8)(r >> 8);
    outblock[7] = (u8)r;
}

// mangled: _ZN5CCast11cast_setkeyEP8cast_keyPhi
void CCast::cast_setkey(cast_key *key, unsigned char *rawkey, int keybytes) {
    u32 t[4], z[4], x[4];
    int i;

    key->rounds = (keybytes <= 10 ? 12 : 16);
    for (i = 0; i < 4; i++) {
        x[i] = 0;
        if (i * 4 + 0 < keybytes)
            x[i] |= (u32)rawkey[i * 4] << 24;
        if (i * 4 + 1 < keybytes)
            x[i] |= (u32)rawkey[i * 4 + 1] << 16;
        if (i * 4 + 2 < keybytes)
            x[i] |= (u32)rawkey[i * 4 + 2] << 8;
        if (i * 4 + 3 < keybytes)
            x[i] |= (u32)rawkey[i * 4 + 3];
    }
    for (i = 0; i < 32; i += 4) {
        switch (i & 4) {
        case 0:
            t[0] = z[0] = x[0] ^ S5[U8b(x[3])] ^ S6[U8d(x[3])] ^ S7[U8a(x[3])] ^
                   S8[U8c(x[3])] ^ S7[U8a(x[2])];
            t[1] = z[1] = x[2] ^ S5[U8a(z[0])] ^ S6[U8c(z[0])] ^ S7[U8b(z[0])] ^
                   S8[U8d(z[0])] ^ S8[U8c(x[2])];
            t[2] = z[2] = x[3] ^ S5[U8d(z[1])] ^ S6[U8c(z[1])] ^ S7[U8b(z[1])] ^
                   S8[U8a(z[1])] ^ S5[U8b(x[2])];
            t[3] = z[3] = x[1] ^ S5[U8c(z[2])] ^ S6[U8b(z[2])] ^ S7[U8d(z[2])] ^
                   S8[U8a(z[2])] ^ S6[U8d(x[2])];
            break;
        case 4:
            t[0] = x[0] = z[2] ^ S5[U8b(z[1])] ^ S6[U8d(z[1])] ^ S7[U8a(z[1])] ^
                   S8[U8c(z[1])] ^ S7[U8a(z[0])];
            t[1] = x[1] = z[0] ^ S5[U8a(x[0])] ^ S6[U8c(x[0])] ^ S7[U8b(x[0])] ^
                   S8[U8d(x[0])] ^ S8[U8c(z[0])];
            t[2] = x[2] = z[1] ^ S5[U8d(x[1])] ^ S6[U8c(x[1])] ^ S7[U8b(x[1])] ^
                   S8[U8a(x[1])] ^ S5[U8b(z[0])];
            t[3] = x[3] = z[3] ^ S5[U8c(x[2])] ^ S6[U8b(x[2])] ^ S7[U8d(x[2])] ^
                   S8[U8a(x[2])] ^ S6[U8d(z[0])];
            break;
        }
        switch (i & 12) {
        case 0:
        case 12:
            key->xkey[i + 0] = S5[U8a(t[2])] ^ S6[U8b(t[2])] ^ S7[U8d(t[1])] ^ S8[U8c(t[1])];
            key->xkey[i + 1] = S5[U8c(t[2])] ^ S6[U8d(t[2])] ^ S7[U8b(t[1])] ^ S8[U8a(t[1])];
            key->xkey[i + 2] = S5[U8a(t[3])] ^ S6[U8b(t[3])] ^ S7[U8d(t[0])] ^ S8[U8c(t[0])];
            key->xkey[i + 3] = S5[U8c(t[3])] ^ S6[U8d(t[3])] ^ S7[U8b(t[0])] ^ S8[U8a(t[0])];
            break;
        case 4:
        case 8:
            key->xkey[i + 0] = S5[U8d(t[0])] ^ S6[U8c(t[0])] ^ S7[U8a(t[3])] ^ S8[U8b(t[3])];
            key->xkey[i + 1] = S5[U8b(t[0])] ^ S6[U8a(t[0])] ^ S7[U8c(t[3])] ^ S8[U8d(t[3])];
            key->xkey[i + 2] = S5[U8d(t[1])] ^ S6[U8c(t[1])] ^ S7[U8a(t[2])] ^ S8[U8b(t[2])];
            key->xkey[i + 3] = S5[U8b(t[1])] ^ S6[U8a(t[1])] ^ S7[U8c(t[2])] ^ S8[U8d(t[2])];
            break;
        }
        switch (i & 12) {
        case 0:
            key->xkey[i + 0] ^= S5[U8c(z[0])];
            key->xkey[i + 1] ^= S6[U8c(z[1])];
            key->xkey[i + 2] ^= S7[U8b(z[2])];
            key->xkey[i + 3] ^= S8[U8a(z[3])];
            break;
        case 4:
            key->xkey[i + 0] ^= S5[U8a(x[2])];
            key->xkey[i + 1] ^= S6[U8b(x[3])];
            key->xkey[i + 2] ^= S7[U8d(x[0])];
            key->xkey[i + 3] ^= S8[U8d(x[1])];
            break;
        case 8:
            key->xkey[i + 0] ^= S5[U8b(z[2])];
            key->xkey[i + 1] ^= S6[U8a(z[3])];
            key->xkey[i + 2] ^= S7[U8c(z[0])];
            key->xkey[i + 3] ^= S8[U8c(z[1])];
            break;
        case 12:
            key->xkey[i + 0] ^= S5[U8d(x[0])];
            key->xkey[i + 1] ^= S6[U8d(x[1])];
            key->xkey[i + 2] ^= S7[U8a(x[2])];
            key->xkey[i + 3] ^= S8[U8b(x[3])];
            break;
        }
        if (i >= 16) {
            key->xkey[i + 0] &= 31;
            key->xkey[i + 1] &= 31;
            key->xkey[i + 2] &= 31;
            key->xkey[i + 3] &= 31;
        }
    }
}

// mangled: _ZN5CCast4CastEPhbS0_j
void CCast::Cast(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    cast_key stkey;
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        cast_setkey(&stkey, key, 16);
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++) {
            if (benc)
                cast_encrypt(&stkey, buf + i * 8, buf + i * 8);
            else
                cast_decrypt(&stkey, buf + i * 8, buf + i * 8);
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 8, buflen - nLen * 8);
    }
}

// mangled: _ZN5CCast4InitEPvS0_
void CCast::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN5CCast6UnInitEv
void CCast::UnInit() {
}

// mangled: _ZN5CCastC1Ev / _ZN5CCastC2Ev
CCast::CCast() {
    Init(0, 0);
}

// mangled: _ZN5CCastD1Ev / _ZN5CCastD2Ev
CCast::~CCast() {
    UnInit();
}

// mangled: _Z7TenCastPhbS_j
void TenCast(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    CCast cast;

    cast.Cast(key, benc, buf, buflen);
}
