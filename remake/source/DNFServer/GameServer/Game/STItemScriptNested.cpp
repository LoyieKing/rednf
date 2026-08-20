// df_game_r Game/ STItemScript 嵌套结构实现（独立 TU，从 GameStubs.cpp 拆出）。
// 包含：DyeInfo / STSpecialMonsterDrop / STItemScript::SEXPERTJOB。
// 声明在 STItemScript.h。
#include "STItemScript.h"

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

// ---- STSpecialMonsterDrop（ORIG 0x8513a10 族）----
STSpecialMonsterDrop::STSpecialMonsterDrop() {}
void STSpecialMonsterDrop::clear() {}

// ---- STItemScript::SEXPERTJOB（ORIG 0x89c36aa 族）----
STItemScript::SEXPERTJOB::SEXPERTJOB() {}
STItemScript::SEXPERTJOB::~SEXPERTJOB() {}

