#include "ActiveStaticInfo.h"
#include <cstring>

// ActiveStaticInfo 实现（ORIG：ctor 0x8150ec6 / dtor 0x8150ef6 /
// clear 0x81509aa / getHpMax 0x85bedb0 / getMpMax 0x88ba4be；
// 实例计数与累计尺寸为 unmangled 全局符号）
int g_ActiveStaticInfoCount asm("g_ActiveStaticInfoCount") = 0;
int g_ActiveStaticInfoSize asm("g_ActiveStaticInfoSize") = 0;

ActiveStaticInfo::ActiveStaticInfo()
{
    ++g_ActiveStaticInfoCount;
    g_ActiveStaticInfoSize = g_ActiveStaticInfoCount * 0x2f0;
    clear();
}

ActiveStaticInfo::~ActiveStaticInfo()
{
    --g_ActiveStaticInfoCount;
    g_ActiveStaticInfoSize = g_ActiveStaticInfoCount * 0x2f0;
}

void ActiveStaticInfo::clear()
{
    m_level = 0;
    m_hpBase = 0;
    m_hpRatio = 1.0f;
    m_mpBase = 0;
    m_mpRatio = 1.0f;
    m_flag14 = 1;
    m_attack = 0;
    m_attackRatio = 1.0f;
    m_magicAttack = 0;
    m_magicAttackRatio = 1.0f;
    m_defense = 0;
    m_defenseRatio = 1.0f;
    m_magicDefense = 0;
    m_magicDefenseRatio = 1.0f;
    for (int i = 0; i < 4; ++i)
    {
        m_elemAttack[i] = 0;
        m_elemAttackRatio[i] = 1.0f;
        m_elemDefense[i] = 0;
        m_elemDefenseRatio[i] = 1.0f;
    }
    for (int i = 0; i < 17; ++i)
    {
        m_attr17Base[i] = 0;
        m_attr17Ratio[i] = 1.0f;
    }
    m_statBase0 = 0;
    m_statRatio0 = 1.0f;
    m_statBase1 = 0;
    m_statRatio1 = 1.0f;
    m_statBase2 = 0;
    m_statRatio2 = 1.0f;
    m_statBase3 = 1000;
    m_statRatio3 = 1.0f;
    m_statBase4 = 1000;
    m_statRatio4 = 1.0f;
    m_statBase5 = 1000;
    m_statRatio5 = 1.0f;
    m_statBase6 = 0;
    m_statRatio6 = 1.0f;
    m_statBase7 = 0;
    m_statRatio7 = 1.0f;
    m_statBase8 = 0;
    m_statRatio8 = 1.0f;
    m_statBase9 = 100;
    m_statRatio9 = 1.0f;
    m_statBase10 = 0;
    m_statRatio10 = 1.0f;
    m_statBase11 = 0;
    m_statRatio11 = 1.0f;
    m_statBase12 = 0;
    m_statRatio12 = 1.0f;
    m_statBase13 = 0;
    m_statRatio13 = 1.0f;
    m_statBase14 = 0;
    m_statRatio14 = 1.0f;
    m_statBase15 = 0;
    m_statBase16 = 0;
    m_statBase17 = 0;
    m_statBase18 = 0;
    m_statBase19 = 0;
    m_statBase20 = 0;
    m_statBase21 = 0;
    m_statBase22 = 0;
    m_statBase23 = 0;
    m_statBase24 = 0;
    m_statBase25 = 0;
    m_statBase26 = 0;
    m_statBase27 = 0;
    m_statBase28 = 0;
    m_statBase29 = 0;
    m_statBase30 = 0;
    m_statRatio15 = 1.0f;
    m_statBase31 = 0;
    m_statBase32 = 0;
    m_statBase33 = 0;
    m_statBase34 = 0;
    m_statRatio16 = 1.0f;
    m_statBase35 = 0;
    m_statBase36 = 0;
    m_statRatio17 = 1.0f;
    m_statBase37 = 0;
    for (int i = 0; i < 18; ++i)
    {
        m_table1[i] = 0;
        m_table2[i] = 0;
        m_table3[i] = 0;
    }
    m_f2cc = 0;
    m_f2d0 = 0;
    m_f2d4 = 0;
    m_f2d8 = 0;
    m_f2dc = 1.0f;
    m_f2e0 = 1.0f;
    m_f2e4 = 0;
    m_f2e8 = 0;
    m_f2ec = 1.0f;
}

int ActiveStaticInfo::getHpMax()
{
    return static_cast<int>(static_cast<double>(m_hpBase) *
                            static_cast<double>(m_hpRatio));
}

int ActiveStaticInfo::getMpMax()
{
    return static_cast<int>(static_cast<double>(m_mpBase) *
                            static_cast<double>(m_mpRatio));
}

