// df_game_r BattleData —— 唯一声明点（BattleData.cpp 实现；CParty 子对象 +0x32c，总尺寸 0x7f8）。
// 布局：+0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、+0x74 Inven_Item[4][2]（0x3d 每项）、
//   +0x25c Inven_Item[4][2]、+0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
#ifndef GAME_BATTLEDATA_H_
#define GAME_BATTLEDATA_H_

#include "CMDPacketStruct.h"   // CMDPacketStruct::_STReqEventDungeon* 权威定义

// 事件地下城清除数据（0x8 每项，数组首地址 this+0x62c，共 12 项）
struct stEventClearData
{
    unsigned short m_field0;    // +0x00
    unsigned short m_roomIdx;   // +0x02（请求结构 +0x0e）
    unsigned short m_monsterIdx;// +0x04（请求结构 +0x10）
    unsigned char m_clearFlag;  // +0x06（请求结构 +0x12）
    unsigned char m_pad7;       // +0x07
};

#include "Inven_Item.h"   // 权威物品布局（pack(1)，0x3d 每项；BattleData.cpp
                          //   placement-new Inven_Item 实证，替代 char 占位镜像）

#include "GameResultType.h"   // ORIG C1/D1 反汇编实证：m_gameResults 是真实
                              //   GameResultType[4] 成员（隐式数组构造/析构 + EH 清理），
                              //   不是 char 占位镜像。

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
    // P180 u64 别名（Main 批准批）：ORIG GetMapPlayingTime 0x085b6768 /
    // ClearDungeon 0x085a94e0 均以双 sub/sbb 直对 64 位字段运算；int 拼接会
    // 产生 sar/or 展开。union 别名不改任何偏移/尺寸。
    union {                       // +0x08 played 结果（lo/hi）
        struct { int m_field8; int m_fieldc; };
        unsigned long long m_playedTime;
    };
    union {                       // +0x10 减数 1（baseTick）
        struct { int m_field10; int m_field14; };
        unsigned long long m_baseTick;
    };
    union {                       // +0x18 减数 2（enterTick）
        struct { int m_field18; int m_field1c; };
        unsigned long long m_enterTick;
    };
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
    Inven_Item m_itemGroups0[4][2];  // +0x74
    Inven_Item m_itemGroups1[4][2];  // +0x25c
    Inven_Item m_itemGroups2[4][2];  // +0x444
    stEventClearData m_eventClear[12];// +0x62c
    int m_field68c;      // +0x68c
    GameResultType m_gameResults[4];  // +0x690（0x50 每项）
    int m_arr7d0[4];     // +0x7d0
    float m_hellPartyValueTotal;      // +0x7e0
    char m_arr7e4[4];    // +0x7e4
    int m_arr7e8[4];     // +0x7e8
};

#endif  // GAME_BATTLEDATA_H_