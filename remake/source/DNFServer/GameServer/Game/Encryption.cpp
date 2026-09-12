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

// ============================================================================
// CRC-32
// ============================================================================

// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10Encryption7XorSeedE ⇒
// XorSeed 须为本 TU 首个强符号（此前 crc_table 在它之前 ⇒ 键名错为
// _ZN10Encryption9crc_tableE）。三者均 BSS/常量初始化（非动态初始化对象），
// 前置定义只改 GI 选名，符号集合与构造序零变化。
unsigned char Encryption::XorSeed = 0;   // ORIG B 0x0943d560 size 1
unsigned int Encryption::ShiftSeed = 0;  // ORIG B 0x0943d564（= XorSeed+3 对齐填充后，紧随）
// [R21scanEC §27.3 顺序轴] ORIG nm -S 相对偏移：XorSeed@0(1) → ShiftSeed@+4(4) →
// crc_table@+0x20(0x400)（0x943d560/0x943d564/0x943d580）⇒ crc_table 定义须后置；
// XorSeed 仍是本 TU 首个强符号 ⇒ P327 GI 键名 `_GLOBAL__I__ZN10Encryption7XorSeedE` 不变。
unsigned int Encryption::crc_table[256];

#include "index_array.inc"

// [b28d-solo-Encryption] P254 扩展：ORIG 对 data[i] 发
// `movzbl (%eax),%eax; mov %eax,%ecx`（4.1 两段中转），unsigned char 直载恒折单段
// ⇒ 用 1 字节 fixed-underlying enum 视图（仅函数体内视图，不入任何签名/mangled）。
enum EncryptionByte : unsigned char { EB_ZERO = 0 };

#include "Encryption/make_crc_table.inc"


#include "Encryption/crc32.inc"


#include "Encryption/crc32N.inc"


#include "Encryption/_NS_PI_2ND_MakeChecksumTo1Byte.inc"


// ============================================================================
// 种子流（MS LCG）
// ============================================================================

#include "Encryption/SeedRand.inc"


#include "Encryption/Encrypt.inc"


#include "Encryption/Decrypt.inc"


// ============================================================================
// getOriginalBuf（插入置换；index_array 为 ORIG 提取表）
// ============================================================================

#include "Encryption/getOriginalBuf.inc"


// ============================================================================
// DecryptBody（ORIG：mode 恒 5 → G_EncryptTool 解密；其余 XOR 分支为死代码）
// ============================================================================

#include "Encryption/DecryptBody.inc"

