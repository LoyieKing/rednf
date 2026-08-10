/* gramary.cpp -- Gramary 自定义块密码（由二进制反汇编还原）
   gra_round(mode, a0..a7)：按模式 0..15 把 8 个参数分配到 8 个槽位，
   结果 = s0 | s1<<1 | s2<<2 | s3<<3 | s4<<4 | s5<<5 | s7<<6 | s6<<7
   （槽位排列表由 uni_call oracle 提取；gra_crypt 中参数为单比特）。
   gra_crypt(benc, key, buf) 逐 8 字节块（结构与 shiftCrypt 同构）：
   - local[8]：benc 按 GRAIP+GRABB 对 buf 比特收集；dec local=buf[0..7]
   - 主循环：b = buf[i]；benc 时先 gra_round(key[i]&0xf, b 的 8 位) 再
     gra_round(key[i]>>4, ...)；dec 顺序相反；buf[i] = 结果
   - enc：buf=local 后返回；dec：buf 清零后按 GRAIP+GRABB 展开回写
   Gramary 包装：8 字节块循环 + TsLocal->ProcessLastBytes 余数。
   验证：uni_call 调二进制 gra_round/gra_crypt/Gramary 逐字节一致。 */

#include "inc/gramary.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned char CGramary::FUCKING_GRABB[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};
const unsigned char CGramary::FUCKING_GRAIP[64] = {
    0x07, 0x18, 0x10, 0x27, 0x39, 0x0b, 0x31, 0x1b,
    0x22, 0x1a, 0x30, 0x20, 0x32, 0x28, 0x12, 0x21,
    0x11, 0x09, 0x26, 0x3a, 0x2a, 0x0a, 0x02, 0x19,
    0x3b, 0x13, 0x33, 0x08, 0x2b, 0x23, 0x3e, 0x0d,
    0x3c, 0x05, 0x36, 0x2e, 0x01, 0x1e, 0x04, 0x0e,
    0x06, 0x3d, 0x35, 0x2d, 0x25, 0x1d, 0x34, 0x15,
    0x2c, 0x24, 0x38, 0x0f, 0x17, 0x1f, 0x00, 0x2f,
    0x37, 0x1c, 0x3f, 0x14, 0x0c, 0x03, 0x16, 0x29
};

// 由 uni_call oracle 提取：槽位 k 使用输入参数 perm[mode][k]
static const unsigned char gra_perm[16][8] = {
    {1, 0, 3, 2, 5, 4, 7, 6},
    {2, 3, 0, 1, 6, 7, 4, 5},
    {3, 2, 1, 0, 7, 6, 5, 4},
    {4, 5, 6, 7, 0, 1, 2, 3},
    {5, 6, 7, 4, 1, 0, 3, 2},
    {6, 7, 4, 5, 2, 3, 0, 1},
    {7, 6, 5, 4, 3, 2, 1, 0},
    {7, 2, 1, 4, 3, 6, 5, 0},
    {6, 3, 4, 1, 2, 7, 0, 5},
    {7, 4, 5, 6, 1, 2, 3, 0},
    {6, 5, 7, 4, 3, 1, 0, 2},
    {2, 6, 0, 5, 7, 3, 1, 4},
    {3, 7, 5, 0, 6, 2, 4, 1},
    {4, 6, 3, 2, 0, 7, 1, 5},
    {1, 0, 4, 7, 2, 6, 5, 3},
    {3, 6, 5, 0, 7, 2, 1, 4}
};

// mangled: _ZN8CGramary9gra_roundEhhhhhhhhh
unsigned char CGramary::gra_round(unsigned char mode, unsigned char a0,
                                  unsigned char a1, unsigned char a2,
                                  unsigned char a3, unsigned char a4,
                                  unsigned char a5, unsigned char a6,
                                  unsigned char a7) {
    const unsigned char *p = gra_perm[mode & 15];
    unsigned char args[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
    unsigned char s0 = args[p[0]];
    unsigned char s1 = args[p[1]];
    unsigned char s2 = args[p[2]];
    unsigned char s3 = args[p[3]];
    unsigned char s4 = args[p[4]];
    unsigned char s5 = args[p[5]];
    unsigned char s6 = args[p[6]];
    unsigned char s7 = args[p[7]];

    return (unsigned char)(s0 | (s1 << 1) | (s2 << 2) | (s3 << 3) |
                           (s4 << 4) | (s5 << 5) | (s7 << 6) | (s6 << 7));
}

// mangled: _ZN8CGramary9gra_cryptEbPhS0_
void CGramary::gra_crypt(bool benc, unsigned char *key, unsigned char *buf) {
    unsigned char local[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int i;

    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_GRAIP[i];

        if (benc) {
            if (buf[ip >> 3] & FUCKING_GRABB[ip & 7])
                local[i >> 3] |= (unsigned char)(1 << (i & 7));
        } else {
            if (i < 8)
                local[i] = buf[i];
        }
    }

    for (i = 0; i < 8; i++) {
        unsigned char b = buf[i];
        unsigned char m1;
        unsigned char m2;
        unsigned char r;

        if (benc) {
            m1 = key[i] & 0xf;
            m2 = key[i] >> 4;
        } else {
            m1 = key[i] >> 4;
            m2 = key[i] & 0xf;
        }

        r = gra_round(m1, b & 1, (b >> 1) & 1, (b >> 2) & 1,
                      (b >> 3) & 1, (b >> 4) & 1, (b >> 5) & 1,
                      (b >> 6) & 1, (b >> 7) & 1);
        r = gra_round(m2, r & 1, (r >> 1) & 1, (r >> 2) & 1,
                      (r >> 3) & 1, (r >> 4) & 1, (r >> 5) & 1,
                      (r >> 6) & 1, (r >> 7) & 1);
        buf[i] = r;
    }

    if (benc) {
        for (i = 0; i < 8; i++)
            buf[i] = local[i];
        return;
    }

    for (i = 0; i < 8; i++)
        buf[i] = 0;

    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_GRAIP[i];

        if ((local[i >> 3] >> (i & 7)) & 1)
            buf[ip >> 3] |= FUCKING_GRABB[ip & 7];
    }
}

// mangled: _ZN8CGramary4InitEPvS0_
void CGramary::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN8CGramary6UnInitEv
void CGramary::UnInit() {
}

// mangled: _ZN8CGramaryC1Ev / _ZN8CGramaryC2Ev
CGramary::CGramary() {
    Init(0, 0);
}

// mangled: _ZN8CGramaryD1Ev / _ZN8CGramaryD2Ev
CGramary::~CGramary() {
    UnInit();
}

// mangled: _ZN8CGramary7GramaryEPhbS0_j
void CGramary::Gramary(unsigned char *key, bool benc, unsigned char *buf,
                       unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;

    blocks = buflen >> 3;
    for (i = 0; i < blocks; i++)
        gra_crypt(benc, key, buf + 8 * i);

    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 8,
                                      buf + blocks * 8);
}

// mangled: _Z10TenGramaryPhbS_j
void TenGramary(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CGramary gramary;

    gramary.Gramary(key, benc, buf, buflen);
}
