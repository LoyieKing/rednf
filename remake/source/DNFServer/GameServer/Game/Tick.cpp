// Tick.cpp —— 按报告 Tick/*.md 忠实实现。
#include "Tick.h"

#include <ctime>

Tick::Tick() : m_value(0) {}

Tick::Tick(unsigned int v) : m_value(v) {}

// ORIG @0x08ade844（sret）：*sret = Tick(nowAsInternalValue())。
Tick Tick::now()
{
    return Tick(nowAsInternalValue());
}

void Tick::setNow()
{
    m_value = nowAsInternalValue();
}

unsigned int Tick::nowAsInternalValue()
{
    // [b28d-39 判词 · N18a/跨版本] ORIG 0x08ade86a：ORIG 保留截断 64 位乘的
    // 死符号扩展 `mov %eax,%edx; sar $0x1f,%edx`（4.1 未消除），4.4 对
    // `(long long)tv_sec*1000` 的截断结果恒折 32 位 imul（少 2 条）；时钟源/
    // 毫秒换算/magic 除法逐条等价。
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned int)((long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void Tick::setInternalValue(unsigned int v) { m_value = v; }
unsigned int Tick::getInternalValue() const { return m_value; }

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
