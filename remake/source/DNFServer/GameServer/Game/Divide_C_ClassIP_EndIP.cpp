// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Divide_C_ClassIP_EndIP/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Divide_C_ClassIP_EndIP 实现（W7 链接收敛）。
// 忠实转写 objdump ORIG T @0x086b1dd5 / @0x086b1e85（无独立 md 报告，
// 反汇编见 /tmp/df_batches/W7_linkfix_W7E.md 证据节）。

#include "Divide_C_ClassIP_EndIP.h"

#include <string.h>

// [b28d-solo-Divide_C_ClassIP_EndIP · P306 族/跨编译环境] ORIG 的 libstdc++ 在 gcc 4.1
// 环境（glibc string.h 未定义 __CORRECT_ISO_CPP_STRING_H_PROTO）以 inline 形式发射
// std::strrchr(char*, int)（ORIG nm：W 0x08265244，体 = 转发 strrchr@plt，14 条指令）；
// 本仓 4.4 头因 __GNUC_PREREQ(4,4) 走 glibc extern "C++" 重载（asm 名 strrchr），
// std::strrchr 恒解析为 C 符号 ⇒ 调用点与 ORIG 差一个 _ZSt7strrchrPci。
// 按 ORIG 同一 inline 体在此声明（非发明符号：符号/体/弱绑定均见 ORIG 二进制）。
#if decompile_verify
namespace std {
inline char* strrchr(char* s, int c)
{
    return __builtin_strrchr(s, c);
}
}
#endif  /* 现代 libstdc++ 自带 std::strrchr(char*, int) 重载 */

#include "Divide_C_ClassIP_EndIP/Divide_C_ClassIP_EndIP.inc"


#include "Divide_C_ClassIP_EndIP/Divide_C_ClassIP_EndIP_1.inc"
