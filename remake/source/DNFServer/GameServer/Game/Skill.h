// Skill：CDungeonJobNode 的 map<int,Skill> 值类型（0x10 字节，IData EBO）。
// 布局证据：Skill::Reset W 0x084868b8（清 +0/+4/+8/+c 四 int）、
// Skill::operator+= W 0x08486902（+0/+4/+8 相加、+0xc 自增）、
// ctor W 0x084868e4 = IData::C1 + Reset、dtor W 0x08486950（trivial）。
// SQL 列（CDungeonJobNode::InsertDB 0x08485e00）：skill_count=+0、skill_cool_time=+4、
// skill_use=+8；+0xc != 0 才写 DB（有数据标志）。
#ifndef DNF_GAME_SKILL_H_
#define DNF_GAME_SKILL_H_

#include "IData.h"

struct Skill : public IData
{
public:
    Skill() : IData()     // ORIG W 0x084868e4
    {
        Reset();
    }
    ~Skill() {}           // ORIG W 0x08486950（trivial）

    void Reset()          // ORIG W 0x084868b8
    {
        m_skillCount = 0;
        m_coolTime = 0;
        m_skillUse = 0;
        m_useCount = 0;
    }
    // ORIG W 0x08486902 尾无 `mov 0x8(%ebp),%eax`（不 reload this 作 return）⇒ 返回 void
    // （Skill& 版尾多 1 条死 mov，T-CU5 定案；调用点均语句形不消费返回值）。
    void operator+=(const Skill& other)
    {
        m_skillCount += other.m_skillCount;
        m_coolTime += other.m_coolTime;
        m_skillUse += other.m_skillUse;
        ++m_useCount;
    }

    int m_skillCount;   // +0x00
    int m_coolTime;     // +0x04
    int m_skillUse;     // +0x08
    int m_useCount;     // +0x0c
};
// sizeof 0x10

#endif  // DNF_GAME_SKILL_H_
