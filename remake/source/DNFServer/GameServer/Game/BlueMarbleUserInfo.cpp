// df_game_r BlueMarbleUserInfo（G2-4 大富翁子对象）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleUserInfo.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// BuffInfo/TileIndexInfo 为跨类依赖；本 TU 直接按 ORIG C1/reset 语义清零字段，
// 不调用 BuffInfo.h（0x10 与 ORIG 0xc 布局分歧）/TileIndexInfo 的 reset 符号
// （避免跨 TU 重复定义与尺寸错配），§9 落地。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN18BlueMarbleUserInfoC2Ev`（ORIG 0x80d9e63）；ctor 已是首强符号定义。
#include <iostream>

#include "BlueMarbleUserInfo.h"

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// BlueMarbleScriptManager 最大 buff 值位于 +0x64（ORIG setExp/setGold/setDrop 直读）。
namespace
{
struct BM_ScriptMaxBuff
{
    char m_pad[0x64];
    int m_maxBuffValue;  // +0x64
};
}

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// ORIG 0x080d6a90：标量按声明序 init-list 直写 → 成员 C1 call(+0x10/+0x1c)
// → +0x24 直写 → 显式 reset() 两 call（C1 后二次 reset 语义照抄）。
BlueMarbleUserInfo::BlueMarbleUserInfo()
    : m_user(0),
      m_userState((BlueMarbleUserState::T)0),
      m_boardZone(1),
      m_grade((BlueMarbleUserGrade::T)4),
      m_buff(),
      m_tileIndex(),
      m_pScript(0)
{
    m_buff.reset();
    m_tileIndex.reset();
}

BlueMarbleUserInfo::~BlueMarbleUserInfo()
{
}

void BlueMarbleUserInfo::resetBlueMarbleUserInfo()
{
    m_user = 0;
    m_userState = (BlueMarbleUserState::T)0;
    m_boardZone = 1;
    m_grade = (BlueMarbleUserGrade::T)4;
    resetBuff();   // ORIG BuffInfo::reset（m_exp=m_gold=m_drop=1）
    resetIndex();
}
void BlueMarbleUserInfo::resetBuff()
{
    m_buff.reset();   // ORIG BuffInfo::reset（定义在 BuffInfo.cpp）
}

void BlueMarbleUserInfo::resetIndex()
{
    m_tileIndex.reset();   // ORIG TileIndexInfo::reset
}

void BlueMarbleUserInfo::setUser(CUser* user)
{
    m_user = user;
}

CUser* BlueMarbleUserInfo::getUser() const
{
    return m_user;
}

void BlueMarbleUserInfo::setScript(BlueMarbleScriptManager* script)
{
    m_pScript = script;
}

void BlueMarbleUserInfo::setUserState(BlueMarbleUserState::T state)
{
    m_userState = state;
}

BlueMarbleUserState::T BlueMarbleUserInfo::getUserState() const
{
    return m_userState;
}

void BlueMarbleUserInfo::setBoardZone(int zone, int pos)
{
    if (m_boardZone + zone >= pos)
    {
        m_boardZone = pos;
    }
    else if (m_boardZone + zone <= 1)
    {
        m_boardZone = 1;
    }
    else
    {
        m_boardZone = m_boardZone + zone;
    }
}

int BlueMarbleUserInfo::getBoardZone() const
{
    return m_boardZone;
}

void BlueMarbleUserInfo::setGrade(BlueMarbleUserGrade::T grade)
{
    m_grade = grade;
}

int BlueMarbleUserInfo::getGrade() const
{
    return m_grade;
}

void BlueMarbleUserInfo::setDungeon(int dungeon)
{
    m_tileIndex.m_dungeon = dungeon;
}

int BlueMarbleUserInfo::getDungeon() const
{
    return m_tileIndex.m_dungeon;
}

void BlueMarbleUserInfo::setItem(unsigned int itemIdx)
{
    m_tileIndex.m_item = itemIdx;
}

int BlueMarbleUserInfo::getItem() const
{
    return m_tileIndex.m_item;
}

void BlueMarbleUserInfo::setExp(int exp)
{
    if (m_buff.m_exp * exp >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_exp = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_exp = m_buff.m_exp * exp;
    }
}

int BlueMarbleUserInfo::getExp() const
{
    return m_buff.m_exp;
}

void BlueMarbleUserInfo::setGold(int gold)
{
    if (m_buff.m_gold * gold >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_gold = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_gold = m_buff.m_gold * gold;
    }
}

int BlueMarbleUserInfo::getGold() const
{
    return m_buff.m_gold;
}

void BlueMarbleUserInfo::setDrop(int drop)
{
    if (m_buff.m_drop * drop >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_drop = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_drop = m_buff.m_drop * drop;
    }
}

int BlueMarbleUserInfo::getDrop() const
{
    return m_buff.m_drop;
}
