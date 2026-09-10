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
int Arad_GMCommandManger::Init()
{
    return 1; // ORIG 0x8198230: mov $0x1,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_ON(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x8198254: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_POWER_WAR_START(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x819825e: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_POWER_WAR_END(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x8198268: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_USER_REPEL(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x8198272: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_REQUEST_MID(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x819827c: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_INVISIBLE(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x8198286: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_VISIBLE(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x8198290: mov $0x0,%eax
}

int GMCommandProcess::Command_DEBUG_COMMAND_SET_USER_DEFINE(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 0; // ORIG 0x819829a: mov $0x0,%eax
}

void Arad_GMCommandManger::Destory()
{
    // ORIG 0x819823a: push %ebp; mov %esp,%ebp; pop %ebp; ret —— 真实空体
}

int Arad_GMCommandManger::Process(CUser* /*user*/, MSG_DEBUG_COMMAND& /*msg*/)
{
    return 1; // ORIG 0x8198240: mov $0x1,%eax
}

int Arad_GMCommandManger::__FindPorcess(ENUM_DEBUG_COMMAND /*command*/)
{
    return 0; // ORIG 0x819824a: mov $0x0,%eax
}

} // namespace ARAD
