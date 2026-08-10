/* gost.cpp -- GOST 28147-89 块密码
   S 盒（FUCKING_WZ_SP）与密钥索引表（FUCKING_WZ_SPKEY）从二进制 .rodata
   逐字节提取；轮结构/旋转由反汇编核对（f = S 盒替换 + 循环左移 11）。 */

#include "inc/gost.h"
#include "include/TenCrypt.h"

// 从 secagent .rodata 0x081ccf00 提取（8×16，S[0] 对应最低 4 位）
static const unsigned char WZ_SP[8][16] = {
    {0x04,0x0a,0x09,0x02,0x0d,0x08,0x00,0x0e,0x06,0x0b,0x01,0x0c,0x07,0x0f,0x05,0x03},
    {0x0e,0x0b,0x04,0x0c,0x06,0x0d,0x0f,0x0a,0x02,0x03,0x08,0x01,0x00,0x07,0x05,0x09},
    {0x05,0x08,0x01,0x0d,0x0a,0x03,0x04,0x02,0x0e,0x0f,0x0c,0x07,0x06,0x00,0x09,0x0b},
    {0x07,0x0d,0x0a,0x01,0x00,0x08,0x09,0x0f,0x0e,0x04,0x06,0x0c,0x0b,0x02,0x05,0x03},
    {0x06,0x0c,0x07,0x01,0x05,0x0f,0x0d,0x08,0x04,0x0a,0x09,0x0e,0x00,0x03,0x0b,0x02},
    {0x04,0x0b,0x0a,0x00,0x07,0x02,0x01,0x0d,0x03,0x06,0x08,0x05,0x09,0x0c,0x0f,0x0e},
    {0x0d,0x0b,0x04,0x01,0x03,0x0f,0x05,0x09,0x00,0x0a,0x0e,0x07,0x06,0x08,0x02,0x0c},
    {0x01,0x0f,0x0d,0x00,0x05,0x07,0x0a,0x04,0x09,0x02,0x03,0x0e,0x06,0x0b,0x08,0x0c},
};

// 32 轮密钥索引（正序×3 + 逆序），从 .rodata 0x081cce80 提取
static const unsigned int WZ_SPKEY[32] = {
    0,1,2,3,4,5,6,7, 0,1,2,3,4,5,6,7,
    0,1,2,3,4,5,6,7, 7,6,5,4,3,2,1,0,
};

// mangled: _ZN5CGost1fEj
unsigned int CGost::f(unsigned int x) {
    return (unsigned int)(((unsigned int)WZ_SP[7][(x >> 28) & 0xf] << 28) |
                          ((unsigned int)WZ_SP[6][(x >> 24) & 0xf] << 24) |
                          ((unsigned int)WZ_SP[5][(x >> 20) & 0xf] << 20) |
                          ((unsigned int)WZ_SP[4][(x >> 16) & 0xf] << 16) |
                          ((unsigned int)WZ_SP[3][(x >> 12) & 0xf] << 12) |
                          ((unsigned int)WZ_SP[2][(x >> 8) & 0xf] << 8) |
                          ((unsigned int)WZ_SP[1][(x >> 4) & 0xf] << 4) |
                          ((unsigned int)WZ_SP[0][x & 0xf])) << 11 |
           ((((unsigned int)WZ_SP[7][(x >> 28) & 0xf] << 28) |
             ((unsigned int)WZ_SP[6][(x >> 24) & 0xf] << 24) |
             ((unsigned int)WZ_SP[5][(x >> 20) & 0xf] << 20) |
             ((unsigned int)WZ_SP[4][(x >> 16) & 0xf] << 16) |
             ((unsigned int)WZ_SP[3][(x >> 12) & 0xf] << 12) |
             ((unsigned int)WZ_SP[2][(x >> 8) & 0xf] << 8) |
             ((unsigned int)WZ_SP[1][(x >> 4) & 0xf] << 4) |
             ((unsigned int)WZ_SP[0][x & 0xf])) >> 21);
}

// mangled: _ZN5CGost9gost_swapEPjS0_
void CGost::gost_swap(unsigned int *Ldata, unsigned int *Rdata) {
    unsigned int tempbuf;

    tempbuf = *Ldata;
    *Ldata = *Rdata;
    *Rdata = tempbuf;
}

// mangled: _ZN5CGost10encry_dataEPjS0_S0_
void CGost::encry_data(unsigned int *Ldata, unsigned int *Rdata, unsigned int *key) {
    unsigned int i;
    unsigned int tmp_value;
    unsigned int tempbuf;

    for (i = 0; i < 32; i++) {
        tmp_value = *Rdata;
        tempbuf = f((*Ldata) + key[WZ_SPKEY[i]]) ^ *Rdata;
        *Rdata = *Ldata;
        *Ldata = tempbuf;
    }
    /* 反汇编确认：encry_data 尾部交换 L/R（标准 GOST 输出 (n2, n1)） */
    gost_swap(Ldata, Rdata);
}

// mangled: _ZN5CGost8gost_encEPjS0_
void CGost::gost_enc(unsigned int *data, unsigned int *key) {
    unsigned int *Ldata;
    unsigned int *Rdata;

    Ldata = &data[0];
    Rdata = &data[1];
    encry_data(Ldata, Rdata, key);
}

// mangled: _ZN5CGost11dencry_dataEPjS0_S0_
void CGost::dencry_data(unsigned int *Ldata, unsigned int *Rdata, unsigned int *key) {
    unsigned int i;
    unsigned int tmp_value;
    unsigned int tempbuf;

    for (i = 0; i < 32; i++) {
        tmp_value = *Rdata;
        tempbuf = f((*Ldata) + key[WZ_SPKEY[31 - i]]) ^ *Rdata;
        *Rdata = *Ldata;
        *Ldata = tempbuf;
    }
    gost_swap(Ldata, Rdata);
}

// mangled: _ZN5CGost8gost_decEPjS0_
void CGost::gost_dec(unsigned int *data, unsigned int *key) {
    unsigned int *Ldata;
    unsigned int *Rdata;

    Ldata = &data[0];
    Rdata = &data[1];
    dencry_data(Ldata, Rdata, key);
}

// mangled: _ZN5CGost4InitEPvS0_
void CGost::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN5CGost6UnInitEv
void CGost::UnInit() {
}

// mangled: _ZN5CGost4GostEPhbS0_j
void CGost::Gost(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++) {
            if (benc)
                gost_enc((unsigned int *)(buf + i * 8), (unsigned int *)key);
            else
                gost_dec((unsigned int *)(buf + i * 8), (unsigned int *)key);
        }
        pLocal = CreateTsLocal();
        pLocal->ProcessLastBytes(key, benc, buflen, 32, buf);
    }
}

// mangled: _ZN5CGostC1Ev / _ZN5CGostC2Ev
CGost::CGost() {
    Init(0, 0);
}

// mangled: _ZN5CGostD1Ev / _ZN5CGostD2Ev
CGost::~CGost() {
    UnInit();
}

// mangled: _Z7TenGostPhbS_j
void TenGost(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    CGost gost;

    gost.Gost(key, benc, buf, buflen);
}
