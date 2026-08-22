// df_game_r BattleData 还原（G2-5 批次）。
// CParty +0x32c，总尺寸 0x7f8。符号（nm -C，W/T 全实现）：
//   C1 0x082a4cea / D1 0x082a54d6 / Reset 0x082a4e8a /
//   GetHellPartyValueTotal 0x0822d7c4 / SetHellPartyValueTotal 0x0822d7b2 /
//   GetTotalKilledMonsterCount 0x085bf456 /
//   IncEventDungeonDestoryObjectPoint 0x085be6e2 /
//   ResetEventDungeonClearPoint 0x085be6ba /
//   ClearEventDungeonRoom 0x085be7aa / IsClearEventDungeonRoom 0x085be862
// 布局（由 Reset/构造/析构反汇编推导）：
//   +0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、
//   +0x74 Inven_Item[4][2]（0x3d 每项）、+0x25c Inven_Item[4][2]、
//   +0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
// Inven_Item/GameResultType 跨类方法以 extern asm 标签引用（ORIG 真实符号）。

#include <string.h>

#include "BattleData.h"
#include "Inven_Item.h"
#include "GameResultType.h"

// ============================================================================
// 构造 / 析构 / Reset
// ============================================================================

BattleData::BattleData()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            new (&m_itemGroups0[i][j]) Inven_Item();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            new (&m_itemGroups1[i][j]) Inven_Item();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            new (&m_itemGroups2[i][j]) Inven_Item();
    for (int i = 0; i < 4; ++i)
        new (&m_gameResults[i]) GameResultType();
}

BattleData::~BattleData()
{
    for (int i = 0; i < 4; ++i)
        reinterpret_cast<GameResultType*>(&m_gameResults[i])->~GameResultType();
}

void BattleData::Reset()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = 0;
    m_flag28[0] = 0;
    m_flag28[1] = 0;
    m_flag28[2] = 0;
    m_flag28[3] = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field34 = 0;
    m_field38 = 0;
    m_killedMonster0 = 0;
    m_killedMonster1 = 0;
    m_killedMonster2 = 0;
    m_field48 = 0;
    m_field4c = 0;
    m_field68c = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_arr50[i] = 0;
        m_arr54[i] = 0;
        m_arr58[i] = 0;
        m_arr5c[i] = 0;
        m_arr60[i] = 0;
        reinterpret_cast<GameResultType*>(&m_gameResults[i])->Clear();
        m_arr7e8[i] = 0x7fffffff;
        m_arr64[i] = 0;
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][1])->reset();
        m_arr7d0[i] = 0;
        m_arr7e4[i] = 0;
    }
}

int BattleData::GetHellPartyValueTotal()
{
    return (int)m_hellPartyValueTotal;
}

void BattleData::SetHellPartyValueTotal(int value)
{
    m_hellPartyValueTotal = (float)value;
}

int BattleData::GetTotalKilledMonsterCount() const
{
    return m_killedMonster0 + m_killedMonster1 + m_killedMonster2;
}

void BattleData::IncEventDungeonDestoryObjectPoint(
    int param_1, const CMDPacketStruct::_STReqEventDungeonDestoryObject& st)
{
    if (st.m_roomIndex >= 3)
    {
        return;
    }
    int idx = param_1 * 3 + st.m_roomIndex;
    if (st.m_destroyType == 1)
    {
        m_eventClear[idx].m_monsterIdx += st.m_point;
    }
    else
    {
        m_eventClear[idx].m_roomIdx += st.m_point;
    }
}

void BattleData::ResetEventDungeonClearPoint()
{
    memset(m_eventClear, 0, sizeof(m_eventClear));
}

int BattleData::ClearEventDungeonRoom(
    int param_1, int param_2, const CMDPacketStruct::_STReqEventDungeonClearRoom& st)
{
    if (st.m_roomIndex < 3)
    {
        int idx = param_1 * 3 + st.m_roomIndex;
        if (m_eventClear[idx].m_monsterIdx == st.m_monsterIndex &&
            m_eventClear[idx].m_roomIdx == st.m_monsterCount &&
            st.m_monsterIndex <= param_2)
        {
            m_eventClear[idx].m_clearFlag = 1;
            return 1;
        }
        return 0;
    }
    return 0;
}

char BattleData::IsClearEventDungeonRoom(int param_1, unsigned short roomIdx) const
{
    if (roomIdx >= 3)
    {
        return 0;
    }
    return m_eventClear[param_1 * 3 + roomIdx].m_clearFlag;
}
