// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkFinalDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Wave6 IPG 批依赖收口(LinkFinal):全局数值串缓冲与睡眠原语。
//
// ORIG 证据(df_game_r elf32-i386,nm -S + objdump):
//   gNumberToStringBuffer        B 0x200(共 8 槽 × 0x40)
//   _Z14NumberToStringji         T 0x0810904b (0x4d) unsigned int 版:
//       slot = buf + slotIdx*0x40;sprintf(slot,"%u"@0x8b446ea,value);
//       返回 slot(radix 形参未被读取,保留以对齐调用约定)
//   _Z14NumberToStringyi         T 0x08109098 (0x62) unsigned long long 版:
//       同槽位公式;sprintf(slot,"%qu"@0x8b446ed,value);返回 slot
//   _ZN7OS_API5SleepEi           T 0x0858c4e0 (0x19):
//       imul $0x3e8 → usleep(ms*1000);无返回值装载(void 等价)
// 注:ORIG 以 %gs 相对寻址取静态缓冲基址(i386 寻址技巧),重建直取同名数组;
//   "int"/"void" 等非类返回类型不参与 mangled,void 选择与 ORIG 符号严格一致。

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN6OS_API5SleepEi`（ORIG 0x858c890）。
#include <iostream>

#include <cstdio>
#include <unistd.h>

// W7_BuildHarmonize 去重：NumberToString(_Z14NumberToStringji/_Z14NumberToStringyi)
// 与 gNumberToStringBuffer 唯一 owner 为 ServerCommon/DNFFunctionLib.cpp
// （ORIG TLS 缓冲语义），此处定义移除、仅保留声明。
char* NumberToString(unsigned int value, int radix);
char* NumberToString(unsigned long long value, int radix);

namespace OS_API {

// ORIG T 0x0858c4e0:usleep(ms*1000)
#include "LinkFinalDeps/Sleep.inc"
{
    usleep(static_cast<useconds_t>(ms) * 1000u);
}

}  // namespace OS_API
