// RefPvpGrade 权威实现。布局与语义按 ORIG 反汇编逐条转写（见 RefPvpGrade.h 注释）。
// ORIG 符号（T）：
//   _ZN11RefPvpGradeC1Ev (0x085d46e6) / D1Ev (0x085d470a)
//   _ZN11RefPvpGrade5ClearEv (0x085d4722)
//   _ZN11RefPvpGrade12SetLastGradeEi (0x085d4768)
//   _ZN11RefPvpGrade12GetLastGradeEv / _ZN11RefPvpGrade11SetPvpGradeEiRKNS_15RefPvpGradeDataE
//   _ZNK11RefPvpGrade11GetPvpGradeEi / GetPvpPointEi / GetPvpNextRankPointEi / GetPvpCurrRankPointEi
//   _ZN11RefPvpGrade3setERK15stPvpRefTable_t (0x085d48a0)

#include "RefPvpGrade.h"

// ORIG ctor 0x85d46e6：构造 vector@+0x140（成员自动），随后 Clear()。
RefPvpGrade::RefPvpGrade()
    : m_lastGrade(0),
      m_pvpExpLoss1(0),
      m_pvpExpLoss2(0),
      m_rankRateGrade(0)
{
    Clear();
}

// ORIG dtor 0x85d470a：仅 vector@+0x140 析构（成员自动）。
RefPvpGrade::~RefPvpGrade()
{
}
// ORIG W 0x0844d54c：返回 this+0x134（m_pvpExpLoss1）。
int RefPvpGrade::getPvPExpLossGrade()
{
    return m_pvpExpLoss1;
}

// ORIG W 0x0844d55a：返回 this+0x138（m_pvpExpLoss2）。
int RefPvpGrade::getPvPExpLossExp()
{
    return m_pvpExpLoss2;
}

// ORIG W 0x0844d568：返回 this+0x13c（m_rankRateGrade）。
int RefPvpGrade::getPvPRankRateGrade()
{
    return m_rankRateGrade;
}

// ORIG W 0x0844d576：mov 0x8(%ebp),%eax; add $0x140,%eax; ret
// —— 返回 this+0x140（m_rankRate 对象地址），即 vector 引用。
std::vector<int>& RefPvpGrade::getPvPRankRate()
{
    return m_rankRate;
}

// ORIG Clear 0x85d4722：this+0 = 0；循环 i=0..0x25：this+4+i*8=0、this+8+i*8=0。
void RefPvpGrade::Clear()
{
    m_lastGrade = 0;
    for (int i = 0; i < 0x26; ++i)
    {
        m_data[i].curr = 0;
        m_data[i].next = 0;
    }
}

// ORIG SetLastGrade 0x85d4768：grade<0 || grade>0x26 → 0；否则 this+0=grade，返回 1。
int RefPvpGrade::SetLastGrade(int grade)
{
    if (grade < 0 || grade > 0x26)
    {
        return 0;
    }
    m_lastGrade = grade;
    return 1;
}

// ORIG GetLastGrade 0x85d478e：返回 this+0。
int RefPvpGrade::GetLastGrade() const
{
    return m_lastGrade;
}

// ORIG SetPvpGrade 0x85d4798：grade<0 || grade>0x25 → 0；否则写 data[grade]，返回 1。
int RefPvpGrade::SetPvpGrade(int grade, const RefPvpGradeData& data)
{
    if (grade < 0 || grade > 0x25)
    {
        return 0;
    }
    m_data[grade].curr = data.curr;
    m_data[grade].next = data.next;
    return 1;
}

// ORIG GetPvpGrade 0x85d47ce：idx < this+0xc(=m_data[1].curr) → 0；
// 循环 i=1..<GetLastGrade()：m_data[i].curr < idx && m_data[i].next >= idx → i；否则 -1。
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

// ORIG GetPvpPoint 0x85d4832：idx < 1 → 0；idx >= 0x26 → +0x12c(=m_data[0x25].curr)；
// 否则 this+idx*8+4(=m_data[idx].curr)。
int RefPvpGrade::GetPvpPoint(int idx) const
{
    if (idx <= 0)
    {
        return 0;
    }
    if (idx >= 0x26)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
}

// ORIG GetPvpNextRankPoint 0x85d4860：idx >= 0x26 → +0x130(=m_data[0x25].next)；否则 m_data[idx].next。
int RefPvpGrade::GetPvpNextRankPoint(int idx) const
{
    if (idx >= 0x26)
    {
        return m_data[0x25].next; // +0x130
    }
    return m_data[idx].next;
}

// ORIG GetPvpCurrRankPoint 0x85d4880：idx >= 0x26 → +0x12c(=m_data[0x25].curr)；否则 m_data[idx].curr。
int RefPvpGrade::GetPvpCurrRankPoint(int idx) const
{
    if (idx >= 0x26)
    {
        return m_data[0x25].curr; // +0x12c
    }
    return m_data[idx].curr;
}

// ORIG set 0x85d48a0：
//   this+0x134 = param+0x10 (m_pvpExpLoss1)
//   this+0x138 = param+0x14 (m_pvpExpLoss2)
//   this+0x13c = param+0x18 (m_rankRateGrade)
//   vector@+0x140 operator= param+0x1c (m_rankRate)
void RefPvpGrade::set(const stPvpRefTable_t& refTable)
{
    m_pvpExpLoss1   = refTable.m_pvpExpLoss1;
    m_pvpExpLoss2   = refTable.m_pvpExpLoss2;
    m_rankRateGrade = refTable.m_rankRateGrade;
    m_rankRate      = refTable.m_rankRate;
}
