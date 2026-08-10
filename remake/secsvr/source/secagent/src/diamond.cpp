/* diamond.cpp -- Diamond2 / Diamond2 Lite 块密码（Michael Paul Johnson 公版）
   参考：AC 源码集 DIAMOND/DIAMOND2.CPP。
   与 AC 的差异（由二进制反汇编确认）：
   - 全局静态变量改为 CDiamond 成员；Init 固定分配 Ccitt32Table(0x400) +
     s(0xf000) + si(0xf000)，set_diamond_key 不分配内存
   - keyrand 无 sbox 参数：始终直接取 key[keyindex]（等同 AC 的 sbox=NULL 路径）
   - makeonebox 只有 (i, j) 两参，不做 sbox 链式传递
   - set_diamond_key 返回 int：0 成功 / -1 轮数非法 / -2 块大小不支持
     （AC 版用 puts+exit）；8 字节块要求 rounds>2，16 字节块要求 rounds>4，
     rounds*blocksize<=255
   - Diamond 包装层固定 set_diamond_key(key, 16, 3, !benc, 8)（Lite 模式），
     按 8 字节块循环调用 lite_encrypt/decrypt_block
   验证：uni_call 调二进制 set_diamond_key/lite_encrypt_block/lite_decrypt_block
   逐字节一致（随机多组）。 */

#include "inc/diamond.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned char byte;
typedef unsigned int uint;

// mangled: _Z12permute_litePhS_
void permute_lite(byte *a, byte *b) {
    b[0] = (a[0] & 1) + (a[1] & 2) + (a[2] & 4) + (a[3] & 8) + (a[4] & 0x10) +
        (a[5] & 0x20) + (a[6] & 0x40) + (a[7] & 0x80);
    b[1] = (a[1] & 1) + (a[2] & 2) + (a[3] & 4) + (a[4] & 8) + (a[5] & 0x10) +
        (a[6] & 0x20) + (a[7] & 0x40) + (a[0] & 0x80);
    b[2] = (a[2] & 1) + (a[3] & 2) + (a[4] & 4) + (a[5] & 8) + (a[6] & 0x10) +
        (a[7] & 0x20) + (a[0] & 0x40) + (a[1] & 0x80);
    b[3] = (a[3] & 1) + (a[4] & 2) + (a[5] & 4) + (a[6] & 8) + (a[7] & 0x10) +
        (a[0] & 0x20) + (a[1] & 0x40) + (a[2] & 0x80);
    b[4] = (a[4] & 1) + (a[5] & 2) + (a[6] & 4) + (a[7] & 8) + (a[0] & 0x10) +
        (a[1] & 0x20) + (a[2] & 0x40) + (a[3] & 0x80);
    b[5] = (a[5] & 1) + (a[6] & 2) + (a[7] & 4) + (a[0] & 8) + (a[1] & 0x10) +
        (a[2] & 0x20) + (a[3] & 0x40) + (a[4] & 0x80);
    b[6] = (a[6] & 1) + (a[7] & 2) + (a[0] & 4) + (a[1] & 8) + (a[2] & 0x10) +
        (a[3] & 0x20) + (a[4] & 0x40) + (a[5] & 0x80);
    b[7] = (a[7] & 1) + (a[0] & 2) + (a[1] & 4) + (a[2] & 8) + (a[3] & 0x10) +
        (a[4] & 0x20) + (a[5] & 0x40) + (a[6] & 0x80);
}

// mangled: _Z13ipermute_litePhS_
void ipermute_lite(byte *b, byte *a) {
    a[0] = (b[0] & 1) + (b[7] & 2) + (b[6] & 4) + (b[5] & 8) + (b[4] & 0x10) +
        (b[3] & 0x20) + (b[2] & 0x40) + (b[1] & 0x80);
    a[1] = (b[1] & 1) + (b[0] & 2) + (b[7] & 4) + (b[6] & 8) + (b[5] & 0x10) +
        (b[4] & 0x20) + (b[3] & 0x40) + (b[2] & 0x80);
    a[2] = (b[2] & 1) + (b[1] & 2) + (b[0] & 4) + (b[7] & 8) + (b[6] & 0x10) +
        (b[5] & 0x20) + (b[4] & 0x40) + (b[3] & 0x80);
    a[3] = (b[3] & 1) + (b[2] & 2) + (b[1] & 4) + (b[0] & 8) + (b[7] & 0x10) +
        (b[6] & 0x20) + (b[5] & 0x40) + (b[4] & 0x80);
    a[4] = (b[4] & 1) + (b[3] & 2) + (b[2] & 4) + (b[1] & 8) + (b[0] & 0x10) +
        (b[7] & 0x20) + (b[6] & 0x40) + (b[5] & 0x80);
    a[5] = (b[5] & 1) + (b[4] & 2) + (b[3] & 4) + (b[2] & 8) + (b[1] & 0x10) +
        (b[0] & 0x20) + (b[7] & 0x40) + (b[6] & 0x80);
    a[6] = (b[6] & 1) + (b[5] & 2) + (b[4] & 4) + (b[3] & 8) + (b[2] & 0x10) +
        (b[1] & 0x20) + (b[0] & 0x40) + (b[7] & 0x80);
    a[7] = (b[7] & 1) + (b[6] & 2) + (b[5] & 4) + (b[4] & 8) + (b[3] & 0x10) +
        (b[2] & 0x20) + (b[1] & 0x40) + (b[0] & 0x80);
}

// mangled: _ZN8CDiamond13BuildCRCTableEv
void CDiamond::BuildCRCTable() {
    unsigned int i;

    for (i = 0; i < 256; i++) {
        unsigned int crc = i;
        int j;
        for (j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xedb88320;
            else
                crc >>= 1;
        }
        Ccitt32Table[i] = crc;
    }
}

// mangled: _ZN8CDiamond7keyrandEj
uint CDiamond::keyrand(uint max_value) {
    uint prandvalue;
    uint i;
    unsigned long mask;

    if (!max_value)
        return 0;

    mask = 0;
    for (i = max_value; i > 0; i = i >> 1)
        mask = (mask << 1) | 1;

    i = 0;
    do {
        accum = ((accum >> 8) & 0x00FFFFFF) ^
            Ccitt32Table[(accum ^ key[keyindex]) & 0xFF];
        keyindex++;
        if (keyindex >= keysize) {
            keyindex = 0;
            accum = ((accum >> 8) & 0x00FFFFFF) ^
                Ccitt32Table[(accum ^ (keysize & 0xFF)) & 0xFF];
            accum = ((accum >> 8) & 0x00FFFFFF) ^
                Ccitt32Table[(accum ^ ((keysize >> 8) & 0xFF)) & 0xFF];
        }
        prandvalue = (uint)(accum & mask);
        if ((++i > 97) && (prandvalue > max_value))
            prandvalue -= max_value;
    } while (prandvalue > max_value);
    return prandvalue;
}

// mangled: _ZN8CDiamond10makeoneboxEjj
void CDiamond::makeonebox(uint i, uint j) {
    int n;
    uint pos;
    uint m;
    uint p;
    bool filled[256];

    for (m = 0; m < 256; m++)
        filled[m] = false;

    for (n = 255; n >= 0; n--) {
        pos = keyrand((uint)n);
        p = 0;
        while (filled[p])
            p++;
        for (m = 0; m < pos; m++) {
            p++;
            while (filled[p])
                p++;
        }
        s[roundsize * i + 256 * j + p] = (byte)n;
        filled[p] = true;
    }
}

// mangled: _ZN8CDiamond15set_diamond_keyEPhjjbj
int CDiamond::set_diamond_key(byte *external_key, uint key_size,
                              uint rounds, bool invert, uint block_size) {
    uint i;
    uint j;
    uint k;

    numrounds = rounds;
    if (block_size == 8) {
        blocksize = 8;
        roundsize = 2048U;
        if (numrounds <= 2)
            return -1;
    } else if (block_size == 16) {
        blocksize = 16;
        roundsize = 4096U;
        if (numrounds <= 4)
            return -1;
    } else {
        return -2;
    }

    if ((numrounds * blocksize) > 255)
        return -1;

    BuildCRCTable();
    key = external_key;
    keysize = key_size;
    keyindex = 0;
    accum = 0xFFFFFFFFL;

    for (i = 0; i < numrounds; i++)
        for (j = 0; j < blocksize; j++)
            makeonebox(i, j);

    if (invert) {
        for (i = 0; i < numrounds; i++)
            for (j = 0; j < blocksize; j++)
                for (k = 0; k < 256; k++)
                    si[roundsize * i + 256 * j +
                       s[roundsize * i + 256 * j + k]] = (byte)k;
    }
    return 0;
}

// mangled: _ZN8CDiamond10substituteEjPhS0_
void CDiamond::substitute(uint round, byte *x, byte *y) {
    uint i;

    for (i = 0; i < blocksize; i++)
        y[i] = *(s + (roundsize * round) + (256 * i) + x[i]);
}

// mangled: _ZN8CDiamond6isubstEjPhS0_
void CDiamond::isubst(uint round, byte *x, byte *y) {
    uint i;

    for (i = 0; i < blocksize; i++)
        y[i] = *(si + (roundsize * round) + (256 * i) + x[i]);
}

// mangled: _ZN8CDiamond7permuteEPhS0_
void CDiamond::permute(byte *x, byte *y) {
    uint i;
    uint j;

    for (i = 0; i < 16; i++) {
        y[i] = 0;
        for (j = 0; j < 8; j++)
            y[i] |= x[(i + j) & 15] & (1 << j);
    }
}

// mangled: _ZN8CDiamond8ipermuteEPhS0_
void CDiamond::ipermute(byte *x, byte *y) {
    uint i;
    uint j;

    for (i = 0; i < 16; i++) {
        y[i] = 0;
        for (j = 0; j < 8; j++)
            y[i] |= x[(i + 16 - j) & 15] & (1 << j);
    }
}

// mangled: _ZN8CDiamond21diamond_encrypt_blockEPhS0_
void CDiamond::diamond_encrypt_block(byte *x, byte *y) {
    uint round;
    byte z[16];

    substitute(0, x, y);
    for (round = 1; round < numrounds; round++) {
        permute(y, z);
        substitute(round, z, y);
    }
}

// mangled: _ZN8CDiamond21diamond_decrypt_blockEPhS0_
void CDiamond::diamond_decrypt_block(byte *x, byte *y) {
    int round;
    byte z[16];

    isubst(numrounds - 1, x, y);
    for (round = (int)numrounds - 2; round >= 0; round--) {
        ipermute(y, z);
        isubst((uint)round, z, y);
    }
}

// mangled: _ZN8CDiamond18lite_encrypt_blockEPhS0_
void CDiamond::lite_encrypt_block(byte *x, byte *y) {
    uint round;
    byte z[8];

    substitute(0, x, y);
    for (round = 1; round < numrounds; round++) {
        permute_lite(y, z);
        substitute(round, z, y);
    }
}

// mangled: _ZN8CDiamond18lite_decrypt_blockEPhS0_
void CDiamond::lite_decrypt_block(byte *x, byte *y) {
    int round;
    byte z[8];

    isubst(numrounds - 1, x, y);
    for (round = (int)numrounds - 2; round >= 0; round--) {
        ipermute_lite(y, z);
        isubst((uint)round, z, y);
    }
}

// mangled: _ZN8CDiamond4InitEPvS0_
void CDiamond::Init(void *arg0, void *arg1) {
    Ccitt32Table = new unsigned int[0x400 / 4];
    memset(Ccitt32Table, 0, 0x400);
    s = new byte[0xf000];
    memset(s, 0, 0xf000);
    si = new byte[0xf000];
    memset(si, 0, 0xf000);
    key = 0;
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN8CDiamond6UnInitEv
void CDiamond::UnInit() {
    if (Ccitt32Table) {
        delete[] Ccitt32Table;
        Ccitt32Table = 0;
    }
    if (s) {
        delete[] s;
        s = 0;
    }
    if (si) {
        delete[] si;
        si = 0;
    }
}

// mangled: _ZN8CDiamondC1Ev / _ZN8CDiamondC2Ev
CDiamond::CDiamond() {
    Init(0, 0);
}

// mangled: _ZN8CDiamondD1Ev / _ZN8CDiamondD2Ev
CDiamond::~CDiamond() {
    UnInit();
}

// mangled: _ZN8CDiamond7DiamondEPhbS0_j
void CDiamond::Diamond(byte *key, bool benc, byte *buf, uint buflen) {
    uint blocks;
    uint i;

    if (!buflen)
        return;

    set_diamond_key(key, 0x10, 3, !benc, 8);

    blocks = buflen >> 3;
    for (i = 0; i < blocks; i++) {
        if (benc)
            lite_encrypt_block(buf + 8 * i, buf + 8 * i);
        else
            lite_decrypt_block(buf + 8 * i, buf + 8 * i);
    }
}

// mangled: _Z10TenDiamondPhbS_j
void TenDiamond(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CDiamond diamond;

    diamond.Diamond(key, benc, buf, buflen);
}
