// 迁移自 GameStubs.cpp RefPvpGrade 桩（batch-6）。
// 访问器语义按 ORIG 反汇编逐条转写（见 RefPvpGrade.h 布局注释）。
// 符号（T）：GetLastGrade/GetPvpCurrRankPoint/GetPvpNextRankPoint/GetPvpGrade/GetPvpPoint。

#include "RefPvpGrade.h"

int RefPvpGrade::GetLastGrade() const
{
    return m_lastGrade;
}

int RefPvpGrade::GetPvpCurrRankPoint(int idx) const
{
    if (idx > 0x25)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
}

int RefPvpGrade::GetPvpNextRankPoint(int idx) const
{
    if (idx > 0x25)
    {
        return m_data[0x25].next; // +0x130
    }
    return m_data[idx].next;
}

int RefPvpGrade::GetPvpPoint(int idx) const
{
    if (idx <= 0)
    {
        return 0;
    }
    if (idx > 0x25)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
}

int RefPvpGrade::GetPvpGrade(int idx) const
{
    if (m_data[1].curr > idx)
    {
        return 0;
    }
    for (int i = 1; i < GetLastGrade(); ++i)
    {
        if (m_data[i].curr < idx && m_data[i].next >= idx)
        {
            return i;
        }
    }
    return -1;
}