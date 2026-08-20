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
struct map_item
{
    int m_count;              // +0x00
    int m_itemIndex;          // +0x04
    unsigned short m_dropIndex;  // +0x08
    char m_pad0a[2];          // +0x0a
    int m_createTick;         // +0x0c
    Inven_Item m_item;        // +0x10（0x3d 字节）
    char m_pad4d[3];          // +0x4d
    int m_ownerId;            // +0x50（ctor=0）

    map_item();
    map_item(const map_item& other);
    ~map_item();
    map_item& operator=(const map_item& other);
};

// ---- map_monster（0x34c 字节；ctor 081511c8） ----
struct map_monster
{
    int m_mobId;              // +0x00
    int m_instId;             // +0x04（实例序号，短整使用）
    char m_roleType;          // +0x08
    char m_pad9[3];           // +0x09
    int m_mobIndex;           // +0x0c（击杀逻辑的怪物索引）
    char m_level;             // +0x10（byte）
    short m_field11;          // +0x11
    char m_field13;           // +0x13
    char m_field14;           // +0x14
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
    short m_hellPartyGroupOrder;    // +0x336
    char m_hellPartyScriptIdx;      // +0x338
    char m_pad339;                  // +0x339
    short m_bloodSmallRoundOrder;   // +0x33a
    short m_bloodPhaseTime;         // +0x33c
    short m_bloodRoundOrder;        // +0x33e
    short m_bloodSpawnIdx;          // +0x340
    short m_bloodPhaseIdx;          // +0x342
    char m_bloodKillFlag;           // +0x344
    char m_bloodDifficulty;         // +0x345
    short m_bloodRoundOrder2;       // +0x346
    char m_bloodSpawned;            // +0x348
    char m_isBoss;                  // +0x349
    char m_bloodRound;              // +0x34a
    char m_bloodRoundFlag;          // +0x34b

    map_monster();                          // ORIG 081511c8
    map_monster(const map_monster& other);  // ORIG 08151aea
    ~map_monster();                         // ORIG 081515ae
    map_monster& operator=(const map_monster& other);  // ORIG 08151d3a
    bool isNamedMonster() const;            // ORIG 08151182
};


// ---- MapInfo 支撑结构（0x18/0x10；init ORIG 081512ec/0815132c） ----
struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    int m_dieCnt;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14
    void init();                           // ORIG 081512ec
};

// 4 个 int（0x10 字节）
struct stMapPlayInfo_t
{
    unsigned int m_startTick;   // +0x00
    unsigned int m_clearTick;   // +0x04
    int m_useItemType2Count;    // +0x08
    int m_useItemOtherCount;    // +0x0c
    void init();               // ORIG 0815132c
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
    char m_dummyBossMap;              // +0x45
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
    char m_layeredMap;                // +0xe8
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
    int GetFieldItem(int idx, map_item& out);        // 086b9fe6
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
