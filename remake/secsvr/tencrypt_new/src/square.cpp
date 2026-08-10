/* square.cpp -- Square 块密码（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-10）。

   基于 Paulo S.L.M. Barreto 的公版 Square 参考实现
   （Daemen/Knudsen/Rijmen 1997 设计，128 位块 / 128 位密钥 / 8 轮，
   参考源码见 Crypt-Square CPAN 模块 _square.c 及 Crypto++ square.cpp 上游），
   C++ 类化后的差异：
   - Se/Sd/phi/Te0..3/Td0..3 表由静态全局改为 Init 内 static const 局部表
     （与二进制 .rodata 的 C.14..C.24 常量布局一致），Init 惰性分配成员并
     memmove；offset[8]={1,2,4,...,0x80} 为局部数组；
   - SquareTransform 就地变换单个轮密钥（phi[b0] ^ ROTL8(phi[b1]) ^
     ROTL16(phi[b2]) ^ ROTL24(phi[b3])，即二进制 ror24/16/8 组合）；
   - GenerateRoundKeys：rk_e[0]=key 四字（小端装载），t=1..8 演化
     rk_e[t][0]=rk_e[t-1][0]^ROTR8(rk_e[t-1][3])^offset[t-1]（其余依次 XOR），
     解密轮密钥 rk_d[8-t]=rk_e[t] 逆序存放，rk_d[8]=已变换的 rk_e[0]；
     rk_e[0..7] 在演化循环内逐个 SquareTransform；
   - Encrypt/Decrypt 原地处理（text 兼作输入输出，temp 中转）：
     初始 rk[0] 加密钥 -> 7 轮 Te0..3/Td0..3 T 表 -> 末轮 Se/Sd 置换 + rk[8]；
   - Square 包装：buflen==0 直接返回；按 16 字节块循环 Encrypt/Decrypt；
     余数交 CreateTsLocal()->Seattos(key, benc, buf+16*nLen, buflen-16*nLen)；
   - TenSquare 为直通包装。
   验证：uni_call 调二进制 CSquare::* / TenSquare 与本地实现交叉对拍，
   11 张表与二进制 .rodata 逐字节一致。 */

#include "inc/square.h"
#include "include/TenCrypt.h"

#include <string.h>

#define ROUNDS          8
#define BLOCK_SIZE      16

#define ROTL(x, s)      (((x) << (s)) | ((x) >> (32 - (s))))
#define ROTR(x, s)      (((x) >> (s)) | ((x) << (32 - (s))))

#define GETB0(x)        ((unsigned char)((x)))
#define GETB1(x)        ((unsigned char)((x) >> 8))
#define GETB2(x)        ((unsigned char)((x) >> 16))
#define GETB3(x)        ((unsigned char)((x) >> 24))

#define PUTB0(x)        ((unsigned int)(x))
#define PUTB1(x)        ((unsigned int)(x) << 8)
#define PUTB2(x)        ((unsigned int)(x) << 16)
#define PUTB3(x)        ((unsigned int)(x) << 24)

// mangled: _ZN7CSquare15SquareTransformEPj
void CSquare::SquareTransform(unsigned int *roundKey) {
    roundKey[0] = phi[GETB0(roundKey[0])] ^
                  ROTL(phi[GETB1(roundKey[0])], 8) ^
                  ROTL(phi[GETB2(roundKey[0])], 16) ^
                  ROTL(phi[GETB3(roundKey[0])], 24);
    roundKey[1] = phi[GETB0(roundKey[1])] ^
                  ROTL(phi[GETB1(roundKey[1])], 8) ^
                  ROTL(phi[GETB2(roundKey[1])], 16) ^
                  ROTL(phi[GETB3(roundKey[1])], 24);
    roundKey[2] = phi[GETB0(roundKey[2])] ^
                  ROTL(phi[GETB1(roundKey[2])], 8) ^
                  ROTL(phi[GETB2(roundKey[2])], 16) ^
                  ROTL(phi[GETB3(roundKey[2])], 24);
    roundKey[3] = phi[GETB0(roundKey[3])] ^
                  ROTL(phi[GETB1(roundKey[3])], 8) ^
                  ROTL(phi[GETB2(roundKey[3])], 16) ^
                  ROTL(phi[GETB3(roundKey[3])], 24);
}

// mangled: _ZN7CSquare17GenerateRoundKeysEPhPA4_jS2_
void CSquare::GenerateRoundKeys(unsigned char *key,
                                unsigned int (*roundKeys_e)[4],
                                unsigned int (*roundKeys_d)[4]) {
    unsigned int *kp = (unsigned int *)key;
    int t;

    roundKeys_e[0][0] = kp[0];
    roundKeys_e[0][1] = kp[1];
    roundKeys_e[0][2] = kp[2];
    roundKeys_e[0][3] = kp[3];
    for (t = 1; t < ROUNDS + 1; t++) {
        /* 密钥演化函数 */
        roundKeys_d[ROUNDS - t][0] = roundKeys_e[t][0] =
            roundKeys_e[t - 1][0] ^ ROTR(roundKeys_e[t - 1][3], 8) ^ offset[t - 1];
        roundKeys_d[ROUNDS - t][1] = roundKeys_e[t][1] =
            roundKeys_e[t - 1][1] ^ roundKeys_e[t][0];
        roundKeys_d[ROUNDS - t][2] = roundKeys_e[t][2] =
            roundKeys_e[t - 1][2] ^ roundKeys_e[t][1];
        roundKeys_d[ROUNDS - t][3] = roundKeys_e[t][3] =
            roundKeys_e[t - 1][3] ^ roundKeys_e[t][2];
        /* theta 扩散：就地变换上一轮密钥 */
        SquareTransform(roundKeys_e[t - 1]);
    }
    roundKeys_d[ROUNDS][0] = roundKeys_e[0][0];
    roundKeys_d[ROUNDS][1] = roundKeys_e[0][1];
    roundKeys_d[ROUNDS][2] = roundKeys_e[0][2];
    roundKeys_d[ROUNDS][3] = roundKeys_e[0][3];
}

// mangled: _ZN7CSquare9ExpandKeyEPhPA4_j
void CSquare::ExpandKey(unsigned char *key,
                        unsigned int (*roundKeys_e)[4]) {
    unsigned int *kp = (unsigned int *)key;
    int t;

    roundKeys_e[0][0] = kp[0];
    roundKeys_e[0][1] = kp[1];
    roundKeys_e[0][2] = kp[2];
    roundKeys_e[0][3] = kp[3];
    for (t = 1; t < ROUNDS + 1; t++) {
        /* 密钥演化函数 */
        roundKeys_e[t][0] =
            roundKeys_e[t - 1][0] ^ ROTR(roundKeys_e[t - 1][3], 8) ^ offset[t - 1];
        roundKeys_e[t][1] = roundKeys_e[t - 1][1] ^ roundKeys_e[t][0];
        roundKeys_e[t][2] = roundKeys_e[t - 1][2] ^ roundKeys_e[t][1];
        roundKeys_e[t][3] = roundKeys_e[t - 1][3] ^ roundKeys_e[t][2];
        /* theta 扩散：就地变换上一轮密钥 */
        SquareTransform(roundKeys_e[t - 1]);
    }
}

static void squareRound(const unsigned int *text, unsigned int *temp,
                        const unsigned int *T0, const unsigned int *T1,
                        const unsigned int *T2, const unsigned int *T3,
                        const unsigned int *roundKey) {
    temp[0] = T0[GETB0(text[0])] ^ T1[GETB0(text[1])] ^
              T2[GETB0(text[2])] ^ T3[GETB0(text[3])] ^ roundKey[0];
    temp[1] = T0[GETB1(text[0])] ^ T1[GETB1(text[1])] ^
              T2[GETB1(text[2])] ^ T3[GETB1(text[3])] ^ roundKey[1];
    temp[2] = T0[GETB2(text[0])] ^ T1[GETB2(text[1])] ^
              T2[GETB2(text[2])] ^ T3[GETB2(text[3])] ^ roundKey[2];
    temp[3] = T0[GETB3(text[0])] ^ T1[GETB3(text[1])] ^
              T2[GETB3(text[2])] ^ T3[GETB3(text[3])] ^ roundKey[3];
}

static void squareFinal(unsigned int *text, const unsigned int *temp,
                        const unsigned char *S,
                        const unsigned int *roundKey) {
    text[0] = PUTB0(S[GETB0(temp[0])]) ^ PUTB1(S[GETB0(temp[1])]) ^
              PUTB2(S[GETB0(temp[2])]) ^ PUTB3(S[GETB0(temp[3])]) ^
              roundKey[0];
    text[1] = PUTB0(S[GETB1(temp[0])]) ^ PUTB1(S[GETB1(temp[1])]) ^
              PUTB2(S[GETB1(temp[2])]) ^ PUTB3(S[GETB1(temp[3])]) ^
              roundKey[1];
    text[2] = PUTB0(S[GETB2(temp[0])]) ^ PUTB1(S[GETB2(temp[1])]) ^
              PUTB2(S[GETB2(temp[2])]) ^ PUTB3(S[GETB2(temp[3])]) ^
              roundKey[2];
    text[3] = PUTB0(S[GETB3(temp[0])]) ^ PUTB1(S[GETB3(temp[1])]) ^
              PUTB2(S[GETB3(temp[2])]) ^ PUTB3(S[GETB3(temp[3])]) ^
              roundKey[3];
}

// mangled: _ZN7CSquare7EncryptEPjPA4_j
void CSquare::Encrypt(unsigned int *text, unsigned int (*roundKeys)[4]) {
    unsigned int temp[4];

    /* 初始加密钥 */
    text[0] ^= roundKeys[0][0];
    text[1] ^= roundKeys[0][1];
    text[2] ^= roundKeys[0][2];
    text[3] ^= roundKeys[0][3];

    /* 7 个完整轮 */
    squareRound(text, temp, Te0, Te1, Te2, Te3, roundKeys[1]);
    squareRound(temp, text, Te0, Te1, Te2, Te3, roundKeys[2]);
    squareRound(text, temp, Te0, Te1, Te2, Te3, roundKeys[3]);
    squareRound(temp, text, Te0, Te1, Te2, Te3, roundKeys[4]);
    squareRound(text, temp, Te0, Te1, Te2, Te3, roundKeys[5]);
    squareRound(temp, text, Te0, Te1, Te2, Te3, roundKeys[6]);
    squareRound(text, temp, Te0, Te1, Te2, Te3, roundKeys[7]);

    /* 末轮（扩散仅剩转置） */
    squareFinal(text, temp, Se, roundKeys[ROUNDS]);
}

// mangled: _ZN7CSquare7DecryptEPjPA4_j
void CSquare::Decrypt(unsigned int *text, unsigned int (*roundKeys)[4]) {
    unsigned int temp[4];

    /* 初始加密钥 */
    text[0] ^= roundKeys[0][0];
    text[1] ^= roundKeys[0][1];
    text[2] ^= roundKeys[0][2];
    text[3] ^= roundKeys[0][3];

    /* 7 个完整轮 */
    squareRound(text, temp, Td0, Td1, Td2, Td3, roundKeys[1]);
    squareRound(temp, text, Td0, Td1, Td2, Td3, roundKeys[2]);
    squareRound(text, temp, Td0, Td1, Td2, Td3, roundKeys[3]);
    squareRound(temp, text, Td0, Td1, Td2, Td3, roundKeys[4]);
    squareRound(text, temp, Td0, Td1, Td2, Td3, roundKeys[5]);
    squareRound(temp, text, Td0, Td1, Td2, Td3, roundKeys[6]);
    squareRound(text, temp, Td0, Td1, Td2, Td3, roundKeys[7]);

    /* 末轮（扩散仅剩转置） */
    squareFinal(text, temp, Sd, roundKeys[ROUNDS]);
}

// mangled: _ZN7CSquare6SquareEPhbS0_j
void CSquare::Square(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned int roundKeys_e[ROUNDS + 1][4];
    unsigned int roundKeys_d[ROUNDS + 1][4];
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen == 0)
        return;
    GenerateRoundKeys(key, roundKeys_e, roundKeys_d);
    nLen = buflen / BLOCK_SIZE;
    for (i = 0; i < nLen; i++) {
        if (benc)
            Encrypt((unsigned int *)(buf + i * BLOCK_SIZE), roundKeys_e);
        else
            Decrypt((unsigned int *)(buf + i * BLOCK_SIZE), roundKeys_d);
    }
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + nLen * BLOCK_SIZE,
                    buflen - nLen * BLOCK_SIZE);
}

// mangled: _ZN7CSquare4InitEPvS0_
void CSquare::Init(void *pSys, void *pLocal) {
    static const unsigned char _Se[256] = {
#include "square_se.inc"
    };
    static const unsigned char _Sd[256] = {
#include "square_sd.inc"
    };
    static const unsigned int _phi[256] = {
#include "square_phi.inc"
    };
    static const unsigned int _Te0[256] = {
#include "square_te0.inc"
    };
    static const unsigned int _Te1[256] = {
#include "square_te1.inc"
    };
    static const unsigned int _Te2[256] = {
#include "square_te2.inc"
    };
    static const unsigned int _Te3[256] = {
#include "square_te3.inc"
    };
    static const unsigned int _Td0[256] = {
#include "square_td0.inc"
    };
    static const unsigned int _Td1[256] = {
#include "square_td1.inc"
    };
    static const unsigned int _Td2[256] = {
#include "square_td2.inc"
    };
    static const unsigned int _Td3[256] = {
#include "square_td3.inc"
    };
    unsigned int _offset[8] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80 };

    Se = 0;
    Sd = 0;
    phi = 0;
    offset = 0;
    Te0 = 0;
    Te1 = 0;
    Te2 = 0;
    Te3 = 0;
    Td0 = 0;
    Td1 = 0;
    Td2 = 0;
    Td3 = 0;
    m_pSys = pSys;
    m_pLocal = pLocal;
    if (!Se) {
        Se = new unsigned char[256];
        memset(Se, 0, 256);
    }
    memmove(Se, _Se, 256);
    if (!Sd) {
        Sd = new unsigned char[256];
        memset(Sd, 0, 256);
    }
    memmove(Sd, _Sd, 256);
    if (!phi) {
        phi = new unsigned int[256];
        memset(phi, 0, 256 * sizeof(unsigned int));
    }
    memmove(phi, _phi, 256 * sizeof(unsigned int));
    if (!offset) {
        offset = new unsigned int[8];
        memset(offset, 0, 8 * sizeof(unsigned int));
    }
    memmove(offset, _offset, 8 * sizeof(unsigned int));
    if (!Te0) {
        Te0 = new unsigned int[256];
        memset(Te0, 0, 256 * sizeof(unsigned int));
    }
    memmove(Te0, _Te0, 256 * sizeof(unsigned int));
    if (!Te1) {
        Te1 = new unsigned int[256];
        memset(Te1, 0, 256 * sizeof(unsigned int));
    }
    memmove(Te1, _Te1, 256 * sizeof(unsigned int));
    if (!Te2) {
        Te2 = new unsigned int[256];
        memset(Te2, 0, 256 * sizeof(unsigned int));
    }
    memmove(Te2, _Te2, 256 * sizeof(unsigned int));
    if (!Te3) {
        Te3 = new unsigned int[256];
        memset(Te3, 0, 256 * sizeof(unsigned int));
    }
    memmove(Te3, _Te3, 256 * sizeof(unsigned int));
    if (!Td0) {
        Td0 = new unsigned int[256];
        memset(Td0, 0, 256 * sizeof(unsigned int));
    }
    memmove(Td0, _Td0, 256 * sizeof(unsigned int));
    if (!Td1) {
        Td1 = new unsigned int[256];
        memset(Td1, 0, 256 * sizeof(unsigned int));
    }
    memmove(Td1, _Td1, 256 * sizeof(unsigned int));
    if (!Td2) {
        Td2 = new unsigned int[256];
        memset(Td2, 0, 256 * sizeof(unsigned int));
    }
    memmove(Td2, _Td2, 256 * sizeof(unsigned int));
    if (!Td3) {
        Td3 = new unsigned int[256];
        memset(Td3, 0, 256 * sizeof(unsigned int));
    }
    memmove(Td3, _Td3, 256 * sizeof(unsigned int));
}

// mangled: _ZN7CSquare6UnInitEv
void CSquare::UnInit() {
    if (Se) {
        delete[] Se;
        Se = 0;
    }
    if (Sd) {
        delete[] Sd;
        Sd = 0;
    }
    if (phi) {
        delete[] phi;
        phi = 0;
    }
    if (offset) {
        delete[] offset;
        offset = 0;
    }
    if (Te0) {
        delete[] Te0;
        Te0 = 0;
    }
    if (Te1) {
        delete[] Te1;
        Te1 = 0;
    }
    if (Te2) {
        delete[] Te2;
        Te2 = 0;
    }
    if (Te3) {
        delete[] Te3;
        Te3 = 0;
    }
    if (Td0) {
        delete[] Td0;
        Td0 = 0;
    }
    if (Td1) {
        delete[] Td1;
        Td1 = 0;
    }
    if (Td2) {
        delete[] Td2;
        Td2 = 0;
    }
    if (Td3) {
        delete[] Td3;
        Td3 = 0;
    }
}

// mangled: _ZN7CSquareC1Ev / _ZN7CSquareC2Ev
CSquare::CSquare() {
    Init(0, 0);
}

// mangled: _ZN7CSquareD1Ev / _ZN7CSquareD2Ev
CSquare::~CSquare() {
    UnInit();
}

// mangled: _Z9TenSquarePhbS_j
void TenSquare(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CSquare square;

    square.Square(key, benc, buf, buflen);
}
