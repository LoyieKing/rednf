// Tick.cpp —— 按报告 Tick/*.md 忠实实现。
#include "Tick.h"

#include <ctime>

Tick::Tick() : m_value(0) {}

Tick::Tick(unsigned int v) : m_value(v) {}

void Tick::setNow()
{
    m_value = nowAsInternalValue();
}

unsigned int Tick::nowAsInternalValue()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned int)((long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void Tick::setInternalValue(unsigned int v) { m_value = v; }
unsigned int Tick::getInternalValue() const { return m_value; }

Tick& Tick::operator=(const Tick& rhs)
{
    if (this != &rhs)
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
