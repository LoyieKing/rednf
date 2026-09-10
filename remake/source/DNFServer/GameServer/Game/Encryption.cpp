// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Encryption/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Encryption（G7 轻量包加密）还原（2026-08-16）。
// CRC-32 + MS-LCG 种子 + 逐字节旋转/XOR；算法正确即可。

#include "Encryption.h"

#include <cstring>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10Encryption7XorSeedE 的前提，P338）

#include "EncryptTool.h"   // EncryptTool（m_rijndael@+0x204）
#include "CRijndael.h"     // CRijndael::Decrypt

// index_array（ORIG 0x9370f60，.data，200*300 int）：getOriginalBuf 的插入
// 位置表，从 ORIG 二进制提取，60000 个 0..299 值。
#include "index_array.inc"

// ============================================================================
// CRC-32
// ============================================================================

// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10Encryption7XorSeedE ⇒
// XorSeed 须为本 TU 首个强符号（此前 crc_table 在它之前 ⇒ 键名错为
// _ZN10Encryption9crc_tableE）。三者均 BSS/常量初始化（非动态初始化对象），
// 前置定义只改 GI 选名，符号集合与构造序零变化。
unsigned int Encryption::XorSeed = 0;
unsigned int Encryption::crc_table[256];
unsigned int Encryption::ShiftSeed = 0;

// [b28d-solo-Encryption] P254 扩展：ORIG 对 data[i] 发
// `movzbl (%eax),%eax; mov %eax,%ecx`（4.1 两段中转），unsigned char 直载恒折单段
// ⇒ 用 1 字节 fixed-underlying enum 视图（仅函数体内视图，不入任何签名/mangled）。
enum EncryptionByte : unsigned char { EB_ZERO = 0 };

#include "Encryption/make_crc_table.inc"
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

#include "Encryption/crc32.inc"
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

#include "Encryption/crc32N.inc"
{
    unsigned int crc = crc32(0, 0, 0);
    crc = crc32(crc, (const char*)data, (unsigned int)len);
    _NS_PI_2ND_MakeChecksumTo1Byte((char*)&crc);
    return crc;
}

#include "Encryption/_NS_PI_2ND_MakeChecksumTo1Byte.inc"
{
    // [round5-A26 判词·N18a] ORIG 0x0808cc17 形：p0 先装载入 %edx 并在**末尾**
    // `xor %eax,%edx` 合流，右链 ((p1^p2)^p3)^0x18 落 %eax（4.1 先 expand op0 再 op1，
    // 且常量 xor 在链尾）。本轮三形实测（21/21 条，仅寄存器镜像）：
    // ①`p1^p2^p3^0x18^p0`：链落 %ecx、p0 落 %edx、常量被 4.4 提链尾；
    // ②`p0 ^= (p1^p2^p3^0x18)`（复合赋值形）：23 条，4.4 把 p0 保活入 %ebx（多 push/pop）；
    // ③本形（p0 前置 5 项平铺）：链落 %edx、p0 落 %ecx —— 与①互为镜像，同 21 条。
    // ⇒ 4.4 恒把 5 项链尾项/常量做链尾，与 4.1 的「p0 最外层」树形不可同构，封顶。
    ((unsigned char*)p)[0] =
        (unsigned char)(((unsigned char*)p)[0] ^ ((unsigned char*)p)[1] ^
                        ((unsigned char*)p)[2] ^ ((unsigned char*)p)[3] ^ 0x18U);
}

// ============================================================================
// 种子流（MS LCG）
// ============================================================================

#include "Encryption/SeedRand.inc"
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

#include "Encryption/Encrypt.inc"
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

#include "Encryption/Decrypt.inc"
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

#include "Encryption/getOriginalBuf.inc"
{
    // [round5-Encryption] ORIG 0x0848dc4c 全函数**统一以 -0x144 为数组基址**：
    // 写 `-0x144(%ebp,%eax,1)`(eax=j)、读 `-0x144(%ebp,%edx,1)`(edx=j-1)、
    // 末尾 `-0x144(%ebp,%eax,1)`(eax=pos)、memcpy 源 `lea -0x144(%ebp)` 单条。
    // ⇒ 原码数组为 300 字节、有效数据自 index0 起（`tmp[j]=tmp[j-1]` /
    // `tmp[pos]=data[i]` / `memcpy(data,tmp,len)`）；此前写成「301 字节 +
    // 全体下标 +1」使 4.4 对每个 +1 发 lea/add 且 disp 停在 -0x145（gap=-1）。
    // 槽序 pos=-0x18/idx=-0x14/i=-0x10/j=-0xc；声明序照 ORIG（深→浅）。
    int pos;
    int idx = seed % 200;
    char tmp[300];
    int i;
    int j;
    for (i = 0; i < len; ++i)
    {
        // [round5-A26 判词·N（4.4 mem 直用族）] ORIG 0x0848dc8c 为
        // `mov -0x14,%edx; mov -0x10,%eax; imul $0x12c,%edx,%edx;
        // lea (%edx,%eax,1),%eax`（**两操作数均入寄存器**后 lea 合并，4 条）；
        // 4.4 恒折 `mov idx,%eax; imul $0x12c,%eax,%eax; add -0x10(%ebp),%eax`
        // （mem 直用，3 条，gap=-1）。本轮穷举 6 形均不产 ORIG 形：
        // ①`idx*300+i`（本形，56 条）②`i+idx*300`（同 56）③`300*idx+i`（同 56）
        // ④`*(index_array + idx*300 + i)` 数组名指针算术（60 条，劣）
        // ⑤`register int i`（57 条但整帧改观：i 失槽 + 数组 disp -0x140，语义等价性
        //   尚可，但改变了 ORIG 的 i 槽形 ⇒ 不用）⑥`unsigned int idx`（59 条，劣）。
        // ⇒ 4.4 对「寄存器+内存」加法恒 mem 直用（P209 附同族），源形不可择，封顶。
        pos = (int)index_array[idx * 300 + i];
        j = i;
        while (pos < j)
        {
            tmp[j] = tmp[j - 1];
            --j;
        }
        tmp[pos] = data[i];
    }
    std::memcpy(data, tmp, len);
}

// ============================================================================
// DecryptBody（ORIG：mode 恒 5 → G_EncryptTool 解密；其余 XOR 分支为死代码）
// ============================================================================

#include "Encryption/DecryptBody.inc"
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
