// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Tick/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Tick.cpp —— 按报告 Tick/*.md 忠实实现。
#include "Tick.h"

#include <ctime>

#include "Tick/Tick.inc"

#include "Tick/Tick_1.inc"

#include "Tick/now.inc"


#include "Tick/setNow.inc"


#include "Tick/nowAsInternalValue.inc"


#include "Tick/setInternalValue.inc"
#include "Tick/getInternalValue.inc"

// operator=：ORIG @0x08ade8e0（_ZN4TickaSES_ 按值参）：value_ = rhs.value_，返回 *this。
Tick& Tick::operator=(Tick rhs)
{
    m_value = rhs.m_value;
    return *this;
}

Tick& Tick::operator+=(int span) { m_value += span; return *this; }
Tick& Tick::operator-=(int span) { m_value -= span; return *this; }
Tick Tick::operator+(int span) const { return Tick(m_value + span); }
Tick Tick::operator-(int span) const { return Tick(m_value - span); }

// operator-(Tick)：ORIG @0x8ade97a 返回 int32 差值。
int Tick::operator-(const Tick rhs) const
{
    return this->m_value - rhs.m_value;
}

bool Tick::operator==(const Tick rhs) const { return m_value == rhs.m_value; }
bool Tick::operator!=(const Tick rhs) const { return m_value != rhs.m_value; }

// 比较运算符按 ORIG 差值技巧（signed 差比较）：
bool Tick::operator<(const Tick rhs) const
{
    return ((unsigned int)(m_value - rhs.m_value)) >> 31;
}

bool Tick::operator>=(const Tick rhs) const
{
    return (((unsigned int)~(m_value - rhs.m_value))) >> 31;
}

bool Tick::operator<=(const Tick rhs) const
{
    return (int)(m_value - rhs.m_value) <= 0;   // ORIG: sub+test+setle
}

bool Tick::operator>(const Tick rhs) const
{
    return (int)(m_value - rhs.m_value) > 0;    // ORIG: sub+test+setg
}
