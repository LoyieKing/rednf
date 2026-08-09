/* idea.cpp -- IDEA 块密码（Xuejia Lai / Massey，8 轮 + 输出变换）
   轮结构/密钥扩展由反汇编逐寄存器核对（标准 IDEA，输出 X2/X3 位置
   与反汇编一致）。mul/inv/en_key_idea/de_key_idea 为经典实现。 */

#include "inc/idea.h"
#include "include/TenCrypt.h"

// 乘法 mod (2^16+1)；a==0 或 b==0 时特殊处理
#define MUL(x, y) \
    do { \
        tmp_p = (unsigned int)(x) * (unsigned int)(y); \
        if (tmp_p) { \
            tmp_a = (unsigned short)(tmp_p & 0xffff); \
            tmp_b = (unsigned short)(tmp_p >> 16); \
            (x) = (unsigned short)(tmp_a - tmp_b + (tmp_a < tmp_b)); \
        } else { \
            (x) = (unsigned short)(1 - (x) - (y)); \
        } \
    } while (0)

// mangled: _ZN5CIdea3invEt
unsigned short CIdea::inv(unsigned short x) {
    unsigned short t0;
    unsigned short t1;
    unsigned short q;
    unsigned short y;

    if (x <= 1)
        return x;                    /* 0 和 1 自逆 */
    t1 = (unsigned short)(0x10001u / x);
    y = (unsigned short)(0x10001u % x);
    if (y == 1)
        return (unsigned short)(1 - t1);
    t0 = 1;
    do {
        q = (unsigned short)(x / y);
        x = (unsigned short)(x % y);
        t0 = (unsigned short)(t0 + q * t1);
        if (x == 1)
            return t0;
        q = (unsigned short)(y / x);
        y = (unsigned short)(y % x);
        t1 = (unsigned short)(t1 + q * t0);
    } while (y != 1);
    return (unsigned short)(1 - t1);
}

// mangled: _ZN5CIdea11en_key_ideaEPtS0_
void CIdea::en_key_idea(unsigned short *userkey, unsigned short *Z) {
    int i;
    int j;

    for (j = 0; j < 8; j++)
        Z[j] = *userkey++;

    /* 标准 IDEA 密钥扩展（等价 SCRT-FIN 的 Z += i & 8 指针技巧） */
    for (i = 8; i < 52; i++) {
        j = i % 8;
        if (j < 6)
            Z[i] = (unsigned short)((Z[i - 7] << 9) | (Z[i - 6] >> 7));
        else if (j == 6)
            Z[i] = (unsigned short)((Z[i - 7] << 9) | (Z[i - 14] >> 7));
        else
            Z[i] = (unsigned short)((Z[i - 15] << 9) | (Z[i - 14] >> 7));
    }
}

// mangled: _ZN5CIdea11de_key_ideaEPtS0_
void CIdea::de_key_idea(unsigned short *Z, unsigned short *DK) {
    int j;
    unsigned short t1;
    unsigned short t2;
    unsigned short t3;
    unsigned short T[52];
    unsigned short *p;

    p = T + 52;
    t1 = inv(*Z++);
    t2 = (unsigned short)(-*Z++);
    t3 = (unsigned short)(-*Z++);
    *--p = inv(*Z++);
    *--p = t3;
    *--p = t2;
    *--p = t1;

    for (j = 1; j < 8; j++) {
        t1 = *Z++;
        *--p = *Z++;
        *--p = t1;

        t1 = inv(*Z++);
        t2 = (unsigned short)(-*Z++);
        t3 = (unsigned short)(-*Z++);
        *--p = inv(*Z++);
        *--p = t2;
        *--p = t3;
        *--p = t1;
    }
    t1 = *Z++;
    *--p = *Z++;
    *--p = t1;

    t1 = inv(*Z++);
    t2 = (unsigned short)(-*Z++);
    t3 = (unsigned short)(-*Z++);
    *--p = inv(*Z++);
    *--p = t3;
    *--p = t2;
    *--p = t1;

    for (j = 0; j < 52; j++)
        DK[j] = T[j];
}

// mangled: _ZN5CIdea3mulEtt
unsigned short CIdea::mul(unsigned short a, unsigned short b) {
    unsigned int p;

    if (a) {
        if (b) {
            p = (unsigned int)a * (unsigned int)b;
            b = (unsigned short)(p & 0xffff);
            a = (unsigned short)(p >> 16);
            return (unsigned short)(b - a + (b < a));
        } else {
            return (unsigned short)(1 - a);
        }
    } else {
        return (unsigned short)(1 - b);
    }
}

// mangled: _ZN5CIdea11cipher_ideaEPtS0_S0_
void CIdea::cipher_idea(unsigned short *in, unsigned short *out, unsigned short *Z) {
    unsigned short x1, x2, x3, x4, t1, t2;
    int r;
    unsigned short tmp_a;
    unsigned short tmp_b;
    unsigned int tmp_p;

    x1 = in[0];
    x2 = in[1];
    x3 = in[2];
    x4 = in[3];

    for (r = 0; r < 8; r++) {
        MUL(x1, Z[r * 6]);
        x2 += Z[r * 6 + 1];
        x3 += Z[r * 6 + 2];
        MUL(x4, Z[r * 6 + 3]);
        t1 = x3 ^ x1;
        MUL(t1, Z[r * 6 + 4]);
        t2 = x2 ^ x4;
        t2 += t1;
        MUL(t2, Z[r * 6 + 5]);
        t1 += t2;
        x1 ^= t2;
        x4 ^= t1;
        /* 轮末：x2' = x3 ^ t2；x3' = x2 ^ t1；然后交换 x2/x3 */
        t2 ^= x3;
        t1 ^= x2;
        x3 = t1;
        x2 = t2;
    }

    MUL(x1, Z[48]);
    x3 += Z[49];
    x2 += Z[50];
    MUL(x4, Z[51]);
    out[0] = x1;
    out[1] = x3;
    out[2] = x2;
    out[3] = x4;
}

// mangled: _ZN5CIdea11cipher_fileEPhjPt
unsigned int CIdea::cipher_file(unsigned char *in, unsigned int len, unsigned short *key) {
    unsigned short input[4];
    unsigned short output[4];
    unsigned short Z[52];
    unsigned char *pch;
    unsigned int nlen;
    unsigned int x;

    en_key_idea(key, Z);
    nlen = len / 8;
    pch = in;
    for (x = 0; x < nlen; x++) {
        input[0] = (unsigned short)(pch[0] << 8 | pch[1]);
        input[1] = (unsigned short)(pch[2] << 8 | pch[3]);
        input[2] = (unsigned short)(pch[4] << 8 | pch[5]);
        input[3] = (unsigned short)(pch[6] << 8 | pch[7]);
        cipher_idea(input, output, Z);
        /* 反汇编确认：读入 big-endian，写回 little-endian（非对称） */
        pch[0] = (unsigned char)output[0];
        pch[1] = (unsigned char)(output[0] >> 8);
        pch[2] = (unsigned char)output[1];
        pch[3] = (unsigned char)(output[1] >> 8);
        pch[4] = (unsigned char)output[2];
        pch[5] = (unsigned char)(output[2] >> 8);
        pch[6] = (unsigned char)output[3];
        pch[7] = (unsigned char)(output[3] >> 8);
        pch += 8;
    }
    return nlen * 8;
}

// mangled: _ZN5CIdea12decipher_fileEPhjPt
unsigned int CIdea::decipher_file(unsigned char *in, unsigned int len, unsigned short *key) {
    unsigned short input[4];
    unsigned short output[4];
    unsigned short Z[52];
    unsigned short DK[52];
    unsigned char *pch;
    unsigned int nlen;
    unsigned int x;

    en_key_idea(key, Z);
    de_key_idea(Z, DK);
    nlen = len / 8;
    pch = in;
    for (x = 0; x < nlen; x++) {
        /* decipher_file：读入 little-endian，写回 big-endian（与 cipher_file 镜像） */
        input[0] = (unsigned short)(pch[0] | pch[1] << 8);
        input[1] = (unsigned short)(pch[2] | pch[3] << 8);
        input[2] = (unsigned short)(pch[4] | pch[5] << 8);
        input[3] = (unsigned short)(pch[6] | pch[7] << 8);
        cipher_idea(input, output, DK);
        pch[0] = (unsigned char)(output[0] >> 8);
        pch[1] = (unsigned char)output[0];
        pch[2] = (unsigned char)(output[1] >> 8);
        pch[3] = (unsigned char)output[1];
        pch[4] = (unsigned char)(output[2] >> 8);
        pch[5] = (unsigned char)output[2];
        pch[6] = (unsigned char)(output[3] >> 8);
        pch[7] = (unsigned char)output[3];
        pch += 8;
    }
    return nlen * 8;
}

// mangled: _ZN5CIdea4InitEPvS0_
void CIdea::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN5CIdea6UnInitEv
void CIdea::UnInit() {
}

// mangled: _ZN5CIdea4IdeaEPhbS0_j
void CIdea::Idea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;

    if (buflen != 0) {
        nLen = benc ? cipher_file(buf, buflen, (unsigned short *)key)
                    : decipher_file(buf, buflen, (unsigned short *)key);
        pLocal = CreateTsLocal();
        pLocal->ProcessLastBytes(key, benc, buflen, 16, buf);
    }
}

// mangled: _ZN5CIdeaC1Ev / _ZN5CIdeaC2Ev
CIdea::CIdea() {
    Init(0, 0);
}

// mangled: _ZN5CIdeaD1Ev / _ZN5CIdeaD2Ev
CIdea::~CIdea() {
    UnInit();
}
