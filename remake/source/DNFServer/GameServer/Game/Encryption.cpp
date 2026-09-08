// df_game_r Encryption（G7 轻量包加密）还原（2026-08-16）。
// CRC-32 + MS-LCG 种子 + 逐字节旋转/XOR；算法正确即可。

#include "Encryption.h"

#include <cstring>

#include "EncryptTool.h"   // EncryptTool（m_rijndael@+0x204）
#include "CRijndael.h"     // CRijndael::Decrypt

// index_array（ORIG 0x9370f60，.data，200*300 int）：getOriginalBuf 的插入
// 位置表，从 ORIG 二进制提取，60000 个 0..299 值。
#include "index_array.inc"

// ============================================================================
// CRC-32
// ============================================================================

unsigned int Encryption::crc_table[256];
unsigned int Encryption::XorSeed = 0;
unsigned int Encryption::ShiftSeed = 0;

// [b28d-solo-Encryption] P254 扩展：ORIG 对 data[i] 发
// `movzbl (%eax),%eax; mov %eax,%ecx`（4.1 两段中转），unsigned char 直载恒折单段
// ⇒ 用 1 字节 fixed-underlying enum 视图（仅函数体内视图，不入任何签名/mangled）。
enum EncryptionByte : unsigned char { EB_ZERO = 0 };

void Encryption::make_crc_table()
{
    // [b28d-solo-Encryption] verdict=AE 42=42。
    // ORIG 0x0848d3e4 槽序 crc=-0x4/j=-0x8/bit=-0xc：
    // 4.4 按**声明序深→浅**分配（与 4.1 相反），故声明序须倒排为
    // bit→j→crc，语句序仍保持 crc=1、crc_table[0]=0、bit=0x80（照 ORIG）。
    unsigned int bit;
    unsigned int j;
    unsigned int crc = 1;
    crc_table[0] = 0;
    bit = 0x80;
    for (; bit != 0; bit >>= 1)
    {
        // ORIG：crc>>1 先落 %edx、三元值后落 %eax（`xor %edx,%eax`）且无 v 槽（P0）
        // ⇒ 三元内联且 crc>>1 置左（4.4 先算左操作数）。
        crc = (crc >> 1) ^ ((crc & 1) ? 0x4db89129U : 0);
        for (j = 0; j < 0x100; j += bit * 2)
        {
            crc_table[bit + j] = crc_table[j] ^ crc;
        }
    }
}

unsigned int Encryption::crc32(unsigned int seed, const char* data, unsigned int len)
{
    // [b28d-solo-Encryption] verdict=AE 29=29。
    // ORIG 0x0848d472：仅查 crc_table[255]（&crc_table+0x3fc）；
    // 累加器直接用形参 seed 槽（notl 0x8(%ebp)/mov %eax,0x8(%ebp)，无 crc 局部）；
    // 索引用 (signed char)*data（movzbl+movsbl 两段）。
    if (crc_table[255] == 0)
        make_crc_table();
    seed = ~seed;
    while (len-- != 0)
    {
        // ORIG 先算 seed>>8 到 %edx 再算索引（`xor %edx,%eax`）⇒ 左操作数放 seed>>8。
        seed = (seed >> 8) ^ crc_table[((signed char)*data ^ seed) & 0xff];
        ++data;
    }
    return ~seed;
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
    // [b28d-solo-Encryption] verdict=DIFF 21=21，N 封顶（N18a 寄存器分配）。
    // ORIG 0x0808cc17 无帧（无 sub esp）、无具名槽：全寄存器
    // 累积（edx=p[0], ecx=p[1]^p[2], eax=p[3]^ecx^0x18, edx^=eax）⇒ P0 下
    // 源码是单一赋值表达式（原两句式引入 unsigned char b 栈槽 -0x1）。
    // 4.4 版：p0 置链尾后 4.4 产出 21 条（链落 %ecx、p0 落 %edx），
    // ORIG 链落 %eax、p0 直载 %edx 属 4.1 寄存器分配（N18a 族）封顶。
    ((unsigned char*)p)[0] = (unsigned char)(((unsigned char*)p)[1] ^
                                             ((unsigned char*)p)[2] ^
                                             ((unsigned char*)p)[3] ^ 0x18U ^
                                             ((unsigned char*)p)[0]);
}

// ============================================================================
// 种子流（MS LCG）
// ============================================================================

unsigned int Encryption::SeedRand(unsigned int& seed)
{
    // [b28d-solo-Encryption] verdict=IDENTICAL 44=44。
    // ORIG 0x0848d51a：单累加器槽 -0x8 被 u1/u2/u3 复用
    // （imul→mov 槽→addl 槽，三次），结果累加器 -0x4 逐段 <<10/^=；
    // 原三独立局部写法生成 3 槽 + 不同求值序 ⇒ 按 ORIG 逐句还原。
    // r 声明 int（与 RHS 的 unsigned 异型，P211/P254）才出 ORIG 的
    // `mov %eax,%edx` 中转 + `xor %edx,%eax` 寄存器形（unsigned r 恒折内存 RMW）。
    unsigned int u = seed;
    u *= 0x41c64e6dU;
    u += 0x3039U;
    int r = (u >> 0x10) & 0x7ff;
    u *= 0x41c64e6dU;
    u += 0x3039U;
    r <<= 10;
    r = r ^ ((u >> 0x10) & 0x3ff);
    u *= 0x41c64e6dU;
    u += 0x3039U;
    r <<= 10;
    r = r ^ ((u >> 0x10) & 0x3ff);
    seed = u;
    return r;
}

void Encryption::Encrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    // [b28d-solo-Encryption] verdict=AE 83=83（.o 重定位标签伪差）。
    // ORIG 0x0848da70：无 p/xorByte/b 具名局部，逐次重算 data+i（三次 lea），
    // 首条用整字 seedVal 异或后 mov %al 存回；data[i] 经 EncryptionByte
    // 1 字节 enum 视图读，复现 ORIG `movzbl (%eax),%eax; mov %eax,%ecx`
    // 两段中转（unsigned char 直载恒折单段，P254 扩展）。
    for (i = 0; i < len; ++i)
    {
        ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        ((EncryptionByte*)data)[i] =
            (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                             (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
    }
}

void Encryption::Decrypt(void* data, int len, unsigned int& seed)
{
    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    // [b28d-solo-Encryption] verdict=AE 83=83。ORIG 0x0848db5e：与 Encrypt 镜像，
    // 先旋转后 XOR；同样用 EncryptionByte 视图复现两段装载中转。
    for (i = 0; i < len; ++i)
    {
        ((EncryptionByte*)data)[i] =
            (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                             (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
        ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
    }
}

// ============================================================================
// getOriginalBuf（插入置换；index_array 为 ORIG 提取表）
// ============================================================================

void Encryption::getOriginalBuf(char* data, int len, unsigned int& seed)
{
    // [b28d-solo-Encryption] verdict=DIFF 58/57，N 封顶（N4/P30/N102 + 4.4 SAVE_EXPR）。
    // ORIG 0x0848dc4c 槽序 pos=-0x18/idx=-0x14/i=-0x10/j=-0xc：
    // 4.4 按声明序深→浅分配，故声明倒排为 pos→idx→i→j（语句序照 ORIG）。
    // 槽序 + idx 类型（int 消 `seed%200` 商临时槽）已全对齐；残余 1 条为
    // `idx*300+i`：ORIG `mov idx,%edx; imul; mov i,%eax; lea (%edx,%eax,1)`
    // vs 4.4 恒折 `imul %eax; add -0x10(%ebp),%eax`（P30/N102 源形不可择），
    // 另 `tmp[j+1]`/`tmp[pos+1]`/`memcpy(tmp+1)` 的 ORIG disp 折叠
    // `-0x144(%ebp,%reg,1)` 4.4 恒 lea+add（P30/N4；两形互试同劣）。
    int pos;
    int idx = seed % 200;
    char tmp[301];
    int i;
    int j;
    for (i = 0; i < len; ++i)
    {
        pos = (int)index_array[idx * 300 + i];
        j = i;
        while (pos < j)
        {
            tmp[j + 1] = tmp[j];
            --j;
        }
        tmp[pos + 1] = data[i];
    }
    std::memcpy(data, &tmp[1], len);
}

// ============================================================================
// DecryptBody（ORIG：mode 恒 5 → G_EncryptTool 解密；其余 XOR 分支为死代码）
// ============================================================================

void Encryption::DecryptBody(unsigned short, void* data, int len, unsigned int& seed)
{
    // [b28d-solo-Encryption] verdict=NEAR 293=293（助记符序列全等；残余为
    // N7 间接 call 目标寄存器 %ebx(ORIG) vs %edx(4.4.7) + 参数装载寄存器）。
    // ORIG 0x0848dd0c 逐块还原：
    // ① len==0 直返；② mode 先 1 后 5（原码遗留双写，P171 照抄）；
    // ③ switch(mode) 每 case 独立 for 循环（非循环内 switch）；i 槽 -0x1c 复用；
    // ④ 无 p/xorByte/b 具名局部，全部 data+i 重算；⑤ tmp 0x578（基址 -0x59d）。
    if (len == 0)
        return;
    int mode = 1;
    mode = 5;
    if (mode == 4)
    {
        getOriginalBuf((char*)data, len, seed);
        return;
    }
    if (mode == 5)
    {
        // ORIG 0x848dd5e：G_EncryptTool()->m_rijndael(+0x204) 虚调用
        // Decrypt(data, tmp, len) 后 memcpy 拷回（加密类，算法正确即可）。
        extern EncryptTool* G_EncryptTool();
        char tmp[0x578];
        G_EncryptTool()->m_rijndael->Decrypt((const char*)data, tmp, len);
        std::memcpy(data, tmp, len);
        return;
    }

    unsigned int seedVal = 0x04453eb5U;
    if (seed != 0)
        seedVal = SeedRand(seed);
    int i = 0;
    unsigned char rot;
    std::memcpy(&rot, ((char*)&seedVal) + 1, 1);
    rot &= 7;

    switch (mode)
    {
    case 0:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
        }
        return;
    case 1:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        }
        return;
    case 2:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) >> rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) << (8 - rot)));
        }
        return;
    case 3:
        for (i = 0; i < len; ++i)
        {
            ((EncryptionByte*)data)[i] =
                (EncryptionByte)((((unsigned char)((EncryptionByte*)data)[i]) << rot) |
                                 (((unsigned char)((EncryptionByte*)data)[i]) >> (8 - rot)));
            ((EncryptionByte*)data)[i] = (EncryptionByte)(seedVal ^ ((EncryptionByte*)data)[i]);
        }
        return;
    default:
        return;
    }
}
