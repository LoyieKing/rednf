/* gene.cpp -- Gene / GeneNew / SetKey 自定义流式密码（由二进制反汇编还原）
   SetGeneKey(key, benc, buf, table)：
   - buf[0..7] = key[0..7] ^ {0x8a,0xe6,0x9b,0xf3,0xc1,0x7d,0x40,0x25}
   - table[i] = i（单位置换），以 buf 前 8 字节组成 32 位种子（高 4 字节
     dword ^ 低 4 字节 dword），MSVC rand LCG（*0x343fd+0x269ec3）迭代 256 次，
     每次交换 table[i-1] 与 table[(seed>>16)&0xff]
   - benc==0 时用 table 构建逆置换（table[table[i]] = i）
   Gene：逐字节 buf[i]：enc 先 ^i；^kbuf[i&7]；查表；^kbuf[i&7]；dec 再 ^i
   GeneNew：逐字节 buf[i] = buf[i] ^ i ^ key[i&7]（加解密同式）
   SetKey（自由函数，genenew.cpp 桩）：newKey[0..7] = key ^ 固定表；
   以 newKey 前 8 字节组成种子，LCG 迭代 16 次洗牌 buffer[16]（初始 buffer[i]=i，
   idx=(seed>>16)&0xf），再把 buffer 中命中 bit 掩码（0x81/0x40/.../0x01）的位
   OR 回 newKey[i>>1] 的 (i&7) 位。
   验证：uni_call 调二进制 SetGeneKey/Gene/GeneNew/SetKey 逐字节一致。 */

#include "inc/gene.h"
#include "include/TenCrypt.h"

#include <string.h>

// mangled: _ZN5CGene10SetGeneKeyEPhbS0_S0_
void CGene::SetGeneKey(unsigned char *key, bool benc, unsigned char *buf,
                       unsigned char *table) {
    static const unsigned char xork[8] = {
        0x8a, 0xe6, 0x9b, 0xf3, 0xc1, 0x7d, 0x40, 0x25
    };
    unsigned int seed;
    unsigned int i;

    for (i = 0; i < 8; i++)
        buf[i] = key[i] ^ xork[i];

    for (i = 0; i < 256; i++)
        table[i] = (unsigned char)i;

    seed = (((unsigned int)buf[0] << 24) | ((unsigned int)buf[1] << 16) |
            ((unsigned int)buf[2] << 8) | buf[3]) ^
           (((unsigned int)buf[4] << 24) | ((unsigned int)buf[5] << 16) |
            ((unsigned int)buf[6] << 8) | buf[7]);

    for (i = 1; i <= 256; i++) {
        unsigned char temp;
        unsigned int idx;

        seed = seed * 0x343fd + 0x269ec3;
        idx = (seed >> 16) & 0xff;
        temp = table[i - 1];
        table[i - 1] = table[idx];
        table[idx] = temp;
    }

    if (!benc) {
        unsigned char t2[256];

        memcpy(t2, table, 256);
        for (i = 0; i < 256; i++)
            table[t2[i]] = (unsigned char)i;
    }
}

// mangled: _ZN5CGene4GeneEPhbS0_j
void CGene::Gene(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    unsigned char kbuf[8];
    unsigned char table[256];
    unsigned int i;

    if (!buflen)
        return;

    SetGeneKey(key, benc, kbuf, table);

    for (i = 0; i < buflen; i++) {
        if (benc)
            buf[i] ^= (unsigned char)i;
        buf[i] = (unsigned char)(buf[i] ^ kbuf[i & 7]);
        buf[i] = table[buf[i]];
        buf[i] = (unsigned char)(buf[i] ^ kbuf[i & 7]);
        if (!benc)
            buf[i] ^= (unsigned char)i;
    }
}

// mangled: _Z6SetKeyPhS_
void SetKey(unsigned char *key, unsigned char *newKey) {
    static const unsigned char xork[8] = {
        0x8a, 0xe6, 0x9b, 0xf3, 0xc1, 0x7d, 0x40, 0x25
    };
    static const unsigned char bitm[8] = {
        0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
    };
    unsigned char buffer[16];
    unsigned int seed;
    unsigned int i;
    unsigned int idx;

    for (i = 0; i < 8; i++)
        newKey[i] = (unsigned char)(key[i] ^ xork[i]);

    for (i = 0; i < 16; i++)
        buffer[i] = (unsigned char)i;

    seed = (((unsigned int)newKey[0] << 24) | ((unsigned int)newKey[1] << 16) |
            ((unsigned int)newKey[2] << 8) | newKey[3]) ^
           (((unsigned int)newKey[4] << 24) | ((unsigned int)newKey[5] << 16) |
            ((unsigned int)newKey[6] << 8) | newKey[7]);

    for (i = 0; i < 16; i++) {
        unsigned char temp;

        seed = seed * 0x343fd + 0x269ec3;
        idx = (seed >> 16) & 0xf;
        temp = buffer[i];
        buffer[i] = buffer[idx];
        buffer[idx] = temp;
    }

    for (i = 0; i < 16; i++) {
        if (buffer[i] & bitm[i & 7])
            newKey[i >> 1] |= (unsigned char)(1 << (i & 7));
    }
}

// mangled: _Z7GeneNewPhbS_j
void GeneNew(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    unsigned int i;

    if (!buflen)
        return;

    for (i = 0; i < buflen; i++) {
        if (benc)
            buf[i] = (unsigned char)((buf[i] ^ (unsigned char)i) ^
                                     key[i & 7]);
        else
            buf[i] = (unsigned char)((buf[i] ^ key[i & 7]) ^
                                     (unsigned char)i);
    }
}

// mangled: _ZN5CGene4InitEPvS0_
void CGene::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN5CGene6UnInitEv
void CGene::UnInit() {
}

// mangled: _ZN5CGeneC1Ev / _ZN5CGeneC2Ev
CGene::CGene() {
    Init(0, 0);
}

// mangled: _ZN5CGeneD1Ev / _ZN5CGeneD2Ev
CGene::~CGene() {
    UnInit();
}

// mangled: _Z7TenGenePhbS_j
void TenGene(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    CGene gene;

    gene.Gene(key, benc, buf, buflen);
}

// mangled: _Z10TenGeneNewPhbS_j
void TenGeneNew(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    GeneNew(key, benc, buf, buflen);
}
