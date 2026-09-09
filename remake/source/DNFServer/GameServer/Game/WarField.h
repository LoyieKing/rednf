#ifndef DNF_GAME_WARFIELD_H_
#define DNF_GAME_WARFIELD_H_

// df_game_r WarField 还原（G2-4 战场域，PvP WarRoom 内嵌 0xf8 字节）。
// 布局依据 ORIG 构造（0x86c0b8e）/析构（0x86c0ba4）/Reset（0x86b8a14）：
//   +0x00 char[8]（未初始化）   +0x08 MapInfo（0xec）
//   +0xf4 int m_stageIdx（ConsistMap 自增 / Reset 清零）
// map_item/map_monster/MapInfo 与 CBattle_Field.h 同型同布局（ORIG 构造体
// 0x81512cc/0x81511c8/0x8151394 实证；集成时与 CBattle_Field.h 去重）。
// 注意：map_monster 头字段按 ORIG copy ctor（0x8151aea）修正 —— +0x10 为
// byte、+0x11 为 word、+0x13/+0x14 各为 byte（CBattle_Field.h 的 int m_level
// 布局有偏差，本头以 ORIG 为准）。

#include <map>
#include <vector>

#include "ActiveStaticInfo.h"
#include "Inven_Item.h"

class CDungeon;
class CMap;
class CUser;
class PacketGuard;
class CMonster;

// ---- map_item（0x54 字节；ctor 081512cc / dtor 081512dc 同址别名） ----
// +0x00/+0x01 为两个独立字节字段（ORIG 实证，非 int）：
//   - ctor 081512cc 仅构造 Inven_Item(+0x10) 并清零 +0x50，不清零首 dword。
//   - m_count 写入全部为 movb：_MakeDropItems 0830b5ab、add_item_curr_map
//     08302f67、CParty::drop_item 085a64e5、CBossStage 0814cb23；
//     读取亦为 movzbl 字节测试：handlePickupItem 0814fd3a/0814fde7。
//   - +0x01 为物品生成等级字节（非"绑定标记"——反汇编不支持绑定语义）：
//     add_item_curr_map 08302f84 写入 _mapItem.m_field0c（地图脚本对象条目）；
//     CBossStage/CDeathTower consistMap（0814c9da/0814ca34）写入
//     stDungeonAssignItem_t.m_assignItemIdx，-1 时回退 CDungeon::
//     get_standard_level()（VerifyMap 校验该值域 1..200 等级区间）；
//     序列化为有符号字节：MakePacket 08309526、SendMapInfoPacket 0814d05a。
struct map_item
{
    // +0x00 掉落堆叠数量（byte）。ORIG 读取恒为 bool 取反物化
    // `movzbl F; xor $1,%eax; test %al,%al; je`（CParty::isMonsterDrop_item
    // 0x085a77c3、get_item 0x085a57e2 实证），且全仓写入恒 0/1
    // （_MakeDropItems 0830b5ab、CParty::drop_item 085a64e5 等 movb）⇒ 字段
    // 真型为 bool（P248 档②）；uchar 形 4.4 恒折 `cmpb $1`。宽度/偏移/mangled 不变。
    bool m_count;             // +0x00
    char m_standardLevel;     // +0x01 物品生成等级（byte；来源见上）
    char m_pad02[2];          // +0x02
    int m_itemIndex;          // +0x04
    unsigned short m_dropIndex;  // +0x08（CheckPickupItem 86ba149：0xffff=自由
                              //      拾取，否则为归属玩家 uid —— 拾取绑定域）
    char m_pad0a[2];          // +0x0a
    int m_createTick;         // +0x0c（CheckPickupItem 86ba128 与当前秒比较）
    Inven_Item m_item;        // +0x10（0x3d 字节）
    char m_pad4d[3];          // +0x4d
    int m_ownerId;            // +0x50（ctor=0）

    map_item();
    // ORIG 无拷贝构造/operator=/~map_item 符号（nm 仅 081512cc 默认构造）：
    // 保持隐式平凡拷贝/析构（用户声明拷贝构造会使 DropItem/Add_Item 调用
    // C1ERKS 而非 ORIG 的 rep movsl 位拷贝，偏离 df_game_r）。
    // ORIG 无 ~map_item 符号：保持隐式 trivial 析构（用户定义空体会使
    // std::list/map 节点析构产生额外 call 与 D1 符号，偏离 df_game_r）。
};

// ---- map_monster（0x34c 字节；ctor 081511c8） ----
// ORIG 布局 +0x10 byte/+0x11 word/+0x13 byte…：m_field11(short) 位于奇数偏移
// +0x11（非 2 对齐，ORIG ConsistMap 逐字节写 +0x11/+0x12 实证）⇒ 仅该成员带
// __attribute__((packed))，结构整体保持自然对齐 4。依据：ORIG C1ERKS_/aSERKS_
// （0x8151aea/0x8151d3a）对 m_active(+0x38, 0x2f0) 发无对齐检查的
// `rep movsl`（4.4 在 pack(1) 结构下恒走对齐剥离慢路）；成员 packed 方案下
// 4.4.7 复现同形，且全部成员偏移与 sizeof 0x34c 逐项实测不变（探针
// /tmp/b28d_map_probe/layout2.cpp 34 项偏移断言 + sizeof/align 断言）。
struct map_monster
{
    int m_mobId;              // +0x00
    int m_instId;             // +0x04（实例序号，短整使用）
    char m_roleType;          // +0x08
    char m_pad9[3];           // +0x09
    int m_mobIndex;           // +0x0c（击杀逻辑的怪物索引）
    unsigned char m_level;    // +0x10（byte；ORIG 读取全 movzbl → unsigned）
    // [SweepBattleMonster 2026-09-09] ORIG 对 +0x11/+0x12 恒发两条独立 movb（逐字节写，
    // 见 CBattle_Field::add_monsters_current_map 0x0830194e/0x0830195c）⇒ 匿名 union
    // 双视角：short 视图保留既有消费点，字节视图供逐字节复现。packed 保持对齐 1、
    // size 2 ⇒ sizeof(map_monster) 与全部成员偏移不变。
    union __attribute__((packed))
    {
        short m_field11;                      // +0x11（非 2 对齐，ORIG 实证）
        struct
        {
            unsigned char m_field11b0;        // +0x11
            unsigned char m_field11b1;        // +0x12
        } m_field11b;
    };
    unsigned char m_field13;  // +0x13（ORIG 读取 movzbl）
    unsigned char m_field14;  // +0x14（ORIG 读取 movzbl）
    char m_field15;           // +0x15
    char m_field16;           // +0x16
    char m_field17;           // +0x17
    float m_field18;          // +0x18（ConsistMap 写 float）
    int m_field1c;            // +0x1c
    int m_field20;            // +0x20
    int m_field24;            // +0x24
    float m_field28;          // +0x28（ConsistMap 写 float）
    int m_dropCnt;            // +0x2c
    int m_dropRate;           // +0x30（默认 100）
    char m_flag34;            // +0x34
    char m_pad35[3];          // +0x35
    ActiveStaticInfo m_active;      // +0x38（0x2f0 字节）
    std::vector<int> m_mobList;     // +0x328
    short m_hellPartyGroupIdx;      // +0x334
    unsigned short m_hellPartyGroupOrder;    // +0x336（ORIG 0x0830d704 movzwl 零扩展读实证）
    char m_hellPartyScriptIdx;      // +0x338
    char m_pad339;                  // +0x339
    short m_bloodSmallRoundOrder;   // +0x33a
    short m_bloodPhaseTime;         // +0x33c
    short m_bloodRoundOrder;        // +0x33e
    short m_bloodSpawnIdx;          // +0x340
    short m_bloodPhaseIdx;          // +0x342
    // b28d-60：char→bool。依据 ORIG kill_blood_monster 0x0830d66c
    // `movzbl -0x28(%ebp),%eax; xor $0x1,%eax; test %al,%al; jne`（P292/P73：
    // bool 字段 + 裸 if(!f) 才出 xor$1 取反物化；char 产 `cmpb $1`）。
    // 宽度 1B、pack(1) 下全部偏移/sizeof 0x34c 不变。
    bool m_bloodKillFlag;           // +0x344
    char m_bloodDifficulty;         // +0x345
    short m_bloodRoundOrder2;       // +0x346
    char m_bloodSpawned;            // +0x348
    bool m_isBoss;                  // +0x349（[b28c-35] ORIG 0x0830e8b8 读 `movzbl; xor$1; test; je`
                                    //   ⇒ 与 bool 形参/裸测同型；char 产 cmp$1/setne）
    bool m_bloodRound;              // +0x34a（[b28c-35] ORIG 0x0830e97c 读 `movzbl; test %al,%al`
                                    //   ⇒ 裸测 bool；char 多 setne 物化）
    char m_bloodRoundFlag;          // +0x34b

    map_monster();                          // ORIG 081511c8
    map_monster(const map_monster& other);  // ORIG 08151aea
    ~map_monster();                         // ORIG 081515ae
    map_monster& operator=(const map_monster& other);  // ORIG 08151d3a
    bool isNamedMonster();                 // ORIG 08151182（非 const）
};


// ---- MapInfo 支撑结构（0x18/0x10；init ORIG 081512ec/0815132c） ----
struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    unsigned int m_dieCnt;                 // +0x0c（ORIG setter store 展开序 this 先 ⇒ 与形参 unsigned 同型）
    unsigned int m_field10;                // +0x10（同上；checkLongTermMobDie 无符号差运算）
    int m_field14;                         // +0x14
    void init();                           // ORIG 081512ec
    void initMonsterInfo();                // ORIG W 0x830e71c
    void setUseSkillMaterial(unsigned int tick);   // ORIG W 0x830e72c
    unsigned int getUseSkillMaterialTime();  // ORIG W 0x830e78c（非 const）
    int getUseSkillMaterialCount();          // ORIG W 0x822cb86（非 const）
    int dieMonster(unsigned int tick, unsigned int limit);       // ORIG W 0x830e748
    void setLastMobDieTime(unsigned int tick);     // ORIG W 0x830e796
    int checkLongTermMobDie(unsigned int tick, unsigned int limit);  // ORIG W 0x830e7a4
    void setUncontinuallyMobDieCnt(unsigned int cnt);      // ORIG W 0x830e7dc
};

// 4 个 int（0x10 字节）
struct stMapPlayInfo_t
{
    unsigned int m_startTick;   // +0x00
    unsigned int m_clearTick;   // +0x04
    int m_useItemType2Count;    // +0x08
    int m_useItemOtherCount;    // +0x0c
    void init();               // ORIG 0815132c
    unsigned int getPlayTick();  // ORIG W 0x830e7ea（非 const：_ZN15stMapPlayInfo_t11getPlayTickEv 无 K）
};
// ---- MapInfo（0xec 字节；ctor 08151394） ----
struct MapInfo
{
    char m_key;                       // +0x00
    char m_visited;                   // +0x01
    char m_visitedGoto;               // +0x02
    char m_pad3[1];                   // +0x03
    int m_mapIndex;                   // +0x04
    char m_gridValue;                 // +0x08
    char m_pad9[3];                   // +0x09
    std::map<int, map_monster> m_monsterMap;   // +0x0c
    std::map<int, map_item> m_itemMap;         // +0x24
    int m_assignItemIdx;              // +0x3c
    int m_dropItemCnt;                // +0x40
    char m_mapType;                   // +0x44
    // [SweepBattleMonster 2026-09-09] P248 档②：ORIG add_monsters_current_map 0x08301a33
    // `movzbl 0x45(%eax),%eax; xor $0x1,%eax; test %al,%al; je` ⇒ bool 字段 + 裸 `!field`。
    // 宽度 1B / 偏移 +0x45 / sizeof 0xec 不变。
    bool m_dummyBossMap;              // +0x45
    char m_pad46[2];                  // +0x46
    int m_mapBasisLevel;              // +0x48
    stMapMonsterKillChecker_t m_killChecker1;  // +0x4c（0x18）
    stMapMonsterKillChecker_t m_killChecker2;  // +0x64（0x18）
    stMapPlayInfo_t m_playInfo;               // +0x7c（0x10）
    std::map<int, int> m_hellPartyGroup;      // +0x8c
    char m_assignNPCByPotion;         // +0xa4
    char m_padA5[3];                  // +0xa5
    std::map<int, int> m_mapA8;       // +0xa8
    std::vector<int> m_vecC0;         // +0xc0
    short m_cc;                       // +0xcc
    char m_padCE[2];                  // +0xce
    std::multimap<int, map_monster> m_tournamentMob;  // +0xd0
    bool m_layeredMap;                // +0xe8（P292 档②：ORIG GetMapInfoFromPos
                                      //   0x0830cd42 直判 `test %al,%al; je` 无
                                      //   setne ⇒ bool；同宽 1B，偏移/sizeof 不变）
    char m_padE9[3];                  // +0xe9

    MapInfo();                                // ORIG 08151394
    MapInfo(const MapInfo& other);            // ORIG 08235d40
    ~MapInfo();                               // ORIG 081518bc
    MapInfo& operator=(const MapInfo& other); // ORIG 08151946
    void Clear();                             // ORIG 081514ae
    void Add_Mob(map_monster mob);            // ORIG 08151612（按值）
    void Add_Item(map_item item);             // ORIG 081517e0（按值）
    void SelectDonsterItemDropLimit(unsigned int limit);  // ORIG 0830e800
};

// ---- WarField（0xf8 字节；WarRoom +0x34） ----
class WarField
{
public:
    WarField();   // ORIG 086c0b8e
    ~WarField();  // ORIG 086c0ba4

    void Reset();                                    // 086b8a14
    void ConsistMap(void* packet, const CDungeon* dungeon, const CMap* map,
                    unsigned char a, int playerCount);  // 086b8a38
    void MakePacket(int packetId, void* packet);     // 086b92e2
    bool IsGridClear();                              // 086b954c
    bool GetFieldMob(int idx, map_monster& out);     // 086b9578
    bool HandleMonsterKill(int uid, PacketGuard& packet, map_monster& mob,
                           CUser* user, const CDungeon* dungeon);  // 086b9672
    // 086b9fe6；HandleGetItem 调用点 `mov %al,-0x25` bool 槽直落（无 test/setne）⇒ bool 返回。
    bool GetFieldItem(int idx, map_item& out);
    void PickupItem(std::map<int, map_item>::iterator it);  // 086ba082
    int CheckPickupItem(int idx, int owner,
                        std::map<int, map_item>::iterator& out);  // 086ba0a0
    int DropItem(map_item item);                     // 086ba190
    int HandleWpPerMonster(CUser* user, int a, int b, map_monster& out);  // 086ba1ea

    char m_pad0[8];            // +0x00
    MapInfo m_mapInfo;         // +0x08（0xec）
    int m_stageIdx;            // +0xf4
};

// 尺寸断言暂缓（布局以 check_tu_game_orig.sh 对 ORIG 的逐函数比对为准）

#endif  // DNF_GAME_WARFIELD_H_
