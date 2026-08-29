// df_game_r Game/ STItemScript 嵌套结构实现（独立 TU，从 GameStubs.cpp 拆出）。
// 包含：DyeInfo / STSpecialMonsterDrop / STItemScript::SEXPERTJOB。
// 声明在 STItemScript.h。
#include "STItemScript.h"
#include <string.h>

// ---- DyeInfo（ORIG 0x8513ac6 族）----
DyeInfo::DyeInfo() : m_vec4() { clear(); }
DyeInfo::~DyeInfo() {}

void DyeInfo::clear()
{
    m_field0 = 1;
    m_vec4.clear();
}

DyeInfo& DyeInfo::operator=(const DyeInfo& other)
{
    m_field0 = other.m_field0;
    m_vec4 = other.m_vec4;
    return *this;
}

// ORIG 0x898d37c：m_field0==1 且 m_vec4 非空 → m_vec4[0].first，否则 -1
int DyeInfo::getDyeColorIndex()
{
    if (m_field0 == 1)
    {
        if (m_vec4.empty())
            return -1;
        return m_vec4[0].first;
    }
    return -1;
}

// ---- STSpecialMonsterDrop（ORIG 0x8513a10 族）----
STSpecialMonsterDrop::STSpecialMonsterDrop()
{
    clear();   // ORIG C1 0x8513a10：仅调 clear
}
void STSpecialMonsterDrop::clear()
{
    // ORIG 0x8513a24：m_enable 字节直写 0 + memset(+0x01,0,5) + memset(+0x06,0,3)。
    m_enable = 0;
    memset(m_difficulty, 0, sizeof(m_difficulty));
    memset(m_dungeonType, 0, sizeof(m_dungeonType));
}

// ---- STItemScript::SEXPERTJOB（ORIG 0x89c36aa 族）----
STItemScript::SEXPERTJOB::SEXPERTJOB() {}
STItemScript::SEXPERTJOB::~SEXPERTJOB() {}

