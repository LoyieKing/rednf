// df_game_r BattleData —— 唯一声明点（BattleData.cpp 实现；CParty 子对象 +0x32c，总尺寸 0x7f8）。
// 布局：+0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、+0x74 Inven_Item[4][2]（0x3d 每项）、
//   +0x25c Inven_Item[4][2]、+0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
#ifndef GAME_BATTLEDATA_H_
#define GAME_BATTLEDATA_H_

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

// ORIG 符号：C1 0x082a4cea / D1 0x082a54d6 / Reset 0x082a4e8a /
//   GetHellPartyValueTotal 0x0822d7c4 / SetHellPartyValueTotal 0x0822d7b2 /
//   GetTotalKilledMonsterCount 0x085bf456 / IncEventDungeonDestoryObjectPoint 0x085be6e2 /
//   ResetEventDungeonClearPoint 0x085be6ba / ClearEventDungeonRoom 0x085be7aa /
//   IsClearEventDungeonRoom 0x085be862。
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

#endif  // GAME_BATTLEDATA_H_