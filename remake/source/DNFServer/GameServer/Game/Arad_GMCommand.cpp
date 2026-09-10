// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_GMCommand/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// ARAD::GMCommandProcess + ARAD::Arad_GMCommandManger
// ORIG: 0x08198230..0x081982a3, 每个 ORIG 函数均为 0xa 字节纯固定返回
// （逐函数核对 docs/class_func_reports/ARAD__GMCommandProcess/ 与
//   docs/class_func_reports/ARAD__Arad_GMCommandManger/ 全部报告，
//   汇编与 Ghidra 反编译均确认：Init→return 1，Process→return 1，
//   __FindPorcess 与 8 个 Command_* → return 0，Destory 空体）。
//
// 本 TU 为这两类声明的唯一归属点（grep 全 Game 目录无既有声明）。
// MSG_DEBUG_COMMAND / ENUM_DEBUG_COMMAND / CUser 在 ORIG 中未被这些函数实际
// 解引用（Ghidra C 未访问任何字段），此处给出最小声明；若后续
// CUser 为全局作用域类型（ORIG mangled: P5CUser）；函数体未解引用，仅指针透传。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit，
// 使本 TU 发射 `_GLOBAL__I__ZN4ARAD20Arad_GMCommandManger4InitEv`（ORIG 0x81982e4）；
// Init 定义随之前移到本 TU 首个强外部符号位置（键名 = 首强符号）。零语义/布局改动。
#include <iostream>

class CUser;

// --- DEBUG 命令类型（全局作用域，匹配 mangled R17MSG_DEBUG_COMMAND / 18ENUM_DEBUG_COMMAND） ---
struct MSG_DEBUG_COMMAND;
enum ENUM_DEBUG_COMMAND
{
    ENUM_DEBUG_COMMAND_DUMMY = 0 // 占位：ORIG 函数体未消费该枚举值
};

namespace ARAD {

class GMCommandProcess
{
public:
    static int Command_DEBUG_COMMAND_ON(CUser*, MSG_DEBUG_COMMAND&);             // 0x08198254
    static int Command_DEBUG_COMMAND_POWER_WAR_START(CUser*, MSG_DEBUG_COMMAND&); // 0x0819825e
    static int Command_DEBUG_COMMAND_POWER_WAR_END(CUser*, MSG_DEBUG_COMMAND&);  // 0x08198268
    static int Command_DEBUG_COMMAND_USER_REPEL(CUser*, MSG_DEBUG_COMMAND&);     // 0x08198272
    static int Command_DEBUG_COMMAND_REQUEST_MID(CUser*, MSG_DEBUG_COMMAND&);    // 0x0819827c
    static int Command_DEBUG_COMMAND_INVISIBLE(CUser*, MSG_DEBUG_COMMAND&);      // 0x08198286
    static int Command_DEBUG_COMMAND_VISIBLE(CUser*, MSG_DEBUG_COMMAND&);        // 0x08198290
    static int Command_DEBUG_COMMAND_SET_USER_DEFINE(CUser*, MSG_DEBUG_COMMAND&);// 0x0819829a
};

class Arad_GMCommandManger
{
public:
    int Init();                                        // 0x08198230
    void Destory();                                    // 0x0819823a
    int Process(CUser*, MSG_DEBUG_COMMAND&);           // 0x08198240
    int __FindPorcess(ENUM_DEBUG_COMMAND);             // 0x0819824a
};

// [GI-ioinit L21] 本 TU 首个强外部符号定义（_GLOBAL__I_ 键名来源，P327）。
#include "Arad_GMCommand/Init.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_ON.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_POWER_WAR_START.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_POWER_WAR_END.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_USER_REPEL.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_REQUEST_MID.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_INVISIBLE.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_VISIBLE.inc"

#include "Arad_GMCommand/Command_DEBUG_COMMAND_SET_USER_DEFINE.inc"

#include "Arad_GMCommand/Destory.inc"

#include "Arad_GMCommand/Process.inc"

#include "Arad_GMCommand/__FindPorcess.inc"

} // namespace ARAD
