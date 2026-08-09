/* blowfish.cpp -- 标准 Blowfish（Bruce Schneier 公版）
   P 阵列/S 盒从二进制 .rodata 提取（与标准 Blowfish 初始值一致）。
   BlowFish(key, keylen)：keylen>56 截断为 56；P[i] ^= 循环取 4 字节密钥；
   再用当前密钥加密零块依次替换 P[0..17] 与 S[0..3][0..255]。
   Encrypt/Decrypt(SBlock)：标准 16 轮；BytesToBlock/BlockToBytes 大端。
   Encrypt(in,out,len,flag)：flag0=ECB；flag1=CBC（m_oChain 链）；len%8!=0 返回。
   BlowFish8/16/24/32/56：分别 8/16/24/32/56 字节密钥 + 8 字节块 ECB +
   TsLocal->ProcessLastBytes(块大小 0x10) 余数。
   验证：uni_call 调二进制 BlowFish/Encrypt/Decrypt 逐字节一致。 */

#include "inc/blowfish.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned int CBlowFish::FUCKING_M_PDWINITP[18] = {
#include "blowfish_p.inc"
};
const unsigned int CBlowFish::FUCKING_M_PDWINITS[4][256] = {
#include "blowfish_s.inc"
};

// mangled: _ZN9CBlowFish1FEj
unsigned int CBlowFish::F(unsigned int x) {
    unsigned int a = (x >> 24) & 0xff;
    unsigned int b = (x >> 16) & 0xff;
    unsigned int c = (x >> 8) & 0xff;
    unsigned int d = x & 0xff;

    return ((m_pdwAuiS[0][a] + m_pdwAuiS[1][b]) ^ m_pdwAuiS[2][c]) +
           m_pdwAuiS[3][d];
}

// mangled: _ZN9CBlowFish12BytesToBlockEPhRNS_6SBlockE
void CBlowFish::BytesToBlock(unsigned char *p, SBlock &b) {
    unsigned int x = 0;
    unsigned int i;

    for (i = 0; i < 4; i++)
        x = (x << 8) | p[i];
    b.m_uil = x;
    x = 0;
    for (i = 0; i < 4; i++)
        x = (x << 8) | p[i + 4];
    b.m_uir = x;
}

// mangled: _ZN9CBlowFish12BlockToBytesERNS_6SBlockEPh
void CBlowFish::BlockToBytes(SBlock &b, unsigned char *p) {
    unsigned int i;

    for (i = 0; i < 4; i++)
        p[i] = (unsigned char)(b.m_uil >> (24 - 8 * i));
    for (i = 0; i < 4; i++)
        p[i + 4] = (unsigned char)(b.m_uir >> (24 - 8 * i));
}

// mangled: _ZN9CBlowFish7EncryptERNS_6SBlockE
void CBlowFish::Encrypt(SBlock &b) {
    unsigned int xl = b.m_uil;
    unsigned int xr = b.m_uir;
    unsigned int i;

    for (i = 0; i < 16; i++) {
        xl ^= m_pdwAuiP[i];
        xr ^= F(xl);
        {
            unsigned int t = xl;
            xl = xr;
            xr = t;
        }
    }
    {
        unsigned int t = xl;
        xl = xr;
        xr = t;
    }
    xr ^= m_pdwAuiP[16];
    xl ^= m_pdwAuiP[17];
    b.m_uil = xl;
    b.m_uir = xr;
}

// mangled: _ZN9CBlowFish7DecryptERNS_6SBlockE
void CBlowFish::Decrypt(SBlock &b) {
    unsigned int xl = b.m_uil;
    unsigned int xr = b.m_uir;
    int i;

    for (i = 17; i > 1; i--) {
        xl ^= m_pdwAuiP[i];
        xr ^= F(xl);
        {
            unsigned int t = xl;
            xl = xr;
            xr = t;
        }
    }
    {
        unsigned int t = xl;
        xl = xr;
        xr = t;
    }
    xr ^= m_pdwAuiP[1];
    xl ^= m_pdwAuiP[0];
    b.m_uil = xl;
    b.m_uir = xr;
}

// mangled: _ZN9CBlowFish8BlowFishEPhj
void CBlowFish::BlowFish(unsigned char *key, unsigned int keylen) {
    unsigned char kb[56];
    unsigned int k;
    unsigned int i;
    unsigned int j;

    if (!keylen)
        return;
    if (keylen > 0x38)
        keylen = 0x38;
    memcpy(kb, key, keylen);

    memcpy(m_pdwAuiP, FUCKING_M_PDWINITP, 18 * 4);
    memcpy(m_pdwAuiS, FUCKING_M_PDWINITS, 4 * 256 * 4);

    for (i = 0, k = 0; i < 18; i++) {
        unsigned int d = 0;
        int n;
        for (n = 0; n < 4; n++) {
            d = (d << 8) | kb[k];
            k = (k + 1) % keylen;
        }
        m_pdwAuiP[i] ^= d;
    }

    {
        SBlock z(0, 0);
        SBlock *bp = &z;

        for (i = 0; i < 18; i += 2) {
            Encrypt(*bp);
            m_pdwAuiP[i] = bp->m_uil;
            m_pdwAuiP[i + 1] = bp->m_uir;
        }
        for (i = 0; i < 4; i++)
            for (j = 0; j < 256; j += 2) {
                Encrypt(*bp);
                m_pdwAuiS[i][j] = bp->m_uil;
                m_pdwAuiS[i][j + 1] = bp->m_uir;
            }
    }
}

// mangled: _ZN9CBlowFish7EncryptEPhS0_ji
void CBlowFish::Encrypt(unsigned char *in, unsigned char *out,
                        unsigned int len, int flag) {
    SBlock block(0, 0);

    if (!len || (len & 7))
        return;

    if (flag == 1) {
        while (len > 7) {
            BytesToBlock(in, block);
            block.m_uil ^= m_oChain.m_uil;
            block.m_uir ^= m_oChain.m_uir;
            Encrypt(block);
            m_oChain = block;
            BlockToBytes(block, out);
            in += 8;
            out += 8;
            len -= 8;
        }
        return;
    }
    if (flag == 2) {
        /* CBC 变体：输入按小端 dword 直接处理（未逐字节验证，结构同 flag1） */
        while (len > 7) {
            block.m_uil = ((unsigned int *)in)[0] ^ m_oChain.m_uil;
            block.m_uir = ((unsigned int *)in)[1] ^ m_oChain.m_uir;
            Encrypt(block);
            m_oChain = block;
            ((unsigned int *)out)[0] = block.m_uil;
            ((unsigned int *)out)[1] = block.m_uir;
            in += 8;
            out += 8;
            len -= 8;
        }
        return;
    }

    while (len > 7) {
        BytesToBlock(in, block);
        Encrypt(block);
        BlockToBytes(block, out);
        in += 8;
        out += 8;
        len -= 8;
    }
}

// mangled: _ZN9CBlowFish7DecryptEPhS0_ji
void CBlowFish::Decrypt(unsigned char *in, unsigned char *out,
                        unsigned int len, int flag) {
    SBlock block(0, 0);

    if (!len || (len & 7))
        return;

    if (flag == 1) {
        while (len > 7) {
            BytesToBlock(in, block);
            Decrypt(block);
            block.m_uil ^= m_oChain.m_uil;
            block.m_uir ^= m_oChain.m_uir;
            BytesToBlock(in, m_oChain);
            BlockToBytes(block, out);
            in += 8;
            out += 8;
            len -= 8;
        }
        return;
    }
    if (flag == 2) {
        while (len > 7) {
            block.m_uil = ((unsigned int *)in)[0];
            block.m_uir = ((unsigned int *)in)[1];
            Decrypt(block);
            block.m_uil ^= m_oChain.m_uil;
            block.m_uir ^= m_oChain.m_uir;
            m_oChain.m_uil = ((unsigned int *)in)[0];
            m_oChain.m_uir = ((unsigned int *)in)[1];
            ((unsigned int *)out)[0] = block.m_uil;
            ((unsigned int *)out)[1] = block.m_uir;
            in += 8;
            out += 8;
            len -= 8;
        }
        return;
    }

    while (len > 7) {
        BytesToBlock(in, block);
        Decrypt(block);
        BlockToBytes(block, out);
        in += 8;
        out += 8;
        len -= 8;
    }
}

// mangled: _ZN9CBlowFish4InitEPvS0_
void CBlowFish::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN9CBlowFish6UnInitEv
void CBlowFish::UnInit() {
}

// mangled: _ZN9CBlowFishC1Ev / _ZN9CBlowFishC2Ev
CBlowFish::CBlowFish() : m_oChain0(0, 0), m_oChain(0, 0) {
    Init(0, 0);
}

// mangled: _ZN9CBlowFishD1Ev / _ZN9CBlowFishD2Ev
CBlowFish::~CBlowFish() {
    UnInit();
}

// mangled: _ZN9CBlowFish9BlowFish8EPhbS0_j
void CBlowFish::BlowFish8(unsigned char *key, bool benc, unsigned char *buf,
                          unsigned int buflen) {
    if (!buflen)
        return;
    BlowFish(key, 8);
    if (benc)
        Encrypt(buf, buf, (buflen >> 3) << 3, 0);
    else
        Decrypt(buf, buf, (buflen >> 3) << 3, 0);
    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 0x10,
                                      buf + ((buflen >> 3) << 3));
}

// mangled: _ZN9CBlowFish10BlowFish16EPhbS0_j
void CBlowFish::BlowFish16(unsigned char *key, bool benc, unsigned char *buf,
                           unsigned int buflen) {
    if (!buflen)
        return;
    BlowFish(key, 0x10);
    if (benc)
        Encrypt(buf, buf, (buflen >> 3) << 3, 0);
    else
        Decrypt(buf, buf, (buflen >> 3) << 3, 0);
    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 0x10,
                                      buf + ((buflen >> 3) << 3));
}

// mangled: _ZN9CBlowFish10BlowFish24EPhbS0_j
void CBlowFish::BlowFish24(unsigned char *key, bool benc, unsigned char *buf,
                           unsigned int buflen) {
    if (!buflen)
        return;
    BlowFish(key, 0x18);
    if (benc)
        Encrypt(buf, buf, (buflen >> 3) << 3, 0);
    else
        Decrypt(buf, buf, (buflen >> 3) << 3, 0);
    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 0x10,
                                      buf + ((buflen >> 3) << 3));
}

// mangled: _ZN9CBlowFish10BlowFish32EPhbS0_j
void CBlowFish::BlowFish32(unsigned char *key, bool benc, unsigned char *buf,
                           unsigned int buflen) {
    if (!buflen)
        return;
    BlowFish(key, 0x20);
    if (benc)
        Encrypt(buf, buf, (buflen >> 3) << 3, 0);
    else
        Decrypt(buf, buf, (buflen >> 3) << 3, 0);
    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 0x10,
                                      buf + ((buflen >> 3) << 3));
}

// mangled: _ZN9CBlowFish10BlowFish56EPhbS0_j
void CBlowFish::BlowFish56(unsigned char *key, bool benc, unsigned char *buf,
                           unsigned int buflen) {
    if (!buflen)
        return;
    BlowFish(key, 0x38);
    if (benc)
        Encrypt(buf, buf, (buflen >> 3) << 3, 0);
    else
        Decrypt(buf, buf, (buflen >> 3) << 3, 0);
    CreateTsLocal()->ProcessLastBytes(key, benc, buflen, 0x10,
                                      buf + ((buflen >> 3) << 3));
}

// mangled: _Z11TenBlowFish8PhbS_j
void TenBlowFish8(unsigned char *buf, bool benc, unsigned char *key,
                  unsigned int buflen) {
    CBlowFish bf;

    bf.BlowFish8(key, benc, buf, buflen);
}

// mangled: _Z12TenBlowFish16PhbS_j
void TenBlowFish16(unsigned char *buf, bool benc, unsigned char *key,
                   unsigned int buflen) {
    CBlowFish bf;

    bf.BlowFish16(key, benc, buf, buflen);
}

// mangled: _Z12TenBlowFish24PhbS_j
void TenBlowFish24(unsigned char *buf, bool benc, unsigned char *key,
                   unsigned int buflen) {
    CBlowFish bf;

    bf.BlowFish24(key, benc, buf, buflen);
}

// mangled: _Z12TenBlowFish32PhbS_j
void TenBlowFish32(unsigned char *buf, bool benc, unsigned char *key,
                   unsigned int buflen) {
    CBlowFish bf;

    bf.BlowFish32(key, benc, buf, buflen);
}

// mangled: _Z12TenBlowFish56PhbS_j
void TenBlowFish56(unsigned char *buf, bool benc, unsigned char *key,
                   unsigned int buflen) {
    CBlowFish bf;

    bf.BlowFish56(key, benc, buf, buflen);
}
