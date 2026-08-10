/* twofish.cpp -- CTwofish：Twofish 块密码（tencrypt 复刻）

   算法来源：Counterpane AES 提交参考实现（Doug Whiting 优化版 twofish.c，
   Schneier/Kelsey/Whiting/Wagner/Hall，FULL_KEY 变体），gunnersvr/zergsvr/
   secagent 的 tencrypt_new 与其逐字节等价。P8x8 表与二进制 .rodata
   （0x081b7960/0x081b7a60）一致；RS_MDS_Encode / BuildMDS（SetMDS 全用
   P_N0 下标）/ F32（P_01=0、P_31=1 等 P_ij 与参考一致）/ 子钥 / S 盒 /
   轮函数均与参考相同。与参考的差异（经 secagent 二进制反汇编确认）：

   1. makeKey 把密钥解析拆成 ParseHexDword：key32[i] 按字节序 (b1,b0,b3,b2)
      打包（字节 i -> ((i^1)&3)*8），并回写规范文本；
   2. keyInstance/cipherInstance 布局按二进制（见 twofish.h）；_sBox_ 为
      CTwofish 对象成员（参考 REENTRANT=0 的 static fullSbox），2x512 dword
      交错表（c0/c1），表尺寸与 FULL_KEY 的 2*i+(N&1)+2*J 索引配套；
   3. 方向约定 0=加密/1=解密（与参考 DIR_ENCRYPT=0/DIR_DECRYPT=1 相同），
      reKey 对加密方向调 ReverseRoundSubkeys；
   4. blockEncrypt/blockDecrypt 无字节序交换（直接小端 dword 读写），
      CBC 链取 cipher->iv32；blockDecrypt 不把链回写 iv32；
   5. cipherInit 仅 mode!=1 时解析 128 位 IV 文本（iv32 + IV 字节副本）；
   6. 余数处理：Twofish16/24/32 先按 16 字节块 ECB，余数交
      CreateTsLocal()->Seattos(key, benc, buf+已处理, 余长)。

   验证：uni_call 与 secagent 二进制逐字节对拍（keyInstance 全字段、
   _sBox_ 全表、Twofish16/24/32 加解密含非 16 对齐余数）。 */

#include "inc/twofish.h"
#include "include/TenCrypt.h"

#include <string.h>

#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

#define b0(x) ((BYTE)(x))
#define b1(x) ((BYTE)((x) >> 8))
#define b2(x) ((BYTE)((x) >> 16))
#define b3(x) ((BYTE)((x) >> 24))

/* 固定 8x8 置换（与二进制 .rodata 0x081b7a60/0x081b7960 逐字节一致） */
static const unsigned char TWOFISH_P8x8[2][256] = {
    {
        0xA9, 0x67, 0xB3, 0xE8, 0x04, 0xFD, 0xA3, 0x76,
        0x9A, 0x92, 0x80, 0x78, 0xE4, 0xDD, 0xD1, 0x38,
        0x0D, 0xC6, 0x35, 0x98, 0x18, 0xF7, 0xEC, 0x6C,
        0x43, 0x75, 0x37, 0x26, 0xFA, 0x13, 0x94, 0x48,
        0xF2, 0xD0, 0x8B, 0x30, 0x84, 0x54, 0xDF, 0x23,
        0x19, 0x5B, 0x3D, 0x59, 0xF3, 0xAE, 0xA2, 0x82,
        0x63, 0x01, 0x83, 0x2E, 0xD9, 0x51, 0x9B, 0x7C,
        0xA6, 0xEB, 0xA5, 0xBE, 0x16, 0x0C, 0xE3, 0x61,
        0xC0, 0x8C, 0x3A, 0xF5, 0x73, 0x2C, 0x25, 0x0B,
        0xBB, 0x4E, 0x89, 0x6B, 0x53, 0x6A, 0xB4, 0xF1,
        0xE1, 0xE6, 0xBD, 0x45, 0xE2, 0xF4, 0xB6, 0x66,
        0xCC, 0x95, 0x03, 0x56, 0xD4, 0x1C, 0x1E, 0xD7,
        0xFB, 0xC3, 0x8E, 0xB5, 0xE9, 0xCF, 0xBF, 0xBA,
        0xEA, 0x77, 0x39, 0xAF, 0x33, 0xC9, 0x62, 0x71,
        0x81, 0x79, 0x09, 0xAD, 0x24, 0xCD, 0xF9, 0xD8,
        0xE5, 0xC5, 0xB9, 0x4D, 0x44, 0x08, 0x86, 0xE7,
        0xA1, 0x1D, 0xAA, 0xED, 0x06, 0x70, 0xB2, 0xD2,
        0x41, 0x7B, 0xA0, 0x11, 0x31, 0xC2, 0x27, 0x90,
        0x20, 0xF6, 0x60, 0xFF, 0x96, 0x5C, 0xB1, 0xAB,
        0x9E, 0x9C, 0x52, 0x1B, 0x5F, 0x93, 0x0A, 0xEF,
        0x91, 0x85, 0x49, 0xEE, 0x2D, 0x4F, 0x8F, 0x3B,
        0x47, 0x87, 0x6D, 0x46, 0xD6, 0x3E, 0x69, 0x64,
        0x2A, 0xCE, 0xCB, 0x2F, 0xFC, 0x97, 0x05, 0x7A,
        0xAC, 0x7F, 0xD5, 0x1A, 0x4B, 0x0E, 0xA7, 0x5A,
        0x28, 0x14, 0x3F, 0x29, 0x88, 0x3C, 0x4C, 0x02,
        0xB8, 0xDA, 0xB0, 0x17, 0x55, 0x1F, 0x8A, 0x7D,
        0x57, 0xC7, 0x8D, 0x74, 0xB7, 0xC4, 0x9F, 0x72,
        0x7E, 0x15, 0x22, 0x12, 0x58, 0x07, 0x99, 0x34,
        0x6E, 0x50, 0xDE, 0x68, 0x65, 0xBC, 0xDB, 0xF8,
        0xC8, 0xA8, 0x2B, 0x40, 0xDC, 0xFE, 0x32, 0xA4,
        0xCA, 0x10, 0x21, 0xF0, 0xD3, 0x5D, 0x0F, 0x00,
        0x6F, 0x9D, 0x36, 0x42, 0x4A, 0x5E, 0xC1, 0xE0
    },
    {
        0x75, 0xF3, 0xC6, 0xF4, 0xDB, 0x7B, 0xFB, 0xC8,
        0x4A, 0xD3, 0xE6, 0x6B, 0x45, 0x7D, 0xE8, 0x4B,
        0xD6, 0x32, 0xD8, 0xFD, 0x37, 0x71, 0xF1, 0xE1,
        0x30, 0x0F, 0xF8, 0x1B, 0x87, 0xFA, 0x06, 0x3F,
        0x5E, 0xBA, 0xAE, 0x5B, 0x8A, 0x00, 0xBC, 0x9D,
        0x6D, 0xC1, 0xB1, 0x0E, 0x80, 0x5D, 0xD2, 0xD5,
        0xA0, 0x84, 0x07, 0x14, 0xB5, 0x90, 0x2C, 0xA3,
        0xB2, 0x73, 0x4C, 0x54, 0x92, 0x74, 0x36, 0x51,
        0x38, 0xB0, 0xBD, 0x5A, 0xFC, 0x60, 0x62, 0x96,
        0x6C, 0x42, 0xF7, 0x10, 0x7C, 0x28, 0x27, 0x8C,
        0x13, 0x95, 0x9C, 0xC7, 0x24, 0x46, 0x3B, 0x70,
        0xCA, 0xE3, 0x85, 0xCB, 0x11, 0xD0, 0x93, 0xB8,
        0xA6, 0x83, 0x20, 0xFF, 0x9F, 0x77, 0xC3, 0xCC,
        0x03, 0x6F, 0x08, 0xBF, 0x40, 0xE7, 0x2B, 0xE2,
        0x79, 0x0C, 0xAA, 0x82, 0x41, 0x3A, 0xEA, 0xB9,
        0xE4, 0x9A, 0xA4, 0x97, 0x7E, 0xDA, 0x7A, 0x17,
        0x66, 0x94, 0xA1, 0x1D, 0x3D, 0xF0, 0xDE, 0xB3,
        0x0B, 0x72, 0xA7, 0x1C, 0xEF, 0xD1, 0x53, 0x3E,
        0x8F, 0x33, 0x26, 0x5F, 0xEC, 0x76, 0x2A, 0x49,
        0x81, 0x88, 0xEE, 0x21, 0xC4, 0x1A, 0xEB, 0xD9,
        0xC5, 0x39, 0x99, 0xCD, 0xAD, 0x31, 0x8B, 0x01,
        0x18, 0x23, 0xDD, 0x1F, 0x4E, 0x2D, 0xF9, 0x48,
        0x4F, 0xF2, 0x65, 0x8E, 0x78, 0x5C, 0x58, 0x19,
        0x8D, 0xE5, 0x98, 0x57, 0x67, 0x7F, 0x05, 0x64,
        0xAF, 0x63, 0xB6, 0xFE, 0xF5, 0xB7, 0x3C, 0xA5,
        0xCE, 0xE9, 0x68, 0x44, 0xE0, 0x4D, 0x43, 0x69,
        0x29, 0x2E, 0xAC, 0x15, 0x59, 0xA8, 0x0A, 0x9E,
        0x6E, 0x47, 0xDF, 0x34, 0x35, 0x6A, 0xCF, 0xDC,
        0x22, 0xC9, 0xC0, 0x9B, 0x89, 0xD4, 0xED, 0xAB,
        0x12, 0xA2, 0x0D, 0x52, 0xBB, 0x02, 0x2F, 0xA9,
        0xD7, 0x61, 0x1E, 0xB4, 0x50, 0x04, 0xF6, 0xC2,
        0x16, 0x25, 0x86, 0x56, 0x55, 0x09, 0xBE, 0x91
    }
};

/* P_ij（二进制变体：p8(01)->P0、p8(31)->P1，与参考分发版 P_01=1/P_31=0 不同） */
static const int P_IDX_1[4] = { 0, 0, 1, 1 };   /* p8(N1)：S 盒外层置换 */
static const int P_IDX_2[4] = { 0, 1, 0, 1 };   /* p8(N2)：S 盒内层置换 */
static const int P_IDX_3[4] = { 1, 1, 0, 0 };   /* p8(N3) */
static const int P_IDX_4[4] = { 1, 0, 0, 1 };   /* p8(N4) */

// mangled: _ZN8CTwofish4InitEPvS0_
void CTwofish::Init(void *pSys, void *pLocal) {
    unsigned char nRounds[4];
    unsigned char tmp[256];
    int i;

    P8x8 = 0;
    MDStab = 0;
    _sBox_ = 0;
    numRounds = 0;
    m_pSys = pSys;
    m_pLocal = pLocal;

    nRounds[0] = 0;
    nRounds[1] = 0x10;
    nRounds[2] = 0x10;
    nRounds[3] = 0x10;

    if (!P8x8) {
        P8x8 = new unsigned char[0x200];
        memset(P8x8, 0, 0x200);
    }
    if (!MDStab) {
        MDStab = new DWORD[0x400];
        memset(MDStab, 0, 0x1000);
    }
    if (!_sBox_) {
        _sBox_ = new DWORD[0x400];
        memset(_sBox_, 0, 0x1000);
    }
    if (!numRounds) {
        numRounds = new unsigned char[4];
        numRounds[0] = 0;
    }
    memmove(numRounds, nRounds, 4);

    for (i = 0; i < 0x100; i++)
        tmp[i] = TWOFISH_P8x8[0][i];
    memmove(P8x8, tmp, 0x100);
    for (i = 0; i < 0x100; i++)
        tmp[i] = TWOFISH_P8x8[1][i];
    memmove(P8x8 + 0x100, tmp, 0x100);
}

// mangled: _ZN8CTwofish6UnInitEv
void CTwofish::UnInit() {
    if (P8x8) {
        delete[] P8x8;
        P8x8 = 0;
    }
    if (MDStab) {
        delete[] MDStab;
        MDStab = 0;
    }
    if (numRounds) {
        delete[] numRounds;
        numRounds = 0;
    }
    if (_sBox_) {
        delete[] _sBox_;
        _sBox_ = 0;
    }
}

// mangled: _ZN8CTwofishC1Ev / _ZN8CTwofishC2Ev
CTwofish::CTwofish() {
    Init(0, 0);
}

// mangled: _ZN8CTwofishD1Ev / _ZN8CTwofishD2Ev
CTwofish::~CTwofish() {
    UnInit();
}

// mangled: _ZN8CTwofish9setRoundsEii
int CTwofish::setRounds(int keyLen, int nRounds) {
    if (nRounds < 2 || nRounds > 16 || (nRounds & 1))
        return 0;
    if (keyLen == 0xc0) {
        this->numRounds[2] = (BYTE)nRounds;
        return 1;
    }
    if (keyLen == 0x100) {
        this->numRounds[3] = (BYTE)nRounds;
        return 1;
    }
    if (keyLen == 0x80) {
        this->numRounds[1] = (BYTE)nRounds;
        return 1;
    }
    return 0;
}

// mangled: _Z13ParseHexDwordiPKhPjPh
// 十六进制文本 -> DWORD 数组：字节 i 移 ((i^1)&3)*8（组内字节序 b1,b0,b3,b2）；
// dstTxt 非空时回写源文本；成功返回 0。
int ParseHexDword(int bits, const unsigned char *srcTxt, DWORD *d,
                  unsigned char *dstTxt) {
    int i;
    int n;

    if (bits > 0) {
        for (i = 0; i * 8 < bits; i += 4)
            d[i / 4] = 0;
    }
    n = bits / 8;
    for (i = 0; i < n; i++) {
        unsigned char c = srcTxt[i];
        if (dstTxt)
            dstTxt[i] = c;
        d[i / 4] |= (DWORD)c << (((i ^ 1) & 3) * 8);
    }
    return 0;
}

// mangled: _Z13RS_MDS_Encodejj
// (12,8) Reed-Solomon 码：先并入 k1 后并入 k0，各做 4 次 RS_rem（与参考一致）。
DWORD RS_MDS_Encode(DWORD k0, DWORD k1) {
    DWORD r = 0;
    int i;
    int j;

    for (i = 0; i < 2; i++) {
        r ^= (i) ? k0 : k1;
        for (j = 0; j < 4; j++) {
            BYTE b = (BYTE)(r >> 24);
            DWORD g2 = ((b << 1) ^ ((b & 0x80) ? 0x14d : 0)) & 0xff;
            DWORD g3 = ((b >> 1) & 0x7f) ^ ((b & 1) ? 0xa6 : 0) ^ g2;
            r = (r << 8) ^ (g3 << 24) ^ (g2 << 16) ^ (g3 << 8) ^ b;
        }
    }
    return r;
}

// mangled: _ZN8CTwofish8BuildMDSEv
// 由 P[0]/P[1] 预计算 MDS 矩阵 4x256 表（0x169 本原多项式，LFSR1/LFSR2）。
void CTwofish::BuildMDS() {
    int i;

    for (i = 0; i < 256; i++) {
        BYTE m1[2];
        BYTE mX[2];
        BYTE mY[2];
        DWORD d;

        m1[0] = P8x8[i];
        m1[1] = P8x8[i + 0x100];
        mX[0] = (BYTE)(m1[0] ^ ((m1[0] >> 2) ^
                 ((m1[0] & 0x02) ? 0xb4 : 0) ^ ((m1[0] & 0x01) ? 0x5a : 0)));
        mY[0] = (BYTE)(m1[0] ^ ((m1[0] >> 1) ^ ((m1[0] & 0x01) ? 0xb4 : 0)) ^
                 ((m1[0] >> 2) ^ ((m1[0] & 0x02) ? 0xb4 : 0) ^
                  ((m1[0] & 0x01) ? 0x5a : 0)));
        mX[1] = (BYTE)(m1[1] ^ ((m1[1] >> 2) ^
                 ((m1[1] & 0x02) ? 0xb4 : 0) ^ ((m1[1] & 0x01) ? 0x5a : 0)));
        mY[1] = (BYTE)(m1[1] ^ ((m1[1] >> 1) ^ ((m1[1] & 0x01) ? 0xb4 : 0)) ^
                 ((m1[1] >> 2) ^ ((m1[1] & 0x02) ? 0xb4 : 0) ^
                  ((m1[1] & 0x01) ? 0x5a : 0)));

        /* SetMDS(N) 全部用 P_N0 下标（二进制变体，与 F32/S 盒的 P_ij 不同）：
           N=0：m1[1]|mX[1]<<8|mY[1]<<16|mY[1]<<24
           N=1：mY[0]|mY[0]<<8|mX[0]<<16|m1[0]<<24
           N=2：mX[1]|mY[1]<<8|m1[1]<<16|mY[1]<<24
           N=3：mX[0]|m1[0]<<8|mY[0]<<16|mX[0]<<24 */
        d = (DWORD)m1[1] | ((DWORD)mX[1] << 8) | ((DWORD)mY[1] << 16) |
            ((DWORD)mY[1] << 24);
        MDStab[0 * 256 + i] = d;
        d = (DWORD)mY[0] | ((DWORD)mY[0] << 8) | ((DWORD)mX[0] << 16) |
            ((DWORD)m1[0] << 24);
        MDStab[1 * 256 + i] = d;
        d = (DWORD)mX[1] | ((DWORD)mY[1] << 8) | ((DWORD)m1[1] << 16) |
            ((DWORD)mY[1] << 24);
        MDStab[2 * 256 + i] = d;
        d = (DWORD)mX[0] | ((DWORD)m1[0] << 8) | ((DWORD)mY[0] << 16) |
            ((DWORD)mX[0] << 24);
        MDStab[3 * 256 + i] = d;
    }
}

// mangled: _ZN8CTwofish19ReverseRoundSubkeysEPNS_11keyInstanceEh
void CTwofish::ReverseRoundSubkeys(keyInstance *key, BYTE newDir) {
    DWORD t0;
    DWORD t1;
    DWORD *r0 = key->subKeys + 8;
    DWORD *r1 = r0 + 2 * key->numRounds - 2;

    for (; r0 < r1; r0 += 2, r1 -= 2) {
        t0 = r0[0];
        t1 = r0[1];
        r0[0] = r1[0];
        r0[1] = r1[1];
        r1[0] = t0;
        r1[1] = t1;
    }
    key->direction = newDir;
}

// mangled: _Z6Xor256PvS_h
void Xor256(void *dst, void *src, BYTE b) {
    DWORD x = b * 0x01010101u;
    DWORD *d = (DWORD *)dst;
    DWORD *s = (DWORD *)src;
    int i;

    for (i = 0; i < 64; i++)
        d[i] = s[i] ^ x;
}

/* F32：由 64 位密钥字生成一个 S 盒/MDS 值（k64Cnt=2/3/4 对应 128/192/256） */
static inline DWORD TwofishF32(DWORD x, const DWORD *k32, int k64Cnt,
                               const unsigned char *P, const DWORD *MDStab) {
    DWORD t = x;

    switch (k64Cnt & 3) {
    case 0: /* 256-bit：先经 P_4 置换 */
        t = (t & 0xffffff00u) |
            (DWORD)(P[0x100 + (t & 0xff)] ^ b0(k32[3]));
        t = (t & 0xffff00ffu) |
            ((DWORD)(P[0 + ((t >> 8) & 0xff)] ^ b1(k32[3])) << 8);
        t = (t & 0xff00ffffu) |
            ((DWORD)(P[0 + ((t >> 16) & 0xff)] ^ b2(k32[3])) << 16);
        t = (t & 0x00ffffffu) |
            ((DWORD)(P[0x100 + ((t >> 24) & 0xff)] ^ b3(k32[3])) << 24);
        /* fall thru */
    case 3: /* 192-bit */
        t = (t & 0xffffff00u) |
            (DWORD)(P[0x100 + (t & 0xff)] ^ b0(k32[2]));
        t = (t & 0xffff00ffu) |
            ((DWORD)(P[0x100 + ((t >> 8) & 0xff)] ^ b1(k32[2])) << 8);
        t = (t & 0xff00ffffu) |
            ((DWORD)(P[0 + ((t >> 16) & 0xff)] ^ b2(k32[2])) << 16);
        t = (t & 0x00ffffffu) |
            ((DWORD)(P[0 + ((t >> 24) & 0xff)] ^ b3(k32[2])) << 24);
        /* fall thru */
    case 2: /* 128-bit 基轮 */
        return MDStab[0 * 256 + (P[0 + (P[0 + b0(t)] ^ b0(k32[1]))] ^ b0(k32[0]))] ^
               MDStab[1 * 256 + (P[0 + (P[0x100 + b1(t)] ^ b1(k32[1]))] ^ b1(k32[0]))] ^
               MDStab[2 * 256 + (P[0x100 + (P[0 + b2(t)] ^ b2(k32[1]))] ^ b2(k32[0]))] ^
               MDStab[3 * 256 + (P[0x100 + (P[0x100 + b3(t)] ^ b3(k32[1]))] ^ b3(k32[0]))];
    }
    return 0;
}

/* sbSet：写入 _sBox_（表 N&2 内 2*i+(N&1)+2*J 槽位） */
#define sbSet(N, i, J, v) \
    (_sBox_[(N & 2) * 256 + 2 * (i) + (N & 1) + 2 * (J)] = MDStab[N * 256 + (v)])

/* 取 sboxKey 的第 N 字节 */
static inline BYTE bn(int N, DWORD x) {
    switch (N) {
    case 0:
        return (BYTE)x;
    case 1:
        return (BYTE)(x >> 8);
    case 2:
        return (BYTE)(x >> 16);
    }
    return (BYTE)(x >> 24);
}

// mangled: _ZN8CTwofish5reKeyEPNS_11keyInstanceE
int CTwofish::reKey(keyInstance *key) {
    int i;
    int j;
    int k64Cnt;
    int keyLen;
    int subkeyCnt;
    DWORD A = 0;
    DWORD B = 0;
    DWORD q;
    DWORD sKey[4];
    DWORD k32e[4];
    DWORD k32o[4];
    BYTE L0[256];
    BYTE L1[256];
    int N;

    BuildMDS();

    subkeyCnt = 8 + 2 * key->numRounds;
    keyLen = key->keyLen;
    k64Cnt = (keyLen + 63) / 64;
    for (i = 0, j = k64Cnt - 1; i < k64Cnt; i++, j--) {
        k32e[i] = key->key32[2 * i];
        k32o[i] = key->key32[2 * i + 1];
        sKey[j] = key->sboxKeys[j] = RS_MDS_Encode(k32e[i], k32o[i]);
    }

    for (i = 0, q = 0; i < subkeyCnt / 2; i++, q += 0x02020202u) {
        A = TwofishF32(q, k32e, k64Cnt, P8x8, MDStab);
        B = TwofishF32(q + 0x01010101u, k32o, k64Cnt, P8x8, MDStab);
        B = ROL(B, 8);
        key->subKeys[2 * i] = A + B;
        key->subKeys[2 * i + 1] = ROL(A + 2 * B, 9);
    }

    if (keyLen == 0x80) {
        for (N = 0; N < 4; N++) {
            DWORD k0;
            Xor256(L0, P8x8 + P_IDX_2[N] * 0x100, bn(N, sKey[1]));
            k0 = bn(N, sKey[0]);
            for (i = 0; i < 256; i += 2) {
                sbSet(N, i, 0, P8x8[P_IDX_1[N] * 0x100 + L0[i]] ^ k0);
                sbSet(N, i, 1, P8x8[P_IDX_1[N] * 0x100 + L0[i + 1]] ^ k0);
            }
        }
    } else if (keyLen == 0xc0) {
        for (N = 0; N < 4; N++) {
            DWORD k0;
            DWORD k1;
            Xor256(L0, P8x8 + P_IDX_3[N] * 0x100, bn(N, sKey[2]));
            k0 = bn(N, sKey[0]);
            k1 = bn(N, sKey[1]);
            for (i = 0; i < 256; i += 2) {
                sbSet(N, i, 0, P8x8[P_IDX_1[N] * 0x100 +
                      (P8x8[P_IDX_2[N] * 0x100 + L0[i]] ^ k1)] ^ k0);
                sbSet(N, i, 1, P8x8[P_IDX_1[N] * 0x100 +
                      (P8x8[P_IDX_2[N] * 0x100 + L0[i + 1]] ^ k1)] ^ k0);
            }
        }
    } else { /* 0x100 */
        for (N = 0; N < 4; N++) {
            DWORD k0;
            DWORD k1;
            Xor256(L1, P8x8 + P_IDX_4[N] * 0x100, bn(N, sKey[3]));
            for (i = 0; i < 256; i += 2) {
                L0[i] = P8x8[P_IDX_3[N] * 0x100 + L1[i]];
                L0[i + 1] = P8x8[P_IDX_3[N] * 0x100 + L1[i + 1]];
            }
            Xor256(L0, L0, bn(N, sKey[2]));
            k0 = bn(N, sKey[0]);
            k1 = bn(N, sKey[1]);
            for (i = 0; i < 256; i += 2) {
                sbSet(N, i, 0, P8x8[P_IDX_1[N] * 0x100 +
                      (P8x8[P_IDX_2[N] * 0x100 + L0[i]] ^ k1)] ^ k0);
                sbSet(N, i, 1, P8x8[P_IDX_1[N] * 0x100 +
                      (P8x8[P_IDX_2[N] * 0x100 + L0[i + 1]] ^ k1)] ^ k0);
            }
        }
    }

    if (key->direction == 0)
        ReverseRoundSubkeys(key, 0);

    return 1;
}

// mangled: _ZN8CTwofish7makeKeyEPNS_11keyInstanceEhiPh
int CTwofish::makeKey(keyInstance *key, BYTE direction, int keyLen,
                      unsigned char *keyMaterial) {
    int i;

    key->direction = direction;
    key->keyLen = (keyLen + 63) & ~63;
    key->numRounds = numRounds[(keyLen - 1) / 64];
    for (i = 0; i < 8; i++)
        key->key32[i] = 0;

    if (ParseHexDword(keyLen, keyMaterial, key->key32, key->keyMaterial))
        return -2;
    key->keyMaterial[64] = 0;

    return reKey(key);
}

// mangled: _ZN8CTwofish10cipherInitEPNS_14cipherInstanceEhPh
int CTwofish::cipherInit(cipherInstance *cipher, BYTE mode,
                         unsigned char *IV) {
    int i;

    if (mode == 1) {
        cipher->mode = 1;
        return 1;
    }
    if (ParseHexDword(0x80, IV, cipher->iv32, 0))
        return -8;
    for (i = 0; i < 4; i++)
        ((DWORD *)cipher->IV)[i] = cipher->iv32[i];
    cipher->mode = mode;
    return 1;
}

// mangled: _ZN8CTwofish12blockEncryptEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CTwofish::blockEncrypt(cipherInstance *cipher, keyInstance *key,
                           BYTE *input, int inputLen, BYTE *outBuffer) {
    int i;
    int n;
    DWORD x[4];
    DWORD t0;
    DWORD t1;
    int rounds = key->numRounds;
    BYTE bit;
    BYTE ctBit;
    BYTE carry;
    int mode = cipher->mode;
    DWORD sk[40];
    DWORD IV[4];

    if (mode == 3) {
        cipher->mode = 1;
        for (n = 0; n < inputLen; n++) {
            BYTE bit0;
            blockEncrypt(cipher, key, cipher->IV, 0x80, (BYTE *)x);
            bit0 = (BYTE)(0x80 >> (n & 7));
            ctBit = (BYTE)((input[n / 8] & bit0) ^
                           ((((BYTE *)x)[0] & 0x80) >> (n & 7)));
            outBuffer[n / 8] = (BYTE)((outBuffer[n / 8] & ~bit0) | ctBit);
            carry = (BYTE)(ctBit >> (7 - (n & 7)));
            for (i = 15; i >= 0; i--) {
                bit = (BYTE)(cipher->IV[i] >> 7);
                cipher->IV[i] = (BYTE)((cipher->IV[i] << 1) ^ carry);
                carry = bit;
            }
        }
        cipher->mode = 3;
        return inputLen;
    }

    if (key->direction != 0)
        ReverseRoundSubkeys(key, 0);

    memcpy(sk, key->subKeys, sizeof(DWORD) * (8 + 2 * rounds));
    if (mode == 2) {
        IV[0] = cipher->iv32[0];
        IV[1] = cipher->iv32[1];
        IV[2] = cipher->iv32[2];
        IV[3] = cipher->iv32[3];
    } else {
        IV[0] = IV[1] = IV[2] = IV[3] = 0;
    }

    for (n = 0; n < inputLen; n += 16, input += 16, outBuffer += 16) {
        int R;
        x[0] = ((DWORD *)input)[0] ^ sk[0] ^ IV[0];
        x[1] = ((DWORD *)input)[1] ^ sk[1] ^ IV[1];
        x[2] = ((DWORD *)input)[2] ^ sk[2] ^ IV[2];
        x[3] = ((DWORD *)input)[3] ^ sk[3] ^ IV[3];

#define EncryptRound(K, R) \
        do { \
            t0 = _sBox_[2 * ((x[K]) & 0xff)] ^ \
                 _sBox_[2 * ((x[K] >> 8) & 0xff) + 1] ^ \
                 _sBox_[0x200 + 2 * ((x[K] >> 16) & 0xff)] ^ \
                 _sBox_[0x201 + 2 * ((x[K] >> 24) & 0xff)]; \
            t1 = _sBox_[2 * ((x[K ^ 1] >> 24) & 0xff)] ^ \
                 _sBox_[2 * ((x[K ^ 1]) & 0xff) + 1] ^ \
                 _sBox_[0x200 + 2 * ((x[K ^ 1] >> 8) & 0xff)] ^ \
                 _sBox_[0x201 + 2 * ((x[K ^ 1] >> 16) & 0xff)]; \
            x[K ^ 3] = ROL(x[K ^ 3], 1); \
            x[K ^ 2] ^= t0 + t1 + sk[8 + 2 * (R)]; \
            x[K ^ 3] ^= t0 + 2 * t1 + sk[8 + 2 * (R) + 1]; \
            x[K ^ 2] = ROR(x[K ^ 2], 1); \
        } while (0)

        for (R = 14; R >= 0; R -= 2) {
            EncryptRound(0, R + 1);
            EncryptRound(2, R);
        }

        ((DWORD *)outBuffer)[0] = x[2] ^ sk[4];
        ((DWORD *)outBuffer)[1] = x[3] ^ sk[5];
        ((DWORD *)outBuffer)[2] = x[0] ^ sk[6];
        ((DWORD *)outBuffer)[3] = x[1] ^ sk[7];
        if (mode == 2) {
            IV[0] = ((DWORD *)outBuffer)[0];
            IV[1] = ((DWORD *)outBuffer)[1];
            IV[2] = ((DWORD *)outBuffer)[2];
            IV[3] = ((DWORD *)outBuffer)[3];
        }
    }

    if (mode == 2) {
        cipher->iv32[0] = IV[0];
        cipher->iv32[1] = IV[1];
        cipher->iv32[2] = IV[2];
        cipher->iv32[3] = IV[3];
    }
    return inputLen;
}

// mangled: _ZN8CTwofish12blockDecryptEPNS_14cipherInstanceEPNS_11keyInstanceEPhiS4_
int CTwofish::blockDecrypt(cipherInstance *cipher, keyInstance *key,
                           BYTE *input, int inputLen, BYTE *outBuffer) {
    int i;
    int n;
    DWORD x[4];
    DWORD t0;
    DWORD t1;
    int rounds = key->numRounds;
    BYTE bit;
    BYTE ctBit;
    BYTE carry;
    int mode = cipher->mode;
    DWORD sk[40];
    DWORD IV[4];

    if (mode == 3) {
        cipher->mode = 1;
        for (n = 0; n < inputLen; n++) {
            BYTE bit0;
            blockEncrypt(cipher, key, cipher->IV, 0x80, (BYTE *)x);
            bit0 = (BYTE)(0x80 >> (n & 7));
            ctBit = (BYTE)(input[n / 8] & bit0);
            outBuffer[n / 8] = (BYTE)((outBuffer[n / 8] & ~bit0) |
                             (ctBit ^ ((((BYTE *)x)[0] & 0x80) >> (n & 7))));
            carry = (BYTE)(ctBit >> (7 - (n & 7)));
            for (i = 15; i >= 0; i--) {
                bit = (BYTE)(cipher->IV[i] >> 7);
                cipher->IV[i] = (BYTE)((cipher->IV[i] << 1) ^ carry);
                carry = bit;
            }
        }
        cipher->mode = 3;
        return inputLen;
    }

    if (key->direction != 1)
        ReverseRoundSubkeys(key, 1);

    memcpy(sk, key->subKeys, sizeof(DWORD) * (8 + 2 * rounds));
    if (mode == 2) {
        IV[0] = cipher->iv32[0];
        IV[1] = cipher->iv32[1];
        IV[2] = cipher->iv32[2];
        IV[3] = cipher->iv32[3];
    } else {
        IV[0] = IV[1] = IV[2] = IV[3] = 0;
    }

    for (n = 0; n < inputLen; n += 16, input += 16, outBuffer += 16) {
        int R;
        x[0] = ((DWORD *)input)[2] ^ sk[6];
        x[1] = ((DWORD *)input)[3] ^ sk[7];
        x[2] = ((DWORD *)input)[0] ^ sk[4];
        x[3] = ((DWORD *)input)[1] ^ sk[5];

#define DecryptRound(K, R) \
        do { \
            t0 = _sBox_[2 * ((x[K]) & 0xff)] ^ \
                 _sBox_[2 * ((x[K] >> 8) & 0xff) + 1] ^ \
                 _sBox_[0x200 + 2 * ((x[K] >> 16) & 0xff)] ^ \
                 _sBox_[0x201 + 2 * ((x[K] >> 24) & 0xff)]; \
            t1 = _sBox_[2 * ((x[K ^ 1] >> 24) & 0xff)] ^ \
                 _sBox_[2 * ((x[K ^ 1]) & 0xff) + 1] ^ \
                 _sBox_[0x200 + 2 * ((x[K ^ 1] >> 8) & 0xff)] ^ \
                 _sBox_[0x201 + 2 * ((x[K ^ 1] >> 16) & 0xff)]; \
            x[K ^ 2] = ROL(x[K ^ 2], 1); \
            x[K ^ 2] ^= t0 + t1 + sk[8 + 2 * (R)]; \
            x[K ^ 3] ^= t0 + 2 * t1 + sk[8 + 2 * (R) + 1]; \
            x[K ^ 3] = ROR(x[K ^ 3], 1); \
        } while (0)

        for (R = 14; R >= 0; R -= 2) {
            DecryptRound(2, R + 1);
            DecryptRound(0, R);
        }

        if (mode == 1) {
            ((DWORD *)outBuffer)[0] = x[0] ^ sk[0];
            ((DWORD *)outBuffer)[1] = x[1] ^ sk[1];
            ((DWORD *)outBuffer)[2] = x[2] ^ sk[2];
            ((DWORD *)outBuffer)[3] = x[3] ^ sk[3];
        } else {
            ((DWORD *)outBuffer)[0] = x[0] ^ sk[0] ^ IV[0];
            ((DWORD *)outBuffer)[1] = x[1] ^ sk[1] ^ IV[1];
            ((DWORD *)outBuffer)[2] = x[2] ^ sk[2] ^ IV[2];
            ((DWORD *)outBuffer)[3] = x[3] ^ sk[3] ^ IV[3];
            IV[0] = ((DWORD *)input)[0];
            IV[1] = ((DWORD *)input)[1];
            IV[2] = ((DWORD *)input)[2];
            IV[3] = ((DWORD *)input)[3];
        }
    }

    return inputLen;
}

// mangled: _ZN8CTwofish9Twofish16EPhbS0_m
void CTwofish::Twofish16(unsigned char *key, bool benc, unsigned char *buf,
                         long unsigned int buflen) {
    unsigned char hexIV[65];
    keyInstance keyIns;
    cipherInstance twofish_cipher;
    unsigned int nLen;
    ITsLocal *pLocal;

    if (!buflen)
        return;
    memcpy(hexIV,
           "0123456789ABCDEFFEDCBA9876543210"
           "00112233445566778899AABBCCDDEEFF", 65);
    cipherInit(&twofish_cipher, 1, hexIV);
    nLen = (unsigned int)(buflen & ~0xfUL);
    makeKey(&keyIns, benc ? 0 : 1, 0x80, key);
    if (benc)
        blockEncrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    else
        blockDecrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + nLen, (unsigned int)(buflen - nLen));
}

// mangled: _ZN8CTwofish9Twofish24EPhbS0_m
void CTwofish::Twofish24(unsigned char *key, bool benc, unsigned char *buf,
                         long unsigned int buflen) {
    unsigned char hexIV[65];
    keyInstance keyIns;
    cipherInstance twofish_cipher;
    unsigned int nLen;
    ITsLocal *pLocal;

    if (!buflen)
        return;
    memcpy(hexIV,
           "0123456789ABCDEFFEDCBA9876543210"
           "00112233445566778899AABBCCDDEEFF", 65);
    cipherInit(&twofish_cipher, 1, hexIV);
    nLen = (unsigned int)(buflen & ~0xfUL);
    makeKey(&keyIns, benc ? 0 : 1, 0xc0, key);
    if (benc)
        blockEncrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    else
        blockDecrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + nLen, (unsigned int)(buflen - nLen));
}

// mangled: _ZN8CTwofish9Twofish32EPhbS0_m
void CTwofish::Twofish32(unsigned char *key, bool benc, unsigned char *buf,
                         long unsigned int buflen) {
    unsigned char hexIV[65];
    keyInstance keyIns;
    cipherInstance twofish_cipher;
    unsigned int nLen;
    ITsLocal *pLocal;

    if (!buflen)
        return;
    memcpy(hexIV,
           "0123456789ABCDEFFEDCBA9876543210"
           "00112233445566778899AABBCCDDEEFF", 65);
    cipherInit(&twofish_cipher, 1, hexIV);
    nLen = (unsigned int)(buflen & ~0xfUL);
    makeKey(&keyIns, benc ? 0 : 1, 0x100, key);
    if (benc)
        blockEncrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    else
        blockDecrypt(&twofish_cipher, &keyIns, buf, (int)nLen, buf);
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + nLen, (unsigned int)(buflen - nLen));
}

// mangled: _Z12TenTwofish16PhbS_j
void TenTwofish16(unsigned char *key, bool benc, unsigned char *buf,
                  unsigned int buflen) {
    CTwofish tf;

    tf.Twofish16(key, benc, buf, buflen);
}

// mangled: _Z12TenTwofish24PhbS_j
void TenTwofish24(unsigned char *key, bool benc, unsigned char *buf,
                  unsigned int buflen) {
    CTwofish tf;

    tf.Twofish24(key, benc, buf, buflen);
}

// mangled: _Z12TenTwofish32PhbS_j
void TenTwofish32(unsigned char *key, bool benc, unsigned char *buf,
                  unsigned int buflen) {
    CTwofish tf;

    tf.Twofish32(key, benc, buf, buflen);
}
