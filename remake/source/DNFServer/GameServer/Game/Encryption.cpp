// df_game_r Encryption（G7 轻量包加密）还原（2026-08-16）。
// CRC-32 + MS-LCG 种子 + 逐字节旋转/XOR；算法正确即可。

#include "Encryption.h"

#include <cstring>

// index_array（ORIG 0x9370f60，.data，200*300 int）：getOriginalBuf 的插入
// 位置表，从 ORIG 二进制提取，60000 个 0..299 值。
#include "index_array.inc"

// ============================================================================
// CRC-32
// ============================================================================

unsigned int Encryption::crc_table[256];
unsigned int Encryption::XorSeed = 0;
unsigned int Encryption::ShiftSeed = 0;

void Encryption::make_crc_table()
{
    unsigned int crc = 1;
    crc_table[0] = 0;
    for (unsigned int bit = 0x80; bit != 0; bit >>= 1)
    {
        unsigned int v = (crc & 1) ? 0x4db89129U : 0;
        crc = v ^ (crc >> 1);
        for (unsigned int j = 0; j < 0x100; j += bit * 2)
        {
            crc_table[bit + j] = crc_table[j] ^ crc;
        }
    }
}

unsigned int Encryption::crc32(unsigned int seed, const char* data, unsigned int len)
{
    if (crc_table[0] == 0 && crc_table[255] == 0)
        make_crc_table();
    unsigned int crc = ~seed;
    while (len-- != 0)
    {
        crc = crc_table[((unsigned int)(unsigned char)*data ^ crc) & 0xff] ^ (crc >> 8);
        ++data;
    }
    return ~crc;
}

unsigned int Encryption::crc32N(void* data, int len)
{
    unsigned int crc = crc32(0, 0, 0);
    crc = crc32(crc, (const char*)data, (unsigned int)len);
    _NS_PI_2ND_MakeChecksumTo1Byte((char*)&crc);
    return crc;
}

void _NS_PI_2ND_MakeChecksumTo1Byte(char* p)
{
    unsigned char b = (unsigned char)p[0];
    b = (unsigned char)(b ^ (unsigned char)p[3] ^ (unsigned char)p[1] ^
                        (unsigned char)p[2] ^ 0x18U);
    p[0] = (char)b;
}

// ============================================================================
// 种子流（MS LCG）
// ============================================================================

unsigned int Encryption::SeedRand(unsigned int& seed)
{
    unsigned int u1 = seed * 0x41c64e6dU + 0x3039U;
    unsigned int u2 = u1 * 0x41c64e6dU + 0x3039U;
    unsigned int u3 = u2 * 0x41c64e6dU + 0x3039U;
    seed = u3;
    return ((((u1 >> 0x10) & 0x7ff) << 10 ^ (u2 >> 0x10) & 0x3ff) << 10 ^
            (u3 >> 0x10) & 0x3ff);
}

void Encryption::Encrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;
    unsigned char xorByte = (unsigned char)seedVal;

    unsigned char* p = (unsigned char*)data;
    for (int i = 0; i < len; ++i)
    {
        unsigned char b = (unsigned char)(p[i] ^ xorByte);
        // 8 位循环右移 rot（ORIG：sar 的是零扩展字节，等价逻辑右移）
        p[i] = (unsigned char)((b >> rot) | (b << (8 - rot)));
    }
}

void Encryption::Decrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;
    unsigned char xorByte = (unsigned char)seedVal;

    unsigned char* p = (unsigned char*)data;
    for (int i = 0; i < len; ++i)
    {
        unsigned char b = p[i];
        p[i] = (unsigned char)((b << rot) | (b >> (8 - rot))) ^ xorByte;
    }
}

// ============================================================================
// getOriginalBuf（插入置换；index_array 为 ORIG 提取表）
// ============================================================================

void Encryption::getOriginalBuf(char* data, int len, unsigned int& seed)
{
    unsigned int idx = seed % 200;
    char tmp[301];
    for (int i = 0; i < len; ++i)
    {
        int pos = (int)index_array[idx * 300 + i];
        int j = i;
        while (pos < j)
        {
            tmp[j + 1] = tmp[j];
            --j;
        }
        tmp[pos + 1] = data[i];
    }
    std::memcpy(data, tmp + 1, len);
}

// ============================================================================
// DecryptBody（ORIG：mode 恒 5 → G_EncryptTool 解密；其余 XOR 分支为死代码）
// ============================================================================

void Encryption::DecryptBody(unsigned short, void* data, int len, unsigned int& seed)
{
    int mode = 5;
    if (len == 0)
        return;
    if (mode == 4)
    {
        getOriginalBuf((char*)data, len, seed);
        return;
    }
    if (mode == 5)
    {
        // ORIG：G_EncryptTool()->getCryptObject()->Decrypt(data, tmp, len) 后拷回。
        // EncryptTool 属 G2 批次（df_group_2），本批次不实现；此路径在 ORIG 中
        // 亦无活跃调用（DecryptBody 仅被 Encryption::DecryptBody 引用）。
        return;
    }

    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;
    unsigned char xorByte = (unsigned char)seedVal;

    unsigned char* p = (unsigned char*)data;
    for (int i = 0; i < len; ++i)
    {
        unsigned char b = p[i];
        switch (mode)
        {
        case 0:
            b = (unsigned char)(b ^ xorByte);
            b = (unsigned char)((b << rot) | (b >> (8 - rot)));
            break;
        case 1:
            b = (unsigned char)((b >> rot) | (b << (8 - rot)));
            b = (unsigned char)(b ^ xorByte);
            break;
        case 2:
            b = (unsigned char)(b ^ xorByte);
            b = (unsigned char)((b >> rot) | (b << (8 - rot)));
            break;
        case 3:
            b = (unsigned char)((b << rot) | (b >> (8 - rot)));
            b = (unsigned char)(b ^ xorByte);
            break;
        default:
            break;
        }
        p[i] = b;
    }
}
