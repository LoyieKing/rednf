/* loki97.cpp -- LOKI97 块密码（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）。

   基于 AC 源码集 LOKI97/cref/loki97.c 参考实现，C++ 类化后的差异：
   - S1/S2/P/init_done 由静态全局改为类成员（Init 惰性分配并清零，
     cipherInit 首次调用时计算表并置 init_done=1）；
   - makeKey 的 keyMaterial 为原始字节（byteToULONG64 直接读 16 字节组），
     不是参考实现的 ASCII 十六进制字符串；
   - makeKey 的 deltan 从 DELTA 开始递增（与参考实现一致；二进制插桩确认
     t2 首轮即 +DELTA，WIP 曾误从 0 起步）：
     SK[i] = k4 ^ f(k1+k3+i*DELTA, k2)，i=0..47；
   - cipherInit 的 IV 为原始字节（memcpy 16 字节），blockSize = 0x80；
   - 模式函数 inputLen 仍为位数（%128==0 校验，blocks=inputLen/128），
     返回码与参考一致（TRUE=1，BAD_KEY_DIR=-1 ... BAD_CIPHER_INPUT=-6）；
   - Loki97 包装：cipherInit(ECB, 全 0 IV) -> makeKey(256 位, benc^1)
     -> 每 16 字节块 blockEncrypt/blockDecrypt 原地 -> Seattos 处理余数；
   - f 与参考完全一致（d=KP(A,B)；8 个 S 盒位按 P 表 >>7..>>0 合成 e；
     f.l/f.r 第二层 S 盒 + B.l 高位移位），S1/S2/P 用成员指针。 */

#include "inc/loki97.h"
#include "include/TenCrypt.h"

#include <string.h>

#define DIR_ENCRYPT     0
#define DIR_DECRYPT     1
#define MODE_ECB        1
#define MODE_CBC        2
#define MODE_CFB1       3
#define TRUE            1
#define FALSE           0
#define BAD_KEY_DIR     (-1)
#define BAD_KEY_MAT     (-2)
#define BAD_KEY_INSTANCE (-3)
#define BAD_CIPHER_MODE (-4)
#define BAD_CIPHER_STATE (-5)
#define BAD_CIPHER_INPUT (-6)
#define S1_GEN          0x2911
#define S1_SIZE         0x2000
#define S2_GEN          0xAA7
#define S2_SIZE         0x800
#define ROUNDS          16
#define NUM_SUBKEYS     48
#define BLOCK_SIZE      16
/* 二进制 makeKey 把 keyMaterial 循环拷贝 64 字节（DWARF 数组为 65，
   多出的 1 字节是参考实现 ASCII 密钥串的终止符，二进制实际只拷 64） */
#define MAX_KEY_SIZE    64

// mangled: _ZN7CLoki974multEiiii
int CLoki97::mult(int a, int b, int g, int n) {
    int p = 0;

    while (b != 0) {
        if ((b & 0x01) != 0)
            p ^= a;
        a <<= 1;
        if (a >= n)
            a ^= g;
        b >>= 1;
    }
    return p;
}

// mangled: _ZN7CLoki974exp3Eiii
int CLoki97::exp3(int b, int g, int n) {
    int r = b;

    if (b == 0)
        return 0;
    b = mult(r, b, g, n);
    r = mult(r, b, g, n);
    return r;
}

// mangled: _ZN7CLoki971fE8loki_B64S0_
loki_B64 CLoki97::f(loki_B64 A, loki_B64 B) {
    loki_B64 d, e, f;
    int s;

    /* d = KP(A,B)：按 B.r 的位交换 A.l/A.r 对应位 */
    d.l = ((A.l & ~B.r) | (A.r & B.r));
    d.r = ((A.r & ~B.r) | (A.l & B.r));

    /* e = P(Sa(d))：8 个 12/11 位 S 盒输入，输出经 P 表 >>7..>>0 */
    s = S1[((d.l >> 24) | (d.r << 8)) & 0x1FFF];
    e.l = P[s].l >> 7;  e.r = P[s].r >> 7;
    s = S2[(d.l >> 16) & 0x7FF];
    e.l |= P[s].l >> 6; e.r |= P[s].r >> 6;
    s = S1[(d.l >> 8) & 0x1FFF];
    e.l |= P[s].l >> 5; e.r |= P[s].r >> 5;
    s = S2[d.l & 0x7FF];
    e.l |= P[s].l >> 4; e.r |= P[s].r >> 4;
    s = S2[((d.r >> 24) | (d.l << 8)) & 0x7FF];
    e.l |= P[s].l >> 3; e.r |= P[s].r >> 3;
    s = S1[(d.r >> 16) & 0x1FFF];
    e.l |= P[s].l >> 2; e.r |= P[s].r >> 2;
    s = S2[(d.r >> 8) & 0x7FF];
    e.l |= P[s].l >> 1; e.r |= P[s].r >> 1;
    s = S1[d.r & 0x1FFF];
    e.l |= P[s].l;     e.r |= P[s].r;

    /* f = Sb(e, B)：第二层 S 盒，高位来自 B.l */
    f.l = S2[(((e.l >> 24) & 0xFF) | ((B.l >> 21) & 0x700))] << 24 |
          S2[(((e.l >> 16) & 0xFF) | ((B.l >> 18) & 0x700))] << 16 |
          S1[(((e.l >> 8) & 0xFF) | ((B.l >> 13) & 0x1F00))] << 8 |
          S1[((e.l & 0xFF) | ((B.l >> 8) & 0x1F00))];
    f.r = S2[(((e.r >> 24) & 0xFF) | ((B.l >> 5) & 0x700))] << 24 |
          S2[(((e.r >> 16) & 0xFF) | ((B.l >> 2) & 0x700))] << 16 |
          S1[(((e.r >> 8) & 0xFF) | ((B.l << 3) & 0x1F00))] << 8 |
          S1[((e.r & 0xFF) | ((B.l << 8) & 0x1F00))];

    return f;
}

// mangled: _ZN7CLoki975add64E8loki_B64S0_
loki_B64 CLoki97::add64(loki_B64 a, loki_B64 b) {
    loki_B64 sum;

    sum.r = a.r + b.r;
    sum.l = a.l + b.l;
    if (sum.r < b.r)
        sum.l++;
    return sum;
}

// mangled: _ZN7CLoki975sub64E8loki_B64S0_
loki_B64 CLoki97::sub64(loki_B64 a, loki_B64 b) {
    loki_B64 diff;

    diff.r = a.r - b.r;
    diff.l = a.l - b.l;
    if (diff.r > a.r)
        diff.l--;
    return diff;
}

// mangled: _ZN7CLoki9713byteToULONG64EPh
loki_B64 CLoki97::byteToULONG64(unsigned char *inp) {
    loki_B64 I;

    I.l = (*inp++ << 24);
    I.l |= (*inp++ << 16);
    I.l |= (*inp++ << 8);
    I.l |= *inp++;
    I.r = (*inp++ << 24);
    I.r |= (*inp++ << 16);
    I.r |= (*inp++ << 8);
    I.r |= *inp++;
    return I;
}

// mangled: _ZN7CLoki9713ULONG64ToBYTEEPh8loki_B64
unsigned char *CLoki97::ULONG64ToBYTE(unsigned char *buf, loki_B64 I) {
    unsigned char *sav = buf;

    *buf++ = (unsigned char)(I.l >> 24);
    *buf++ = (unsigned char)(I.l >> 16);
    *buf++ = (unsigned char)(I.l >> 8);
    *buf++ = (unsigned char)I.l;
    *buf++ = (unsigned char)(I.r >> 24);
    *buf++ = (unsigned char)(I.r >> 16);
    *buf++ = (unsigned char)(I.r >> 8);
    *buf++ = (unsigned char)I.r;
    return sav;
}

// mangled: _ZN7CLoki9710cipherInitEPNS_14cipherInstanceEhPh
int CLoki97::cipherInit(cipherInstance *cipher, unsigned char mode,
                        unsigned char *IV) {
    int S1_MASK = S1_SIZE - 1;
    int S2_MASK = S2_SIZE - 1;
    int i, j, k;
    int b;
    long pval;
    unsigned char *input;

    if (!init_done) {
        for (i = 0; i < S1_SIZE; i++) {
            b = i ^ S1_MASK;
            S1[i] = (unsigned char)exp3(b, S1_GEN, S1_SIZE);
        }
        for (i = 0; i < S2_SIZE; i++) {
            b = i ^ S2_MASK;
            S2[i] = (unsigned char)exp3(b, S2_GEN, S2_SIZE);
        }
        for (i = 0; i < 0x100; i++) {
            pval = 0L;
            for (j = 0, k = 7; j < 4; j++, k += 8)
                pval |= (long)((i >> j) & 0x1) << k;
            P[i].r = (unsigned int)pval;
            pval = 0L;
            for (j = 4, k = 7; j < 8; j++, k += 8)
                pval |= (long)((i >> j) & 0x1) << k;
            P[i].l = (unsigned int)pval;
        }
        init_done = 1;
    }

    cipher->mode = mode;
    if (IV != NULL) {
        memcpy(cipher->IV, IV, sizeof(cipher->IV));
        input = cipher->IV;
        cipher->IVL = byteToULONG64(input);
        input += 8;
        cipher->IVR = byteToULONG64(input);
    } else {
        memset(cipher->IV, 0, sizeof(cipher->IV));
        cipher->IVL.l = cipher->IVL.r = cipher->IVR.l = cipher->IVR.r = 0L;
    }
    cipher->blockSize = BLOCK_SIZE * 8;

    if ((mode == MODE_ECB) || (mode == MODE_CBC) || (mode == MODE_CFB1))
        return TRUE;
    else
        return BAD_CIPHER_MODE;
}

// mangled: _ZN7CLoki977makeKeyEPNS_11keyInstanceEhiPh
int CLoki97::makeKey(keyInstance *key, unsigned char direction, int keyLen,
                     unsigned char *keyMaterial) {
    loki_B64 k4, k3, k2, k1;
    /* 二进制插桩确认：deltan 从 DELTA 起（t2 首轮即 +DELTA），每轮再加 DELTA */
    loki_B64 deltan = {0x9E3779B9, 0x7F4A7C15};
    loki_B64 t1, t2;
    loki_B64 f_out;
    int i = 0;

    if ((key == NULL) || (keyMaterial == NULL))
        return BAD_KEY_INSTANCE;
    if (!(direction == DIR_ENCRYPT || direction == DIR_DECRYPT))
        return BAD_KEY_DIR;
    if (!(keyLen == 128 || keyLen == 192 || keyLen == 256))
        return BAD_KEY_MAT;

    key->direction = direction;
    key->keyLen = keyLen;
    memcpy(key->keyMaterial, keyMaterial, MAX_KEY_SIZE);

    k4 = byteToULONG64(keyMaterial);
    k3 = byteToULONG64(keyMaterial + 16);
    if (keyLen == 128) {
        k2 = f(k3, k4);
        k1 = f(k4, k3);
    } else {
        k2 = byteToULONG64(keyMaterial + 32);
        if (keyLen == 192)
            k1 = f(k4, k3);
        else
            k1 = byteToULONG64(keyMaterial + 48);
    }

    for (i = 0; i < NUM_SUBKEYS; i++) {
        t1 = add64(k1, k3);
        t2 = add64(t1, deltan);
        f_out = f(t2, k2);
        key->SK[i].l = k4.l ^ f_out.l;
        key->SK[i].r = k4.r ^ f_out.r;
        k4 = k3;
        k3 = k2;
        k2 = k1;
        k1 = key->SK[i];
        deltan = add64(deltan, DELTA);
    }
    return TRUE;
}

// mangled: _ZN7CLoki9712blockEncryptEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::blockEncrypt(cipherInstance *cipher, keyInstance *key,
                          unsigned char *input, int inputLen,
                          unsigned char *outBuffer) {
    if (!init_done)
        return BAD_CIPHER_STATE;
    if (cipher == NULL)
        return BAD_CIPHER_STATE;
    if (key == NULL)
        return BAD_KEY_INSTANCE;
    if (key->direction != DIR_ENCRYPT)
        return BAD_KEY_DIR;

    if (cipher->mode == MODE_ECB)
        return enECB(cipher, key, input, inputLen, outBuffer);
    else if (cipher->mode == MODE_CBC)
        return enCBC(cipher, key, input, inputLen, outBuffer);
    else if (cipher->mode == MODE_CFB1)
        return enCFB1(cipher, key, input, inputLen, outBuffer);
    else
        return BAD_CIPHER_MODE;
}

// mangled: _ZN7CLoki975enECBEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::enECB(cipherInstance *cipher, keyInstance *key,
                   unsigned char *input, int inputLen,
                   unsigned char *outBuffer) {
    int i, j, k;
    int blocks = inputLen / (BLOCK_SIZE * 8);
    loki_B64 L, R;
    loki_B64 nR, f_out;

    if (inputLen % (BLOCK_SIZE * 8) != 0)
        return BAD_CIPHER_INPUT;

    for (j = 0; j < blocks; j++) {
        L = byteToULONG64(input);
        input += 8;
        R = byteToULONG64(input);
        input += 8;

        k = 0;
        for (i = 0; i < ROUNDS; i++) {
            nR = add64(R, key->SK[k++]);
            f_out = f(nR, key->SK[k++]);
            nR = add64(nR, key->SK[k++]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }

        ULONG64ToBYTE(outBuffer, R);
        outBuffer += 8;
        ULONG64ToBYTE(outBuffer, L);
        outBuffer += 8;
    }
    return TRUE;
}

// mangled: _ZN7CLoki975enCBCEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::enCBC(cipherInstance *cipher, keyInstance *key,
                   unsigned char *input, int inputLen,
                   unsigned char *outBuffer) {
    int i, j, k;
    int blocks = inputLen / (BLOCK_SIZE * 8);
    loki_B64 L, R;
    loki_B64 nR, f_out;

    if (inputLen % (BLOCK_SIZE * 8) != 0)
        return BAD_CIPHER_INPUT;

    for (j = 0; j < blocks; j++) {
        L = byteToULONG64(input);
        input += 8;
        R = byteToULONG64(input);
        input += 8;

        L.l ^= cipher->IVL.l;
        L.r ^= cipher->IVL.r;
        R.l ^= cipher->IVR.l;
        R.r ^= cipher->IVR.r;

        k = 0;
        for (i = 0; i < ROUNDS; i++) {
            nR = add64(R, key->SK[k++]);
            f_out = f(nR, key->SK[k++]);
            nR = add64(nR, key->SK[k++]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }

        cipher->IVL = R;
        cipher->IVR = L;

        ULONG64ToBYTE(outBuffer, R);
        outBuffer += 8;
        ULONG64ToBYTE(outBuffer, L);
        outBuffer += 8;
    }
    return TRUE;
}

// mangled: _ZN7CLoki976enCFB1EPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::enCFB1(cipherInstance *cipher, keyInstance *key,
                    unsigned char *input, int inputLen,
                    unsigned char *outBuffer) {
    int i, j, k;
    int b;
    unsigned char msgbit, keybit;
    loki_B64 L, R;
    loki_B64 nR, f_out;

    L = cipher->IVL;
    R = cipher->IVR;

    b = 7;
    *outBuffer = 0;

    for (j = 0; j < inputLen; j++) {
        msgbit = (unsigned char)((*input >> b) & 01);

        k = 0;
        for (i = 0; i < ROUNDS; i++) {
            nR = add64(R, key->SK[k++]);
            f_out = f(nR, key->SK[k++]);
            nR = add64(nR, key->SK[k++]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }
        L = R;
        R = nR;

        keybit = (unsigned char)(L.l >> 31);
        msgbit ^= keybit;
        *outBuffer |= (unsigned char)(msgbit << b);

        L.l = (L.l << 1) | (L.r >> 31);
        L.r = (L.r << 1) | (R.l >> 31);
        R.l = (R.l << 1) | (R.r >> 31);
        R.r = (R.r << 1) | msgbit;

        b--;
        if (b < 0) {
            b = 7;
            input++;
            outBuffer++;
            *outBuffer = 0;
        }
    }

    cipher->IVL = L;
    cipher->IVR = R;
    return TRUE;
}

// mangled: _ZN7CLoki9712blockDecryptEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::blockDecrypt(cipherInstance *cipher, keyInstance *key,
                          unsigned char *input, int inputLen,
                          unsigned char *outBuffer) {
    if (!init_done)
        return BAD_CIPHER_STATE;
    if (cipher == NULL)
        return BAD_CIPHER_STATE;
    if (key == NULL)
        return BAD_KEY_INSTANCE;
    if (key->direction != DIR_DECRYPT)
        return BAD_KEY_DIR;

    if (cipher->mode == MODE_ECB)
        return deECB(cipher, key, input, inputLen, outBuffer);
    else if (cipher->mode == MODE_CBC)
        return deCBC(cipher, key, input, inputLen, outBuffer);
    else if (cipher->mode == MODE_CFB1)
        return deCFB1(cipher, key, input, inputLen, outBuffer);
    else
        return BAD_CIPHER_MODE;
}

// mangled: _ZN7CLoki975deECBEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::deECB(cipherInstance *cipher, keyInstance *key,
                   unsigned char *input, int inputLen,
                   unsigned char *outBuffer) {
    int i, j, k;
    int blocks = inputLen / (BLOCK_SIZE * 8);
    loki_B64 L, R;
    loki_B64 nR, f_out;

    if (inputLen % (BLOCK_SIZE * 8) != 0)
        return BAD_CIPHER_INPUT;

    for (j = 0; j < blocks; j++) {
        L = byteToULONG64(input);
        input += 8;
        R = byteToULONG64(input);
        input += 8;

        k = NUM_SUBKEYS - 1;
        for (i = 0; i < ROUNDS; i++) {
            nR = sub64(R, key->SK[k--]);
            f_out = f(nR, key->SK[k--]);
            nR = sub64(nR, key->SK[k--]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }

        ULONG64ToBYTE(outBuffer, R);
        outBuffer += 8;
        ULONG64ToBYTE(outBuffer, L);
        outBuffer += 8;
    }
    return TRUE;
}

// mangled: _ZN7CLoki975deCBCEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::deCBC(cipherInstance *cipher, keyInstance *key,
                   unsigned char *input, int inputLen,
                   unsigned char *outBuffer) {
    int i, j, k;
    int blocks = inputLen / (BLOCK_SIZE * 8);
    loki_B64 L, R;
    loki_B64 newIVL, newIVR;
    loki_B64 nR, f_out;

    if (inputLen % (BLOCK_SIZE * 8) != 0)
        return BAD_CIPHER_INPUT;

    for (j = 0; j < blocks; j++) {
        L = byteToULONG64(input);
        input += 8;
        R = byteToULONG64(input);
        input += 8;

        newIVL = L;
        newIVR = R;

        k = NUM_SUBKEYS - 1;
        for (i = 0; i < ROUNDS; i++) {
            nR = sub64(R, key->SK[k--]);
            f_out = f(nR, key->SK[k--]);
            nR = sub64(nR, key->SK[k--]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }

        R.l ^= cipher->IVL.l;
        R.r ^= cipher->IVL.r;
        L.l ^= cipher->IVR.l;
        L.r ^= cipher->IVR.r;

        cipher->IVL = newIVL;
        cipher->IVR = newIVR;

        ULONG64ToBYTE(outBuffer, R);
        outBuffer += 8;
        ULONG64ToBYTE(outBuffer, L);
        outBuffer += 8;
    }
    return TRUE;
}

// mangled: _ZN7CLoki976deCFB1EPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CLoki97::deCFB1(cipherInstance *cipher, keyInstance *key,
                    unsigned char *input, int inputLen,
                    unsigned char *outBuffer) {
    int i, j, k;
    int b;
    unsigned char msgbit, prev, keybit;
    loki_B64 L, R;
    loki_B64 nR, f_out;

    L = cipher->IVL;
    R = cipher->IVR;

    b = 7;
    *outBuffer = 0;

    for (j = 0; j < inputLen; j++) {
        msgbit = (unsigned char)((*input >> b) & 01);
        prev = msgbit;

        k = 0;
        for (i = 0; i < ROUNDS; i++) {
            nR = add64(R, key->SK[k++]);
            f_out = f(nR, key->SK[k++]);
            nR = add64(nR, key->SK[k++]);
            R.l = L.l ^ f_out.l;
            R.r = L.r ^ f_out.r;
            L = nR;
        }
        L = R;
        R = nR;

        keybit = (unsigned char)(L.l >> 31);
        msgbit ^= keybit;
        *outBuffer |= (unsigned char)(msgbit << b);

        L.l = (L.l << 1) | (L.r >> 31);
        L.r = (L.r << 1) | (R.l >> 31);
        R.l = (R.l << 1) | (R.r >> 31);
        R.r = (R.r << 1) | prev;

        b--;
        if (b < 0) {
            b = 7;
            input++;
            outBuffer++;
            *outBuffer = 0;
        }
    }

    cipher->IVL = L;
    cipher->IVR = R;
    return TRUE;
}

// mangled: _ZN7CLoki974InitEPvS0_
void CLoki97::Init(void *pSys, void *pLocal) {
    S1 = 0;
    S2 = 0;
    P = 0;
    m_pSys = pSys;
    m_pLocal = pLocal;
    if (!S1) {
        S1 = new unsigned char[S1_SIZE];
        memset(S1, 0, S1_SIZE);
    }
    if (!S2) {
        S2 = new unsigned char[S2_SIZE];
        memset(S2, 0, S2_SIZE);
    }
    DELTA.l = 0x9E3779B9;
    DELTA.r = 0x7F4A7C15;
    if (!P) {
        P = new loki_B64[0x100];
        memset(P, 0, 0x100 * sizeof(loki_B64));
    }
    init_done = 0;
}

// mangled: _ZN7CLoki976UnInitEv
void CLoki97::UnInit() {
    if (S1) {
        delete[] S1;
        S1 = 0;
    }
    if (S2) {
        delete[] S2;
        S2 = 0;
    }
    if (P) {
        delete[] P;
        P = 0;
    }
}

// mangled: _ZN7CLoki97C1Ev / _ZN7CLoki97C2Ev
CLoki97::CLoki97() {
    Init(0, 0);
}

// mangled: _ZN7CLoki97D1Ev / _ZN7CLoki97D2Ev
CLoki97::~CLoki97() {
    UnInit();
}

// mangled: _ZN7CLoki976Loki97EPhbS0_j
void CLoki97::Loki97(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned char hexIV[16] = {0};
    keyInstance keyIns;
    cipherInstance loki97_cipher;
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen == 0)
        return;
    cipherInit(&loki97_cipher, MODE_ECB, hexIV);
    makeKey(&keyIns, benc ? DIR_ENCRYPT : DIR_DECRYPT, 256, key);
    nLen = buflen / 16;
    for (i = 0; i < nLen; i++) {
        if (benc)
            blockEncrypt(&loki97_cipher, &keyIns, buf + i * 16, 128,
                         buf + i * 16);
        else
            blockDecrypt(&loki97_cipher, &keyIns, buf + i * 16, 128,
                         buf + i * 16);
    }
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
}

// mangled: _Z9TenLoki97PhbS_j
void TenLoki97(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CLoki97 loki97;

    loki97.Loki97(key, benc, buf, buflen);
}
