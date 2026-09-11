// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 md5/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ MD5 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 算法正确即可，不逐指令对齐。
//
// [Md5RestLane·2026-09-11 二批] 按 ORIG 结构补齐 `_ZL11md5_processP11md5_contextPh`
// （原我方把压缩宏内联进 md5_update）：update/finish/process 三件现与 ORIG 同构
// —— total 为**字节计数**（total[0] += len；finish 才 <<3 出 bit 长度）、
// update 每 64B 调 md5_process、finish 用 rodata `md5_padding` 并逐字节展开写 out。
// 同时删除我方发明符号 `_ZL8md5_rotlji`（ORIG 无；旋转改由 rotate 表达式内联）。
#include "md5.h"
#include <cstring>
#include <cstdio>

#include "md5/md5_starts.inc"


#include "md5/md5_process.inc"


#include "md5/md5_update.inc"


// ============================================================================
// md5_padding —— ORIG r @0x08b1f0e0，size 0x40（.rodata 逐字节 80 00 … 00 = 0x80 + 63×0）。
// md5_finish 0x080b3c25 以立即数 $0x8b1f0e0 作为 md5_update 的第二实参。
// ============================================================================
// 注：C++ namespace-scope `const` 隐式内部链接 ⇒ 符号名 = 裸名 + local(r)；
// 加 `static` 会变 `_ZL11md5_padding`（与 ORIG 名不符），故不写 static。
const unsigned char md5_padding[0x40] = {0x80};

#include "md5/md5_finish.inc"


#include "md5/md5.inc"


#include "md5/md5_merging.inc"


#include "md5/md5_file.inc"


#include "md5/md5_hmac_starts.inc"


#include "md5/md5_hmac_update.inc"


#include "md5/md5_hmac_finish.inc"


#include "md5/md5_hmac.inc"


// ============================================================================
// md5_self_test 用的 rodata 表（ORIG r 符号：md5_test_str @0x08b1f120 size 0x237、
// md5_test_sum @0x08b1f360 size 0x70；同族 md5_padding @0x08b1f0e0 见上文
// md5_finish 链，已按 ORIG 定义补齐）。
//
// 布局证据：md5_self_test 取串地址段 0x080b41c7 的索引步长 = i*81
// （shl$3 → add i → lea(,%eax,8) → add i*9 ⇒ i*9+i*72），且 0x237 = 7*81；
// 摘要表步长 = i*16（`shl $0x4`），0x70 = 7*16。最长向量为 80 字符 + NUL = 81。
// 表内容逐字节取自 ORIG .rodata（objdump -s -j .rodata 0x08b1f120..0x08b1f3d0）。
// ============================================================================
const char md5_test_str[7][0x51] =
{
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

const unsigned char md5_test_sum[7][16] =
{
    { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e },
    { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 },
    { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 },
    { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b },
    { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f },
    { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a }
};

#include "md5/md5_self_test.inc"

