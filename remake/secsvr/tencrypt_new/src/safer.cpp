/* safer.cpp -- SAFER 变体块密码（16 字节块，自定义 PHT/置换，与二进制逐字节核对）
   do_fr/do_ir 已用 uni_call 对二进制 8 组随机输入验证；
   SetKey 由反汇编还原（双/单 expf 查找 + lk 索引回绕）。 */

#include "inc/safer.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned char safer_expf[256] = {
#include "safer_expf.inc"
};

static const unsigned char safer_logf[512] = {
#include "safer_logf.inc"
};

// mangled: _ZN6CSafer5do_frEPhS0_
void CSafer::do_fr(unsigned char *x, unsigned char *kp) {
    unsigned char t;

    x[0] = (unsigned char)(expf[x[0] ^ kp[0]] + kp[16]);
    x[1] = (unsigned char)(kp[17] ^ logf[x[1] + kp[1]]);
    x[2] = (unsigned char)(kp[18] ^ logf[x[2] + kp[2]]);
    x[3] = (unsigned char)(expf[x[3] ^ kp[3]] + kp[19]);
    x[4] = (unsigned char)(expf[x[4] ^ kp[4]] + kp[20]);
    x[5] = (unsigned char)(kp[21] ^ logf[x[5] + kp[5]]);
    x[6] = (unsigned char)(kp[22] ^ logf[x[6] + kp[6]]);
    x[7] = (unsigned char)(expf[x[7] ^ kp[7]] + kp[23]);
    x[8] = (unsigned char)(expf[x[8] ^ kp[8]] + kp[24]);
    x[9] = (unsigned char)(kp[25] ^ logf[x[9] + kp[9]]);
    x[10] = (unsigned char)(kp[26] ^ logf[x[10] + kp[10]]);
    x[11] = (unsigned char)(expf[x[11] ^ kp[11]] + kp[27]);
    x[12] = (unsigned char)(expf[x[12] ^ kp[12]] + kp[28]);
    x[13] = (unsigned char)(kp[29] ^ logf[x[13] + kp[13]]);
    x[14] = (unsigned char)(kp[30] ^ logf[x[14] + kp[14]]);
    x[15] = (unsigned char)(expf[x[15] ^ kp[15]] + kp[31]);

#define PHT(a, b)                                 \
    t = (unsigned char)(x[a] + x[b]);             \
    x[a] = t;                                     \
    x[b] = (unsigned char)(x[b] + t)
    PHT(1, 0); PHT(3, 2); PHT(5, 4); PHT(7, 6);
    PHT(9, 8); PHT(11, 10); PHT(13, 12); PHT(15, 14);
    PHT(7, 0); PHT(1, 2); PHT(3, 4); PHT(5, 6);
    PHT(11, 8); PHT(9, 10); PHT(15, 12); PHT(13, 14);
    PHT(3, 0); PHT(15, 2); PHT(7, 4); PHT(1, 6);
    PHT(5, 8); PHT(13, 10); PHT(11, 12); PHT(9, 14);
    PHT(13, 0); PHT(5, 2); PHT(9, 4); PHT(11, 6);
    PHT(15, 8); PHT(1, 10); PHT(3, 12);
#undef PHT

    /* 特殊段：PHT(14,7) 输出到 (0,1) + 字节置换 */
    t = (unsigned char)(x[7] + x[14]);
    {
        unsigned char o0 = x[0], o1 = x[1], o12 = x[12], o10 = x[10];
        unsigned char o2 = x[2], o8 = x[8], o4 = x[4], o11 = x[11];
        unsigned char o5 = x[5], o13 = x[13], o15 = x[15], o3 = x[3];
        x[0] = (unsigned char)(t + x[14]);
        x[1] = t;
        x[2] = o8;
        x[3] = o15;
        x[4] = o0;
        x[5] = o13;
        x[7] = o11;
        x[8] = o4;
        x[10] = o2;
        x[11] = o5;
        x[12] = o10;
        x[13] = o1;
        x[14] = o12;
        x[15] = o3;
    }
}

// mangled: _ZN6CSafer5do_irEPhS0_
void CSafer::do_ir(unsigned char *x, unsigned char *kp) {
    unsigned char t;
    unsigned char o[16];
    unsigned char *p;
    int i;

    /* 逆字节置换 + 逆特殊 PHT */
    o[0] = x[4]; o[1] = x[13]; o[2] = x[10]; o[3] = x[15];
    o[4] = x[8]; o[5] = x[11]; o[6] = x[6];
    o[7] = (unsigned char)(2 * x[1] - x[0]);
    o[8] = x[2]; o[9] = x[9]; o[10] = x[12]; o[11] = x[7];
    o[12] = x[14]; o[13] = x[5];
    o[14] = (unsigned char)(x[0] - x[1]);
    o[15] = x[3];
    memcpy(x, o, 16);

#define IPHT(a, b)                                  \
    t = (unsigned char)(2 * x[a] - x[b]);           \
    x[b] = (unsigned char)(x[b] - x[a]);            \
    x[a] = t
    IPHT(13, 0); IPHT(5, 2); IPHT(9, 4); IPHT(11, 6);
    IPHT(15, 8); IPHT(1, 10); IPHT(3, 12);
    IPHT(3, 0); IPHT(15, 2); IPHT(7, 4); IPHT(1, 6);
    IPHT(5, 8); IPHT(13, 10); IPHT(11, 12); IPHT(9, 14);
    IPHT(7, 0); IPHT(1, 2); IPHT(3, 4); IPHT(5, 6);
    IPHT(11, 8); IPHT(9, 10); IPHT(15, 12); IPHT(13, 14);
    IPHT(1, 0); IPHT(3, 2); IPHT(5, 4); IPHT(7, 6);
    IPHT(9, 8); IPHT(11, 10); IPHT(13, 12); IPHT(15, 14);
#undef IPHT

    /* 逆字节替换 */
    for (i = 0; i < 16; i++) {
        if (i % 4 == 0 || i % 4 == 3) {
            /* x[i] = expf[old ^ kp[i]] + kp[i+16] → old = expf_inv[x[i]-kp[i+16]] ^ kp[i] */
            int v = 0;
            while (expf[v] != (unsigned char)(x[i] - kp[i + 16]))
                v++;
            x[i] = (unsigned char)(v ^ kp[i]);
        } else {
            /* x[i] = kp[i+16] ^ logf[old + kp[i]] → old = logf_inv[x[i]^kp[i+16]] - kp[i] */
            int v = 0;
            while (logf[v] != (unsigned char)(x[i] ^ kp[i + 16]))
                v++;
            x[i] = (unsigned char)(v - kp[i]);
        }
    }
}

// mangled: _ZN6CSafer6SetKeyEPhj
void CSafer::SetKey(unsigned char *lk, unsigned int key_len) {
    unsigned char by;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int l;
    unsigned int m;

    k_bytes = key_len / 8;
    lk[k_bytes] = 0;
    for (i = 0; i < k_bytes; i++) {
        lk[k_bytes] ^= lk[i];
        l_key[i] = lk[i];
    }
    i = 0;
    j = 0;
    k = 0;
    while (i < k_bytes) {
        for (l = 0; l <= k_bytes; l++)
            lk[l] = (unsigned char)((lk[l] << 3) | (lk[l] >> 5));
        m = (i + 1) % (k_bytes + 1);
        if (m > k_bytes)
            m = 0;
        by = 0;
        for (l = 0; l < 16; l++) {
            if (i <= 15)
                by = (unsigned char)(expf[expf[(j + 0x23 + l) & 255]] + lk[m]);
            else
                by = (unsigned char)(expf[(j + 0x23 + l) & 255] + lk[m]);
            l_key[k + 0x10 + l] = by;
            m++;
            if (m > k_bytes)
                m = 0;
        }
        i++;
        k += 0x10;
        j += 0x11;
    }
}

// mangled: _ZN6CSafer7encryptEPh
void CSafer::encrypt(unsigned char *blk) {
    unsigned char *kp;
    unsigned int i;

    do_fr(blk, l_key + 0x00);
    do_fr(blk, l_key + 0x20);
    do_fr(blk, l_key + 0x40);
    do_fr(blk, l_key + 0x60);
    do_fr(blk, l_key + 0x80);
    do_fr(blk, l_key + 0xa0);
    do_fr(blk, l_key + 0xc0);
    do_fr(blk, l_key + 0xe0);
    if (k_bytes > 16) {
        do_fr(blk, l_key + 0x100);
        do_fr(blk, l_key + 0x120);
        do_fr(blk, l_key + 0x140);
        do_fr(blk, l_key + 0x160);
        if (k_bytes > 24) {
            do_fr(blk, l_key + 0x180);
            do_fr(blk, l_key + 0x1a0);
            do_fr(blk, l_key + 0x1c0);
            do_fr(blk, l_key + 0x1e0);
        }
    }
    kp = l_key + k_bytes * 16;
    for (i = 0; i < 16; i++) {
        if (i % 4 == 0 || i % 4 == 3)
            blk[i] ^= kp[i];
        else
            blk[i] = (unsigned char)(blk[i] + kp[i]);
    }
}

// mangled: _ZN6CSafer7decryptEPh
void CSafer::decrypt(unsigned char *blk) {
    unsigned char *kp;
    unsigned int i;

    kp = l_key + k_bytes * 16;
    for (i = 0; i < 16; i++) {
        if (i % 4 == 0 || i % 4 == 3)
            blk[i] ^= kp[i];
        else
            blk[i] = (unsigned char)(blk[i] - kp[i]);
    }
    do_ir(blk, l_key + 0xe0);
    do_ir(blk, l_key + 0xc0);
    do_ir(blk, l_key + 0xa0);
    do_ir(blk, l_key + 0x80);
    do_ir(blk, l_key + 0x60);
    do_ir(blk, l_key + 0x40);
    do_ir(blk, l_key + 0x20);
    do_ir(blk, l_key + 0x00);
    if (k_bytes > 16) {
        do_ir(blk, l_key + 0x160);
        do_ir(blk, l_key + 0x140);
        do_ir(blk, l_key + 0x120);
        do_ir(blk, l_key + 0x100);
        if (k_bytes > 24) {
            do_ir(blk, l_key + 0x1e0);
            do_ir(blk, l_key + 0x1c0);
            do_ir(blk, l_key + 0x1a0);
            do_ir(blk, l_key + 0x180);
        }
    }
}

// mangled: _ZN6CSafer5SaferEPhbS0_j
void CSafer::Safer(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        SetKey(key, 0x80);
        nLen = buflen / 16;
        for (i = 0; i < nLen; i++) {
            if (benc)
                encrypt(buf + i * 16);
            else
                decrypt(buf + i * 16);
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
    }
}

// mangled: _ZN6CSafer4InitEPvS0_
void CSafer::Init(void *pSys, void *pLocal) {
    unsigned char _logf[512];
    unsigned char _expf[256];

    m_pSys = pSys;
    m_pLocal = pLocal;
    l_key = 0;
    logf = 0;
    expf = 0;
    k_bytes = 0;
    if (l_key == 0)
        l_key = new unsigned char[0x210];
    if (logf == 0)
        logf = new unsigned char[0x200];
    if (expf == 0)
        expf = new unsigned char[0x100];
    memcpy(_logf, safer_logf, 0x200);
    memmove(logf, _logf, 0x200);
    memcpy(_expf, safer_expf, 0x100);
    memmove(expf, _expf, 0x100);
}

// mangled: _ZN6CSafer6UnInitEv
void CSafer::UnInit() {
    if (l_key != 0) {
        delete[] l_key;
        l_key = 0;
    }
    if (logf != 0) {
        delete[] logf;
        logf = 0;
    }
    if (expf != 0) {
        delete[] expf;
        expf = 0;
    }
}

// mangled: _ZN6CSaferC1Ev / _ZN6CSaferC2Ev
CSafer::CSafer() {
    Init(0, 0);
}

// mangled: _ZN6CSaferD1Ev / _ZN6CSaferD2Ev
CSafer::~CSafer() {
    UnInit();
}

// mangled: _Z8TenSaferPhbS_j
void TenSafer(unsigned char *key, bool benc, unsigned char *buf,
              unsigned int buflen) {
    CSafer safer;

    safer.Safer(key, benc, buf, buflen);
}
