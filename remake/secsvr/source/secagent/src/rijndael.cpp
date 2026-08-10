/* rijndael.cpp -- Rijndael/AES 块密码（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-10）。

   算法为标准 AES（FIPS-197，AES-128/192/256），密钥扩展/轮函数与
   Daemen/Rijmen 公版 Rijndael 参考实现一致，按二进制的类布局与
   表结构整理：
   - 状态为 4 个 dword（列），dword = 列内 4 字节按 LSB 在前的
     pack（byte0 为列首字节），ShiftRows 通过 idx/iidx 索引表完成；
   - 加密轮：t2[c] = rotl24(dtbl[b3]) ^ rotl16(dtbl[b2]) ^
     rotl8(dtbl[b1]) ^ dtbl[b0]，末轮 sbox + ShiftRows + 加轮密钥；
   - 解密轮：itbl + iidx 对称结构，末轮 isbox + 逆 ShiftRows + 加轮密钥；
   - 6 张常量表（dtbl/itbl/sbox/isbox/Logtable/Alogtable）从二进制
     .rodata 提取，Init 惰性分配并拷贝；idx/iidx 运行时生成；
   - ikeys = 逆轮密钥表（首末轮直拷，中间轮 inv_mix_column）；
   - RijnDael：固定 32 字节 key 的 ECB，余数交 CreateTsLocal()->
     Seattos(key, benc, buf+对齐长度, 余数)；
   - block_encrypt/block_decrypt 按 ctx->mode 分派：
     0/4=空操作，1=ECB，2=CBC，3=CFB-128，5=OFB（IV 恒加密流），
     6=CTR（IV 大端计数器递增）。
   验证：uni_call 调 secagent 各方法多组随机向量逐字节一致。 */

#include "inc/rijndael.h"
#include "include/TenCrypt.h"

#include <string.h>

static const unsigned int RIJNDAEL_DTBL[256] = {
#include "rijndael_dtbl.inc"
};
static const unsigned int RIJNDAEL_ITBL[256] = {
#include "rijndael_itbl.inc"
};
static const unsigned char RIJNDAEL_SBOX[256] = {
#include "rijndael_sbox.inc"
};
static const unsigned char RIJNDAEL_ISBOX[256] = {
#include "rijndael_isbox.inc"
};
static const unsigned char RIJNDAEL_LOG[256] = {
#include "rijndael_log.inc"
};
static const unsigned char RIJNDAEL_ALOG[256] = {
#include "rijndael_alog.inc"
};

/* ShiftRows 索引表（运行时生成并 memmove 到成员 idx/iidx） */
static const unsigned int RIJNDAEL_IDX[16] = {
    0, 1, 2, 3, 1, 2, 3, 0, 2, 3, 0, 1, 3, 0, 1, 2
};
static const unsigned int RIJNDAEL_IIDX[16] = {
    0, 1, 2, 3, 3, 0, 1, 2, 2, 3, 0, 1, 1, 2, 3, 0
};

static inline unsigned int rijndael_rotl(unsigned int x, int n) {
    return (x << n) | (x >> (32 - n));
}

// mangled: _ZN9CRijndael10InitializeEii
void CRijndael::Initialize(int nrounds, int mode) {
    memset(this, 0, 0x1e8);
    m_Context.nrounds = nrounds;
    m_Context.mode = mode;
}

// mangled: _ZN9CRijndael4InitEPvS0_
void CRijndael::Init(void *pSys, void *pLocal) {
    unsigned int _dtbl[256];
    unsigned int _itbl[256];
    unsigned char _sbox[256];
    unsigned char _isbox[256];
    unsigned char _Logtable[256];
    unsigned char _Alogtable[256];
    unsigned int _idx[16];
    unsigned int _iidx[16];
    int i;

    dtbl = 0;
    itbl = 0;
    sbox = 0;
    isbox = 0;
    Logtable = 0;
    Alogtable = 0;
    idx = 0;
    iidx = 0;
    m_pSys = pSys;
    m_pLocal = pLocal;

    memcpy(_dtbl, RIJNDAEL_DTBL, sizeof(_dtbl));
    memcpy(_itbl, RIJNDAEL_ITBL, sizeof(_itbl));
    memcpy(_sbox, RIJNDAEL_SBOX, sizeof(_sbox));
    memcpy(_isbox, RIJNDAEL_ISBOX, sizeof(_isbox));
    memcpy(_Logtable, RIJNDAEL_LOG, sizeof(_Logtable));
    memcpy(_Alogtable, RIJNDAEL_ALOG, sizeof(_Alogtable));

    if (!dtbl) {
        dtbl = new unsigned int[256];
        memset(dtbl, 0, 256 * 4);
    }
    memmove(dtbl, _dtbl, sizeof(_dtbl));
    if (!itbl) {
        itbl = new unsigned int[256];
        memset(itbl, 0, 256 * 4);
    }
    memmove(itbl, _itbl, sizeof(_itbl));
    if (!sbox) {
        sbox = new unsigned char[256];
        memset(sbox, 0, 256);
    }
    memmove(sbox, _sbox, sizeof(_sbox));
    if (!isbox) {
        isbox = new unsigned char[256];
        memset(isbox, 0, 256);
    }
    memmove(isbox, _isbox, sizeof(_isbox));
    if (!Logtable) {
        Logtable = new unsigned char[256];
        memset(Logtable, 0, 256);
    }
    memmove(Logtable, _Logtable, sizeof(_Logtable));
    if (!Alogtable) {
        Alogtable = new unsigned char[256];
        memset(Alogtable, 0, 256);
    }
    memmove(Alogtable, _Alogtable, sizeof(_Alogtable));

    for (i = 0; i < 16; i++) {
        _idx[i] = RIJNDAEL_IDX[i];
        _iidx[i] = RIJNDAEL_IIDX[i];
    }
    if (!idx) {
        idx = new unsigned int[16];
        memset(idx, 0, 16 * 4);
    }
    memmove(idx, _idx, sizeof(_idx));
    if (!iidx) {
        iidx = new unsigned int[16];
        memset(iidx, 0, 16 * 4);
    }
    memmove(iidx, _iidx, sizeof(_iidx));
}

// mangled: _ZN9CRijndael6UnInitEv
void CRijndael::UnInit() {
    if (dtbl) {
        delete[] dtbl;
        dtbl = 0;
    }
    if (itbl) {
        delete[] itbl;
        itbl = 0;
    }
    if (sbox) {
        delete[] sbox;
        sbox = 0;
    }
    if (isbox) {
        delete[] isbox;
        isbox = 0;
    }
    if (Logtable) {
        delete[] Logtable;
        Logtable = 0;
    }
    if (Alogtable) {
        delete[] Alogtable;
        Alogtable = 0;
    }
    if (idx) {
        delete[] idx;
        idx = 0;
    }
    if (iidx) {
        delete[] iidx;
        iidx = 0;
    }
    memset(this, 0, 0x1e8);
}

// mangled: _ZN9CRijndaelC1Ev / _ZN9CRijndaelC2Ev
CRijndael::CRijndael() {
    Initialize(1, 1);
    Init(0, 0);
}

// mangled: _ZN9CRijndaelD1Ev / _ZN9CRijndaelD2Ev
CRijndael::~CRijndael() {
    UnInit();
}

// mangled: _ZN9CRijndael5xtimeEh
unsigned char CRijndael::xtime(unsigned char a) {
    return (unsigned char)((a << 1) ^ (((a >> 7) & 1) * 0x1b));
}

// mangled: _ZN9CRijndael3mulEhh
unsigned char CRijndael::mul(unsigned char a, unsigned char b) {
    if (!a || !b)
        return 0;
    return Alogtable[(Logtable[a] + Logtable[b]) % 255];
}

// mangled: _ZN9CRijndael14inv_mix_columnEPjS0_
void CRijndael::inv_mix_column(unsigned int *a, unsigned int *b) {
    unsigned char c[4][4];
    int i;
    int j;

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            c[i][j] = (unsigned char)(mul(0xe, (unsigned char)(a[j] >> (8 * i))) ^
                                      mul(0xb, (unsigned char)(a[j] >> (8 * ((i + 1) & 3)))) ^
                                      mul(0xd, (unsigned char)(a[j] >> (8 * ((i + 2) & 3)))) ^
                                      mul(0x9, (unsigned char)(a[j] >> (8 * ((i + 3) & 3)))));
        }
    }
    for (j = 0; j < 4; j++)
        b[j] = c[0][j] | (c[1][j] << 8) | (c[2][j] << 16) | (c[3][j] << 24);
}

// mangled: _ZN9CRijndael18key_addition_8to32EPKhPjS2_
void CRijndael::key_addition_8to32(const unsigned char *txt,
                                   unsigned int *keys,
                                   unsigned int *out) {
    const unsigned char *ptr = txt;
    int i;
    int j;
    unsigned int val;

    for (i = 0; i < 4; i++) {
        val = 0;
        for (j = 0; j < 4; j++)
            val |= (unsigned int)ptr[4 * i + j] << (8 * j);
        out[i] = val ^ keys[i];
    }
}

// mangled: _ZN9CRijndael14key_addition32EPKjPjS2_
void CRijndael::key_addition32(const unsigned int *txt,
                               unsigned int *keys,
                               unsigned int *out) {
    int i;

    for (i = 0; i < 4; i++)
        out[i] = txt[i] ^ keys[i];
}

// mangled: _ZN9CRijndael17key_addition32to8EPKjPjPh
void CRijndael::key_addition32to8(const unsigned int *txt,
                                  unsigned int *keys,
                                  unsigned char *out) {
    unsigned char *ptr = out;
    int i;
    int j;
    unsigned int val;

    for (i = 0; i < 4; i++) {
        val = txt[i] ^ keys[i];
        for (j = 0; j < 4; j++)
            ptr[4 * i + j] = (unsigned char)(val >> (8 * j));
    }
}

// mangled: _ZN9CRijndael14rijndael_setupEPNS_18tagRIJNDAELContextEjPKh
void CRijndael::rijndael_setup(tagRIJNDAELContext *ctx,
                               unsigned int keysize,
                               const unsigned char *key) {
    int nk;
    int nr;
    int i;
    int lastkey;
    unsigned int temp;
    unsigned int rcon;

    if (keysize > 0x1f) {
        nk = 8;
        nr = 0xe;
        lastkey = 0x3c;
    } else if (keysize > 0x17) {
        nk = 6;
        nr = 0xc;
        lastkey = 0x34;
    } else {
        nk = 4;
        nr = 0xa;
        lastkey = 0x2c;
    }
    ctx->nrounds = nr;

    for (i = 0; i < nk; i++)
        ctx->keys[i] = key[4 * i] | (key[4 * i + 1] << 8) |
                       (key[4 * i + 2] << 16) | (key[4 * i + 3] << 24);

    rcon = 1;
    for (i = nk; i < lastkey; i++) {
        temp = ctx->keys[i - 1];
        if (i % nk == 0) {
            temp = rijndael_rotl(temp, 24);
            temp = sbox[temp & 0xff] |
                   (sbox[(temp >> 8) & 0xff] << 8) |
                   (sbox[(temp >> 16) & 0xff] << 16) |
                   (sbox[(temp >> 24) & 0xff] << 24);
            temp ^= rcon;
            rcon = xtime((unsigned char)rcon);
        } else if (nk > 6 && i % nk == 4) {
            temp = sbox[temp & 0xff] |
                   (sbox[(temp >> 8) & 0xff] << 8) |
                   (sbox[(temp >> 16) & 0xff] << 16) |
                   (sbox[(temp >> 24) & 0xff] << 24);
        }
        ctx->keys[i] = ctx->keys[i - nk] ^ temp;
    }

    for (i = 0; i < 4; i++) {
        ctx->ikeys[i] = ctx->keys[i];
        ctx->ikeys[lastkey - 4 + i] = ctx->keys[lastkey - 4 + i];
    }
    for (i = 4; i < lastkey - 4; i += 4)
        inv_mix_column(&ctx->keys[i], &ctx->ikeys[i]);
}

// mangled: _ZN9CRijndael16rijndael_encryptEPNS_18tagRIJNDAELContextEPKhPh
void CRijndael::rijndael_encrypt(tagRIJNDAELContext *ctx,
                                 const unsigned char *plaintext,
                                 unsigned char *ciphertext) {
    unsigned int wtxt[4];
    unsigned int t[4];
    int r;
    int j;

    key_addition_8to32(plaintext, ctx->keys, wtxt);

    for (r = 1; r < ctx->nrounds; r++) {
        for (j = 0; j < 4; j++) {
            t[j] = rijndael_rotl(dtbl[(wtxt[idx[j + 12]] >> 24) & 0xff], 24) ^
                   rijndael_rotl(dtbl[(wtxt[idx[j + 8]] >> 16) & 0xff], 16) ^
                   rijndael_rotl(dtbl[(wtxt[idx[j + 4]] >> 8) & 0xff], 8) ^
                   dtbl[wtxt[j] & 0xff];
        }
        key_addition32(t, &ctx->keys[4 * r], wtxt);
    }

    for (j = 0; j < 4; j++) {
        t[j] = sbox[wtxt[j] & 0xff] |
               (sbox[(wtxt[idx[j + 4]] >> 8) & 0xff] << 8) |
               (sbox[(wtxt[idx[j + 8]] >> 16) & 0xff] << 16) |
               (sbox[(wtxt[idx[j + 12]] >> 24) & 0xff] << 24);
    }
    key_addition32to8(t, &ctx->keys[4 * ctx->nrounds], ciphertext);
}

// mangled: _ZN9CRijndael16rijndael_decryptEPNS_18tagRIJNDAELContextEPKhPh
void CRijndael::rijndael_decrypt(tagRIJNDAELContext *ctx,
                                 const unsigned char *ciphertext,
                                 unsigned char *plaintext) {
    unsigned int wtxt[4];
    unsigned int t[4];
    int r;
    int j;

    key_addition_8to32(ciphertext, &ctx->ikeys[4 * ctx->nrounds], wtxt);

    for (r = ctx->nrounds - 1; r >= 1; r--) {
        for (j = 0; j < 4; j++) {
            t[j] = rijndael_rotl(itbl[(wtxt[iidx[j + 12]] >> 24) & 0xff], 24) ^
                   rijndael_rotl(itbl[(wtxt[iidx[j + 8]] >> 16) & 0xff], 16) ^
                   rijndael_rotl(itbl[(wtxt[iidx[j + 4]] >> 8) & 0xff], 8) ^
                   itbl[wtxt[j] & 0xff];
        }
        key_addition32(t, &ctx->ikeys[4 * r], wtxt);
    }

    for (j = 0; j < 4; j++) {
        unsigned int v = wtxt[j];
        wtxt[j] = isbox[v & 0xff] |
                  (isbox[(v >> 8) & 0xff] << 8) |
                  (isbox[(v >> 16) & 0xff] << 16) |
                  (isbox[(v >> 24) & 0xff] << 24);
    }
    for (j = 0; j < 4; j++) {
        t[j] = (wtxt[j] & 0xff) |
               ((wtxt[iidx[j + 4]] >> 8) & 0xff) << 8 |
               ((wtxt[iidx[j + 8]] >> 16) & 0xff) << 16 |
               ((wtxt[iidx[j + 12]] >> 24) & 0xff) << 24;
    }
    key_addition32to8(t, ctx->ikeys, plaintext);
}

// mangled: _ZN9CRijndael13block_encryptEPNS_18tagRIJNDAELContextEPhiS2_S2_
void CRijndael::block_encrypt(tagRIJNDAELContext *ctx, unsigned char *input,
                              int inputlen, unsigned char *output,
                              unsigned char *iv) {
    unsigned char block[16];
    unsigned char block2[16];
    int i;
    int j;
    int nblocks;
    int carry_flg;

    nblocks = inputlen >> 4;

    switch (ctx->mode) {
    case 0:
    case 4:
        return;
    case 1: /* ECB */
        for (i = 0; i < nblocks; i++)
            rijndael_encrypt(ctx, input + 16 * i, output + 16 * i);
        return;
    case 2: /* CBC */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            for (j = 0; j < 16; j++)
                block[j] ^= input[16 * i + j];
            rijndael_encrypt(ctx, block, block);
            memmove(output + 16 * i, block, 16);
        }
        return;
    case 3: /* CFB-128 */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            memmove(block, output + 16 * i, 16);
        }
        return;
    case 5: /* OFB：block = block2 反馈，标准 OFB */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            memmove(block, block2, 16);
        }
        return;
    case 6: /* CTR：IV 作为大端计数器逐块 +1 */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            carry_flg = 1;
            for (j = 15; j >= 0; j--) {
                block[j] = (unsigned char)(block[j] + carry_flg);
                if (block[j])
                    carry_flg = 0;
            }
        }
        return;
    }
}

// mangled: _ZN9CRijndael13block_decryptEPNS_18tagRIJNDAELContextEPhiS2_S2_
void CRijndael::block_decrypt(tagRIJNDAELContext *ctx, unsigned char *input,
                              int inputlen, unsigned char *output,
                              unsigned char *iv) {
    unsigned char block[16];
    unsigned char block2[16];
    int i;
    int j;
    int nblocks;
    int carry_flg;

    nblocks = inputlen >> 4;

    switch (ctx->mode) {
    case 0:
    case 4:
        return;
    case 1: /* ECB */
        for (i = 0; i < nblocks; i++)
            rijndael_decrypt(ctx, input + 16 * i, output + 16 * i);
        return;
    case 2: /* CBC */
        rijndael_decrypt(ctx, input, block);
        for (j = 0; j < 16; j++)
            output[j] = (unsigned char)(block[j] ^ iv[j]);
        for (i = 1; i < nblocks; i++) {
            rijndael_decrypt(ctx, input + 16 * i, block);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(block[j] ^ input[16 * (i - 1) + j]);
        }
        return;
    case 3: /* CFB-128 */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            memmove(block, input + 16 * i, 16);
        }
        return;
    case 5: /* OFB */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            memmove(block, block2, 16);
        }
        return;
    case 6: /* CTR */
        memmove(block, iv, 16);
        for (i = 0; i < nblocks; i++) {
            rijndael_encrypt(ctx, block, block2);
            for (j = 0; j < 16; j++)
                output[16 * i + j] = (unsigned char)(input[16 * i + j] ^ block2[j]);
            carry_flg = 1;
            for (j = 15; j >= 0; j--) {
                block[j] = (unsigned char)(block[j] + carry_flg);
                if (block[j])
                    carry_flg = 0;
            }
        }
        return;
    }
}

// mangled: _ZN9CRijndael8RijnDaelEPhbS0_j
void CRijndael::RijnDael(unsigned char *key, bool benc, unsigned char *buf,
                         unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;

    rijndael_setup(&m_Context, 0x20, key);
    nLen = buflen >> 4;
    for (i = 0; i < nLen; i++) {
        if (benc)
            rijndael_encrypt(&m_Context, buf + 16 * i, buf + 16 * i);
        else
            rijndael_decrypt(&m_Context, buf + 16 * i, buf + 16 * i);
    }
    pLocal = CreateTsLocal();
    pLocal->Seattos(key, benc, buf + (nLen << 4), buflen - (nLen << 4));
}

// mangled: _Z11TenRijnDaelPhbS_j
void TenRijnDael(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    CRijndael rd;

    rd.RijnDael(key, benc, buf, buflen);
}

// mangled: （二进制无独立符号，DWARF 有定义；被调用点内联）
void CRijndael::SetKey(unsigned int keysize, const unsigned char *key) {
    rijndael_setup(&m_Context, keysize, key);
}

// mangled: （二进制无独立符号，DWARF 有定义；被调用点内联）
void CRijndael::Encrypt(const unsigned char *in, unsigned char *out) {
    rijndael_encrypt(&m_Context, in, out);
}

// mangled: （二进制无独立符号，DWARF 有定义；被调用点内联）
void CRijndael::Decrypt(const unsigned char *in, unsigned char *out) {
    rijndael_decrypt(&m_Context, in, out);
}

// mangled: （二进制无独立符号，DWARF 有定义；被调用点内联）
void CRijndael::BlockEncrypt(unsigned char *in, int inputlen,
                             unsigned char *out, unsigned char *iv) {
    block_encrypt(&m_Context, in, inputlen, out, iv);
}

// mangled: （二进制无独立符号，DWARF 有定义；被调用点内联）
void CRijndael::BlockDecrypt(unsigned char *in, int inputlen,
                             unsigned char *out, unsigned char *iv) {
    block_decrypt(&m_Context, in, inputlen, out, iv);
}
