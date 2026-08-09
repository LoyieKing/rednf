/* desbig.cpp -- CDesBig：自定义大表版 DES（tencrypt 闭源实现复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）：

   - des_ky：64 位密钥先做逐位反转（nibble/字节/16 位三段交换 + 交叉），
     随后 16 轮：28 位半密钥按 FUCKING_KS_TAB[i]（0=右移1, 1=右移2）轮转，
     以 a0..a3/c0..c3 字节派生 8 个 6 位索引，查 FUCKING_P2_TAB[8][64] 得
     usky1/usky2，交叉拼成 64 位轮子钥（des_ec/des_dc 按 8 字节/轮消费）。
   - des_ecm：无 IP/FP 的核心 16 轮：X_{r+1} = F(X_r, sk_r) ^ X_{r-1}
     （X_0=in[0], X_{-1}=in[1]），输出 (X15, X16)。
     二进制中无调用点（死代码，但与符号表对齐）。
   - des_ec / des_dc：自定义 IP 置换（逐位反转式位操作，非标准 DES IP）→
     16 轮（des_dc 子钥逆序 sk15..sk0）→ 自定义 FP 置换。
     DesBig 仅调用 des_ec/des_dc。
   - DesBig：des_ky(key, usky) → 8 字节块 ECB（就地）→
     CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 8, buf)。
   F 轮：a = x ^ sk_lo，t = ror4(x) ^ sk_hi，8 个字节各查 SX_TAB 组
   （偶数组取 a 字节、奇数组取 t 字节）后 OR。
   三张表全部从 secagent .rodata 提取（地址见 desbig.h 注释）。 */

#include "inc/desbig.h"
#include "include/TenCrypt.h"

#include <string.h>

// ---- 表（从二进制 .rodata 提取，对应成员 FUCKING_SX_TAB/P2_TAB/KS_TAB；
//      成员为 private 静态数据，此处以文件静态数组承载）----
static const unsigned int SX_TAB[8][256] = {
#include "fucking_sx_tab.inc"
};

static const unsigned char KS_TAB[16] = {
#include "fucking_ks_tab.inc"
};

static const unsigned int P2_TAB[8][64] = {
#include "fucking_p2_tab.inc"
};

static inline unsigned int ror32(unsigned int v, unsigned int n) {
    return (v >> n) | (v << (32 - n));
}

/* 自定义 IP 置换（0x80bb5fa..0x80bb6dc 位操作逐条转写，单比特验证一致）：
   X 收奇数输入位、Y 收偶数输入位（与标准 DES IP 不同）。 */
static void desbig_ip(unsigned int L, unsigned int R,
                      unsigned int *X, unsigned int *Y) {
    unsigned int t, s84, b4, b0, s80, ac, s7c, a8, m;

    t = ror32(R, 4);
    s7c = (L ^ t) & 0x0f0f0f0f;   /* eax &= 0x0f0f0f0f（[ebp-0xec] 未再用） */
    t ^= s7c;
    L ^= s7c;

    b4 = t;
    t = ror32(t, 0xc);
    b4 = t;                        /* [ebp-0xb4] 被 ror12 结果覆盖 */
    t ^= L;
    t &= 0xffff0000;               /* [ebp-0x84] 存后清低 16 位 */
    s84 = t;
    t ^= L;
    b0 = b4 ^ s84;
    b0 = ror32(b0, 0x12);
    ac = b0;
    b0 ^= t;
    s80 = b0;
    s80 &= 0x33333333;
    t ^= s80;
    ac ^= s80;
    ac = ror32(ac, 0x16);
    a8 = ac;
    ac ^= t;
    s7c = ac;
    s7c &= 0xff00ff00;
    t ^= s7c;
    a8 ^= s7c;
    a8 = ror32(a8, 0x9);           /* [ebp-0xa8] 覆盖 */
    m = a8 ^ t;                    /* eax ^= edx（[ebp-0x78] 未再用） */
    m &= 0x55555555;
    b0 = a8;                       /* mov ecx,[ebp-0xa8] 重载 */
    t ^= m;
    t = ror32(t, 0x1f);
    *X = t;                        /* [ebp-0xe8] */
    t = ror32(t, 0x4);
    b0 ^= m;
    b0 = ror32(b0, 0x1e);
    *Y = b0;                       /* [ebp-0xa4] */
}

/* 自定义 FP 置换（0x80bc597..0x80bc62f 转写，与 IP 非互逆、各自独立）。
   注意开头先 ror2(B)，后续 “edx ^= [ebp-0xc8]” 用的是旋转后的 B。 */
static void desbig_fp(unsigned int A, unsigned int B,
                      unsigned int *O0, unsigned int *O1) {
    unsigned int t, c4, c0, bc, b8;

    B = ror32(B, 2);               /* ror DWORD [ebp-0xc8], 0x2 */
    A = ror32(A, 1);
    t = B ^ A;
    t &= 0x55555555;
    A ^= t;
    t ^= B;                        /* [ebp-0xc8]（ror2 后） */
    t = ror32(t, 0x17);
    c4 = t;
    t ^= A;
    t &= 0xff00ff00;
    A ^= t;
    t ^= c4;
    t = ror32(t, 0xa);
    c0 = t;
    t ^= A;
    t &= 0x33333333;
    A ^= t;
    t ^= c0;
    t = ror32(t, 0xe);
    bc = t;
    t ^= A;
    t &= 0xffff0000;
    A ^= t;
    t ^= bc;
    t = ror32(t, 0x14);
    b8 = t;
    t ^= A;
    t &= 0x0f0f0f0f;
    A ^= t;
    *O0 = A;
    b8 ^= t;
    *O1 = ror32(b8, 0x1c);
}

/* F 轮：a/x 字节走偶数 SX 组，t 字节走奇数组（0x080bd6a0 轮体结构）。 */
static inline unsigned int desbig_f(unsigned int x, const unsigned int *sk) {
    unsigned int a = x ^ sk[0];
    unsigned int t = ror32(x, 4) ^ sk[1];

    return SX_TAB[0][a & 0xff]
         | SX_TAB[1][t & 0xff]
         | SX_TAB[2][(a >> 8) & 0xff]
         | SX_TAB[3][(t >> 8) & 0xff]
         | SX_TAB[4][(a >> 16) & 0xff]
         | SX_TAB[5][(t >> 16) & 0xff]
         | SX_TAB[6][(a >> 24) & 0xff]
         | SX_TAB[7][(t >> 24) & 0xff];
}

/* 16 轮 X_{r+1} = F(X_r, sk_r) ^ X_{r-1}（X_0=in[0], X_{-1}=in[1]），
   结果状态 (X15, X16)；des_ec 经 FP 输出，des_ecm 直接输出。
   （注意：二进制无“额外第 17 轮”，最后一轮结果即 X16。） */

// mangled: _ZN7CDesBig6des_ecEPvS0_S0_
void CDesBig::des_ec(void *i_blk, void *o_blk, void *key) {
    unsigned int q0, q1, l0, l1, tt;
    unsigned int X, Y;
    const unsigned int *sk = (const unsigned int *)key;
    unsigned int i;

    q0 = ((unsigned int *)i_blk)[0];
    q1 = ((unsigned int *)i_blk)[1];
    desbig_ip(q0, q1, &X, &Y);

    l0 = X;                        /* X_0 */
    l1 = Y;                        /* X_{-1} */
    for (i = 0; i < 16; i++) {
        tt = desbig_f(l0, sk + 2 * i);
        l1 ^= tt;
        tt = l1;
        l1 = l0;
        l0 = tt;
    }
    desbig_fp(l1, l0, &X, &Y);   /* A=X15, B=X16 */
    ((unsigned int *)o_blk)[0] = X;
    ((unsigned int *)o_blk)[1] = Y;
}

// mangled: _ZN7CDesBig6des_dcEPvS0_S0_
void CDesBig::des_dc(void *i_blk, void *o_blk, void *key) {
    unsigned int q0, q1, l0, l1, tt;
    unsigned int X, Y;
    const unsigned int *sk = (const unsigned int *)key;
    unsigned int i;

    q0 = ((unsigned int *)i_blk)[0];
    q1 = ((unsigned int *)i_blk)[1];
    desbig_ip(q0, q1, &X, &Y);

    l0 = X;
    l1 = Y;
    for (i = 0; i < 16; i++) {
        tt = desbig_f(l0, sk + 2 * (15 - i));
        l1 ^= tt;
        tt = l1;
        l1 = l0;
        l0 = tt;
    }
    desbig_fp(l1, l0, &X, &Y);   /* A=X15, B=X16 */
    ((unsigned int *)o_blk)[0] = X;
    ((unsigned int *)o_blk)[1] = Y;
}

// mangled: _ZN7CDesBig7des_ecmEPvS0_S0_
void CDesBig::des_ecm(void *i_blk, void *o_blk, void *key) {
    unsigned int q0, q1, l0, l1, tt;
    const unsigned int *sk = (const unsigned int *)key;
    unsigned int i;

    q0 = ((unsigned int *)i_blk)[0];
    q1 = ((unsigned int *)i_blk)[1];
    l0 = q0;                       /* X_0 */
    l1 = q1;                       /* X_{-1} */
    for (i = 0; i < 16; i++) {
        tt = desbig_f(l0, sk + 2 * i);
        l1 ^= tt;
        tt = l1;
        l1 = l0;
        l0 = tt;
    }
    ((unsigned int *)o_blk)[0] = l1;   /* X15 */
    ((unsigned int *)o_blk)[1] = l0;   /* X16 */
}

// mangled: _ZN7CDesBig6des_kyEPvS0_
void CDesBig::des_ky(void *kval, void *key) {
    unsigned int v0, v1, l0, l1, tt;
    unsigned short i;

    /* ---- 64 位密钥逐位反转（0x80be1be..0x80be253） ---- */
    v0 = ((unsigned int *)kval)[0];
    v1 = ((unsigned int *)kval)[1];
    tt = ((v1 >> 4) ^ v0) & 0x0f0f0f0f;
    v0 ^= tt;
    l0 = (tt << 4) ^ v1;
    l1 = ((v0 >> 4) ^ l0) & 0x01010101;
    l0 ^= l1;
    l1 = (l1 << 4) ^ v0;
    v0 = l0;                       /* pathA（esi） */
    v1 = l1;                       /* pathB（eax） */

    tt = ((v1 >> 9) ^ v1) & 0x550055;
    v1 ^= tt;
    v1 ^= tt << 9;
    tt = ((v0 >> 9) ^ v0) & 0x550055;
    v0 ^= tt;
    v0 ^= tt << 9;

    tt = ((v1 >> 0x12) ^ v1) & 0x3333;
    v1 ^= tt;
    v1 ^= tt << 0x12;
    tt = ((v0 >> 0x12) ^ v0) & 0x3333;
    v0 ^= tt;
    v0 ^= tt << 0x12;

    tt = ((v0 >> 0x10) ^ v0) & 0xff;
    v0 ^= tt;
    v0 ^= tt << 0x10;

    /* v1 → [ebp-0x24]（a 字节源），v0 → [ebp-0x20]（c 字节源） */
    l0 = v1;
    l1 = v0;

    /* ---- 16 轮子钥生成 ---- */
    for (i = 0; i < 16; i++) {
        unsigned int a0, a1, a2, a3, c0, c1, c2, c3;
        unsigned int b0, b1, b2, b3, b4, b5, b6, b7, b9;
        unsigned int usky1, usky2, w0, w1;

        v0 = l0 & 0x0fffffff;
        v1 = l1 & 0x0fffffff;
        if (KS_TAB[i])
            v0 = (v0 >> 2) | (v0 << 26);
        else
            v0 = (v0 >> 1) | (v0 << 27);
        if (KS_TAB[i])
            v1 = (v1 >> 2) | (v1 << 26);
        else
            v1 = (v1 >> 1) | (v1 << 27);
        l0 = v0;
        l1 = v1;

        a0 = v0 & 0xff;
        a1 = (v0 >> 8) & 0xff;
        a2 = (v0 >> 16) & 0xff;
        a3 = (v0 >> 24) & 0xff;
        c0 = v1 & 0xff;
        c1 = (v1 >> 8) & 0xff;
        c2 = (v1 >> 16) & 0xff;
        c3 = (v1 >> 24) & 0xff;

        b0 = a0 & 0x3f;
        b1 = (a1 >> 1) & 0x3f;
        b2 = (a2 & 0x1d) | ((a2 >> 1) & 0x20) | ((a2 >> 6) & 2);
        b3 = (a1 >> 7) & 1;
        b4 = ((a0 >> 2) & 0x30) | (a3 & 0xe);
        b5 = (c1 & 0x3d) | ((c1 >> 6) & 2);
        b6 = c0 & 0x3f;
        b7 = c2 & 0x3f;
        b9 = ((c0 >> 7) << 1) | ((c2 >> 2) & 0x30) | (c3 & 0xd);

        usky1 = P2_TAB[0][b0] | P2_TAB[1][b1]
              | P2_TAB[2][b2] | P2_TAB[3][b3 | b4];
        usky2 = P2_TAB[4][b6] | P2_TAB[5][b5]
              | P2_TAB[6][b7] | P2_TAB[7][b9];

        w0 = ((usky2 & 0xffff) << 16) | (usky1 & 0xffff);
        w1 = (usky2 & 0xffff0000) | (usky1 >> 16);
        ((unsigned int *)key)[2 * i] = w0;
        ((unsigned int *)key)[2 * i + 1] = w1;
    }
}

// mangled: _ZN7CDesBig4InitEPvS0_
void CDesBig::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN7CDesBig6UnInitEv
void CDesBig::UnInit() {
}

// mangled: _ZN7CDesBig6DesBigEPhbS0_j
void CDesBig::DesBig(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned int usky[32];
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    des_ky(key, usky);
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++) {
        if (benc)
            des_ec(buf + i * 8, buf + i * 8, usky);
        else
            des_dc(buf + i * 8, buf + i * 8, usky);
    }
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN7CDesBigC1Ev / _ZN7CDesBigC2Ev
CDesBig::CDesBig() {
    Init(0, 0);
}

// mangled: _ZN7CDesBigD1Ev / _ZN7CDesBigD2Ev
CDesBig::~CDesBig() {
    UnInit();
}

// mangled: _Z9TenDesBigPhbS_j
void TenDesBig(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CDesBig desbig;

    desbig.DesBig(key, benc, buf, buflen);
}
