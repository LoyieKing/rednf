// df_game_r BlueMarbleUserInfo（G2-4 大富翁子对象）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleUserInfo.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// BuffInfo/TileIndexInfo 为跨类依赖，经 asm-label extern 调用真实符号；
// 本 TU 只写本文件，PvP_deps 桩由主 agent 集成时移除。
#include "BlueMarbleUserInfo.h"

// ============================================================================
// 跨类 / 子对象方法（asm-label extern；BuffInfo/TileIndexInfo 权威 TU 交付后自然链接）
// ============================================================================
extern "C" void sub_BuffInfo_C1(void* self) asm("_ZN8BuffInfoC1Ev");
extern "C" void sub_BuffInfo_reset(void* self) asm("_ZN8BuffInfo5resetEv");
extern "C" void sub_TileIndexInfo_C1(void* self) asm("_ZN13TileIndexInfoC1Ev");
extern "C" void sub_TileIndexInfo_reset(void* self) asm("_ZN13TileIndexInfo5resetEv");

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

BlueMarbleUserInfo::BlueMarbleUserInfo()
{
    m_user = 0;
    m_userState = (BlueMarbleUserState::T)0;
    m_boardZone = 1;
    m_grade = (BlueMarbleUserGrade::T)4;
    sub_BuffInfo_C1(&m_buff);
    sub_TileIndexInfo_C1(&m_tileIndex);
    m_pScript = 0;
    sub_BuffInfo_reset(&m_buff);
    sub_TileIndexInfo_reset(&m_tileIndex);
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
    resetBuff();
    resetIndex();
}

void BlueMarbleUserInfo::resetBuff()
{
    sub_BuffInfo_reset(&m_buff);
}

void BlueMarbleUserInfo::resetIndex()
{
    sub_TileIndexInfo_reset(&m_tileIndex);
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
