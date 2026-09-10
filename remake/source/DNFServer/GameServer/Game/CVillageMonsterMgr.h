// df_game_r 还原 —— village_attacked 域权威头（从 GameStubs.cpp / VillageMonster_helpers.cpp 合并）
// 唯一声明点：CVillageMonsterMgr.cpp / VillageMonster_helpers.cpp / Area.cpp / CBattle_Field 域共用。
// 注意：STAttackedMonster 与 MapArea 必须在全局作用域声明（ORIG mangled InsertVillageMonster
// 使用非限定的 17STAttackedMonsterE / 7MapAreaE），不能放进 village_attacked 命名空间。

#ifndef VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H
#define VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H

#include <map>
#include <vector>

class CUser;
class PacketGuard;

// ORIG 反汇编关联：STAttackedMonster（OnCreate 0x86b3bd4 读
// +0x00 u32/+0x04/+0x08/+0x14/+0x18；GetAttackedMonster 0x86b3aea 按 +0x10 int
// 与目标索引比较）。我方 sizeof=0x1c，偏移 0x00/0x04/0x08/0x0c/0x10/0x14/0x18。
struct STAttackedMonster
{
    // [M5 2026-09-11 · Main 批准] ORIG 0x086b3bd4 +0x0a3 / +0x1dc 两处为 4 字节
    // 装载 `mov (%eax),%eax`（我方原 `movzwl (%eax),%eax` 另 +1 条 `movzwl %ax,%eax`）
    // ⇒ m_id 是 4 字节整型；`m_pad02[2]` 是我方臆造的 2 字节填充。
    // 布局证据（/tmp 探针，真实旗标）：sizeof 0x1c、offsetof 0x00/0x04/0x08/0x0c/
    // 0x10/0x14/0x18 前后逐值不变。
    unsigned int m_id;         // +0x00（OnCreate 取作怪物 zone id）
    int m_field04;             // +0x04（初始 x）
    int m_field08;             // +0x08（初始 y）
    int m_field0c;             // +0x0c
    int m_attackWeight;        // +0x10（GetAttackedMonster 阈值/权重）
    int m_field14;             // +0x14（怪物 m_field20 初值）
    int m_field18;             // +0x18（怪物 m_field24 初值）
};

// ORIG 反汇编关联：MapArea 0x1c 字节 —— ORIG 唯一 _ZN7MapArea5clearEv 0x8366050、
// MapArea::MapArea 0x836608e / ~MapArea 0x83660d6 / copy 0x0837e5ca（均落在
// CMap/STMapScript TU，定义见 STMapScript.cpp）。TownPath::clear 0x89e8258 以
// this+0 调 MapArea::clear、TownPath ctor 以 this+0x24 起 vector<int>，
// STMapScript ctor/dtor 以步进 0x1c 构造/析构 MapArea[5]：
//   +0x00 int m_x  +0x04 int m_y  +0x08 int m_w  +0x0c int m_h
//   +0x10 std::vector<int>（Area 传送区域附加列表；ORIG clear 仅清容器）
struct MapArea
{
    int m_x;     // +0x00
    int m_y;     // +0x04
    int m_w;     // +0x08
    int m_h;     // +0x0c
    std::vector<int> m_list; // +0x10

    MapArea();                       // ORIG 0x836608e
    MapArea(const MapArea& other);   // ORIG 0x0837e5ca
    ~MapArea();                      // ORIG 0x83660d6
    void clear();                    // ORIG 0x8366050

    // ORIG 0x86d886a（TownPath::operator= 0x089ea970 以 this+0 调用，实证存在）。
    // out-of-line 定义见 STMapScript.cpp（与本类型其余方法同 TU，保持 call 不发内联）。
    MapArea& operator=(const MapArea& other);

    // ORIG T 0x82f02e6：把 (x,y) 按 margin 裁剪到区域内，结果写 outX/outY；
    // 未发生移动（裁剪后 == 原坐标）返回 1，否则返回 0。
    // P292：ORIG 0x82e5a0b 调用点 `call; test %al,%al; je`（无 setne）⇒ 返回 bool。
    bool include(int x, int y, int& outX, int& outY, int margin);

    // ORIG T 0x82f0298：矩形包含判定（m_x<=x<=m_x+m_w 且 m_y<=y<=m_y+m_h）。
    bool include(int x, int y);
};
// ORIG 枚举 tag：_ZN16village_attacked18CVillageMonsterMgr23OnDestroyVillageMonsterE28ENUM_VILLAGE_ATTACKED_REWARD
// （OnDestroyVillageMonster/SetRewardType 参数，写入 m_field20；ORIG mangled 用
//   非限定 E28ENUM_VILLAGE_ATTACKED_REWARD，故枚举必须位于全局作用域）
enum ENUM_VILLAGE_ATTACKED_REWARD
{
    ENUM_VILLAGE_ATTACKED_REWARD_DEFAULT = 0,
    ENUM_VILLAGE_ATTACKED_REWARD_1 = 1
};

namespace village_attacked
{
// ---- Zone（ORIG ctor 0x86b4b3c：两个 unsigned char；map key） ----
struct Zone
{
    Zone(unsigned char a, unsigned char b) : m_a(a), m_b(b) {}

    // ORIG W 0x086b4af2：const 成员 operator<（mangled ZNK...ZoneltERKS0_）。
    bool operator<(const Zone& o) const
    {
        if (m_a < o.m_a)
            return true;
        if (m_a == o.m_a && m_b < o.m_b)
            return true;
        return false;
    }

    unsigned char m_a;
    unsigned char m_b;
};

// ORIG 枚举 tag：_ZN16village_attacked26ENUM_VILLAGE_MONSTER_STATEE
enum ENUM_VILLAGE_MONSTER_STATE
{
    ENUM_VILLAGE_MONSTER_STATE_0 = 0,
    ENUM_VILLAGE_MONSTER_STATE_1 = 1,
    ENUM_VILLAGE_MONSTER_STATE_2 = 2
};

class CVillageMonsterArea;

// ---- CVillageMonster（布局 ORIG ctor 0x86b3098/0x86b3004 实证，0x28 字节） ----
class CVillageMonster
{
public:
    CVillageMonster();   // ORIG 0x86b3098（全零 + m_partyIndex=-1）
    CVillageMonster(unsigned short id, int x, int y, int slot,
                    CVillageMonsterArea* area, int field20, int field24);
                                                       // ORIG T 0x86b3004（uid 自增分配 +0x00）
    ~CVillageMonster();  // ORIG 0x86b3106（空）
    unsigned short m_id;                 // +0x00
    unsigned short m_zone;               // +0x02
    int m_field04;                       // +0x04
    int m_field08;                       // +0x08
    int m_field0c;                       // +0x0c
    bool m_field10;                      // +0x10（ORIG 0x86b36e6 movzbl+xor$1 ⇒ bool）
    // P254（b28d-L8）：ORIG MakeVillageMonsterListPacket 0x86b3666 对 +0x14 读
    // 出两段 `mov 0x14(%eax),%eax; mov %eax,%edx` ⇒ 源字段类型须为 enum
    //（int 字段 4.4 恒折单段）。布局/mangled 不变。
    ENUM_VILLAGE_MONSTER_STATE m_state;  // +0x14
    int m_partyIndex;                    // +0x18
    CVillageMonsterArea* m_area;         // +0x1c
    int m_field20;                       // +0x20
    int m_field24;                       // +0x24

    int OnKillVillageMonster(CUser* user, int a, int b, int c, bool flag);
    void OnRunVillageMonster(std::vector<MapArea>* areas, int idx);
                                                       // ORIG T 0x86b310c（随机游走/虚拟移动区裁剪）
    int OnFightVillageMonster(CUser* user, int& out1, int& out2);
                                                       // ORIG T 0x86b3240（距离校验+状态1+partyIndex）
    void OnChangeState(ENUM_VILLAGE_MONSTER_STATE state);
    bool MakeVillageMonsterListPacket(PacketGuard& packet);  // ORIG T 0x86b3666
    bool MakeVillageMonsterPositionPacket(PacketGuard& packet);
                                                       // ORIG T 0x86b36e6（仅 m_field10==1 时写位置并清零）
    unsigned short GetUID() const;   // ORIG W 0x86b4b64：return m_id
    void SendVillageMonsterFightResult(CUser* user, bool flag);
};

// ---- CVillageMonsterArea（布局 ORIG ctor 0x86b3752 实证，0x3c 字节） ----
// ORIG 0x86b3f06 对 +0x30 读出发两段 `mov 0x30(%eax),%eax; mov %eax,%edx`
// （P265：int 字段 + 实参处显式 cast 到 enum 触发 4.4 转换中转；unsigned 恒单段）。
// 语义 = 轮转索引槽（仅与 idx%3 比较并自增），4 字节，偏移/mangled 不变。
enum ENUM_VILLAGE_MONSTER_ROTATE
{
    ENUM_VILLAGE_MONSTER_ROTATE_0 = 0
};

class CVillageMonsterArea
{
public:
    CVillageMonsterArea(int a, int b, int c, int d,
                        std::vector<STAttackedMonster>* monsters,
                        std::vector<MapArea>* areas);   // ORIG 0x86b3752
    std::map<unsigned short, CVillageMonster> m_monsters;  // +0x00（0x18）
    void* m_areaList;                 // +0x18（ctor 参数 6：vector<MapArea>*）
    std::vector<STAttackedMonster> m_monsterList;  // +0x1c (0x14)
    int m_field28;                    // +0x28
    int m_field2c;                    // +0x2c
    ENUM_VILLAGE_MONSTER_ROTATE m_field30;  // +0x30（P254：enum 字段复现两段装载）
    unsigned int m_field34;           // +0x34（P211 异型化：ctor int 形参→unsigned 字段
                                      //   触发 4.4 RHS-first store=ORIG 形，b28-L11；
                                      //   读取侧 395 已显式 (unsigned int) cast，行为不变）
    int m_field38;                    // +0x38

    int OnKillVillageMonster(CUser* user, unsigned short zone, bool flag);
    void OnRunVillageMonster();      // ORIG T 0x86b3f06（区域有人才轮转）
    void SendVillageMonsterPosition();   // ORIG T 0x86b3962（0xef 位置包）
    STAttackedMonster* GetAttackedMonster(int idx);   // ORIG T 0x86b3aea
    int OnCreateVillageMonster();    // ORIG T 0x86b3bd4（生成一只并发包）
    void OnDestroyVillageMonster();  // ORIG T 0x86b3ee0（清空+空列表广播）
    int OnFightVillageMonster(CUser* user, unsigned short zone, int& out1, int& out2);
                                                       // ORIG T 0x86b3fe2
    void OnMoveArea(CUser* user);              // ORIG 0x86b3abe：非空则发列表
    void SendVillageMonsterList(CUser* user);  // ORIG 0x86b3c1e
    void SendPacket(PacketGuard& packet);
};

// ---- CVillageMonsterMgr（布局 ORIG ctor 0x86b4a4c 实证，0x24 字节） ----
class CVillageMonsterMgr
{
public:
    CVillageMonsterMgr();    // ORIG 0x86b4a4c（map 默认构造，三字段清零）
    ~CVillageMonsterMgr();   // ORIG 0x86b4a80（map 析构）
    int m_field00;                     // +0x00
    int m_field04;                     // +0x04
    std::map<Zone, CVillageMonsterArea> m_areas;  // +0x08 (0x18)
    int m_field20;                     // +0x20

    int  OnKillVillageMonster(int a, int b, unsigned short c, bool d);
                                                      // _ZN16village_attacked18CVillageMonsterMgr20OnKillVillageMonsterEiitb
                                                      // ORIG 0x086b4964（调用点 CBattle_Field::reset_field）；
                                                      // 定义见 VillageMonster_helpers.cpp
    int OnFightVillageMonster(CUser* user, unsigned short zone);
                                                      // _ZN16village_attacked18CVillageMonsterMgr21OnFightVillageMonsterEP5CUsert (0x86b4672)
    int OnKillVillageMonster(CUser* user, bool flag);
                                                      // _ZN16village_attacked18CVillageMonsterMgr20OnKillVillageMonsterEP5CUserb (0x86b4866)
    int GetRewardType();
    void SetRewardType(ENUM_VILLAGE_ATTACKED_REWARD reward);  // ORIG W 0x0847dd44
                                                      // _ZN16village_attacked18CVillageMonsterMgr13GetRewardTypeEv
    void OnMoveArea(CUser* user);                    // _ZN16village_attacked18CVillageMonsterMgr10OnMoveAreaEP5CUser (0x86b45bc)
    bool OnEvent();                                   // _ZN16village_attacked18CVillageMonsterMgr7OnEventEv (0x82f0f44)
                                                      // ORIG 0x82f0f44 反汇编：返回 bool(m_field04 != 0)
    void InsertVillageMonster(int a, int b, int c, int d,
                              std::vector<STAttackedMonster>* monsters,
                              std::vector<MapArea>* areas);  // _ZN16village_attacked18CVillageMonsterMgr20InsertVillageMonsterEiiiiPSt6vectorI17STAttackedMonsterSaIS2_EEPS1_I7MapAreaSaIS6_EE (0x86b4144)
    // 定时器回调族（TimerDispatcher 村战定时器，ORIG 全部 T 符号）：
    void OnStartVillageAttacked();              // ORIG 0x86b4a20
    void OnRunVillageMonster();                 // ORIG 0x86b44fc
    void OnRegenVillageMonster(int a, int b);   // ORIG 0x86b4338
    void OnCreateVillageMonster();              // ORIG 0x86b4280
    void OnDestroyVillageMonster(ENUM_VILLAGE_ATTACKED_REWARD reward);
                                               // ORIG 0x86b43d4（清空+0xf2 广播+奖励落位）
    void OnEndRewardVillageMonster();           // ORIG 0x86b44ec（m_field20=0）
};

}  // namespace village_attacked

#endif  // VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H
