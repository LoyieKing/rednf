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

// ---- 跨类方法（ORIG 真实符号）----
extern "C" void sub_Inven_Item_ctor(void* self) asm("_ZN10Inven_ItemC1Ev");
extern "C" void sub_Inven_Item_reset(void* self) asm("_ZN10Inven_Item5resetEv");
extern "C" void sub_GameResultType_ctor(void* self) asm("_ZN14GameResultTypeC1Ev");
extern "C" void sub_GameResultType_dtor(void* self) asm("_ZN14GameResultTypeD1Ev");
extern "C" void sub_GameResultType_clear(void* self) asm("_ZN14GameResultType5ClearEv");

// 事件地下城请求结构（布局按反汇编推导；mangling 含类型名，须与 ORIG 一致）。
namespace CMDPacketStruct
{

// _STReqEventDungeonClearRoom：+0x0e ushort 房间号、+0x10 ushort、+0x12 ushort
struct _STReqEventDungeonClearRoom
{
    char m_pad[0x0e];
    unsigned short m_roomIndex;     // +0x0e
    unsigned short m_monsterIndex;  // +0x10
    unsigned short m_monsterCount;  // +0x12
};

// _STReqEventDungeonDestoryObject：+0x0e ushort 房间号、+0x10 int 类型、+0x14 ushort 点数
struct _STReqEventDungeonDestoryObject
{
    char m_pad[0x0e];
    unsigned short m_roomIndex;   // +0x0e
    int m_destroyType;            // +0x10
    unsigned short m_point;       // +0x14
};

}  // namespace CMDPacketStruct

// 事件地下城清除数据（0x8 每项，数组首地址 this+0x62c，共 12 项）
struct stEventClearData
{
    unsigned short m_field0;    // +0x00
    unsigned short m_roomIdx;   // +0x02（请求结构 +0x0e）
    unsigned short m_monsterIdx;// +0x04（请求结构 +0x10）
    unsigned char m_clearFlag;  // +0x06（请求结构 +0x12）
    unsigned char m_pad7;       // +0x07
};

struct stInvenItem
{
    char m_pad[0x3d];
};

struct stGameResult
{
    char m_pad[0x50];
};

class BattleData
{
public:
    BattleData();
    ~BattleData();
    void Reset();
    int GetHellPartyValueTotal();
    void SetHellPartyValueTotal(int value);
    int GetTotalKilledMonsterCount() const;
    void IncEventDungeonDestoryObjectPoint(
        int param_1, const CMDPacketStruct::_STReqEventDungeonDestoryObject& st);
    void ResetEventDungeonClearPoint();
    int ClearEventDungeonRoom(
        int param_1, int param_2, const CMDPacketStruct::_STReqEventDungeonClearRoom& st);
    char IsClearEventDungeonRoom(int param_1, unsigned short roomIdx) const;

    int m_field0;        // +0x00
    int m_field4;        // +0x04
    int m_field8;        // +0x08
    int m_fieldc;        // +0x0c
    int m_field10;       // +0x10
    int m_field14;       // +0x14
    int m_field18;       // +0x18
    int m_field1c;       // +0x1c
    int m_field20;       // +0x20
    int m_field24;       // +0x24
    char m_flag28[4];    // +0x28
    int m_field2c;       // +0x2c
    int m_field30;       // +0x30
    int m_field34;       // +0x34
    int m_field38;       // +0x38
    int m_killedMonster0;// +0x3c
    int m_killedMonster1;// +0x40
    int m_killedMonster2;// +0x44
    int m_field48;       // +0x48
    int m_field4c;       // +0x4c
    char m_arr50[4];     // +0x50
    char m_arr54[4];     // +0x54
    char m_arr58[4];     // +0x58
    char m_arr5c[4];     // +0x5c
    char m_arr60[4];     // +0x60
    int m_arr64[4];      // +0x64
    stInvenItem m_itemGroups0[4][2];  // +0x74
    stInvenItem m_itemGroups1[4][2];  // +0x25c
    stInvenItem m_itemGroups2[4][2];  // +0x444
    stEventClearData m_eventClear[12];// +0x62c
    int m_field68c;      // +0x68c
    stGameResult m_gameResults[4];    // +0x690
    int m_arr7d0[4];     // +0x7d0
    float m_hellPartyValueTotal;      // +0x7e0
    char m_arr7e4[4];    // +0x7e4
    int m_arr7e8[4];     // +0x7e8
};

BattleData::BattleData()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            sub_Inven_Item_ctor(&m_itemGroups0[i][j]);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            sub_Inven_Item_ctor(&m_itemGroups1[i][j]);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            sub_Inven_Item_ctor(&m_itemGroups2[i][j]);
    for (int i = 0; i < 4; ++i)
        sub_GameResultType_ctor(&m_gameResults[i]);
}

BattleData::~BattleData()
{
    for (int i = 0; i < 4; ++i)
        sub_GameResultType_dtor(&m_gameResults[i]);
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
        sub_GameResultType_clear(&m_gameResults[i]);
        m_arr7e8[i] = 0x7fffffff;
        m_arr64[i] = 0;
        sub_Inven_Item_reset(&m_itemGroups0[i][0]);
        sub_Inven_Item_reset(&m_itemGroups0[i][1]);
        sub_Inven_Item_reset(&m_itemGroups1[i][0]);
        sub_Inven_Item_reset(&m_itemGroups1[i][1]);
        sub_Inven_Item_reset(&m_itemGroups2[i][0]);
        sub_Inven_Item_reset(&m_itemGroups2[i][1]);
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
