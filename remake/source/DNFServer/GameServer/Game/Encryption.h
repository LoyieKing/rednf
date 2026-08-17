#ifndef GAME_ENCRYPTION_H_
#define GAME_ENCRYPTION_H_

// df_game_r Encryption（G7 轻量包加密：CRC32 + 种子 XOR/旋转流，2026-08-16）。
// 依据 docs/class_func_reports/Encryption.md 与 ORIG 二进制：
//   - crc32/crc32N/make_crc_table：标准 CRC-32（反射多项式 0x4db89129）；
//   - SeedRand：MS 风格 LCG（0x41c64e6d + 0x3039）三次迭代组合；
//   - Encrypt/Decrypt：逐字节 8 位旋转 + 种子低字节 XOR（默认种子 0x04453eb5）。

class Encryption
{
public:
    static void make_crc_table();
    static unsigned int crc32(unsigned int seed, const char* data, unsigned int len);
    static unsigned int crc32N(void* data, int len);
    static unsigned int SeedRand(unsigned int& seed);
    static void Encrypt(void* data, int len, unsigned int& seed);
    static void Decrypt(void* data, int len, unsigned int& seed);
    static void getOriginalBuf(char* data, int len, unsigned int& seed);
    static void DecryptBody(unsigned short type, void* data, int len, unsigned int& seed);

private:
    static unsigned int crc_table[256];
    static unsigned int XorSeed;     // 0x0943d560（ORIG BSS）
    static unsigned int ShiftSeed;   // 0x0943d564（ORIG BSS）
};

// 全局辅助（ORIG @0x808cc17，crc32N 使用）
void _NS_PI_2ND_MakeChecksumTo1Byte(char* p);

#endif  // GAME_ENCRYPTION_H_
