#ifndef GAME_CDATAMANAGER_H_
#define GAME_CDATAMANAGER_H_

// df_game_r CDataManager（全服数据仓库）还原（2026-08-16）。
// 布局依据 ORIG 构造/析构汇编逐成员推导；ST*Script 属后续脚本批次，
// 此处仅给最小声明（尺寸 + 本类函数需要访问的字段）。

#include <bitset>
#include <ext/hash_map>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <sstream>   // RegenerationROI::_RandomLevel(std::stringstream&）签名需要

#include "GameTypes.h"
#include "CEnvironment.h"
#include "CItemList.h"
#include "CItemGeneratorMgr.h"
#include "STQuestScript.h"   // TrainingQuestScript 唯一声明点（0x1490 权威布局；CDataManager 成员 m_trainingQuest）
#include "itemData.h"       // eventReward itemData 的唯一完整定义（0x0c）

class CUser;
class CMap;
class CDungeon;
class CItem;
class CMonster;
class CSkill;
class CWorldMap;
class CItemLimitEditionMgr;
class Quest;
class CMTRand;   // 全局 MT19937（PvP_deps.h/CMTRand.cpp 定义）

namespace secretshop
{
struct SALE_INFO;   // CSecretShop.cpp 定义（0x1c）
}

namespace WongWork
{
class CQuestClear;
}

struct stSelectQuestParam;

// ORIG PVPMAP_DATA ctor C1Ev 0x0891c7f8（仅 string ctor）+ copy C1ERKS_ 0x0892a2d4
// （拷贝 +0 string 与 +4 int）——首成员为 std::string，非 int。
struct PVPMAP_DATA
{
    std::string m_name;   // +0x00（loadPvpMapBasicDataScript push 前 clear 置空）
    int m_mapIdx;         // +0x04（[pvp map order] 扫描值）
};                        // sizeof 8

struct PvPMapInfo
{
    PvPMapInfo();
    int m_mapIdx;    // +0x00
    char m_playable; // +0x04
};

enum ENUM_QUEST_GRADE
{
    ENUM_QUEST_GRADE_NONE = 0,
    ENUM_QUEST_GRADE_1 = 1,
    ENUM_QUEST_GRADE_2 = 2,
    ENUM_QUEST_GRADE_4 = 4,
    ENUM_QUEST_GRADE_5 = 5,
    ENUM_QUEST_GRADE_6 = 6,
    ENUM_QUEST_GRADE_8 = 8
};

class CMapList
{
public:
    CMapList();
    ~CMapList();
    void destroy();
    int insert_map(int channel, int key, CMap* map,
                   std::vector<PVPMAP_DATA>& pvpData);
    CMap* find_map(int idx);
    static void LoadPvPSinglePlayableMap(char* path);   // ORIG _ZN8CMapList24LoadPvPSinglePlayableMapEPc（调用点无 this → 静态）
    static int GetPvpMapInfo(int idx, PvPMapInfo& info);
    CMapList* GetMapList();
    int GetPVPMapCount() const;

    __gnu_cxx::hash_map<int, CMap*> m_mainMap;         // +0x00
    __gnu_cxx::hash_map<int, CMap*> m_mapList[16];     // +0x14
    __gnu_cxx::hash_map<int, CMap*> m_typeMap1[16];    // +0x154
    __gnu_cxx::hash_map<int, CMap*> m_typeMap2[16];    // +0x294
    int m_pvpMapCount;                                 // +0x3d4

    static std::map<int, PvPMapInfo> mapPvpMapInfo_;   // ORIG 0x943c14c
};
// GCC4.4 下 offsetof 不能用于含非平凡构造/析构成员（hash_map/multimap 等）的类：
// 对非标准布局成员取 offsetof 会触发 "invalid access to non-static data member of
// NULL object"（UB，且 check_tu 环境下视为错误）。故用标准布局镜像校验 ORIG 布局：
// 镜像成员为同尺寸 char 占位（尺寸取自真实成员类型的 sizeof），与真实类同尺寸、
// 同对齐、逐成员同偏移；对镜像取 offsetof 即等价验证真实 ORIG 偏移，GCC4.4 无警告。
struct CMapListLayoutMirror
{
    char m_mainMap[sizeof(__gnu_cxx::hash_map<int, CMap*>)];
    char m_mapList[16][sizeof(__gnu_cxx::hash_map<int, CMap*>)];
    char m_typeMap1[16][sizeof(__gnu_cxx::hash_map<int, CMap*>)];
    char m_typeMap2[16][sizeof(__gnu_cxx::hash_map<int, CMap*>)];
    int  m_pvpMapCount;
};
static_assert(sizeof(CMapList) == 0x3d8, "CMapList size");
static_assert(sizeof(CMapList) == sizeof(CMapListLayoutMirror),
              "CMapList layout mirror size");
static_assert(offsetof(CMapListLayoutMirror, m_mainMap) == 0x00,
              "CMapList::m_mainMap offset");
static_assert(offsetof(CMapListLayoutMirror, m_mapList) == 0x14,
              "CMapList::m_mapList offset");
static_assert(offsetof(CMapListLayoutMirror, m_typeMap1) == 0x154,
              "CMapList::m_typeMap1 offset");
static_assert(offsetof(CMapListLayoutMirror, m_typeMap2) == 0x294,
              "CMapList::m_typeMap2 offset");
static_assert(offsetof(CMapListLayoutMirror, m_pvpMapCount) == 0x3d4,
              "CMapList::m_pvpMapCount offset");

class CMonsterManager
{
public:
    CMonsterManager();
    ~CMonsterManager();
    void destroy();
    int load_monsters();
    int insert_monster(int level, int monsterIdx);
    bool insert_monster(int level, CMonster* monster);
    CMonster* find_monster(int idx);
    int get_levelrandom_mob(int level);

    std::multimap<int, int> m_levelMonsterList;        // +0x00
    __gnu_cxx::hash_map<int, CMonster*> m_monsterMap;  // +0x18
};
struct CMonsterManagerLayoutMirror
{
    char m_levelMonsterList[sizeof(std::multimap<int, int>)];
    char m_monsterMap[sizeof(__gnu_cxx::hash_map<int, CMonster*>)];
};
static_assert(sizeof(CMonsterManager) == 0x2c, "CMonsterManager size");
static_assert(sizeof(CMonsterManager) == sizeof(CMonsterManagerLayoutMirror),
              "CMonsterManager layout mirror size");
static_assert(offsetof(CMonsterManagerLayoutMirror, m_levelMonsterList) == 0x00,
              "CMonsterManager::m_levelMonsterList offset");
static_assert(offsetof(CMonsterManagerLayoutMirror, m_monsterMap) == 0x18,
              "CMonsterManager::m_monsterMap offset");

class CDungeonList
{
public:
    CDungeonList();
    ~CDungeonList();
    int load_list();
    void destroy();
    bool insert_dungeon(CDungeon* dungeon);
    CDungeon* find_dungeon(int idx) const;
    CDungeon* get_dungeon_at_index(int idx) const;
    CDungeonList* getDungeonIndexList();
    int size() { return m_dungeonMap.size(); }

    __gnu_cxx::hash_map<int, CDungeon*> m_dungeonMap;  // +0x00
};
struct CDungeonListLayoutMirror
{
    char m_dungeonMap[sizeof(__gnu_cxx::hash_map<int, CDungeon*>)];
};
static_assert(sizeof(CDungeonList) == 0x14, "CDungeonList size");
static_assert(sizeof(CDungeonList) == sizeof(CDungeonListLayoutMirror),
              "CDungeonList layout mirror size");
static_assert(offsetof(CDungeonListLayoutMirror, m_dungeonMap) == 0x00,
              "CDungeonList::m_dungeonMap offset");

struct stDBLogItem
{
    unsigned int m_itemIdx;  // +0x00
    int m_time;              // +0x04
    int m_value;             // +0x08
};

// ===================== 脚本子结构 POD（ORIG 全局作用域） =====================
// 各元素仅承载 vector/map 模板参数名以匹配 ORIG mangled 符号；字段布局未逐字段
// 确认（仅容器语义），元素体为最小占位（[推断]）。

#ifndef GAME_SITEMGENREF_DEFINED
#define GAME_SITEMGENREF_DEFINED
// sItemGenRef：ServerParameterScript.h 的 m_genRefs 元素类型，须先于其定义。
struct sItemGenRef
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldc;    // +0x0c 固定 1000（无消费方）
    int m_dropRate;  // +0x10 掉落率（脚本第 4 值；SetItemDropRate 按比例缩放）
};
#endif

// EventRewardSection/EventRewardItemInfo 定义于 ServerParameterScript.h。
#include "ServerParameterScript.h"
#include "advancealtar.h"
// stTitleBookInfo 的 stTitleSection/stTitleElement/stAdjust/stTitleRewardBasis 定义于本头。
#include "CTitleBook.h"

// STExpReward 权威唯一定义点已下沉至 STExpReward.h（叶子头）：CUser.h 按值
// 成员需要完整类型，而本头经 CItemGeneratorMgr→CItemGenerator→PvPTypes→CUser.h
// 存在重入环，不能作为 CUser.h 的可见性来源。本头仍经此 include 获得完整类型。
#include "STExpReward.h"

// ---- 公会/称号域 POD（InitScripts_3 按 import* 装载函数报告定稿布局）----

// ORIG C1 0x8979978 / clear 0x897994c / D1 0x897a678；
// importGuildParameterScript [agit upgrade info]（0x8978d48 +0x464..+0x4c4）
// 每条扫描 4 个 int（level/cost/时间/附加），sizeof 0x10。
struct STAgitUpgradeInfo
{
    STAgitUpgradeInfo();
    ~STAgitUpgradeInfo();
    void clear();
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldC;    // +0x0c
};

// ORIG C1 0x89799c2 / clear 0x8979898 / D1 0x897a88a；
// importGuildParameterScript [guild storage upgrade info]（+0x4f6..+0x592）
// 每条以 ScanInt(&ok) 起头共 5 个 int，sizeof 0x14。
struct STGuildStorageUpgradeInfo
{
    STGuildStorageUpgradeInfo();
    ~STGuildStorageUpgradeInfo();
    void clear();
    int m_field0;    // +0x00（ok 校验的首字段）
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldC;    // +0x0c
    int m_field10;   // +0x10
};

// ORIG C1 0x8aa9928（string 成员默认构造 + 两 int 置 -1）/
// D1 0x83e629c / copy 0x8aaa67c；importTitleBookEtcScript
// [custom condition message]（0x8aa88b7..0x8aa8945）每条填
// {convertTabTypeStringToEnum 结果, ScanInt, ScanStr}，sizeof 0x14。


// ORIG C1 0x8aa9928（string 成员默认构造 + 两 int 置 -1）/
// D1 0x83e629c / copy 0x8aaa67c；importTitleBookEtcScript
// [custom condition message]（0x8aa88b7..0x8aa8945）每条填
// {convertTabTypeStringToEnum 结果, ScanInt, ScanStr}，sizeof 0x14。
struct stCustomConditionMsg
{
    stCustomConditionMsg();   // ORIG C1 0x8aa9928：两 int 置 -1
    int m_tabType;        // +0x00（ctor 置 -1）
    int m_value;          // +0x04（ctor 置 -1）
    std::string m_msg;    // +0x08
};
// stMercenaryRewardGroup（ORIG C1 0x89f7a9c / D1 0x83dd67c）
// +0x00 int +0x04 string +0x08 vector<pair<int,int>> +0x14 vector<pair<int,int>>
// +0x20/+0x24 两个掉落范围 dword，sizeof 0x28。
// importMercenarySystemEtcScript [reward group]/[mob index]/[item index] 实证。
struct stMercenaryRewardGroup
{
    stMercenaryRewardGroup();
    int m_groupId;                                  // +0x00
    std::string m_name;                             // +0x04
    std::vector<std::pair<int, int> > m_mobList;    // +0x08（[mob index] 对）
    std::vector<std::pair<int, int> > m_itemList;   // +0x14（[item index] 对）
    unsigned int m_field20;                        // +0x20（mob 掉落范围）
    unsigned int m_field24;                        // +0x24（item 掉落范围）
};

// ORIG C1 0x89f7b28 / D1 0x83dd70e：+0x00 ushort +0x04 vector<stMercenaryRewardGroup>
// +0x10 int（Σ groupId）+0x14 ushort，sizeof 0x18。
struct stMercenaryCompetitionArea
{
    stMercenaryCompetitionArea();
    unsigned short m_worldMapIdx;                    // +0x00（[world map]）
    std::vector<stMercenaryRewardGroup> m_groups;    // +0x04（[/reward group] push_back）
    int m_groupIdSum;                                // +0x10（[/competition area] 写 Σ groupId）
    unsigned short m_areaLevel;                      // +0x14（[area level]）
};
struct STBreakAwaySection         // ORIG C1 0x822afb0（清零）；get_break_away_section_data
{                                 //   sret memmove 3 字节实证 → sizeof 0x3
    STBreakAwaySection() { m_field0 = m_field1 = m_field2 = 0; }  // ORIG 链式清零形态
    char m_field0;                // +0x00
    unsigned char m_field1;       // +0x01（ORIG get_break_away_section_data/low_level
                                  //  对 m_field1 的 movzbl 零扩展实证 ⇒ 无符号比较）
    char m_field2;                // +0x02
};
// ORIG C1 0x891b762（4×int 逐个清零）/ D1 0x893c682（trivial ret）。
// importCharacLinkSystemScript（0x8909529）按 +0x00/+0x04/+0x08/+0x0c 连续 ScanInt。
struct stCharacLinkSystemData
{
    stCharacLinkSystemData() { m_field0 = 0; m_field4 = 0; m_field8 = 0; m_fieldc = 0; }
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldc;   // +0x0c
};
// ORIG C1 0x8a6c146 / D1 0x8a6d686；importQuestParameterScript [piece reward]
// （0x8a6b660 +0x4d5..+0x546）：appendage=loadAppendageType(token)、
// valueType=loadValueType(ScanStr)、value=ScanFloat，sizeof 0xc。
struct pieceQuestRewardStatus
{
    // ORIG C1 0x8a6c146（W）：m_appendageType=0x4d、m_valueType=2、m_value=0。
    pieceQuestRewardStatus()
    {
        m_appendageType = 0x4d;
        m_valueType = 2;
        m_value = 0;
    }
    int m_appendageType;   // +0x00
    int m_valueType;       // +0x04
    float m_value;         // +0x08
};
// ---- stRankSystemInfo 内嵌元素（ORIG importRankSystemScript 0x0890ed7f 逐字段实证）----
// stDungeonClearTimeData：7 个 int（0x1c）；stClearTimeAverage：5 个 int（0x14）；
// stBonusPointValues：2 个 int；stComboIntervalBoundary：4 个 int（0x10）。
struct stClearTimeAverage         { int m_v[5]; };   // +0x00..0x13
struct stBonusPointValues         { int m_a; int m_b; };
// ---- stUnlimitChallengeInfo 元素（ORIG importUnlimitChallengeScript 0x0890bc63 实证）----
struct stUnlimitChallengeRewardInfo
{
    unsigned short m_jobLevel;   // +0x00（16 位写入：首列扫描值）
    unsigned char m_subIdx;      // +0x02（字节写入：段常量 0..6）
    /* +0x03 pad */
    int m_a;                     // +0x04
    int m_b;                     // +0x08
};                               // sizeof 0xc
struct stUnlimitChallengeMessageInfo
{
    short m_id;                  // +0x00（16 位写入）
    std::string m_msg1;          // +0x04
    std::string m_msg2;          // +0x08
};                               // sizeof 0xc
struct stUnlimitChallengeItem
{
    int m_a;                     // +0x00
    int m_b;                     // +0x04
};                               // sizeof 8
// ORIG PvPSkillTreeValue ctor 0x08a5e7a0（W）：仅构造两个 vector<pair<int,int>>
//   @+0x00 与 +0x0c（sizeof 0x18）。getPvPSkillTree 0x8a5db5a 以 value_type
//   基准取 +0x04/+0x10，即本结构 m_vec00/m_vec0c。
struct PvPSkillTreeValue
{
    std::vector<std::pair<int, int> > m_vec00;   // +0x00（[skill] 段填充）
    std::vector<std::pair<int, int> > m_vec0c;   // +0x0c（[static basic skill] 段）
};                                   // sizeof 0x18
// ORIG C1：构造成员后调 clear()。
struct FAIR_PVP_SKILL_TREE_SKILL_PER_POINT
{
    // ORIG ctor 0x08a5e7e6 / clear 0x08a5e874：map<int,int> @+0x00、
    // map<int,int> @+0x18、vector<pair<int,int>> @+0x30、int @+0x3c 置 0
    // （sizeof 0x40）。
    FAIR_PVP_SKILL_TREE_SKILL_PER_POINT();
    void clear();                        // ORIG W 0x08a5e874
    std::map<int, int> m_addSkills;                    // +0x00（加技能→等级表）
    std::map<int, int> m_removeSkills;                 // +0x18（减技能→等级表）
    std::vector<std::pair<int, int> > m_giveSkills;    // +0x30（getGiveSkill 0x8a5e178 返回值）
    int m_pointCost;                                   // +0x3c（getPvPSkillPoint 0x8a5dd62 点数消耗）
};                                   // sizeof 0x40
struct stMonsterIndependentDrop_t
{
    // ORIG sizeof 0x48（__copy_m 元素步长 $0x48 实证）；默认 ctor（0x897cbc4）
    // 仅构造 +0x30 的 vector<pair<int,int>>，其余 int 不初始化。
    int m_itemIdx;   // +0x00（掉落 itemId；PrintItemDropList find_item）
    // +0x04..+0x2c：ORIG 全部访问均为 base(+0x04)+idx*4 动态下标，故建为单一
    // int 表（generateIndependentItem 0x834989a / independentItemRateControl
    // 0x834972f 寻址一致）：
    //   [0..4] 分档掉率（PrintItemDropList 打印 /10000%）；
    //   [5..8] 期望值标量（PrintItemDropList 打印 +0x18..+0x24）；
    //   [9]    单次生成数量上限（std::min 边界，+0x28）；
    //   [10]   主掉率（get_rand_int 上界；ResultToStr 非 0 判定，+0x2c）。
    int m_rate[11];
    std::vector<std::pair<int, int> > m_pairList;  // +0x30（itemId 累积概率表）
    int m_rangeMin;              // +0x3c（范围过滤下界，与 refData 比较）
    int m_rangeMax;              // +0x40（范围过滤上界）
    int m_levelFilter;           // +0x44（>=0 时与 stGenerateRefData_t::m_field28 比对）
};                               // sizeof 0x48
// ORIG IndependentDropListInfo ctor 0x0897cbae（W）：int @+0x00 +
// vector<pair<int,int>> @+0x04（sizeof 0x10）；[list] 段以 (itemId, 累积率) 填充。
struct IndependentDropListInfo
{
    int m_field0;                                    // +0x00（脚本文件列表索引）
    std::vector<std::pair<int, int> > m_list04;      // +0x04
};                               // sizeof 0x10
struct stIndependentDropInfo
{
    int m_field0;                    // +0x00
    int m_monsterIndex;              // +0x04
    stMonsterIndependentDrop_t m_drop;  // +0x08
};
struct stIndependentDropRateControl
{
    // ORIG independentItemRateControl 0x834972f 读 +0x00/+0x04/+0x08 三个 int；
    // vector 元素步长为 0x54，故 +0x0c 起还包含完整 stMonsterIndependentDrop_t。
    int m_itemId;    // +0x00（与 stMonsterIndependentDrop_t::m_itemIdx 匹配）
    int m_field04;   // +0x04（等级差阈值：仅当 m_field04 > diff 时生效）
    int m_field08;   // +0x08（衰减系数：10000 - m_field08*diff 为比率因子）
    stMonsterIndependentDrop_t m_drop0C; // +0x0c，sizeof 0x48
};
struct SlotBound
{
    SlotBound() : m_field0(0), m_field4(0), m_field8(false) {}
    SlotBound(int start, int end, bool enabled)
    {
        m_field0 = start;
        m_field4 = end;
        m_field8 = enabled;
    }
    int m_field0;            // +0x00
    int m_field4;            // +0x04
    unsigned char m_field8;  // +0x08
};                           // sizeof 0xc
// ORIG C1 0x891b682：+0x00=-1、+0x04=0、+0x08/+0x09 字节=0（sizeof 0xc）。
// importConditionEventScript [condition reward]/[condition quick party add reward]
// （0x890e868）：f0/f4=ScanInt×2，b8=(v3!=0)、b9=(v4!=0)。
struct stConditionEventRewardInfo
{
    stConditionEventRewardInfo() { m_field0 = -1; m_field4 = 0; m_flag8 = false; m_flag9 = false; }
    int m_field0;        // +0x00（ctor 置 -1）
    int m_field4;        // +0x04
    bool m_flag8;        // +0x08（导入时 =(第三 ScanInt != 0)）
    bool m_flag9;        // +0x09（导入时 =(第四 ScanInt != 0)）
};
// ORIG C1 0x0891b84c→clear 0x0891b860（逐 int 清零，sizeof 0x20）；
// ImportItemMakingSkill [upgrade with compound]（0x0890f564 +0x1b1..+0x346）每条：
//   key=ScanInt；f00/f04/f08=ScanInt×3；f0C=(f04-f00)*1000+999；
//   f10/f14/f18=ScanInt×3；f1C=(f14-f10)*1000+999。
struct stUpgradeWithCompound
{
    stUpgradeWithCompound() { clear(); }
    void clear()
    {
        m_field00 = 0; m_field04 = 0; m_field08 = 0; m_rangeExp0C = 0;
        m_field10 = 0; m_field14 = 0; m_field18 = 0; m_rangeExp1C = 0;
    }
    int m_field00;      // +0x00
    int m_field04;      // +0x04
    int m_field08;      // +0x08
    int m_rangeExp0C;   // +0x0c（导入时 =(m_field04-m_field00)*1000+999）
    int m_field10;      // +0x10
    int m_field14;      // +0x14
    int m_field18;      // +0x18
    int m_rangeExp1C;   // +0x1c（导入时 =(m_field14-m_field10)*1000+999）
};
// ORIG C1 0x891b1be（两 int 清零，sizeof 0x8）。
// importBloodDungeonScript [ultimate reward prob]（0x8909016 +0x2dc..+0x2fd）：
//   m_field0/m_field4=ScanInt((bool*)0)×2。
struct STUltimateRewardProb
{
    STUltimateRewardProb() { m_field0 = 0; m_field4 = 0; }
    int m_field0;   // +0x00
    int m_field4;   // +0x04
};
// ORIG LoadRewardDataByNoType 0x08917c99：栈上临时 {int@-0x1c,int@-0x18} 两 int
// 连续写入后整体 push_back/set_data（元素步长 8 实证）。
struct StackableItemData
{
    StackableItemData();   // ORIG C1 0x0891bccc（调 clear）
    void clear();          // ORIG 0x0891bce0：+0x00=-1、+0x04=0
    int m_field0;   // +0x00（[推断] 道具 idx）
    int m_field4;   // +0x04（[推断] 数量）
};
// ORIG LevelUpRewardItemData ctor 0x081b88bc→clear 0x081b88d0：+0x00=-1、+0x04=0；
// LoadRewardDataByGrowType/ImportLevelUpRewardItemScript 每条写两 int（步长 8）。
// ORIG 布局（loadNPCCommonData 0x089fafc5 实证，sizeof 0xc）：
// push_back 元素 = {getChangeStatusType 结果@+0, ScanInt 值@+4, bool(串!="+")@+8}。
struct LevelUpRewardItemData
{
    // ORIG W ctor 0x081b88bc：调 clear()（+0x00=-1、+0x04=0）。
    LevelUpRewardItemData() { clear(); }
    void clear()
    {
        m_field0 = -1;
        m_field4 = 0;
    }
    int m_field0;   // +0x00（clear 置 -1；[推断] 道具 idx）
    int m_field4;   // +0x04（clear 置 0；[推断] 数量）
};
// _Select_Part 报告实证：pair+0xc（=second+0x8）为 vector<string>；
// importRegenerationRandomOptionScript 0x08a7e610 实证：second+0x4 为
// std::string（operator[] 命中后 operator= 写入），second+0x8 push_back。
// second+0x00..0x03 未观测，保留占位。
struct str_index_to_itemgroup
{
    char m_pad0[4];                        // +0x00（未还原字段区）
    std::string m_name4;                   // +0x04（importRegenerationRandomOptionScript 写入）
    std::vector<std::string> m_vec8;       // +0x08（item group 名表）
};  // sizeof 0x14
// _Calculate_RegenCost 报告实证：pair.second 两 int——f0==0 时取 f1 计费。
struct _str_cost
{
    // ORIG C1/C2 0x8a7ea82（W）：两 int 形参顺序赋入 +0/+4。
    _str_cost(int a, int b)
    {
        m_field0 = a;
        m_field4 = b;
    }
    // 内联默认 ctor：ORIG map operator[]（0x8a7ee34 实例化）经值初始化新建
    // 元素，零初始化两 int（ORIG 无独立默认 ctor 符号，内联不发射）。
    _str_cost() : m_field0(0), m_field4(0) {}
    int m_field0;   // +0x00
    int m_field4;   // +0x04
};  // sizeof 8
// RegenerationROI::RegenVer_get_option_value 报告实证 12 字节三字。
struct _str_option_value_ratio
{
    // ORIG C1/C2 0x8a7ea98（W）：三形参顺序赋入 +0/+4/+8。
    _str_option_value_ratio(float a, float b, int c)
    {
        m_scaleRange0 = a;
        m_scaleBase4 = b;
        m_weight8 = c;
    }
    float m_scaleRange0;      // +0x00（以 (max-min) 加权）
    float m_scaleBase4;       // +0x04（以 min 加权）
    unsigned int m_weight8;   // +0x08（累积权重）
};  // sizeof 0xc
// ORIG D1 0x8a5a4f8；importPowerParameterScript [straight victory penalty]
// （0x8a592bc +0x848..+0x8cc）每条连续扫描 4 个 int（均 ScanInt(&ok) 校验），
// sizeof 0x10。
struct stVictoryPenalty
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    int m_field8;    // +0x08
    int m_fieldC;    // +0x0c
};

// map key 类型（需要 operator<；字段布局 [推断]）
struct stClearTimeAverageMapKey
{
    int m_a;   // [推断]
    int m_b;   // [推断]
    bool operator<(const stClearTimeAverageMapKey& o) const
    {
        // ORIG W 0x08370d48：jge/mov1/jmp - jne/jge/mov1/mov0（嵌套短路；int 有符号比较）；
        // 单行 || 会生成 jl 极性，与 ORIG 不符。
        if (m_a < o.m_a) return true;
        if (m_a == o.m_a && m_b < o.m_b) return true;
        return false;
    }
};

// ORIG operator< _ZNK21stUnlimitChallengeKeyltERKS_ 0x0891b454：
// 先比 +0 ushort（movzwl），相等再比 +2 字节（movzbl）。
struct stUnlimitChallengeKey
{
    unsigned short m_a;   // +0x00（16 位写入）
    unsigned char m_b;    // +0x02（字节写入：段常量 0..6）
    bool operator<(const stUnlimitChallengeKey& o) const
    {
        // ORIG 0x0891b454：movzwl 判 +0 ushort，相等再 movzbl 判 +2 字节（嵌套短路）。
        if (m_a < o.m_a) return true;
        if (m_a == o.m_a && m_b < o.m_b) return true;
        return false;
    }
};

struct PvPSkillTreeKey
{
    // ORIG ctor 0x08a5e74e（W）：三字节 @+0/+1/+2 逐字节清零（sizeof 4）；
    // importPairPvPSkillPointScript 0x08a5e226 以 mov %al 写三段
    // （[job index]/[grow type index]/[awakening type]），initFairPVPSkillPoint
    // 0x835f484 以 movzbl (%eax) 读 job 首字节。
    unsigned char m_a;   // +0x00（job）
    unsigned char m_b;   // +0x01（firstGrow / grow type index）
    unsigned char m_c;   // +0x02（secondGrow / awakening type）
    PvPSkillTreeKey() { m_a = 0; m_b = 0; m_c = 0; }
    PvPSkillTreeKey(unsigned char a, unsigned char b, unsigned char c)  // ORIG W 0x08a5e768（Ehhh）
        : m_a(a), m_b(b), m_c(c) {}
    bool operator<(const PvPSkillTreeKey& o) const
    {
        // ORIG 0x08a5e6d8：三相 jae/mov1/jmp - jne ret0 - jae/mov1/jmp - jne ret0 - jae ret0/mov1（嵌套相等块）；
        // 原 if(o.m_a<m_a)return false 生成交换操作数的比较，与 ORIG 不符。
        if (m_a < o.m_a) return true;
        if (m_a == o.m_a) {
            if (m_b < o.m_b) return true;
            if (m_b == o.m_b) {
                if (m_c < o.m_c) return true;
            }
        }
        return false;
    }
};

// ORIG 布局（eventReward::ImportRewardScript 0x080ef3a2 实证，sizeof 5）：
// 栈键 -0x1a9..-0x1a5 五字节；[/sub type] 后 rewardSubKey::clear 仅清 +3/+4，
// [/job type] 后整键 clear。operator< 比较序为「推断」（ORIG operator< 未反汇编，
// 保持旧 m_a/m_b 先行语义映射到新字段）。
struct rewardSubKey
{
    // ORIG C1/C2 0x080f1190（W）：两字节清零。
    rewardSubKey()
    {
        m_type = 0;
        m_action = 0;
    }

    unsigned char m_type;     // +0x00（getSubKeyType）
    unsigned char m_action;   // +0x01（getSubKeyAction）

    void clear() { m_type = 0; m_action = 0; }   // ORIG W 0x080f11de 语义

    // ORIG W 0x080f11a2 _ZNK12rewardSubKeyltERKS_：字典序（m_type 优先）。
    bool operator<(const rewardSubKey& other) const;   // 定义 OnlyOrigBatch12.cpp
};

struct rewardKey
{
    // ORIG C1/C2 0x080f11f0（W）：m_job/m_a/m_b 清零后构造 m_sub
    // （初始化列表按声明序：标量先行、子对象后构，与 ORIG 指令序一致）。
    rewardKey()
        : m_job(0), m_a(0), m_b(0), m_sub()
    {
    }

    unsigned char m_job;     // +0x00（[job type]：getCharacterJobToEnumValue）
    unsigned char m_a;       // +0x01（[job type] ScanInt#1）
    unsigned char m_b;       // +0x02（[job type] ScanInt#2）
    rewardSubKey m_sub;      // +0x03（[/sub type] 两字节）

    void clear()   // ORIG W 0x080f12ac 语义（[/job type] 调用）
    {
        m_job = 0;
        m_a = 0;
        m_b = 0;
        m_sub.clear();
    }
    bool operator<(const rewardKey& o) const
    {
        if (m_job != o.m_job) return m_job < o.m_job;
        if (m_a != o.m_a) return m_a < o.m_a;
        if (m_b != o.m_b) return m_b < o.m_b;
        // ORIG 0x080f1267：调用外部 rewardSubKey::operator<（OnlyOrigBatch12.cpp）
        return m_sub < o.m_sub;
    }
};
struct SlotBoundKey
{
    SlotBoundKey() : m_a(0), m_b((Inven_Item::ITEM_TYPE)0) {}
    SlotBoundKey(unsigned short a, Inven_Item::ITEM_TYPE b) : m_a(a), m_b(b) {}
    unsigned short m_a;              // +0x00
    Inven_Item::ITEM_TYPE m_b;       // +0x04
    bool operator<(const SlotBoundKey& o) const
    {
        if (m_a < o.m_a)
            return true;
        if (m_a == o.m_a && m_b < o.m_b)
            return true;
        return false;
    }
};

// ORIG 实布局（ctor 0x0891bac0→clear 0x0891bad4：+0=0xc8/+2=0xb/+3=0；operator<
// 0x081b8830 依次比较 +0x02、+0x03、+0x00(short)），sizeof 4。
struct LevelUpRewardItemTableKey
{
    unsigned short m_lv;     // +0x00（clear 默认 200）
    unsigned char m_job;     // +0x02（clear 默认 0xb）
    unsigned char m_grow;    // +0x03
    LevelUpRewardItemTableKey() { clear(); }   // ORIG C1 调 out-of-line weak clear()（-O0 不内联）
    void clear()
    {
        m_grow = 0;
        m_job = 0xb;
        m_lv = 0xc8;
    }
    // ORIG W 0x081b8896 C2Eiii：(+0)=level 截断 ushort、(+2)=job 字节、(+3)=grow 字节。
    LevelUpRewardItemTableKey(int level, int job, int grow);   // 定义 CUserDepsB.cpp
    // ORIG W 0x081b8830 逐字段 setl（有符号）比较；字段本身无符号加载。
    bool operator<(const LevelUpRewardItemTableKey& o) const
    {
        // ORIG W 0x081b8830：逐字段 setl（有符号）比较但字段为无符号加载（movzbl/movzwl）。
        if (m_job != o.m_job) return m_job < o.m_job;
        if (m_grow != o.m_grow) return m_grow < o.m_grow;
        return m_lv < o.m_lv;
    }
};

// 嵌套 vector 容器子类型（ORIG D1 仅析构各自 vector/map 成员）：
struct stLimitItemUsageInfo
{
    // ORIG 具名 C1 0x837c6be（W）：仅构造 m_list vector。用户构造器使
    // value-init 不再 zero-init+构造双写（对齐 ORIG ExC1 0x837c736 单 call 形态）。
    stLimitItemUsageInfo() : m_list() {}
    // ORIG Add(signed char,int,int) 0x891b4a8：临时元素 +0x00 kind(字节)/+0x04 int/+0x08 int
    // 后 push_back（元素步长 0xc）。importLimitItemUsageInfo [Limit Item Usage Info] 实证。
    struct LimitInfo
    {
        unsigned char m_kind;   // +0x00
        int m_a;                // +0x04
        int m_b;                // +0x08
    };
    std::vector<LimitInfo> m_list;        // +0x00
    // ORIG W 0x891b4a8 _ZN20stLimitItemUsageInfo3AddEcii（char 形参）。
    void Add(char kind, int a, int b);    // 定义 OnlyOrigBatch12.cpp
};

struct stResetItemInfo
{
    // ORIG 具名 C1 0x837c6e6（W）：仅构造 m_list vector。
    stResetItemInfo() : m_list() {}
    // ORIG（CUser::ResetItemByScript 0x8689010 / Add(int,long) 0x891b4dc）：
    // 元素 +0x00 itemspace、+0x04 到期秒。
    struct ResetItem { int m_space; int m_resetSec; };
    std::vector<ResetItem> m_list;        // +0x00
    void Add(int space, long resetSec);           // ORIG W 0x891b4dc
};

struct stRefillItemInfo
{
    // ORIG 具名 C1 0x837c70e（W）：仅构造 m_list vector。
    stRefillItemInfo() : m_list() {}
    // ORIG Add(int,int,long) 0x891b502：元素 +0x00/+0x04/+0x08 三 int（非 ResetItem 同构）。
    // CUser::RefillItemByScript 0x86890be 实证：[0]→find_item/GetInvenData 的物品索引、
    // [1]→数量（写 Inven_Item+0x07）、[2]→与 CSystemTime::getCurSec() 比较的到期秒。
    struct RefillItem { int m_itemId; int m_count; int m_endSec; };
    std::vector<RefillItem> m_list;       // +0x00
    void Add(int itemId, int count, long endSec); // ORIG W 0x891b502
};

// stLimitItemUsageInfoEx 权威定义见 CNPCRelationEventManager 块后（2026-08-24
// 并发编辑收敛：保留后位版本，此处仅留指引注释）。

// stNPCCommonData_t（ORIG C1 0x837612e / D1 0x83761c6 / clear 0x89fae9e）：
// {maxFavorExp@0; favorExp[10]@2}（clear 循环 movw $0,0x2(%eax,%edx,2) +
// getIllustIndex 遍历实证）+0x18 favorableNpcList +0x24 illustrationVariableNpcList
struct stNPCCommonData_t
{
    struct stOneBuff_t
    {
        int m_changeStatusType;    // +0x00
        int m_value;               // +0x04
        unsigned char m_isNotPlus; // +0x08
        ~stOneBuff_t();            // ORIG W 0x08a04b54
    };                             // sizeof 0xc
    // 用户提供默认构造：值初始化仅构造容器成员、不零初始化 POD 前缀。
    // （ORIG CNPCScriptList C1 0x8376262 无 rep stos 预清——隐式构造行为；
    //  无此 ctor 时 gnu++0x 值初始化会在调用点生成 memset。）
    stNPCCommonData_t() {}
    unsigned short m_maxFavorExp;                    // +0x00 [npc favor exp] max
    unsigned short m_favorExp[10];                   // +0x02..+0x14 [npc favor exp]
    std::vector<int> m_favorableNpcList;             // +0x18 [favorable npc list]
    std::vector<int> m_illustrationVariableNpcList;  // +0x24 [illustration variable npc list]
    unsigned short m_favorSystemBeginQuest;          // +0x30 [favor system begin quest]
    unsigned short m_favorSystemBeginLevel;          // +0x32 [favor system begin level]
    unsigned short m_maxFavorLevelNpc;               // +0x34 [max favor level npc]
    char m_pad36[2];                                 // +0x36（未知字节保留 m_pad）
    std::vector<int> m_favorLevelRewardNpcList;      // +0x38 [favor level reward npc]
    std::vector<std::pair<int, std::vector<stOneBuff_t> > > m_favorDurationBuffList; // +0x44
    int getIllustIndex(int npcIdx);  // ORIG T 0x089faeee
    void clear();   // ORIG W 0x089fae9e（定义 ScriptLeafA2.cpp）
};                                                   // （loadNPCCommonData 实证）

// ORIG _Z17loadNPCCommonDataR17stNPCCommonData_tPKc T 0x089fafc5
// （定义 ScriptLeafA2.cpp；文件打开失败返 0x3c，其余 0）。
int loadNPCCommonData(stNPCCommonData_t& out, const char* path);

// RandomOptionItem_Regenerator（ORIG D1 0x85f97e2）：
// +0x00 map<int,str_index_to_itemgroup> +0x18 map<int,map<int,_str_cost>>
// +0x30 vector<int> +0x3c vector<_str_option_value_ratio>，sizeof 0x48
struct RandomOptionItem_Regenerator
{
    RandomOptionItem_Regenerator();  // ORIG T 0x85f971a：4 容器构造后 +0x48=0
    std::map<int, str_index_to_itemgroup> m_map0;      // +0x00
    std::map<int, std::map<int, _str_cost> > m_map18;  // +0x18
    std::vector<int> m_vec30;                          // +0x30
    std::vector<_str_option_value_ratio> m_vec3c;      // +0x3c
    unsigned int m_field48;                            // +0x48（ROI::RegenVer_get_option_value
                                                       //   的 *(u64*)(this+0x4c) randInt 上界）
};

// DimensionActivationRewardData（ORIG C1 0x8371348 / D1 0x83713b4）：
// +0x00 vector<EventRewardSection> +0x0c map<int,vector<vector<EventRewardItemInfo>>>
// +0x24 map<pair<int,int>,vector<vector<EventRewardItemInfo>>>，sizeof 0x3c
struct DimensionActivationRewardData
{
    std::vector<EventRewardSection> m_vec0;                                // +0x00
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > > m_mapC; // +0x0c
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > > m_map24;     // +0x24

    std::vector<std::vector<EventRewardItemInfo> >*
        getRewardGiveItemList(int job, int growType);           // T 0x08916086（GlobalData.cpp）
    // T 0x08915f9a：getRewardGiveItemList(job, growType) 非空且
    // 0<=selectionIdx<size → 返回内层 vector 指针，否则 0。
    std::vector<EventRewardItemInfo>*
        getRewardGiveItems(int selectionIdx, int job, int growType);   // T 0x08915f9a
    EventRewardSection* getSectionData(int idx);                // T 0x08915f16（GlobalData.cpp）
    std::vector<std::vector<EventRewardItemInfo> >*
        getRewardSelectItemList(int packageKey);                // T 0x08916086（GlobalData.cpp）
};

// ===================== ST* 脚本占位类型（后续批次实现） =====================

class reseal_table_t
{
public:
    reseal_table_t();
    ~reseal_table_t();

    // ORIG get_reseal_cost @0x08900b02（T）：返回重封印消耗。
    //   签名 (unsigned char, unsigned char, unsigned char)，mangled _ZN14reseal_table_t15get_reseal_costEhhh。
    //   参数映射（汇编 0xc/0x10/0x14(%ebp)）：
    //     param_1 = 强化等级索引 0..5（下标 m_arr1/m_arr2）
    //     param_2 = 等级 1..99（vector 桶索引 (param_2-1)/10）
    //     param_3 = 选项 0..6（m_arr2 乘数）
    //   返回：-1(param_2==0) / -2(param_1>5) / -3(param_2>99) / -4(param_3>6)，
    int get_reseal_cost(unsigned char param_1, unsigned char param_2,
                        unsigned char param_3);

    // ORIG get_oneshot_reseal_cost T 0x08900be0（定义 CDataManagerScripts.cpp，
    // Dispatcher_ModItemAttr_impl 调用点需要；声明曾被误删，补回）。
    int get_oneshot_reseal_cost(unsigned char param_1, unsigned char param_2,
                                unsigned char param_3);

    // 成员开放给同域装载器 importResealTable（ORIG 0x089008b8 直写三成员；
    // 可见性不影响 ABI）。
    // ORIG 0x8370742：vector<uint>@+0 + int[6]@+0xc + int[6]@+0x24
    std::vector<unsigned int> m_vec;   // +0x00（0xc 字节）
    int m_arr1[6];                     // +0x0c
    int m_arr2[6];                     // +0x24
};

#ifndef GAME_SERVERPARAMETERSCRIPT_H_
#endif

// ServerParameterScript 定义于 ServerParameterScript.h（尺寸 0x49f4，已在上方 include）。
#include "APSystemTypes.h"

class InGameAdvertisementScript
{
public:
    InGameAdvertisementScript();  // ORIG W 0x083732f0
    void clear();                  // ORIG W 0x08373304

private:
    unsigned char m_exposeConditionOfTeen[0x98]; // +0x00..+0x97

    friend bool importInGameAdvertisementEtcScript(
        InGameAdvertisementScript* s, const char* path);
};
typedef char InGameAdvertisementScript_size_0x98[
    (sizeof(InGameAdvertisementScript) == 0x98) ? 1 : -1];

class PcBangItemRentarData
{
public:
    PcBangItemRentarData();
    ~PcBangItemRentarData();

    // ORIG D1 0x837c2c0：+0x00 vector<EventRewardSection>
    // +0x0c map<int,vector<vector<EventRewardItemInfo>>>，sizeof 0x24
    std::vector<EventRewardSection> m_vec0;                     // +0x00
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > > m_mapC;  // +0x0c

    // ---- 查询（RandomRentalA2 批次/GlobalData.cpp 实现）----
    EventRewardSection* getSectionData(int idx);                // T 0x08915518
    std::vector<std::vector<EventRewardItemInfo> >*
        getRewardSelectionList(int packageKey);                 // T 0x0891559c
};

struct DimensionCoinInfo
{
    // ORIG ctor 0x0891bcb6 W：+0x00 int 清零、+0x04 字节清零（对齐后 sizeof=0x8）。
    DimensionCoinInfo() { m_field0 = 0; m_field4 = false; }
    // CDimensionActivationEvent::checkFreeRevivalCondition（ORIG 0x080eeca4）读取
    // map 命中项 value 的 +0x04 字节：!=1 表示该难度今日仍可免费复活。
    // 类型为 bool（ORIG 0x080eed10 movzbl 0x8(%eax),%eax; xor $1 取反测试，同 CDungeon.h m_4）。
    int m_field0;               // +0x00（ctor 清零）
    bool m_field4;              // +0x04（ctor 清零；true==1 表示已使用）
};

class DimensionActivationData
{
public:
    DimensionActivationData();
    ~DimensionActivationData();

    // ORIG C1 0x8371422 / D1 0x837c306：+0x00 int（=0）+0x04
    // map<int,DimensionCoinInfo> +0x1c DimensionActivationRewardData（0x3c），
    // sizeof 0x58
    int m_field0;                                       // +0x00
    std::map<int, DimensionCoinInfo> m_coinMap;         // +0x04
    DimensionActivationRewardData m_rewardData;         // +0x1c
};

// ---- 嵌套元素（ORIG C1 0x089623ba / clear 0x089623ee / D1 0x083dd54a）----
// +0x00 int=0；+0x04 int=0；+0x08 std::string；+0x0c int=0，sizeof 0x10。
struct seriaRoomDecoEventSoundInfo
{
    seriaRoomDecoEventSoundInfo();
    ~seriaRoomDecoEventSoundInfo();
    void clear();                 // ORIG W 0x089623ee

    int m_field0;                 // +0x00
    int m_field4;                 // +0x04
    std::string m_name;           // +0x08（ScanStr 写入）
    int m_fieldC;                 // +0x0c
};                                // sizeof 0x10

class seriaRoomDecoEventScript
{
public:
    seriaRoomDecoEventScript();
    ~seriaRoomDecoEventScript();

    std::vector<int> m_eventItemIdx;    // +0x00
    int m_maxEventItemCount;            // +0x0c
    std::map<int, int> m_matchEventMap; // +0x10
    // ORIG importSeriaRoomDecoEvent [sound info]（0x895e4bf +0x1da）：本偏移为
    // vector<seriaRoomDecoEventSoundInfo>（clear/push_back 实证），sizeof 0x3c。
    std::vector<seriaRoomDecoEventSoundInfo> m_soundList;  // +0x28
    int m_fatigueCondition;             // +0x34
    int m_rewardMailExpireDate;         // +0x38
};

class growthEquipmentScript
{
public:
    growthEquipmentScript();
    ~growthEquipmentScript();

    std::map<int, int> m_rewardMap;     // +0x00
    int m_giftBox;                      // +0x18
    int m_materialItem;                 // +0x1c
    unsigned short m_materialMax;       // +0x20
    unsigned short m_eventBubbleMax;    // +0x22
    unsigned short m_eventSandMax;      // +0x24
    char m_pad26[2];                    // +0x26
};

class createChracScript
{
public:
    createChracScript();
    ~createChracScript();

    // ORIG C1 0x8373042：map 构造后 clear()；D1 0x837c3d4 仅析构 map；
    // sizeof 0xc（仅 map 一个成员，ctor/dtor 指令实证）。
    //
    // rewardInfo —— ORIG C1 0x8962460 / D1 0x83dd574（importCharacterCreateGift
    // Script 0x895eda1 逐字段写入实证）：
    //   +0x00 vector<int>（[item] push_back）
    //   +0x0c/+0x10 long（[new account event start/end time] ConvertStringToTime）
    //   +0x14 string（[create title] ScanStr 直写）
    //   +0x18 string（[create text] ScanStr 直写）
    struct rewardInfo
    {
        rewardInfo()
            : m_items(), m_title(), m_text()
        {
            m_items.clear();
            m_startTime = 0;
            m_endTime = 0;
            m_title.clear();
            m_text.clear();
        }
        std::vector<int> m_items;   // +0x00
        long m_startTime;           // +0x0c
        long m_endTime;             // +0x10
        std::string m_title;        // +0x14
        std::string m_text;         // +0x18
    };

    // rewardInfoList —— ORIG C1 0x84e91fa：vector<rewardInfo> @+0x00。
    struct rewardInfoList
    {
        rewardInfoList();                 // ORIG W 0x84e91fa
        std::vector<rewardInfo> m_list;   // +0x00
    };

    std::map<int, rewardInfoList> m_map0;   // +0x00
};

class useCharacterJobFatigue
{
public:
    useCharacterJobFatigue();
    ~useCharacterJobFatigue();

    struct fatigueMatchGift
    {
        // ORIG C1 0x0896251c：map ctor(+0)、+0x18=0、string ctor(+0x1c/+0x20)
        // 后逐一 clear。copy C1ERKS_ 0x08962c1e（隐式成员拷贝等价）、D1 0x083cf7e0。
        fatigueMatchGift();

        std::map<int, int> m_rewardMap;  // +0x00（[reward list] 三元组 pair(v1,v2)）
        int m_field18;                   // +0x18（第三列扫描值，>=1 校验）
        std::string m_msg1;              // +0x1c（[fatigue title] ScanStr）
        std::string m_msg2;              // +0x20（[fatigue text] ScanStr）
    };                                   // sizeof 0x24
    std::map<int, fatigueMatchGift> m_giftMap;  // +0x00
};

class CharacterCreateCreatureGiftScript
{
public:
    CharacterCreateCreatureGiftScript();
    ~CharacterCreateCreatureGiftScript();

    struct eventJob
    {
        eventJob() : m_minLevel(0), m_maxLevel(0) {}   // ORIG C1 0x89625d0 双 int 清零
        int m_minLevel;   // +0x00（[start] ScanIntPi 写入）
        int m_maxLevel;   // +0x04（[end] ScanIntPi 写入）
    };
    std::map<int, eventJob> m_eventJobMap;  // +0x00
    int m_changeAbleCreatureLev;            // +0x18
    std::map<int, int> m_clientChangeMap;   // +0x1c
    std::map<int, int> m_infinityChangeMap; // +0x34
};

struct seriaRoomDecoRewardInfo
{
    // ORIG C1/C2 0x08962634（W）：三个 int 清零（sizeof 0xc）。
    seriaRoomDecoRewardInfo()
    {
        m_itemIdx = 0;
        m_reward = 0;
        m_field8 = 0;
    }
    int m_itemIdx;   // +0x00
    int m_reward;    // +0x04
    int m_field8;    // +0x08（ORIG C1 清零第三字；vector stride 0xc 实证）
};

// ---- ani 元素（ORIG C1 0x089625e8：string ctor 后 clear；clear 0x08962606：
// +0x04=7、+0x08/+0x0c/+0x10=0；D1 0x083cf9ba；copy C1ERKS_ 0x08962e6e）----
struct seriaRoomDecoAniInfo
{
    seriaRoomDecoAniInfo();
    ~seriaRoomDecoAniInfo();
    void clear();              // ORIG W 0x08962606（不清 m_name）

    std::string m_name;        // +0x00（ScanStr）
    int m_layer;               // +0x04（getDrawLayer 结果；ctor/clear 置 7）
    int m_a;                   // +0x08
    int m_b;                   // +0x0c
    int m_c;                   // +0x10
};                             // sizeof 0x14

class seriaRoomDecoAniEventScript
{
public:
    // 嵌套枚举：ORIG _ZN27seriaRoomDecoAniEventScript8aniLayer1TE
    // （map key 类型；importSeriaRoomAnimationDecoEvent [front]→0/[back]→1）
    struct aniLayer
    {
        enum T
        {
            T_FRONT = 0,
            T_BACK = 1
        };
    };

    seriaRoomDecoAniEventScript();   // ORIG C1 0x83731ca（五容器构造后 clear）
    ~seriaRoomDecoAniEventScript();  // ORIG D1 0x837c46a
    void clear();                    // ORIG 0x83732a4

    std::vector<seriaRoomDecoRewardInfo> m_rewardList;               // +0x00
    std::map<int, int> m_itemMap;                                    // +0x0c
    std::map<int, seriaRoomDecoAniInfo> m_aniList;                   // +0x24
    std::string m_finishSound;                                       // +0x3c
    std::map<aniLayer::T, seriaRoomDecoAniInfo> m_finishAniMap;      // +0x40
};                                                                   // sizeof 0x58
// ---- NPC 关系事件（ORIG 布局，importNPCRelationEventScript 0x890d44f 反汇编实证）----

enum ENUM_NPC_RELATION_EVENT_FAVOR_KIND
{
    ENUM_NPC_RELATION_FAVOR_GOOD = 0,   // [good]
    ENUM_NPC_RELATION_FAVOR_SOSO = 1    // [soso]
};

class stUnlimitChallengeInfo
{
public:
    stUnlimitChallengeInfo();
    ~stUnlimitChallengeInfo();

    // ORIG C1 0x837c53c：+0x00 int +0x04 vector<int> +0x10 vector<int>
    // +0x1c vector<stUnlimitChallengeRewardInfo> +0x28 vector<stUnlimitChallengeMessageInfo>
    // +0x34 multimap<stUnlimitChallengeKey,stUnlimitChallengeItem>，sizeof 0x4c
    int m_field0;                                       // +0x00
    std::vector<int> m_vec4;                            // +0x04
    std::vector<int> m_vec10;                           // +0x10
    std::vector<stUnlimitChallengeRewardInfo> m_vec1c;  // +0x1c
    std::vector<stUnlimitChallengeMessageInfo> m_vec28; // +0x28
    std::multimap<stUnlimitChallengeKey,
                  stUnlimitChallengeItem> m_multiMap34; // +0x34
};


struct stNPCEventItemInfo
{
    int m_itemId;   // +0x00
    int m_count;    // +0x04

    // ORIG W 0x0891b52e _ZN18stNPCEventItemInfoaSES_（按值参数）。
    stNPCEventItemInfo& operator=(stNPCEventItemInfo other);  // 定义 OnlyOrigBatch12.cpp
};

// ORIG AddFavorItemInfo 0x890cf44：items=vector@+0x00、rewardA=int@+0x0c、rewardB=int@+0x10（步长 0x14）。
struct stNPCFavorAndRewardInfo
{
    std::vector<stNPCEventItemInfo> m_items;   // +0x00
    int m_rewardA;                             // +0x0c（AddRewardItemInfo 第一值）
    int m_rewardB;                             // +0x10（AddRewardItemInfo 第二值）
};

// ORIG C1 0x891c946（new 0x48 实证）：favor[3] @+0x00/+0x14/+0x28 + vector<int> @+0x3c。
struct stNPCRelationEventInfo
{
    stNPCFavorAndRewardInfo m_favor[3];   // +0x00/+0x14
    std::vector<int> m_dislikeItems;      // +0x3c（[can not give item] push_back）
    stNPCRelationEventInfo();
};

// ORIG sret 返回类型（GetItemKind 0x0890d1c0 / GetRewardItem 0x0890d326）。
struct stTempRewardInfo
{
    // ORIG C1/C2 0x0891b548（W）：m_kind=3、m_count=1。
    stTempRewardInfo()
    {
        m_kind = 3;
        m_count = 1;
    }
    int m_kind;    // +0x00（0..2 亲密度类别）
    int m_count;   // +0x04（ROUND(count/item.m_count)）
};
struct stNPCRewardItemInfo
{
    int m_rewardA;  // +0x00（node->m_favor[kind].m_rewardA）
    int m_rewardB;  // +0x04
};
class CNPCRelationEventManager
{
public:
    CNPCRelationEventManager();
    ~CNPCRelationEventManager();

    // ORIG D1 0x890ce34：+0x00 map<unsigned short, stNPCRelationEventInfo*>
    std::map<unsigned short, stNPCRelationEventInfo*> m_npcEventMap;  // +0x00
    CItemLimitEditionMgr* m_itemLimitEditionMgr;   // +0x18（getItemLimitEditionMgr 0x08179fa6 返回）

    // ORIG 成员（import 调用集；定义归 CNPCRelationEventManager 报告域批次）：
    void AddNPCInfo(unsigned short idx, stNPCRelationEventInfo* info);                       // 0x890ceea
    void AddFavorItemInfo(unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
                          unsigned int itemId, unsigned int count);                          // 0x890cf44
    void AddRewardItemInfo(unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
                           unsigned int a, unsigned int b);                                  // 0x890cfda
    void AddDislikeItemInfo(unsigned short idx, unsigned int itemId);                        // 0x890d07a
    ENUM_NPC_RELATION_EVENT_FAVOR_KIND GetFavorIndex(const char* name);                      // 0x890d3d0
    bool IsDislikeItem(unsigned short idx, unsigned int itemId);        // T 0x0890d100
    stTempRewardInfo GetItemKind(unsigned short idx, unsigned int itemId,
                                 unsigned int count);                   // T 0x0890d1c0（sret）
    stNPCRewardItemInfo GetRewardItem(
        unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind);   // T 0x0890d326（sret）
};

class stLimitItemUsageInfoEx
{
public:
    stLimitItemUsageInfoEx();
    ~stLimitItemUsageInfoEx();

    // ORIG D1 0x837c7a2：+0x00 stLimitItemUsageInfo +0x0c stResetItemInfo
    // +0x18 stRefillItemInfo，sizeof 0x24
    stLimitItemUsageInfo m_info0;   // +0x00
    stResetItemInfo m_resetC;       // +0x0c
    stRefillItemInfo m_refill18;    // +0x18
};

// ORIG 布局（0x891b560 构造）：+0x0 int +0x4 int +0x8 string +0xc string
// +0x10 char x3 +0x14 string +0x18 vector<string>（std::string 4B 旧 ABI）
struct stOneEmotionData
{
    stOneEmotionData();
    ~stOneEmotionData();
    void clear();
    stOneEmotionData& operator=(const stOneEmotionData& rhs);
    int m_0;              // +0x00
    int m_4;              // +0x04
    std::string m_8;      // +0x08
    std::string m_c;      // +0x0c
    char m_10;            // +0x10
    char m_11;            // +0x11
    char m_12;            // +0x12
    char m_pad13;         // +0x13
    std::string m_14;     // +0x14
    std::vector<std::string> m_18;  // +0x18
};

class stEmotionTypeTagsInfo
{
public:
    stEmotionTypeTagsInfo();
    ~stEmotionTypeTagsInfo();
    void clear();
    std::vector<int> m_tags;              // +0x00
    std::vector<stOneEmotionData> m_data; // +0x0c
};

class stEquipmentGradeSample
{
public:
    stEquipmentGradeSample();
    ~stEquipmentGradeSample();

    // ORIG C1 0x837c810 / D1 0x837c884：5 个 vector<unsigned int>（步长 0xc，
    // +0x00/+0x0c/+0x18/+0x24/+0x30），sizeof 0x3c。
    // importEquipmentGradeSampleInfo（0x890e18d）按 [grade_0]..[grade_4]
    // 五节分别向对应 vector push_back。
    std::vector<unsigned int> m_grade0;   // +0x00
    std::vector<unsigned int> m_grade1;   // +0x0c
    std::vector<unsigned int> m_grade2;   // +0x18
    std::vector<unsigned int> m_grade3;   // +0x24
    std::vector<unsigned int> m_grade4;   // +0x30
};

class stConditionEventInfo
{
public:
    stConditionEventInfo();
    ~stConditionEventInfo();

    // ORIG D1 0x837c938：+0x08 map<int,pair<int,int>> +0x20
    // multimap<int,stConditionEventRewardInfo> +0x38
    // multimap<int,stConditionEventRewardInfo>，sizeof 0x50
    int m_field0;                                      // +0x00
    int m_field4;                                      // +0x04
    std::map<int, std::pair<int, int> > m_map8;        // +0x08
    std::multimap<int, stConditionEventRewardInfo> m_multiMap20;  // +0x20
    std::multimap<int, stConditionEventRewardInfo> m_multiMap38;  // +0x38
};

class ConditionLevelChkDungeon
{
public:
    ConditionLevelChkDungeon();
    ~ConditionLevelChkDungeon();

    // ORIG T 0x089139d4 / 0x08913a62（报告 ConditionLevelChkDungeon/isEventDungeon.md、
    // isApplyLevel.md）：isEventDungeon=m_unableDungeons 不含该副本；isApplyLevel=等级达标。
    bool isEventDungeon(int dungeonIdx);
    bool isApplyLevel(int level);

    // ORIG C1 0x837c9ac / D1 0x837c9c0（owner：CDataManagerScripts.cpp）：仅构造/析构 +0x00 的 vector<int>，
    // sizeof 0x10。
    // importConditionEventChkDungeonScript（0x8913891）：[unable dungeon] 向
    // m_unableDungeons push_back；[apply level] 写 m_applyLevel（+0x0c）。
    std::vector<int> m_unableDungeons;   // +0x00
    int m_applyLevel;                    // +0x0c
};

// ORIG CharacterKind（C1Ehh 0x8693c3e：this[0]=job, this[1]=growType；sizeof 2）
struct CharacterKind
{
    // ORIG W ctor 0x08568652（C2 同址）：两字节清零。
    CharacterKind() { m_job = 0; m_grow = 0; }
    // ORIG 调用点（FairPvPStatScript::FindCharacStat）以 movzbl 扩展 → unsigned char。
    CharacterKind(unsigned char job, unsigned char grow) { m_job = job; m_grow = grow; }
    // ORIG 三段式比较（cmp/jae 形态，_ZNK13CharacterKindltERKS_ 实证）。
    bool operator<(const CharacterKind& o) const
    {
        // ORIG 0x8693bf4: jae/mov1/jmp / jne / jae / mov1/jmp / mov0 (nested short-circuit early-return,
        // 分支极性同 RandomApplicationKey::operator<)。原 if(o.m_job<m_job)return false 会生成交换操作数的比较，与 ORIG 不符。
        if (m_job < o.m_job) return true;
        if (m_job == o.m_job && m_grow < o.m_grow) return true;
        return false;
    }
    unsigned char m_job;     // +0x00
    unsigned char m_grow;    // +0x01
};
// 0x8659faa 逐字段读取；sizeof 0x54）。u16 簇实为 +0x08..0x39 共 25 项：
// [26] 会把尾段推到 +0x40 并因对齐使 sizeof 膨胀到 0x58，与 ORIG 记录布局
// （尾段 int@0x3c … int@0x50）错位 —— make_charac_stat TU 验证实测修正。
struct CharacterStatus
{
    int m_field00;          // +0x00
    int m_field04;          // +0x04
    short m_field08[25];    // +0x08..0x39（make_charac_stat 循环1/2 按
                            // (i+8)*2 与 (i+8)*2+8 寻址，最大下标 24）
    char m_pad3a[2];        // +0x3a（与 _Additioal_info 的 2 字节错位间隙）
    int m_field3c;          // +0x3c
    short m_field40;        // +0x40
    short m_field42;        // +0x42
    int m_field44;          // +0x44
    short m_field48;        // +0x48
    short m_field4a;        // +0x4a
    short m_field4c;        // +0x4c
    short m_field4e;        // +0x4e
    int m_field50;          // +0x50
};

typedef char CharacterStatus_size_0x54[(sizeof(CharacterStatus) == 0x54) ? 1 : -1];

class FairPvPStatScript
{
public:
    FairPvPStatScript();
    ~FairPvPStatScript();
    void clear();   // ORIG clear 0x83724c8：仅清空 map

    // ORIG C1 0x8693c66 W（FindCharacStat）：map.find(CharacterKind(job, grow))，
    // 命中返回 &it->second，未命中返回 0。
    CharacterStatus* FindCharacStat(int job, int growType);

    // ORIG map<CharacterKind, CharacterStatus>（FindCharacStat 0x8693c66 /
    // ctor 0x83724f0 模板实例符号 _ZNSt3mapI13CharacterKind15CharacterStatus...
    // 实证）。std::map 尺寸与 key/value 无关，sizeof 仍 0x18。
    // 公开：importPairPvPStatScript（InitScriptProducers_2.cpp）按 ORIG 0x08977d9c
    // 直接 insert/find 该容器。
    std::map<CharacterKind, CharacterStatus> m_map0;   // +0x00
};

// 权威定义：RandomOptionScript.h（本头不再持有占位镜像，避免与真身冲突）
#include "RandomOptionScript.h"

class CExpertJobList;   // 完整定义见 CExpertJobList.h

class RegenerationROI
{
public:
    RegenerationROI();
    ~RegenerationROI();

    // ORIG _ZN15RegenerationROI36_0_Load_RegenerationRandomOptionItemEv（init 调用）
    int _0_Load_RegenerationRandomOptionItem();

    int Regenerate(int baseLv0, int baseLv1, int srcOptIndex0, int srcOptIndex1,
                   int& outCost, Inven_Item& outItem, CUser* user);

    // ---- 内部实现（本批 RandomRentalA2.cpp 定义；地址=ORIG nm 实证）----
    void SendMessageToCUser(CUser* user, const char* msg);            // T 0x085f853c
    void _Select_Part(int partIdx, std::vector<int>& out);            // T 0x085f8ec2
    void _RandomLevel(int lv0, int lv1, struct select_base_item& sb,
                      std::stringstream& ss);                         // T 0x085f93cc
    int RegenVer_get_option_quantity(int lo, int hi);                 // T 0x085f8cb4
    int _Calculate_RegenCost(int partIdx, int optType, int lv0, int lv1);   // T 0x085f9094
    int RegenVer_get_option_value(int minV, int maxV);                // T 0x085f8da6

    // ORIG D1 0x85f8506：delete m_regeneratorPtr（CMTRand）+ 析构 regenerator。
    // RandomOptionItem_Regenerator（0x48）+0x50 指针，sizeof 0x54。
    // init() 将 new 的 CExpertJobList* 存入 +0x50（GetExpertJobScript 同址读取），
    // 故类型语义化为指针（尺寸不变）。
    CMTRand* m_regeneratorPtr;                 // +0x00
    RandomOptionItem_Regenerator m_regenerator;  // +0x04
    CExpertJobList* m_expertJobList;           // +0x50
};

class stSeriaBlessingScript
{
public:
    stSeriaBlessingScript();
    ~stSeriaBlessingScript();

    // ORIG Value ctor 0x08693aa0：五个 int 逐个清零（sizeof 0x14）；
    // ImportSeriaBlessingScript 0x0891063b 向 map 值写满 5 个 int；
    // FindOneByLv::operator() 0x822aecc 读 +0x00/+0x04 作 min/max 等级。
    struct Value
    {
        // ORIG W 0x8693aa0：五个 int 逐个清零（内联发射弱 C1/C2 符号）。
        Value() : m_minLv(0), m_maxLv(0), m_field8(0), m_fieldC(0), m_field10(0) {}
        int m_minLv;     // +0x00
        int m_maxLv;     // +0x04
        int m_field8;    // +0x08（[推断] buff 参数）
        int m_fieldC;    // +0x0c（[推断] buff 参数）
        int m_field10;   // +0x10（[推断] buff 参数）
    };

    // ORIG W 0x822af22：map 中存在 minLv<=lv<maxLv（或 min==max 且 lv==min）的项。
    bool is_possible_lv(int lv);
    // ORIG W 0x8693ad6：FindOneByLv(lv) 谓词 std::find_if 遍历 m_map0；
    // 命中把 Value 五 int 拷入 out 并返回 true，否则 false。（定义 W7_LinkTailGrowth.cpp）
    bool get_value(int lv, Value& out);

    // ORIG W 0x822aebe C1Ei / 0x822aecc operator()（find_if 谓词；定义
    // ValueCtors.cpp）：条件 (min==max && min==lv) || (min<=lv && lv<max)。
    struct FindOneByLv
    {
        FindOneByLv(int lv);   // ORIG W 0x822aebe：+0x00 = lv
        bool operator()(const std::pair<const int, Value>& p) const;  // ORIG W 0x822aecc
        int m_lv;   // +0x00
    };

    std::map<int, Value> m_map0;   // +0x00（ORIG map<int,Value>，sizeof 0x18 不变；
                                   // ImportSeriaBlessingScript 自由函数直接回填）
};

// stLevelUpRewardItemScript / stStepRewardStackableItemScript（ORIG D1
// 0x8371530 / 0x837151c：各含一个 map 成员，sizeof 0x18）。
class stLevelUpRewardItemScript
{
public:
    stLevelUpRewardItemScript();
    ~stLevelUpRewardItemScript();
    void clear();   // ORIG W 0x083711d6：m_map0.clear()（ImportReturnUserRewardScript 调用）
    // ORIG W 0x081b89de：m_map0.find(key)，命中写 out 两 int 并返回 1，否则 0。
    bool get_data(const LevelUpRewardItemTableKey& key,
                  LevelUpRewardItemData& out);              // 定义 CUserDepsB.cpp
    // ORIG W 0x081b895a：GetGrowUpRewardItemFunctor（同 job+grow、lv<=key）for_each
    // 收集进 out；返回 !out.empty()。
    bool get_data_when_growup(const LevelUpRewardItemTableKey& key,
                              std::vector<LevelUpRewardItemData>& out);  // 定义 CUserDepsB.cpp

    // ORIG W 0x081b88e8 C1 / 0x081b88fe operator()（for_each functor；定义
    // ValueCtors.cpp）：同 job+grow 且 pair key lv<=this key lv 时 push_back。
    struct GetGrowUpRewardItemFunctor
    {
        GetGrowUpRewardItemFunctor(const LevelUpRewardItemTableKey& key,
                                   std::vector<LevelUpRewardItemData>& out);  // W 0x81b88e8
        void operator()(const std::pair<const LevelUpRewardItemTableKey,
                                        LevelUpRewardItemData>& p) const;     // W 0x81b88fe
        const LevelUpRewardItemTableKey* m_key;  // +0x00
        std::vector<LevelUpRewardItemData>* m_out;  // +0x04
    };

    std::map<LevelUpRewardItemTableKey, LevelUpRewardItemData> m_map0;  // +0x00
};  // sizeof 0x18（ORIG C1 0x8371194/D1 0x8371530 仅构造/析构该 map）

class stStepRewardStackableItemScript
{
public:
    stStepRewardStackableItemScript();
    ~stStepRewardStackableItemScript();
    void clear();   // ORIG W 0x08371508：m_map0.clear()（ImportNewAccountLeventUpToJobScript 调用）
    void set_data(int step, StackableItemData& data);  // ORIG W 0x0891bd0e（LoadRewardDataByNoType 调用）

    std::map<int, std::vector<StackableItemData> > m_map0;  // +0x00
};  // sizeof 0x18（ORIG C1 0x83714c6/D1 0x837151c 仅构造/析构该 map）

class stGrowthCapsuleScript
{
public:
    stGrowthCapsuleScript();
    // ORIG C1 0x0837114a：+0x00..+0x10 五个 int 逐个清零；ImportGrowthCapsuleScript
    // 0x0891120c 按名回填同五字段，sizeof 0x14。
    int m_maxGageExp;      // +0x00（[max gage exp]）
    int m_gagePercent;     // +0x04（[gage percent]）
    int m_capsuleId;       // +0x08（[capsule id]）
    int m_capsuleExp;      // +0x0c（[capsule exp]）
    int m_capsuleLimitLv;  // +0x10（[capsule limit lv]）
};

class stRankSystemInfo
{
public:
    stRankSystemInfo();
    ~stRankSystemInfo();
    void clear();

    // ORIG C1 0x8370dcc / clear 0x8370e34：+0x00 map<stClearTimeAverageMapKey,
    // stClearTimeAverage> +0x18 vector<stBonusPointValues>
    // +0x24 ushort m_rankLevel[9]（getRankIndex 读 0..7；importRankSystemScript
    // [rank grade] 0x0890ed7f 循环 i<9 连写 9 项，memset 0x12 字节实证），sizeof 0x38
    // ORIG mangled _ZN16stRankSystemInfo21stDungeonRankSystem_tC1Ev（嵌套于本类）。
    struct stDungeonRankSystem_t
    {
        stDungeonRankSystem_t();
        ~stDungeonRankSystem_t();
        void clear();
        std::map<stClearTimeAverageMapKey, stClearTimeAverage> m_map0;  // +0x00
        std::vector<stBonusPointValues> m_vec18;                        // +0x18
        unsigned short m_rankLevel[9];                                  // +0x24..+0x35
        char m_pad36[2];                                                // +0x36
    };

    // ORIG C1 0x0891b6f2 / clear 0x0891b706（memset(this,0,0x1c)）/
    // D1 0x0893fbc6（空体）。7 个 int（0x1c）。
    struct stDungeonClearTimeData
    {
        stDungeonClearTimeData();          // ORIG W 0x0891b6f2：调 clear
        ~stDungeonClearTimeData();         // ORIG W 0x0893fbc6：空体
        void clear();                      // ORIG W 0x0891b706
        int m_v[7];                        // +0x00..0x1b
    };
    // ORIG C1 0x0891b72a / clear 0x0891b73e（memset(this,0,0x10)）/
    // D1 0x08940176（空体）。4 个 int（0x10）。
    struct stComboIntervalBoundary
    {
        stComboIntervalBoundary();         // ORIG W 0x0891b72a：调 clear
        ~stComboIntervalBoundary();        // ORIG W 0x08940176：空体
        void clear();                      // ORIG W 0x0891b73e
        int m_v[4];                        // +0x00..0x0f
    };


    // ORIG C1 0x8370eba / D1 0x837ce4c：+0x00 stDungeonRankSystem_t（0x38）
    // +0x38 vector<stDungeonClearTimeData> +0x44 map<int,int>
    // +0x5c vector<int> +0x68 vector<stComboIntervalBoundary>，sizeof 0x78
    stDungeonRankSystem_t m_dungeonRank;                           // +0x00
    std::vector<stDungeonClearTimeData> m_vec38;                   // +0x38
    std::map<int, int> m_map44;                                    // +0x44
    std::vector<int> m_vec5c;                                      // +0x5c
    std::vector<stComboIntervalBoundary> m_vec68;                  // +0x68
    int m_field74;                                                 // +0x74
};

struct stDeathTower_t
{
    stDeathTower_t();

    // ORIG C1 0x8370970：+0x00 int=0；float[200]@+0x004=1.0f；
    // int[200]@+0x324=0；float[200]@+0x644=1.0f；int[200]@+0x964=0；
    // +0xc84 int=0；memset(+0xc88,0,12)；+0xc94 float=1.0f；sizeof 0xc98。
    // 字段语义 = importDeathTowerScript（ORIG T 0x089089f3）脚本节名：
    float m_apcExpWeight;                  // +0x000（[apc exp weight] fstps 实证为 float）
    float m_rewardExpWeight[200];          // +0x004（[reward exp weight]，脚本下标从 1 起）
    int   m_rewardCardNum[200];            // +0x324（[reward card num]，下标从 1 起）
    float m_illusionRewardExpWeight[200];  // +0x644（[illusion reward exp weight]）
    int   m_illusionRewardCardNum[200];    // +0x964（[illusion reward card num]）
    float m_monsterExpWeight;              // +0xc84（[monster exp weight] fstps 实证为 float）
    int   m_rewardItemProb[3];             // +0xc88..+0xc93（[reward item prob] 三级累计：
                                           //   [0]=v1、[1]=[0]+v2、[2]=[1]+v3；ORIG memset 区）
    float m_rewardGoldWeight;              // +0xc94（[reward gold weight]，ctor 置 1.0f）
};

struct stBloodDungeon_t
{
    stBloodDungeon_t();
    ~stBloodDungeon_t();
    void clear();

    // ORIG C1 0x8370a3a / clear 0x8370b02：+0x00 float[200]（clear 置 1.0f）
    // +0x320 int（=1）+0x324 int[3] +0x330 float（=1.0f）
    // +0x334 vector<float> +0x340 pad[4] +0x344/+0x348 int（[ultimate
    //   difficulty point] 双 ScanInt 写入）+0x34c pad[0xc] +0x358 map<int,STUltimateRewardProb>
    float m_rate[200];                                    // +0x00
    int m_field320;                                       // +0x320
    int m_field324[3];                                    // +0x324
    float m_field330;                                     // +0x330
    std::vector<float> m_vec334;                          // +0x334
    char m_pad340[4];                                     // +0x340
    int m_ultDiffPoint344;                                // +0x344（[ultimate difficulty point] 第 1 值）
    int m_ultPointTotal348;                               // +0x348（第 2 值）
    char m_pad34c[0xc];                                   // +0x34c
    std::map<int, STUltimateRewardProb> m_map358;         // +0x358
};
// stBloodDungeon_t 补充（importBloodDungeonScript 0x8909016 +0x27b 写入实证）：
//   +0x344 int / +0x348 int 由 [ultimate difficulty point] 连续两个 ScanInt 写入
//   （原 m_pad340[0x18] 拆分，其余 pad 保持）。

// ---- 村庄攻击脚本（ORIG importVillageAttackedScript 0x89093ee 实证：
//      push_back(vector<stAttackedMapInfo_t>)，元素 4 int（Area.cpp 同布局双视图）；
//      ctor/dtor W 0x837cf1e/0x837cf32 为隐式生成，本类型不再声明）----
struct stAttackedMapInfo_t
{
    int m_vill;      // +0x00（村庄 idx）
    int m_area;      // +0x04
    int m_field8;    // +0x08
    int m_fieldC;    // +0x0c
};                   // sizeof 0x10

struct stVillageAttacked_t
{
    std::vector<stAttackedMapInfo_t> m_mapList;   // +0x00（ORIG 即 vector 本体）
};                                                 // sizeof 0xc

namespace APSystem
{
class _ActionGroupIndex;   // 完整定义 CUserProc.h（APSystem::_ActionGroupIndex，int m_value）

// _SIG_LOAD_ACTION_POINT（含嵌套 _TodayRewardItem）/CActionPointEx/_MedalRewardItem
// 权威定义见 APSystemTypes.h（MoreTypes.h 同样包含）。
class CActionPointEtcParameter
{
public:
    CActionPointEtcParameter();
    ~CActionPointEtcParameter();
    void ClearData();
    bool IsValidTodayRewardItem(_SIG_LOAD_ACTION_POINT::_TodayRewardItem item);
    static bool IsValidActionGroupIndex(_ActionGroupIndex group);   // ORIG T 0x08894956（静态成员实证：体仅 [ebp+8]=组值范围判定 [1,0x2d]，两处调用方均只压一个实参；实现 APSystemLeafC3.cpp）
    // ORIG _ZN8APSystem24CActionPointEtcParameter32importActionPointSystemEtcScriptEPKcRS0_
    // （CDataManager::init 对 m_actionPoint1/m_actionPoint2 各调一次）
    bool importActionPointSystemEtcScript(const char* path,
                                          CActionPointEtcParameter& param);
    // 重建辅助访问器（ORIG 调用方内联直读 +0x18；实现见 SyncScriptDeps.cpp）。
    const std::map<unsigned short, const CActionPointEx*>& getActionPointRefs() const;
private:
    // ORIG ClearData 0x088948fc 布局（32 位：4×map(0x18) + 2×vector(0xc) = 0x78）。
    // 声明顺序即布局顺序：ORIG C1 0x0889484a 先构造 4 个 map（+0x0/+0x18/+0x30/+0x48）
    // 再构造 2 个 vector（+0x60/+0x6c）；vector 需保持 public 供跨 TU 直读，
    // 故置于类尾 public 段（顺序不变）。
    std::map<unsigned short, std::vector<CActionPointEx> > m_actionPoints;       // +0x00
    std::map<unsigned short, const CActionPointEx*> m_actionPointRefs;           // +0x18
    std::map<unsigned int, std::string> m_rewardMessages;                        // +0x30
    std::map<unsigned int, std::string> m_rewardMessages2;                       // +0x48
public:
    // Wave5 APSystem 批：m_medalRewards/m_todayRewards 置于 public（DB_LoadRewardMedal::
    // Select / GetTodayRewardAndSendMail / Inter_LoadRewardMedal::SendMail 跨 TU 直读）。
    std::vector<_MedalRewardItem> m_medalRewards;                                // +0x60
    std::vector<APSystem::_TodayRewardItem> m_todayRewards;        // +0x6c
};
}

struct stCharacLinkSystem_t
{
    stCharacLinkSystem_t();
    ~stCharacLinkSystem_t();

    // ORIG C1 0x837cf46：+0x00 int +0x04/+0x10/+0x1c/+0x28
    // vector<stCharacLinkSystemData>，sizeof 0x34
    int m_field0;                                          // +0x00
    std::vector<stCharacLinkSystemData> m_vec4;            // +0x04
    std::vector<stCharacLinkSystemData> m_vec10;           // +0x10
    std::vector<stCharacLinkSystemData> m_vec1c;           // +0x1c
    std::vector<stCharacLinkSystemData> m_vec28;           // +0x28
};

class StrikerScript;       // 最小镜像见 CDataManager.cpp（专属 TU 待落地）
class CharacManageScript;  // 同上

#include "AdvanceAltarField.h"  // advancealtar 共享类型（FieldType/FieldData/_RewardItem/
                                // AchievementRewardData 权威定义；本文件 RewardParameter 需要）

namespace advancealtar
{

// _SummonObject（ORIG C1 0x08134650 W：memset(this,0,0x24)，sizeof=0x24；
//   字段写入点见 importSummonObject 0x0889a21e 报告）。
struct _SummonObject
{
    _SummonObject();             // ORIG 0x08134650 W（memset 0x24）
    int m_id;                    // +0x00（亦作 m_summonMap 的 key）
    unsigned char m_isEnemy;     // +0x04（"enemy"=1 / "friend"=0）
    int m_type;                  // +0x08（"normal"=0 / "named"=1 / "boss"=2）
    int m_field0c;               // +0x0c
    unsigned short m_field10;    // +0x10（ScanInt 低 16 位存储）
    int m_field14;               // +0x14
    int m_field18;               // +0x18
    int m_field1c;               // +0x1c
    int m_field20;               // +0x20
};                               // sizeof 0x24

// _Action（ORIG C1 0x0889b978 W：memset(this,0,0x14)，sizeof=0x14；
//   字段写入点见 importActionDefine 0x0889a56e 报告）。
struct _Action
{
    _Action();                   // ORIG 0x0889b978 W
    int m_summonId;              // +0x00
    _SummonObject* m_pSummon;    // +0x04（importActionDefine 经 m_summonMap find 解析）
    int m_firstDelayMs;          // +0x08（<0 → 错误 0x285）
    int m_nextDelayMs;           // +0x0c（<500 → 错误 0x290）
    unsigned char m_flag;        // +0x10（行第 4 列 !=0 → 置 1）
    unsigned short m_flagValue;  // +0x12（flag 时写入该列低 16 位）
};                               // sizeof 0x14

// ActionDefine（ORIG C1 0x08899c62 T / D1 0x08899c82 T / reset 0x08899c98 T：
//   id=0 + vector<_Action>@+0x04，sizeof=0x10）。
struct ActionDefine
{
    ActionDefine();              // ORIG 0x08899c62 T
    ~ActionDefine();             // ORIG 0x08899c82 T
    void reset();                // ORIG 0x08899c98 T：id=0 + m_actions.clear()
    int m_id;                    // +0x00（[normal]/[named]/[boss] 后首列，亦作 map key）
    std::vector<_Action> m_actions;  // +0x04
};                               // sizeof 0x10

// _StarRestFeeByGold（[star reset fee gold] 段元素；ctor ORIG W 0x88a3760
//   memset(this,0,4)（AltarMisc 批次 objdump 核实），sizeof 4、仅一个 int。
//   用户 inline ctor 使默认构造点发射 ORIG 弱符号 _StarRestFeeByGoldC1/C2）。
struct _StarRestFeeByGold
{
    _StarRestFeeByGold() { std::memset(this, 0, sizeof(*this)); }  // ORIG W 0x88a3760
    int m_field0;                       // +0x00（star 等级 → 金币费用记录值）
};
// ORIG BuyUpgradeData D1 0x88a3630：+0x0c string、+0x10 vector<FieldType::T>、
// +0x1c map<int,map<FieldType::T,FieldData>>（importAdvanceAltarShopParameter
// 0x88a10cc 的 [list] 段把每行 rowMap 插入该嵌套 map，key=FieldType-0 行的
// FieldData::m_value，见 AdvanceAltarField.h 注释）。
struct BuyUpgradeData
{
    BuyUpgradeData();             // ORIG C1 0x088a3570
    int m_nItemId;                    // +0x00
    int m_field4;                     // +0x04（[unlock stage index]）
    bool m_field8;                    // +0x08（[is settable slot]，仅 skill 段；ORIG 视 bool 型，拷贝构仅拷低 1 字节）
    std::string m_strDesc;            // +0x0c（[name]）
    std::vector<FieldType::T> m_fieldTypes;  // +0x10（[field name]*，getFieldType 命中项）
    LevelFieldDataMap m_listMap;      // +0x1c（[list] 行集：key=行首 FieldType-0 值）

    // ORIG T 0x088a2e4c：m_listMap.find(level) 未命中→0；再 find(ft) 未命中→0；
    // 命中返回 &fit->second（FieldData*，调用方取 +4 m_value）。
    FieldData* getFieldDataPoint(int level, FieldType::T ft);

    // ---- misc/查询组（docs/class_func_reports/advancealtar__BuyUpgradeData/）----
    // ORIG W 0x88a5784：+0/+4 int 直拷、+8 仅低 1 字节（ORIG 视其为 bool 型
    // 字段）、+0xc string 赋值、+0x10 vector<FieldType::T> 赋值、+0x1c
    // LevelFieldDataMap 赋值；返 *this。
    BuyUpgradeData& operator=(const BuyUpgradeData& other);   // ORIG W 0x88a5784
    int getFieldData(int level, FieldType::T ft);             // ORIG T 0x088a2c62
    int getFieldType(int idx) const;                          // ORIG T 0x088a3012
    int getMaxLevel() const;                                  // ORIG T 0x088a3322
    int getMinLevel() const;                                  // ORIG T 0x088a33c0
    FieldData* getMinLevelFieldDataPoint(FieldType::T ft, int& outLevel);  // ORIG T 0x088a2f1a
    int getStatFirstPos() const;                              // ORIG T 0x088a2f7a
    int getStatIconIndex(FieldType::T ft) const;              // ORIG T 0x088a3056
    std::string getStatStringIndex(FieldType::T ft) const;    // ORIG T 0x088a3106（sret）
    bool isValidLevel(int level);                             // ORIG T 0x088a345e
};

// ORIG BuyShopData（reset @ 0x88a1090）：+0x00 int；三 vector<BuyUpgradeData> 于 +0x04/+0x10/+0x1c。
struct BuyShopData
{
    BuyShopData();  // ORIG C1 0x88a3630：三 vector 构造后 +0=0、clear×3
    int m_nField0;                        // +0x00
    std::vector<BuyUpgradeData> m_vecT0;  // +0x04（T_0）
    std::vector<BuyUpgradeData> m_vecT1;  // +0x10（T_1）
    std::vector<BuyUpgradeData> m_vecT2;  // +0x1c（T_2）

    // ORIG T 0x088a105c：type 0/1/2 → &m_vecT0/T1/T2，其它 → 0。
    std::vector<BuyUpgradeData>* getBuyUpgradeDataVec(AdvanceAltarShopType::T type);
    void reset();  // ORIG T 0x088a1090：+0=0 + 三 vector clear（misc 批次定义）
};

class StageTimeLineParameter
{
public:
    StageTimeLineParameter();
    ~StageTimeLineParameter();
    void reset();
    // ORIG T 0x889a930 / 0x889a032：[time line]/[phase] 段解析与 ActionDefine
    // 指针回填（AltarMisc 批次定义于 advancealtar_misc.cpp）。
    int importPhase(TimeLine& timeLine);
    int checkAndSetActionDefine(TimeLine& timeLine,
                                const std::map<int, ActionDefine>& defines);
    int importStageTimeLineParameter(const char* path);
    int importSummonObject();   // ORIG _ZN12advancealtar22StageTimeLineParameter18importSummonObjectEv 0x0889a21e T
                                // （[summon object] 段逐行解析；0=段结束/成功，非 0=错误码）
    int importActionDefine();   // ORIG _ZN12advancealtar22StageTimeLineParameter18importActionDefineEv 0x0889a56e T
                                // （[action define] 段解析；[/action define]=返 1→调用方得 0）
    void debugLog();

    // ORIG D1 0x8899e88：+0x00 map<int,_SummonObject> +0x18
    // map<int,ActionDefine> +0x30 TimeLine（0x1c），sizeof 0x4c
    std::map<int, _SummonObject> m_summonMap;    // +0x00
    std::map<int, ActionDefine> m_actionMap;     // +0x18
    TimeLine m_timeLine;                         // +0x30
    char m_pad4c[0x4c - 0x30 - 0x1c];            // +0x4c..0x4b（补齐 0x4c）
};

class AdvanceAltarShopParameter
{
public:
    AdvanceAltarShopParameter();
    ~AdvanceAltarShopParameter();
    // ORIG _ZN12advancealtar25AdvanceAltarShopParameter31importAdvanceAltarShopParameterEPKc
    // 0x88a10cc：恒返 0（init.md +0x1c11 口径：非零=Fail）
    int importAdvanceAltarShopParameter(const char* path);
    // ORIG getFieldType 0x88a27a2 T：字段名→FieldType::T（未命中 -1）；
    // ORIG getdataType 0x88a2b8e T：位集 200（ft==3/6/7 字符串型返回 0，否则 1）。
    // 二者由 importAdvanceAltarShopParameter 调用；定义归 ShopParameter 生产批次。
    int getFieldType(std::string name);
    int getdataType(FieldType::T ft);
    // ORIG T 0x088a0fe8：m_buyShopMap.find(key) 未命中→0；命中转调
    // BuyShopData::getBuyUpgradeDataVec(type)。
    std::vector<BuyUpgradeData>* getBuyUpgradeDataVec(int key,
                                                      AdvanceAltarShopType::T type);
    // ORIG T 0x088a2bc0：遍历 m_starRestMap 取最大 key maxKey（初值 0），
    // 返回 m_starRestMap[maxKey] 元素值（map<int,_StarRestFeeByGold>::operator[]
    // 缺省插入保证非空）（AltarMisc 批次实现于 advancealtar_misc.cpp）。
    int getMaxStarResetGoldFee();
    // ORIG D1 0x88a0fa2：+0x00 map<int,BuyShopData> +0x18
    // map<int,_StarRestFeeByGold>，sizeof 0x30
    std::map<int, BuyShopData> m_buyShopMap;     // +0x00
    std::map<int, _StarRestFeeByGold> m_starRestMap;  // +0x18
};

class RewardParameter
{
public:
    RewardParameter();
    ~RewardParameter();
    int importAchievementRewardParameter(const char* path);  // ORIG _ZN12advancealtar15RewardParameter32importAchievementRewardParameterEPKc（0=成功，0x362…=错误码）
    void reset();                                             // ORIG 0x0889af9c T：m_rewardMap.clear()
    // ORIG T 0x0889b5a8（_ZNK12advancealtar15RewardParameter24getAchievementRewardDataE
    // NS_15StageDifficulty1TEi）：m_rewardMap 查表命中返回元素指针；Wave5 AltarProc
    // 批次追加声明（定义归 RewardParameter owner）
    advancealtar::AchievementRewardData* getAchievementRewardData(
        advancealtar::StageDifficulty::T diff, int clearCount) const;
    // ORIG T 0x0889b544（_ZNK12advancealtar15RewardParameter24getAchievementRewardDataEi）：
    // m_rewardMap.find(type) 命中→元素指针，未命中→0。（AltarMisc 批次）
    advancealtar::AchievementRewardData* getAchievementRewardData(int type) const;

    // ORIG C1 0x889af74 / D1 0x889af88 / reset / import 全部以 this+0 直接作为
    // map<int,AchievementRewardData> 操作（0x889afb0 insert 实证），sizeof 0x18。
    std::map<int, AchievementRewardData> m_rewardMap;  // +0x00
};

class ConfigParameter
{
public:
    ConfigParameter();
    ~ConfigParameter();

    int importConfigParameter(const char* path);   // ORIG _ZN12advancealtar15ConfigParameter21importConfigParameterEPKc（0=成功，0x3e9…=错误码）
    // ORIG C1 0x889b662：+0x00 int、+0x04 float、char*+0x18=&(+0x08)、两段 0x10 清零。
    // importConfigParameter 0x889b6cc 写入：
    int m_0;              // +0x00   [daily refill ticket]
    float m_4;            // +0x04   [gage upgrade cost percent]（ScanFloat 原样存储）
    int m_gageUpRate[4];  // +0x08   [gage up rate] 第 2 列（this+8+idx*4）
    char* m_strPtr;       // +0x18（ctor 置 = reinterpret_cast<char*>(m_gageUpRate)，ORIG &this+8）
    int m_gageUpAux[4];   // +0x1c   [gage up rate] 第 3 列（this+0x1c+idx*4）
    // init() 将 getInsance() 单例存入两指针（CDataManager::init ORIG +0x1e5x 实证），
    // destroy() 按指针 delete；类型语义化为指针（尺寸不变）。
    StrikerScript* m_strikerScript;      // +0x2c
    CharacManageScript* m_characManageScript; // +0x30
};
}

// stQuestShoptInitCost：ORIG 全局类型（mangled _ZN20stQuestShoptInitCostC1Ev
// 无 stQuestShop 前缀），C1 0x837109a（三 int 逆序清零），sizeof 0xc。
struct stQuestShoptInitCost
{
    stQuestShoptInitCost();
    int m_cost0;   // +0x00
    int m_cost1;   // +0x04
    int m_cost2;   // +0x08
};

// stStatusLevelInfo：ORIG 全局类型（mangled _ZN17stStatusLevelInfoC1Ev /
// _ZN17stStatusLevelInfoD1Ev 无 stQuestShop 前缀；vector<stStatusLevelInfo>
// 模板实例 _ZNSt6vectorI17stStatusLevelInfo... 实证），C1 0x891b8b4
// （+0x01 清零后拷至 +0x00），用户空体析构（non-trivial，D1 0x08940782），
// sizeof 0x2。member 名与 CQuestShop.cpp 消费侧镜像一致（m_first=嵌套版 m_max，
// m_second=嵌套版 m_min）。
struct stStatusLevelInfo
{
    stStatusLevelInfo()
    {
        m_second = 0;
        m_first = m_second;
    }
    ~stStatusLevelInfo() {}
    unsigned char m_first;   // +0x00（getNextLevel 返回；= 嵌套版 m_max）
    unsigned char m_second;  // +0x01（getMaxCount/getNextLevel 累计；= 嵌套版 m_min）
};

// stQuestShopStatus：ORIG 全局类型（mangled _ZN17stQuestShopStatusC1Ev /
// D1 0x83cd826 / clear 0x891b8cc 无 stQuestShop 前缀）。4×string @0x00..0x0f；
// clear 置 +0x10=-1、+0x14=0、+0x18=0x4d、+0x1c=2、+0x20=0；
// vector<stStatusLevelInfo> @0x24；sizeof 0x30。方法定义见 CQuestShop.cpp。
struct stQuestShopStatus
{
    int getMaxCount(int level);
    int getNextLevel(int level);
    void clear();                                  // W 0x891b8cc
    stQuestShopStatus();                           // W 0x891b948
    stQuestShopStatus(stQuestShopStatus const&);   // W 0x85f07fe
    ~stQuestShopStatus();                          // W 0x83cd826

    std::string m_name;                    // +0x00
    std::string m_name2;                   // +0x04
    std::string m_explain;                 // +0x08
    std::string m_icon;                    // +0x0c
    int m_field10;                         // +0x10（clear 置 -1）
    unsigned char m_questPoint;            // +0x14（clear 置 0）
    unsigned char m_pad15[3];
    int m_field18;                         // +0x18（clear 置 0x4d；movl 4 字节实证）
    int m_statusType;                      // +0x1c（clear 置 2）
    int m_field20;                         // +0x20（clear 置 0）
    std::vector<stStatusLevelInfo> m_levelInfos; // +0x24
};
static_assert(sizeof(stQuestShopStatus) == 0x30,
              "ORIG stQuestShopStatus layout drift");

class stQuestShop
{
public:
    stQuestShop();
    ~stQuestShop();

    // ORIG C1 0x83710d4：map 构造+clear；+0x18=-1；initCost@0x1c；memset(+0x28,0,0x4d)；sizeof 0x78。
    std::map<int, stQuestShopStatus> m_map0;          // +0x00
    int m_field18;                                    // +0x18（ctor 置 -1；[quest status] 取 max(idx)）
    stQuestShoptInitCost m_initCost;                  // +0x1c（[initialization costs] 三值）
    char m_pad28[0x4d];                               // +0x28（ORIG C1 memset 0x4d）
};


class SlangNameData
{
public:
    SlangNameData();
    ~SlangNameData();

    // 布局依据 importSlangName（ORIG 0x089081a3）实证：
    //   [SLANG] 分支 push_back 到 this+0x00 的 vector<std::string>；
    //   [one byte skip char] → this+0x0c 的 vector<unsigned char>；
    //   [two byte skip char] → this+0x18 的 vector<unsigned short>。
    std::vector<std::string> m_slangList;     // +0x00
    std::vector<unsigned char> m_charList;    // +0x0c
    std::vector<unsigned short> m_shortList;  // +0x18
};  // sizeof 0x24

class SlangFilter;   // 权威声明见 SlangFilter.h（完整类型经 FlexLexer 链引入 .cpp）

class InvalidCharData
{
public:
    InvalidCharData();
    ~InvalidCharData();

    // ORIG C1 0x837d168 / D1 0x837d1fa：+0x00 vector<unsigned char>
    // +0x0c vector<stRangeElement<unsigned char>> +0x18 vector<unsigned short>
    // +0x24 vector<stRangeElement<unsigned short>> +0x30 指针，sizeof 0x34。
    // init() 将 new SlangFilter 存入 +0x30，destroy() 经其 vtable D0 释放；
    // 类型语义化为指针（尺寸不变）。
    template <typename T>
    struct stRangeElement
    {
        T m_min;   // [推断]
        T m_max;   // [推断]
    };
    std::vector<unsigned char> m_charList;                // +0x00
    std::vector<stRangeElement<unsigned char> > m_charRange;  // +0x0c
    std::vector<unsigned short> m_shortList;              // +0x18
    std::vector<stRangeElement<unsigned short> > m_shortRange;  // +0x24
    SlangFilter* m_slangFilter;                           // +0x30
};

// STStackablePackageData 定义于 STStackableScript.h（本头已 include CItemList.h）。

class STGrowthPowerData
{
public:
    STGrowthPowerData();

    // ORIG W 0x822b07e（mangled 无 K → 非 const）：sret 返回最后一个
    // m_field1<=level 的段（未命中返回清零段）。
    STBreakAwaySection get_break_away_section_data(int level);
    // ORIG W 0x822afd6（非 const）：返回最后一个 m_field1<=lv 段的 m_field1，
    // 首个段 m_field1>lv 即返回 -1。
    int get_break_away_section_low_level(int lv);
    // ORIG W 0x822b136：首个满足 m_field1<=lv<m_field2 的 m_vec0 下标，未命中 -1。
    int get_idx_break_away_section(int lv);   // ORIG _ZN20STGrowthPowerData26get_idx_break_away_sectionEi（非 const）
    ~STGrowthPowerData();

    // ORIG C1 0x83711fe / D1 0x837d290：+0x00 vector<STBreakAwaySection>
    // +0x0c vector<STExpReward>
    // +0x18 map<int, vector<vector<STStackablePackageData>>>，sizeof 0x30
    std::vector<STBreakAwaySection> m_vec0;              // +0x00
    std::vector<STExpReward> m_vecC;                     // +0x0c
    std::map<int, std::vector<std::vector<STStackablePackageData> > > m_map18;  // +0x18
};

namespace AvatarVariation
{

struct colorRGB
{
    colorRGB();
    bool operator==(colorRGB const& rhs);
    // ORIG C1 0x088d43f0：三字节各置 0xff；importAvatarColorVariation 以
    // mov %al 写 +0/+1/+2（字节宽度实证），sizeof 3。
    unsigned char m_r;   // +0x00
    unsigned char m_g;   // +0x01
    unsigned char m_b;   // +0x02
};

class AvatarColorInfo
{
public:
    AvatarColorInfo();
    ~AvatarColorInfo();

    // ORIG D1 0x837d358：+0x00 map<int,colorRGB> +0x18 map<int,string>，sizeof 0x30
    std::map<int, colorRGB> m_colorMap;      // +0x00
    std::map<int, std::string> m_nameMap;    // +0x18
};
}

#ifndef DNF_BOOSTERGAGEDATA_DEFINED
class BoosterGageData
{
public:
    BoosterGageData();
    ~BoosterGageData();
private:
    char m_pad[0x10];
};
#endif

class CWorldMapList
{
public:
    CWorldMapList();
    ~CWorldMapList();
    CWorldMap* find_world_map(int idx) const;  // ORIG T 0x83647a2（const）
    void destroy();  // ORIG T 0x8374752：遍历 delete 各 CWorldMap* 后 clear map
    bool load_list();  // ORIG _ZN13CWorldMapList9load_listEv（init 调用）
    // ORIG T 0x8364694：重复 index logFormat 后 false；否则 insert。定义见
    // CtorProducers_1.cpp（Ctor_Group1）。
    bool insert_world_map(CWorldMap* worldMap);
private:
    std::map<int, CWorldMap*> m_map;   // +0x00（ORIG CWorldMapList 即一个 map<int,CWorldMap*>）
};

class StageMapList
{
public:
    // vector 元素（loadList 0x0836485c 实证，stride 0xc→ORIG 结构 0x10）：+0
    // uniqueId、+4 StageData 首字段、+8 nextId、+0xc 标志字节（loadList 写 0/1，
    // getNextStage 0x08364e58 读下一节点该字节；语义【推断】）。
    struct _StageMapSummary
    {
        _StageMapSummary()                      // ORIG W C1/C2 @0x083748aa：memset(this,0,0x10)
        {
            memset(this, 0, 0x10);
        }
        int m_uniqueId;   // +0x00（makeStageMapUniqueId）
        int m_stageId;    // +0x04（map 键）
        int m_nextId;     // +0x08（链式后继 stageId；末元素由 loadList 决定）
        unsigned char m_flags;    // +0x0c（loadList 常规=0 / 幕区收尾=1【推断】）
        unsigned char m_pad0d[3]; // +0x0d..+0x0f（保 sizeof 0x10）
    };

    // ---- LfAltarStage 批追加（advancealtar clearStage/setNextStage 依赖）----
    bool isExist(int stageIdx) const;          // ORIG T 0x08364df6：m_map.find!=end
    int getNextStage(int stageIdx, bool& outFlags) const;  // ORIG T 0x08364e58（定义 advancealtar_Manager.cpp）
    int getFirstStageIndex() const;            // ORIG W 0x081347c0：返 m_firstStageId

    StageMapList();
    ~StageMapList();
    bool loadList();   // ORIG _ZN12StageMapList8loadListEv（init 调用；定义 ScriptLeafA2.cpp）
    static bool compare(const _StageMapSummary& a,
                        const _StageMapSummary& b);  // ORIG T 0x08364848（比较 +0）
    static int makeStageMapUniqueId(int areaId, int districtId, int stageIdx);
    // ORIG T 0x08364818（定义待 StageMapList 域批次）
    int m_firstStageId;                       // +0x00（loadList 写 summaries[0].+4）
    std::map<int, _StageMapSummary> m_map;    // +0x04（键=stageId）
};  // sizeof 0x1c
// ORIG stWarPoint（0x10）：C1 0x0891a866→clear 0x0891a87a（memset 0x10）。
struct stWarPoint
{
    stWarPoint() { clear(); }
    void clear()
    {
        memset(this, 0, 0x10);   // ORIG: memset(this,0,0x10)
    }
    int m_0;       // +0x00
    int m_pts[3];  // +0x04..+0x0f
};

class WarAreaCounter
{
public:
    WarAreaCounter();
    ~WarAreaCounter();
    void clear();               // ORIG W 0x83707a4（ctor 尾调用）
    int GetCurrenTimeTable();   // ORIG 0x89024c4：返回当前时段索引（int）
    int GetWarRoomCountAtPeekTime(int idx);
    int GetWarRoomCountFirstIndex(int idx);
    int GetWarRoomCountLastIndex(int idx);
    // 成员公开：ImportWarAreaCounter（InitScriptProducers_2.cpp）按 ORIG
    // 0x089026f3 直写各段字段；WarField.cpp Get* 族同址读取。
    // 布局依据 ORIG clear(0x83707a4) memset 分段 + ImportWarAreaCounter(0x89026f3)
    // 直访偏移实证：+0x000 counterTable[24] +0x060 timeHourList[24] +0x0c0 doorFee[10]
    //   +0x0e8 curHourIdx +0x0ec banInterval +0x0f0 kickPoint +0x0f4 readyCount
    //   +0x0f8 warAreaDungeon[10] +0x120 wpPerMonsterHp map(greater) +0x138 wpPenalty
    //   +0x13c monsterExpConst(float) +0x140 num +0x144 warRoomCount[24][10]。
    int m_counterTable[24];        // +0x000（[counter] 段写入）
    int m_timeHourList[24];        // +0x060..+0xbf（[time table] 段）
    int m_doorFee[10];             // +0x0c0..+0xe7（[door fee] 段）
    int m_curHourIdx;              // +0x0e8（[counter] 最大值下标；Get* 族当前时段行号）
    int m_banInterval;             // +0x0ec（[ban interval]）
    int m_kickPoint;               // +0x0f0（[kick point]）
    int m_readyCount;              // +0x0f4（[ready count]）
    int m_warAreaDungeon[10];      // +0x0f8..+0x11f（[wararea dungeon]，从下标 2 写入）
    std::map<int, stWarPoint, std::greater<int> > m_wpPerMonsterHp;  // +0x120（[wp per monster hp]）
    int m_wpPenalty;               // +0x138（[wp penalty]）
    float m_monsterExpConst;       // +0x13c（[monster exp const]，ScanFloat）
    int m_num;                     // +0x140（时段数）
    int m_warRoomCount[24][10];    // +0x144..+0x503（[curHourIdx][idx] 目标房间数，
                                   //  ORIG clear memset 0x3c0 = 24×10 int）
};

class QuestParameterScript
{
public:
    QuestParameterScript();
    ~QuestParameterScript();
    void clear();

    // ORIG C1 0x836f1c2 / clear 0x836f130 / D1 0x837d3ca：+0x00 map<char,int>
    // +0x18 int[3] +0x24 vector<int> +0x30 vector<int> +0x3c map<int,int>
    // +0x54 map<int,vector<pieceQuestRewardStatus>>，sizeof 0x6c
    std::map<char, int> m_map0;                              // +0x00
    int m_field18;                                           // +0x18
    int m_field1c;                                           // +0x1c
    int m_field20;                                           // +0x20
    std::vector<int> m_vec24;                                // +0x24
    std::vector<int> m_vec30;                                // +0x30
    std::map<int, int> m_map3c;                              // +0x3c
    std::map<int, std::vector<pieceQuestRewardStatus> > m_map54;  // +0x54

    // ---- 奖励查询（报告 docs/class_func_reports/QuestParameterScript/）----
    // ORIG T 0x08a6bd1c：map<char,int> find，未命中返回 100。
    int getRewardWeight(char level);
    // ORIG T 0x08a6bd86 / 0x08a6bda6：vector<int>::at 取值（越界抛出）。
    int getRewardExp(int idx);                             // m_vec24
    int getRewardGold(int idx);                            // m_vec30
    // ORIG T 0x08a6bdc6 / 0x08a6be66：奖励等级/状态查询（定义 CDataManagerScripts.cpp）。
    int getRewardLevel(int level);
    std::vector<pieceQuestRewardStatus> getRewardStatus(int level);
};

class TrainingQuestScript;

class GuildParameterScript
{
public:
    GuildParameterScript();
    ~GuildParameterScript();
    void clear();
    // ORIG T 0x08979864：公会经验权重（clear_reward 公会经验发放）。
    double getExpWeight(int guildLevel);  // ORIG 返回 double（fldl；m_vecE4 的 second）
    // ---- Wave6-CUser 补声明（ORIG T 实证；定义 owner=guild 脚本批次）----
    int getGuildLevelUpParam(int level);                     // T 0x08979648 _ZN20GuildParameterScript20getGuildLevelUpParamEi
    int getLevelupPrice(int level, int memberCnt);           // T 0x089797a4 _ZN20GuildParameterScript15getLevelupPriceEii
    int getGuildExpBook(int key);                            // T 0x08979672 _ZN20GuildParameterScript15getGuildExpBookEi（m_map10 find→second/0）
    STGuildStorageUpgradeInfo* getGuildCargoUpradeInfo(int capacity); // ORIG T 0x08979726
    int getGuildExpWeight(int level);                       // T 0x089795de
    STAgitUpgradeInfo* getGuildAgitUpgradeInfo(int level);  // T 0x089796dc
    // ORIG C1 0x837058a / clear 0x83704ae / D1 0x837d542，sizeof 0x11c：
    // +0x00 int +0x04 int +0x08 char +0x0c int +0x10 map<int,int>
    // +0x28 int +0x2c vector<STAgitUpgradeInfo> +0x38
    // vector<STGuildStorageUpgradeInfo> +0x44 map<int,int>
    // +0x5c int[0x11][2]（getGuildLevelUpParam 以 +0x5c+(n-1)*8 访问）
    // +0xe4 vector<pair<int,double>> +0xf0 vector<pair<int,vector<int>>>
    // +0xfc double +0x104 int +0x108 int +0x10c int +0x110 double
    int m_field0;                                   // +0x00
    int m_field4;                                   // +0x04
    char m_field8;                                  // +0x08
    int m_fieldC;                                   // +0x0c
    std::map<int, int> m_map10;                     // +0x10
    int m_field28;                                  // +0x28
    std::vector<STAgitUpgradeInfo> m_vec2c;         // +0x2c
    std::vector<STGuildStorageUpgradeInfo> m_vec38; // +0x38
    std::map<int, int> m_map44;                     // +0x44
    int m_levelUpCost[0x11][2];                     // +0x5c（16 项有效，[17] 含）
    std::vector<std::pair<int, double> > m_vecE4;   // +0xe4
    std::vector<std::pair<int, std::vector<int> > > m_vecF0;  // +0xf0
    double m_fieldFC;                               // +0xfc
    int m_field104;                                 // +0x104
    int m_field108;                                 // +0x108
    int m_field10c;                                 // +0x10c
    double m_field110;                              // +0x110
    int m_mail;                                     // +0x118（[guild mail]，importGuildParameterScript +0x2e9 写入）
};

class PowerParameterScript
{
public:
    PowerParameterScript();
    ~PowerParameterScript();
    void clear();   // ORIG W _ZN20PowerParameterScript5clearEv 0x0837187a（importPowerParameterScript +0x038 调用）
    // W6_AssaultPlaceA：_VictoryBonus 胜利惩罚/连破奖励查询
    stVictoryPenalty GetPenalty(int victory);   // ORIG T 0x08a59c6e（sret，按值返回）
    int GetBreakBonus(int winCount);            // ORIG T 0x08a59d58
    bool checkPowerWarServer(int a, int b);                 // T 0x08a59db4
public:
    // 布局依据 ORIG ctor (0x08371a26) / clear (0x0837187a) / D1 (0x0837d634) /
    // importPowerParameterScript (0x08a592bc) / checkPowerWarServer (0x08a59db4,
    // 读 +0x9c) 推导，sizeof 保持 0xd8。D1 析构 7 个容器成员：
    // +0xc8 vector<int> +0xbc vector<stVictoryPenalty> +0xac vector<int>
    // +0x9c vector<pair<int,int>> +0x70 vector<pair<int,int>>
    // +0x38 vector<int> +0x20 vector<int>
    // +0x00..+0x13：importPowerParameterScript 开头 5 个标量（asm 实证
    // +0x09a/+0x0c4/+0x0ef/+0x11d/+0x14b 依次写入）。
    int m_powerJoinCost;                                    // +0x00（[power join cost]）
    int m_powerJoinGuildLevel;                              // +0x04（[power join guild level]）
    int m_powerJoinQuest;                                   // +0x08（[power join quest]）
    int m_killPointToEndPowerWar;                           // +0x0c（[kill point to end power war]）
    int m_expReward;                                        // +0x10（[exp reward]）
    int m_ghostTime;                                        // +0x14（getPowerWarGhostTime 读 CDataManager+0xa63c）
    int m_penaltyThreshold;                                 // +0x18（GetPowerWarResponPenalty 读 +0xa640）
    int m_powerUpRatio;                                     // +0x1c（[power up ratio]，import +0x27e 写入）
    std::vector<int> m_vec20;                               // +0x20
    int m_easyAssaultRange;                                 // +0x2c（[easy assault range]，import +0x176 写入）
    int m_refreshProcessInfo;                               // +0x30（[refresh power war process info]，import +0x231）
    int m_abuserBanTime;                                    // +0x34（[abuser ban time]，import +0x25f）
    std::vector<int> m_vec38;                               // +0x38（ORIG vector<int> 区）
    int m_pcRoomBonus;                                      // +0x44（[pc room bonus]，import +0x625 写入）
    int m_powerUpGeneratePeriod;                            // +0x48（[power up generate period]，import +0x317）
    int m_powerUpAssaultCount;                              // +0x4c（[power up assault count]，import +0x2bb）
    int m_powerUpAssaultVictoryRatio;                       // +0x50（[power up assault victory ratio]，import +0x2e9）
    int m_powerWarPointMultiple;                            // +0x54（[powerwar point multiple number]，import +0x203）
    int m_straightVictories;                                // +0x58（[straight victories]，import +0x399）
    int m_timeAssaultSuperPowerStateOnJoin;              // +0x5c
    int m_timeAssaultSuperPowerStateAfter;               // +0x60
    int m_minEnterLevel;                                    // +0x64（CheckPowerWarEnterLimit 读 +0xa68c）
    int m_minEnterAge;                                      // +0x68（CheckPowerWarEnterLimit 读 +0xa690）
    int m_itemInitialCooltime;                              // +0x6c（[item initial cooltime]，import +0x3c7 写入）
    std::vector< std::pair<int, int> > m_vec70;             // +0x70
    // +0x7c..+0x9b：importPowerParameterScript 依次写入的 8 个 int
    // （asm 实证：+0x45c/+0x48a/+0x4bb/+0x4ec/+0x51d/+0x54e/+0x57f/+0x594）。
    int m_basicPowerWarPoint;                               // +0x7c（[basic powerwar point]）
    int m_firstGradeBonusPoint;                             // +0x80（[first grade bonus powerwar point]）
    int m_rankingBonusGrade;                                // +0x84（[ranking bonus grade]）
    int m_rankingBonusDecreasePoint;                        // +0x88（[ranking bonus decrease point]）
    int m_decreasePointPerDay;                              // +0x8c（[decrease powerwar point per day]）
    int m_requiredUserPoint4Dungeon;                        // +0x90（[required user powerwar point 4 dungeon]）
    int m_powerWarDungeonInfo0;                             // +0x94（[power war dungeon info] 第 1 值）
    int m_powerWarDungeonInfo1;                             // +0x98（[power war dungeon info] 第 2 值）
    std::vector< std::pair<int, int> > m_powerWarServerList; // +0x9c（checkPowerWarServer 读 +0xa6c4）
    int m_powerUpPointMod;                                  // +0xa8（[power up point mod]，import +0x79a 写入）
    std::vector<int> m_vecAC;                               // +0xac
    int m_powerWarCooldownMinutes;                          // +0xb8（ProcessJoinPowerWar 读 +0xa6e0）
    std::vector<stVictoryPenalty> m_vecBC;                  // +0xbc
    std::vector<int> m_vecC8;                               // +0xc8
    int m_victoriousRewardTime;                             // +0xd4（[victorious reward time]，import +0x822 写入）
};

// ---- SecretShopScript 脚本域结构（全局作用域，匹配 ORIG mangled：stSaleInfo/
//      stDungeonSales/stNpcSales/stDungeonNpc/stLevelSection 均无命名空间） ----
struct stSaleInfo
{
    int m_itemIdx;    // +0x00
    char m_rule;      // +0x04（eBuyRule：0=金币随机价，1=配方固定价）
    char m_pad05[3];  // +0x05
    int m_price;      // +0x08
    int m_materialCount;  // +0x0c
    int m_limit;      // +0x10
    int m_rate;       // +0x14（域权重，GetDomainRate/GetItems 累加）
};

struct stDungeonSales
{
    int m_field0;                 // +0x00
    int m_maxCount;               // +0x04（GetItems 读取，本域单次最大售出数）
    std::map<int, stSaleInfo> m_sales;  // +0x08
};

struct stNpcSales
{
    int m_field0;                                  // +0x00
    std::map<int, stDungeonSales> m_dungeonSales;  // +0x04（ORIG D1 0x83ce6d6 析构此 map）
    std::map<int, stDungeonSales> m_eventSales;    // +0x1c（ORIG D1 析构第二个同型 map）[推断]
};  // sizeof 0x34

struct stDungeonNpc
{
    // 2026-08-31：ORIG sizeof 12（multimap 节点 _Rb_tree_node 32 = 16 + pair(4+12)；
    // new_allocator 32B）——原 m_fieldC 多余，删。
    int m_field0;   // +0x00
    int m_npcIdx;   // +0x04（GetNpcIdx 返回）
    int m_rate;     // +0x08（GetNpcIdx 累加权重）
};

struct stLevelSection
{
    int m_levelIdx;   // +0x00（GetLevelIdx 返回）
    int m_minLev;     // +0x04
    int m_maxLev;     // +0x08
    bool operator<(const stLevelSection& o) const
    {
        // ORIG: 比较 m_levelIdx（+0x00；near 集 ORIG mov (%eax) vs 我们 mov 0x4(%eax)）。
        return m_levelIdx < o.m_levelIdx;
    }
};

class SecretShopScript
{
public:
    SecretShopScript();
    ~SecretShopScript();
    char GetNpcByDungeonIdx(int& out, int a, int b);
    char GetNpcByDungeonLev(int& out, int a, int b);
    char GetItemByDungeonIdx(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                             int dungeonIdx, int level, bool event);
    char GetItemByDungeonLev(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                             int dungeonIdx, int level, bool event);
    void clear();
    // ORIG 0x8a82804：对每个 key 组累加 m_rate，总和 !=10000 时打印告警。
    void verifyNpc(std::multimap<int, stDungeonNpc>& npcMap);
public:
    // ORIG：数据成员可被自由函数 importSecretShopScript（0x8a81cf0）直接
    // 访问，故为 public。
    // ---- 布局（依据 ORIG SecretShopScript::SecretShopScript() 构造汇编推导） ----
    // ORIG C1 0x83720f4 实证：容器相邻无 pad —— multimap@+0x00 / multimap@+0x18 /
    // set@+0x30 / map@+0x48 / stDungeonSales@+0x60（sizeof 0x20）；clear 0x8372020
    // 另写 +0x80/+0x84 两 int；总大小 0x88（CDM m_titleBook@+0xa788 边界互证）。
    std::multimap<int, stDungeonNpc> m_npcMap;      // +0x00（[npc] 段）
    std::multimap<int, stDungeonNpc> m_levelNpcMap; // +0x18（[level npc] 段，verifyNpc）
    std::set<stLevelSection> m_levelSections;       // +0x30（[level section] 段，GetLevelIdx）
    std::map<int, stNpcSales> m_dungeonNpcMap;      // +0x48（[dungeon npc] 段，GetItemByDungeonIdx）
    stDungeonSales m_eventDungeonSales;             // +0x60（GetItemByEvent）
    int m_priceVarPercent;                          // +0x80（GetRandItemPrice 读取 /100）
    int m_int84;                                    // +0x84

    // 内部辅助（CSecretShop.cpp 实现，供 GetItemByDungeon* 调用）
    char GetItems(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out,
                  stDungeonSales& sales);
    char GetItemByEvent(CMTRand* rand, std::vector<secretshop::SALE_INFO>& out);
    void CopyItem(secretshop::SALE_INFO& sale, stSaleInfo& info);
    bool IsExistItem(std::vector<secretshop::SALE_INFO>& out, int itemIdx);
    int GetDomainRate(stDungeonSales& sales);
    int GetLevelIdx(int level);
    int GetRandItemPrice(int price);
    // ORIG 0x8a82998：遍历 equal_range 结果累加 stDungeonNpc::m_rate（+0x8），
    // 首次超过 target 时 *out = m_npcIdx（+0x4）并返回 1；否则 0。
    // 供 GetNpcByDungeonIdx/Lev（0x8a828f8/0x8a8293c）调用。
    char GetNpcIdx(int& out, int target,
                   std::pair<std::multimap<int, stDungeonNpc>::iterator,
                             std::multimap<int, stDungeonNpc>::iterator>& range);
};

class stTitleBookInfo
{
public:
    stTitleBookInfo();
    ~stTitleBookInfo();

    // ORIG C1 0x837d828 / D1 0x837d906：+0x00 vector<int>
    // +0x0c vector<stTitleSection> +0x18 map<int,vector<stTitleElement>>
    // +0x30 vector<stCustomConditionMsg> +0x3c vector<stAdjust>
    // +0x48 vector<stTitleRewardBasis*>，sizeof 0x58
    std::vector<int> m_vec0;                                  // +0x00
    std::vector<stTitleSection> m_vecC;                       // +0x0c
    std::map<int, std::vector<stTitleElement> > m_map18;      // +0x18
    std::vector<stCustomConditionMsg> m_vec30;                // +0x30
    std::vector<stAdjust> m_vec3c;                            // +0x3c
    std::vector<stTitleRewardBasis*> m_vec48;                 // +0x48
    // ORIG [view titlebook eventmark]（0x8aa8a86..0x8aa8ab6）：ScanInt(int*)
    // 成功后写一字节 (v != 0) 到 this+0x54。
    char m_viewEventMark;                                     // +0x54
    char m_pad55[3];                                          // +0x55
};

// ORIG 的 AccountCargoScript::m_upgradeMap 为 std::map<int, ::UpgradeInfo>——
// ORIG 反汇编 mangled 无类前缀（_ZNSt3mapIi11UpgradeInfo...），故 UpgradeInfo
// 必须是全局类型（嵌套类会生成 IN18AccountCargoScript11UpgradeInfo mangled，
// 与 ORIG 不符）。importAccountCargoScript（0x088c7e80）构造该值类型。
struct UpgradeInfo
{
    int m_field0;   // +0x00（=档位 key；CAccountCargo.cpp 读作新容量）
    int m_field4;   // +0x04
    int m_field8;   // +0x08（所需道具 idx）
    int m_fieldC;   // +0x0c（所需金币）
};

class AccountCargoScript
{
public:
    AccountCargoScript();
    ~AccountCargoScript();
    void* GetCurrUpgradeInfo(int idx);
    void* GetCreateInfo();
    void* GetNextUpgradeInfo(int idx);
    void clear();   // ORIG W 0x08372442：两 int 清零 + m_upgradeMap.clear()（importAccountCargoScript 调用）

    // ORIG +0x8 为 std::map<int, UpgradeInfo>（C1 0x8372480 构造）；
    // GetCurrUpgradeInfo/GetNextUpgradeInfo/GetCreateInfo（0x88c80ba/0x88c8124/
    // 0x88c8056）在该 map 上 find/begin/++ 返回 value 指针。
    int m_reqLevel;        // +0x00（CAccountCargo.cpp:482 读作 reqLevel）
    int m_itemIdx;         // +0x04
    std::map<int, UpgradeInfo> m_upgradeMap;  // +0x08（ORIG C1 0x8372480 构造于此，
    //   map 尾即 +0x20；sizeof 0x20 由 CDM m_pvpSkillTree@+0xa800 边界互证）
};

class PvPSkillTreeParameterScript
{
public:
    PvPSkillTreeParameterScript();
    ~PvPSkillTreeParameterScript();
    int getPvPSkillPoint(int job, int firstGrow, int secondGrow, int grade,
                         bool flag) const;
    bool getGiveSkill(int job, int firstGrow, int secondGrow,
                      std::vector<std::pair<int, int> >& out) const;
    int getInitSkillTreeGrade(int grade) const; // ORIG W 0x084e913a
    int getGivePvPSkillPoint(int job, int firstGrow, int secondGrow,
                             int grade) const; // ORIG T 0x08a5dc74
    int getPvPSkillPointNext(int job, int firstGrow, int secondGrow,
                             int grade) const;              // T 0x08a5df0e
    // ORIG T 0x08a5db5a：按职业/成长/段位取技能树 (skillIdx,level) 列表
    bool getPvPSkillTree(int job, int firstGrow, int secondGrow, int grade,
                         bool flag,
                         std::vector<std::pair<int, int> >** out) const;
    // ORIG _ZN27PvPSkillTreeParameterScript24load_pvp_skill_tree_listEv /
    //      _ZN27PvPSkillTreeParameterScript29importPairPvPSkillPointScriptEPKc（init 调用）
    bool load_pvp_skill_tree_list();
    bool importPairPvPSkillPointScript(const char* path);
    bool importPvPSkillTreeParameterScript(const char* path);   // ORIG T 0x08a5cf16
    void clear();

    // ORIG C1 0x8371eee / D1 0x837da02：+0x00 map<PvPSkillTreeKey,
    // map<int,PvPSkillTreeValue>> +0x18 map<int,map<int,int>> +0x30
    // map<PvPSkillTreeKey,FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>
    // +0x54 map<int,int>，sizeof 0x6c
    std::map<PvPSkillTreeKey, std::map<int, PvPSkillTreeValue> > m_map0;  // +0x00
    std::map<int, std::map<int, int> > m_map18;                           // +0x18
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT> m_map30;                // +0x30
    int m_field48;                                                        // +0x48
    int m_field4c;                                                        // +0x4c
    char m_field50;                                                       // +0x50
    char m_pad51[3];                                                      // +0x51
    std::map<int, int> m_map54;                                           // +0x54
};

class EventCharacterParameterScript
{
public:
    EventCharacterParameterScript();
    ~EventCharacterParameterScript();
    // ORIG _ZN29EventCharacterParameterScript25load_event_character_listEv（init 调用）
    bool load_event_character_list();
private:
    char m_pad[0x18];
};

class IndependentDropParameterScript
{
public:
    IndependentDropParameterScript();
    ~IndependentDropParameterScript();
    void clear();

    bool load_independent_drop_list();                     // ORIG _ZN30IndependentDropParameterScript26load_independent_drop_listEv
    int  ImportIndependentDrop(const char* path);          // ORIG _ZN30IndependentDropParameterScript21ImportIndependentDropEPKc（0=成功）
    bool importIndependentDropParameterScript(const char* path);  // ORIG T 0x0897c1c6
    // ORIG C1 0x8372d46 / D1 0x837daac：+0x00 vector<IndependentDropListInfo>
    // +0x0c vector<stIndependentDropInfo> +0x18 vector<stIndependentDropRateControl>
    // +0x24 map<int,pair<int,int>>，sizeof 0x3c
    std::vector<IndependentDropListInfo> m_vec0;           // +0x00
    std::vector<stIndependentDropInfo> m_vecC;             // +0x0c
    std::vector<stIndependentDropRateControl> m_vec18;     // +0x18
    std::map<int, std::pair<int, int> > m_map24;           // +0x24
};

class stMercenarySystemInfo
{
public:
    stMercenarySystemInfo();
    ~stMercenarySystemInfo();

    // ORIG C1 0x8373328：+0x00 int（=0xe10）+0x04 ushort（=0xc8）
    // +0x08 vector<pair<int,pair<int,int>>> +0x14 vector<pair<int,float>>
    // +0x20 vector<pair<int,float>> +0x2c string +0x30 vector<pair<int,float>>
    // +0x3c int（=0）+0x40 vector<stMercenaryCompetitionArea>，sizeof 0x4c
    int m_field0;                                          // +0x00（0xe10）
    unsigned short m_field4;                               // +0x04（0xc8）
    char m_pad6[2];                                        // +0x06
    std::vector<std::pair<int, std::pair<int, int> > > m_vec8;   // +0x08
    std::vector<std::pair<int, float> > m_vec14;           // +0x14
    std::vector<std::pair<int, float> > m_vec20;           // +0x20
    std::string m_str2c;                                   // +0x2c
    std::vector<std::pair<int, float> > m_vec30;           // +0x30
    int m_field3c;                                         // +0x3c（0）
    std::vector<stMercenaryCompetitionArea> m_vec40;       // +0x40
};

// ORIG channel_dungeon_info_t（ctor 0x088dd34a / dtor 0x083d8b2c）：
// +0x00 std::string（分类键名副本）+0x04 std::vector<int>（频道区间表），sizeof 0x10。
struct channel_dungeon_info_t
{
    channel_dungeon_info_t();
    ~channel_dungeon_info_t();
    channel_dungeon_info_t& operator=(const channel_dungeon_info_t& o);  // 0x088dd390

    std::string m_keyName;      // +0x00
    std::vector<int> m_intList; // +0x04
};

struct channel_info_t
{
    // ORIG 实布局（ctor 0x088dd2a8 / dtor 0x083f11da / operator= 0x088dd3c6，
    // sizeof 0x40）：getBonusRate 读 +0x10 指针与 +0x14 字节，偏移不变。
    channel_info_t();
    ~channel_info_t();
    channel_info_t& operator=(const channel_info_t& o);

    std::string m_name;             // +0x00（频道名）
    int m_channelType;              // +0x04
    std::string m_dungeonName;      // +0x08（脚本内 dungeon 分类名，"[none]" 表示无）
    std::string m_dungeonKey;       // +0x0c（命中分类后复制的键名副本）
    std::vector<int>* m_pBonusRates;  // +0x10（别名指向 channel_dungeon_info_t::m_intList）
    unsigned char m_bonusRate;      // +0x14（getBonusRate 以 /1000.0f 转 float）
    float m_gridPercent[10];        // +0x18..+0x40（等级区间百分比）
};

struct channel_script_t
{
    // ORIG 布局（ctor 0x837dc56 / CDataManager ctor 0x8356081 反编译，32 位）：
    // +0x00 std::map<std::string, channel_dungeon_info_t>（0x18，applyChannelScript 导入）；
    // +0x18 std::map<int,std::map<int,channel_info_t>>（0x18，getChannelInfo/getBonusRate
    // 查表，外层 key=serverNo，内层 key=channelNo）。
    channel_script_t();
    ~channel_script_t();
    channel_info_t* getChannelInfo(unsigned char channel, unsigned int idx) const;
    float getBonusRate(unsigned char channel, unsigned int a, unsigned int b) const;
    // ORIG T 0x08a8d0ae：服务器加成版本（服务器频道*索引 0/1/2 分流；定义归
    // channel 域导入 TU）。clear_reward 经验通道加成在 server channel 1/2 时调用。
    float getBonusRateEventServer(unsigned char channel, unsigned int a) const;  // T 0x088dcee2
    int GetPvPChannelNumber(unsigned char channel, unsigned int channelNo) const;  // T 0x088dcc28

private:
    std::map<std::string, channel_dungeon_info_t> m_channelDungeonMap;  // +0x00
    std::map<int, std::map<int, channel_info_t> > m_channelMap;         // +0x18

    // ORIG 自由函数（定义 InitScriptProducers_1.cpp），需写两个 map 成员。
    friend int applyChannelScript(channel_script_t* script);
    friend void traversalChannelScript(channel_script_t* script);   // 定义 InitScriptProducers_1.cpp（读两 map）
};

// ---- 售货机三型（ORIG importVendingMachineInfo 0x0890b74e / Core 0x0890b2a3
//      与 stVendingMachineInfo_t ctor 0x0891b36a/clear 0x0891b3fe 实证）----
struct stVendingMachineOutput_t
{
    int m_itemId;           // +0x00（行首列）
    int m_cumWeight;        // +0x04（m_giveCount/m_bonusCount 运行累计）
    unsigned int m_count;   // +0x08（第三列）
};                          // sizeof 0xc

struct stVendingMachineInfo_t
{
    stVendingMachineInfo_t();    // ORIG W 0x0891b36a：三 vector ctor 后 clear()
    ~stVendingMachineInfo_t();   // ORIG W 0x083f2f3a
    void clear();                // ORIG W 0x0891b3fe：+4 置 -1，其余清零/清容器

    int m_groupNum;                                    // +0x00（[group num]）
    int m_payType;                                     // +0x04（[material] 第 1 列；clear=-1）
    int m_payAmount;                                   // +0x08（[material] 第 2 列）
    int m_giveCount;                                   // +0x0c（[output] 权重累计）
    std::vector<stVendingMachineOutput_t> m_vecGive;   // +0x10
    int m_bonusCount;                                  // +0x1c（[bonus output] 权重累计）
    std::vector<stVendingMachineOutput_t> m_vecBonus;  // +0x20
    std::vector<std::pair<int, int> > m_lottery;       // +0x2c（flag==1 行 (itemId,count)）
};                                                     // sizeof 0x38

struct stVendingMachine_t
{
    stVendingMachine_t();     // ORIG W 0x0891c820：仅构造 +0x04 vector（m_id 不初始化）
    ~stVendingMachine_t();    // ORIG W 0x083e636c

    int m_id;                                        // +0x00（列表键）
    std::vector<stVendingMachineInfo_t> m_infoList;  // +0x04
};                                                   // sizeof 0x10


class CMission;  // 前向：m_missionMap/m_kindMissionList 值类型（完整定义见 CMission.h）
struct MissionScript;  // 前向：Factory/Load_list 形参（完整定义见 MissionTypes.h）
class MissionParameterScript;  // 前向：copy_missionParameter 形参
class PvP_MissionSystem
{
public:
    PvP_MissionSystem();                     // ORIG 0x85e1abc
    ~PvP_MissionSystem();                    // ORIG 0x85e1c48
    // ORIG 布局（ctor 0x85e1abc / dtor 0x85e1c48 反编译，32 位）：
    // +0x00 map<int,CMission*>（0x18）；+0x18 vector<list<CMission*>>（ctor resize 36）；
    // +0x24 vector<uint> m_dailyMissionIndices；+0x30 int；+0x34 list<pair<ui,ui>>；
    // +0x3c int；+0x40 bitset<32>；+0x44 CMTRand*；+0x48 vector<uint>。
    CMission* Factory(const MissionScript& script);           // ORIG 0x85e2132
    // ORIG _ZN17PvP_MissionSystem9Load_listEv / _ZN17PvP_MissionSystem17Load_EtcParameterEv（init 调用）
    bool Load_list();
    bool Load_EtcParameter();
    void copy_missionParameter(const MissionParameterScript& param);  // ORIG 0x85e206e
    std::bitset<32> get_daily_mission_kind() const;
    CMission* find_mission(int idx);
    void get_New_DailyMission(std::vector<std::pair<unsigned int, unsigned int> >& out);
    int get_WithinMissionIndex(int idx) const;
    std::list<CMission*>& get_kind_mission_list(unsigned int idx);
    int get_BaseMissionExp_byRank(unsigned short rank) const;
    void sort_list();
private:
    std::map<int, CMission*> m_missionMap;                        // +0x00
    std::vector<std::list<CMission*> > m_kindMissionList;         // +0x18
    std::vector<unsigned int> m_dailyMissionIndices;              // +0x24
    int m_field30;                                                // +0x30
    std::list<std::pair<unsigned int, unsigned int> > m_dailyMissionList;  // +0x34
    int m_field3c;                                                // +0x3c
    std::bitset<32> m_dailyMissionKind;                           // +0x40
    void* m_trand;                                                // +0x44（CMTRand*，布局未落地）
    std::vector<unsigned int> m_baseMissionExp;                   // +0x48
};

struct pvp_channel_info_t
{
    pvp_channel_info_t();

    // +0x00..+0x23 为 importPVPChannelList（ORIG 0x08901f10）逐条扫描的
    // 9 个 int。asm 扫描顺序与偏移：v1→+0x00、v2→+0x04、v3→+0x08、
    // v4→+0x10、v5→+0x0c、v6..v9→+0x14/+0x18/+0x1c/+0x20（+0x4d2 起）。
    int m_field0;             // +0x00（扫描序 1；GetPvPChannelGrade trace 未打印）
    int m_minGrade;           // +0x04（GetPvPChannelGrade trace 第 1 参；扫描序 2）
    int m_maxGrade;           // +0x08（GetMaxGradePvPChannel ORIG 0x822b65a 返回；trace 第 2 参；扫描序 3）
    int m_upgradeRevision;    // +0x0c（trace 第 3 参；扫描序 5）
    int m_channelType;        // +0x10（GetPvPChannelType ORIG W 0x85e68a8 返回；trace 第 4 参 type；扫描序 4）
    int m_minLevel;           // +0x14（trace 第 5 参 minLevel；扫描序 6）
    int m_maxLevel;           // +0x18（trace 第 6 参 maxLevel；扫描序 7）
    int m_field1c;            // +0x1c（扫描序 8）
    int m_field20;            // +0x20（扫描序 9）
    // 2026-08-31：m_pvpMissionSystem 移出——ORIG sizeof 36（new_allocator 36B；
    // C1 0x8370900 只清零 +0x0..+0x20）；该指针是 CDataManager 独立成员
    // +0xb454（find_mission 0x835fa8f 读 CDataManager+0xb454）。
};

class stItemMakingSkill
{
public:
    stItemMakingSkill();
    ~stItemMakingSkill();
    void clear();                 // ORIG 0x837105a

    // ORIG D1 0x837dce2：+0x00 map<int,stUpgradeWithCompound>
    // +0x18 vector<pair<int,int>>，sizeof 0x30。
    // +0x24/+0x28/+0x2c 三 int 由 ImportItemMakingSkill(0x0890f564)
    // [itemmaking exp]/[itemmaking level limit] 段写入。
    std::map<int, stUpgradeWithCompound> m_map0;    // +0x00
    std::vector<std::pair<int, int> > m_vec18;      // +0x18
    int m_exp;                                      // +0x24（[itemmaking exp] 第 1 值）
    int m_bonusExp;                                 // +0x28（[itemmaking exp] 第 2 值）
    int m_levelLimit;                               // +0x2c（[itemmaking level limit]）
};

class BlueMarbleScriptManager;   // 权威声明见 PvPTypes.h

class ChoiceItemInfo
{
public:
    ChoiceItemInfo();
    void clear();               // ORIG W _ZN14ChoiceItemInfo5clearEv：m_checked=0、m_choice=-1
    bool m_checked;             // +0x00
    char m_pad1[3];             // +0x01
    unsigned int m_choice;      // +0x04
    // init() 将 BlueMarbleScriptManager::getInsance() 存入 +0x08，destroy() delete；
    // 类型语义化为指针（尺寸不变）。
    BlueMarbleScriptManager* m_blueMarbleScriptMgr;  // +0x08
};

class ImageCommunicationData
{
public:
    ImageCommunicationData();
    int m_charge;   // +0x00（ORIG importImageCommunicationInfo 写首字）
};

namespace ARAD
{
namespace SCRIPT
{

// ORIG（df_game_r nm -S 实证）：
//   _ZN4ARAD6SCRIPT22_AradJumpingRewardItemD1Ev      W 0x088b597e（5B 空体）
//   _ZN4ARAD6SCRIPT21AradJumpingScriptDataC1Ev       W 0x0837170a（调 vector C1）
//   _ZN4ARAD6SCRIPT21AradJumpingScriptDataD1Ev       W 0x0837171e（调 vector D1）
//   _ZN4ARAD6SCRIPT18AradJumping_ScriptC1Ev          W 0x08371732（转发 ScriptDataC1）
//   _ZN4ARAD6SCRIPT18AradJumping_ScriptD1Ev          W 0x08371746（转发 ScriptDataD1）
// ORIG C1 反汇编【objdump】：AradJumpingScriptData 仅构造 m_rewardItems vector，
// AradJumping_Script ctor/dtor 直接转发基类 —— 即 AradJumping_Script 继承
// AradJumpingScriptData，_AradJumpingRewardItem 位于 ARAD::SCRIPT 名字空间级。
struct _AradJumpingRewardItem
{
    int            m_job;        // +0x00
    int            m_growValue;  // +0x04
    unsigned long  m_money;      // +0x08（ia32 ulong=4B）
    ~_AradJumpingRewardItem();   // ORIG 空体 dtor（显式声明以产出 D1 符号）
};

class AradJumpingScriptData
{
public:
    AradJumpingScriptData();
    ~AradJumpingScriptData();

    std::vector<_AradJumpingRewardItem> m_rewardItems;   // +0x00（vector=0xc 与 ORIG 一致）
};

class AradJumping_Script : public AradJumpingScriptData
{
public:
    AradJumping_Script();
    // ORIG _ZNK4ARAD6SCRIPT18AradJumping_Script16getCurRewardItemEiiRm
    // 0x088b424a（报告 ARAD__SCRIPT__AradJumping_Script/getCurRewardItem.md）：
    // 线性扫 m_rewardItems，(m_job==job && m_growValue==growValue) → out=m_money。
    bool getCurRewardItem(int job, int growValue, unsigned long& out);
    ~AradJumping_Script();

    // ORIG _ZN4ARAD6SCRIPT18AradJumping_Script17getRDARScriptDataEv
    // W 0x088b43ee（8B）：返回 this+0 —— 即 &m_rewardItems。【objdump】
    std::vector<_AradJumpingRewardItem>* getRDARScriptData();

    // ORIG _ZN4ARAD6SCRIPT18AradJumping_Script12importScriptEPcRS1_
    // T 0x088b4128（static：参数表无 this，"path"与"target"从 ebp+8/ebp+12 取）。
    static bool importScript(char* path, AradJumping_Script& target);
};

typedef char arad_jumping_script_size_assert[sizeof(ARAD::SCRIPT::AradJumping_Script) == 0xc ? 1 : -1];
}  // namespace SCRIPT
}  // namespace ARAD

class CLottery_NeedMoney
{
public:
    CLottery_NeedMoney();
    ~CLottery_NeedMoney();
    bool isNeedMoney(unsigned int money) const;
    void add_lottery_needMoney(unsigned int money, unsigned int cost);
private:
    std::map<unsigned int, unsigned int> m_needMoneyMap;  // +0x00（ORIG 0x8365162；GCC4.4 map sizeof 0x18）
};   // sizeof 0x18

namespace EventClassify { class CEventScriptMng; }   // 权威定义见 CEventScriptMng.h

struct live_server_info_t
{
    live_server_info_t();
    ~live_server_info_t();
    // ORIG C1 0x0837dd64 = std::map<int, std::vector<pvp_channel_info_t> > 构造
    // （GetPvPChannelGrade 按此宿主查找/填充）。+0xb504 处为 m_eventScriptMng
    // （init() new CEventScriptMng 写入、get_event_script_mng 读取），
    // 故基址 +0xb4ec 后 map 占 0xc、pad 补到 +0x18；类型语义化为指针（尺寸不变）。
    std::map<int, std::vector<pvp_channel_info_t> > m_pvpChannelMap;  // +0x00（GCC4.4 map sizeof 0x18）
    EventClassify::CEventScriptMng* m_eventScriptMng;  // +0x18
};   // sizeof 0x1c

// EventAvatarListScript 权威定义：EventAvatarListScript.h（set<int> m_avatars，
// ORIG C1 0x8372e10 / clear 0x8372e52；与 importEventAvatarListScript 0x895e1ed
// 的 set::insert 调用一致）。本地 m_pad 镜像已删除以消除跨 TU 双定义。
#include "EventAvatarListScript.h"

class EventEtcScript
{
public:
    EventEtcScript();
    ~EventEtcScript();
    void clear();

    // ORIG C1 0x8372e66 / clear 0x8372ece / D1 0x837dda0：+0x00 vector<int>
    // +0x0c map<uint,uint>，sizeof 0x24
    std::vector<int> m_vec0;            // +0x00
    std::map<unsigned int, unsigned int> m_mapC;  // +0x0c
};

class stReturnUserRewardScript
{
public:
    stReturnUserRewardScript();
    ~stReturnUserRewardScript();

    // 成员公开：ImportReturnUserRewardScript（InitScriptProducers_2.cpp）按
    // ORIG 0x089180ea 直写三个字段。
    // ORIG C1 0x8371544：+0x00 int=0、+0x04 int=0、+0x08 stLevelUpRewardItemScript 构造+clear。
    int m_field0;                        // +0x00
    int m_field4;                        // +0x04
    stLevelUpRewardItemScript m_levelUp; // +0x08（sizeof 0x20）
};

class stBingoScript
{
public:
    stBingoScript();
    ~stBingoScript();

    // ORIG C1 0x83715b6 / D1 0x837ddfc：+0x00 map<int,int> +0x18
    // map<int,vector<int>> +0x30 map<int,pair<int,int>> +0x48 vector<int>
    // sizeof 0x54
    std::map<int, int> m_map0;                     // +0x00
    std::map<int, std::vector<int> > m_map18;      // +0x18
    std::map<int, std::pair<int, int> > m_map30;   // +0x30
    std::vector<int> m_vec48;                      // +0x48
};

class stBroadCastItemScript
{
public:
    stBroadCastItemScript();
    ~stBroadCastItemScript();

    // 查询：isUnCommonItem 0x8918d9c，checkCommonItem 0x8918cdc，
    // checkUnCommonNestedItem 0x8918dfe（均 const，返回 bool）
    bool isUnCommonItem(int itemIdx) const;
    bool checkCommonItem(int itemIdx) const;
    bool checkUnCommonNestedItem(int itemIdx, int subItemIdx) const;

    // ORIG D1 0x837de92：+0x00 vector<int> +0x0c map<int,vector<int>>，sizeof 0x24
    std::vector<int> m_vec0;                     // +0x00
    std::map<int, std::vector<int> > m_mapC;     // +0x0c
};

class stNewAccountLevelUpToJobScript
{
public:
    stNewAccountLevelUpToJobScript();
    ~stNewAccountLevelUpToJobScript();

    // ORIG D1 0x837df44：+0x00 stStepRewardStackableItemScript（0x18）
    // +0x18/+0x30 stLevelUpRewardItemScript（0x18），sizeof 0x48
    stStepRewardStackableItemScript m_stepReward;  // +0x00
    stLevelUpRewardItemScript m_levelUp18;         // +0x18
    stLevelUpRewardItemScript m_levelUp30;         // +0x30
};

class eventReward
{
public:
    eventReward();
    // ORIG _ZN11eventReward17importRewardTableEPKcS1_（init 调用，0=成功；
    // 列表加载失败 0x77 / ImportRewardScript 失败 0x71）
    int importRewardTable(const char* dir, const char* file);
    void StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end, int cap) const;
    void applyItemOption(Inven_Item& inven, const CItem* item, const itemData& data);
    int unsealRandomOption(const CItem* item, Inven_Item& inven);
    bool rewardWriteUserInven(Inven_Item* item, rewardKey key, int count);
    bool rewardWriteUserEquip(Inven_Item* item, rewardKey key, int count);
    int sendPostalReward(CUser& user, rewardKey key, int count,
                         const char* title, const char* body);
    bool createCharacInvenReward(Inven_Item* item, rewardKey key, int count);
    bool createCharacEquipReward(Inven_Item* item, rewardKey key, int count);
    // ---- ImportRewardScript 依赖的解析 helper（ORIG T；定义待 eventReward 域批次，
    //      ScriptLeafA2.cpp 先行声明引用；asm 均传 this，为非静态成员）----
    char getSubKeyType(std::string& token);              // T 0x080ef0c0
    char getSubKeyAction(std::string& token);            // T 0x080ef128
    bool getOptionType(std::string& token, itemData& out);   // T 0x080ef190
    void defaultOption(char action, itemData& out);      // W 0x080ef25c
    // ORIG _ZN11eventReward18ImportRewardScriptEPKcS1_ 0x080ef3a2 T
    // （单脚本解析；声明先行，定义归 eventReward 生产批次）
    int ImportRewardScript(const char* dir, const char* file);
    ~eventReward();

    // ORIG D1 0x837dfb2：+0x00 map<rewardKey,map<int,vector<itemData>>>
    // +0x18 内嵌 CSlotBoundChecker（其唯一成员即 map<SlotBoundKey,SlotBound>，
    // sizeof 0x18；权威 CSlotBoundChecker 于 CInventory.h 为静态数组重建且无
    // 成员，无法内嵌，故此处直接以同布局 map 成员保持 ABI 与生命周期形态）。
    std::map<rewardKey, std::map<int, std::vector<itemData> > > m_map0;  // +0x00
    std::map<SlotBoundKey, SlotBound> m_slotBoundMap;                    // +0x18
};

class DeleteInvalidItemScript
{
public:
    DeleteInvalidItemScript();
    ~DeleteInvalidItemScript();
    void clearScript();           // ORIG T 0x08ac2066
    bool IsInvalidItem2Delete(int idx);
    // ORIG：GetRewardItemInfo 0x8ac20f8 T（_ZN 非 const）/ GetMailInfo 0x8ac2180 T（_ZNK）
    bool GetRewardItemInfo(int idx, int& rewardIdx, int& rewardCnt);
    void GetMailInfo(std::string& title, std::string& body) const;  // ORIG T 0x8ac2180
    bool loadScript();   // ORIG _ZN23DeleteInvalidItemScript10loadScriptEv（init 调用）
private:
    // 布局依据 ORIG C1/D1（0x8ac1d40/0x8ac1dce）：string +0x0 / string +0x4 /
    // map<int, DeleteInvalidItemData> +0x8；IsInvalidItem2Delete（0x8ac2096）
    // 在 +0x8 map 上 find(idx)。
    // 2026-08-31 修正：原实现含 char m_pad14[0x20-0x14]（假设 map 只占 0xc），
    // 实测 std::map<int,T>（c6root gcc 4.4.7 -m32）= 0x18——0x8+0x18=0x20
    // 已对齐 ORIG 尾（CDataManager +0xb658 起 0x20 字节），pad 属多余 0xc，
    // 导致 CDataManager sizeof 0xb684 ≠ ORIG 0xb678（影子测试 malloc 差）。
    struct DeleteInvalidItemData
    {
        DeleteInvalidItemData();  // ORIG _ZN23DeleteInvalidItemScript21DeleteInvalidItemDataC1Ev W 0x08ac220c
        void init();              // ORIG _ZN23DeleteInvalidItemScript21DeleteInvalidItemData4initEv W 0x08ac2220

        int m_field0;   // +0x00
        int m_field4;   // +0x04
        int m_field8;   // +0x08
    };
    std::string m_field0;                                  // +0x00
    std::string m_field4;                                  // +0x04
    std::map<int, DeleteInvalidItemData> m_deleteMap;      // +0x08
};
typedef char assert_sizeof_deleteinvaliditem[sizeof(DeleteInvalidItemScript) == 0x20 ? 1 : -1];

// ===================== 外部依赖类（其它 TU 提供） =====================

class CSkillList
{
public:
    CSkillList();
    ~CSkillList();
    void destroy();
    int init();
    void insert_skill(int jobType, CSkill* skill);
    CSkill* find_skill(int skillId, int level);
    int get_ran_groupskill_index(int jobType, int group, int skillIdx);
    void set_post_learning_skill(int jobType, CSkill* skill);
    bool get_post_learning_skill(int jobType, int skillIdx,
                                 std::vector<std::pair<int, int> >& out);
    __gnu_cxx::hash_map<int, CSkill*>* getSkillList(int jobType);

    __gnu_cxx::hash_map<int, CSkill*> m_skillList[11];  // +0x00
    std::multimap<int, CSkill*> m_groupSkillMap[11];    // +0xdc
    std::map<int, std::vector<std::pair<int, int> > >
        m_postLearningMap[11];                          // +0x1e4
};
struct CSkillListLayoutMirror
{
    char m_skillList[11][sizeof(__gnu_cxx::hash_map<int, CSkill*>)];
    char m_groupSkillMap[11][sizeof(std::multimap<int, CSkill*>)];
    char m_postLearningMap
        [11][sizeof(std::map<int, std::vector<std::pair<int, int> > >)];
};
static_assert(sizeof(CSkillList) == 0x2ec, "CSkillList size");
static_assert(sizeof(CSkillList) == sizeof(CSkillListLayoutMirror),
              "CSkillList layout mirror size");
static_assert(offsetof(CSkillListLayoutMirror, m_skillList) == 0x00,
              "CSkillList::m_skillList offset");
static_assert(offsetof(CSkillListLayoutMirror, m_groupSkillMap) == 0xdc,
              "CSkillList::m_groupSkillMap offset");
static_assert(offsetof(CSkillListLayoutMirror, m_postLearningMap) == 0x1e4,
              "CSkillList::m_postLearningMap offset");

class CCharacList;
class QuestList
{
public:
    QuestList();
    ~QuestList();
    void destroy();
    int load_list();
    void insert_NPC(int npcIdx);
    int check_ahead_quest(int questIdx, const WongWork::CQuestClear& clear);
    int check_anti_quest(int questIdx, const WongWork::CQuestClear& clear);
    int check_clear(int questIdx, const WongWork::CQuestClear& clear);
    bool select_mail_quest(std::list<int>& out,
                           const WongWork::CQuestClear& clear,
                           const stSelectQuestParam& param);
    int Check_Quest(Quest* quest, stSelectQuestParam& param,
                    const WongWork::CQuestClear& clear);
    void allowable_questlist_as_npc(
        std::list<int>& out, const std::multimap<int, int>& npcList,
        stSelectQuestParam& param, const WongWork::CQuestClear& clear,
        bool flag);
    void select_quest(std::list<int>& out, stSelectQuestParam& param,
                      const WongWork::CQuestClear& clear);
    void GetDailyQuestList(std::list<int>& out);
    void GetTrainingQuestList(std::vector<int>& out, short level);
    Quest* find_quest(int idx);
    std::multimap<int, int>* getQuestNPCList(ENUM_QUEST_GRADE grade);
    QuestList* getQuestList() { return this; }   // ORIG W 0x08374c7a（mov this,%eax; ret）

    __gnu_cxx::hash_map<int, Quest*> m_questMap;   // +0x00
    // ORIG ctor/dtor 展开 8 次独立构造/析构（83538f0/8353aa0）→ 独立成员而非数组。
    std::multimap<int, int> m_npcQuestList0;       // +0x14
    std::multimap<int, int> m_npcQuestList1;       // +0x2c
    std::multimap<int, int> m_npcQuestList2;       // +0x44
    std::multimap<int, int> m_npcQuestList3;       // +0x5c
    std::multimap<int, int> m_npcQuestList4;       // +0x74
    std::multimap<int, int> m_npcQuestList5;       // +0x8c
    std::multimap<int, int> m_npcQuestList6;       // +0xa4
    std::multimap<int, int> m_npcQuestList7;       // +0xbc
    std::vector<int> m_vecD4;                      // +0xd4
    std::list<int> m_listE0;                       // +0xe0
};
struct QuestListLayoutMirror
{
    char m_questMap[sizeof(__gnu_cxx::hash_map<int, Quest*>)];
    char m_npcQuestList[8][sizeof(std::multimap<int, int>)];
    char m_vecD4[sizeof(std::vector<int>)];
    char m_listE0[sizeof(std::list<int>)];
};
static_assert(sizeof(QuestList) == 0xe8, "QuestList size");
static_assert(sizeof(QuestList) == sizeof(QuestListLayoutMirror),
              "QuestList layout mirror size");
static_assert(offsetof(QuestListLayoutMirror, m_questMap) == 0x00,
              "QuestList::m_questMap offset");
static_assert(offsetof(QuestListLayoutMirror, m_npcQuestList) == 0x14,
              "QuestList::m_npcQuestList offset");
static_assert(offsetof(QuestListLayoutMirror, m_vecD4) == 0xd4,
              "QuestList::m_vecD4 offset");
static_assert(offsetof(QuestListLayoutMirror, m_listE0) == 0xe0,
              "QuestList::m_listE0 offset");

class DisJoint;
class PacketGuard;  // 前向：CItemUpgrade::make3rdChroniclePacket 参数
class MSG_ENCHANT;  // 前向：CItemUpgrade::ProcUpgrade(MSG_ENCHANT*)（权威定义见 PacketTypes.h）
class upgrade_table_t;   // 权威声明见 CItemAmplifier.h（CItemUpgrade helper 参数）
struct upgrade_info_t;   // 权威声明见 CItemAmplifier.h（CItemUpgrade helper 参数）
#include "CItemUpgrade_Separate.h"  // WongWork::IItemUpgrade（虚基接口：槽0 ProcUpgrade、
                                    // 槽1 LoadUpgradeScript；含守卫，幂等）
namespace WongWork
{
// ORIG vtable _ZTVN8WongWork12CItemUpgradeE @0x08c9d310（仅 2 函数槽，无虚析构：
//   +0x00=ProcUpgrade(MSG_ITEM_UPGRADE) 0x0854779c、+0x04=LoadUpgradeScript
//   0x0854919a；ctor/dtor 写 &vtable+8 于 this+0 实证）。typeinfo 为 __vmi 类
//   （双基：IItemUpgrade 非虚主基 @+0x00、CItemUpgradeTable 表区 @+0x04..+0x4e7）。
class CItemUpgrade : public IItemUpgrade, public CItemUpgradeTable
{
public:
    CItemUpgrade();   // ORIG T 0x085461c8（定义 CtorProducers_2C.cpp）
    ~CItemUpgrade();  // 非虚（ORIG 无 D0 符号）
    // 虚槽覆盖：槽序经 ORIG vtable 实证（ProcUpgrade 在前）。
    // ORIG _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP11MSG_ENCHANTP11PacketGuard
    //   T 0x0854a1ba：三代强化（ENCHANT 消息）主流程（定义 CItemUpgrade.cpp）。
    int ProcUpgrade(CUser* user, MSG_ENCHANT* msg, PacketGuard* packet);
    // ORIG _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP16MSG_ITEM_UPGRADEP11PacketGuard
    //   T 0x0854779c vtable+0x00 槽纯虚覆盖（定义 ItemUpgradeVtableB2.cpp）。
    int ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg, PacketGuard* packet);
    // ORIG _ZN8WongWork12CItemUpgrade11isDoEnchantERK10Inven_ItemPc T 0x0854977a
    //   （定义 CItemUpgrade.cpp；ProcUpgrade(ENCHANT) 调用）。
    bool isDoEnchant(const Inven_Item& item, char* buf);
    // ORIG _ZN8WongWork12CItemUpgrade15_CheckConditionEPK5CUserRK10Inven_ItemPK5CItemS6_h
    //   T 0x085497ea（ENCHANT 版 5 参；定义 CItemUpgrade.cpp）。
    int _CheckCondition(const CUser* user, const Inven_Item& item,
                        const CItem* itemScript, const Inven_Item& ticket,
                        unsigned char param);
    // ORIG _ZN8WongWork12CItemUpgrade12_ProcPenaltyEP5CUserP10Inven_ItemPK5CItemtP11PacketGuard
    //   T 0x08549ac8（ENCHANT 版 5 参；定义 CItemUpgrade.cpp）。
    int _ProcPenalty(CUser* user, Inven_Item* item, const CItem* itemScript,
                     unsigned short slot, PacketGuard* packet);
    // ORIG _ZN8WongWork12CItemUpgrade24ProcUpgrade_3rdChronicleEP5CUser18ENUM_EQUIPMENTTYPEhR10Inven_ItemPK5CItem
    //   T 0x0854a006（定义 CItemUpgrade.cpp）。
    int ProcUpgrade_3rdChronicle(CUser* user, ENUM_EQUIPMENTTYPE equipType,
                                 unsigned char param, Inven_Item& item,
                                 const CItem* itemScript);
    // LoadUpgradeScript 为 IItemUpgrade 纯虚槽1 覆盖；基类返回 int（ORIG eax
    // 透传 import 错误行号，0=成功），派生覆盖随基类返回 int。
    int LoadUpgradeScript(const char* path);                // ORIG T 0x0854919a（槽 +0x04）
    // ORIG _ZN8WongWork12CItemUpgrade29Load3rdChronicleUpgradeScriptEPKc（init 调用）
    int Load3rdChronicleUpgradeScript(const char* path);
    float getUpgradeItemRepairCostRate(int idx, bool flag) const;
    void make3rdChroniclePacket(CUser* user, const Inven_Item& item,
                                PacketGuard* packet);

    // ---- 强化随机增幅（amplify random upgrade）入口 ----
    // ORIG 0x854af38：检查随机增幅券并计算强化值，写入 int&。
    int enableAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                   Inven_Item& ticket, int& outCount);
    // ORIG 0x854ac8a：处理随机增幅结果（写升级值、统计、日志、广播）。
    int ProcAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                 Inven_Item& ticket, int update,
                                 const char* msg);

    // ---- 强化内部辅助（ORIG） ----
    // 0x854678a：按 upgrade_table_t 取 upgrade_info 到 out。
    // 返回 get_upgrade_info 结果（0 失败=越界，1 成功）。
    int _GetUpgradeInfo(const Inven_Item* item, const CItem* itemScript,
                        const upgrade_table_t& table, upgrade_info_t* out,
                        int level);
    // 0x8546342：校验强化条件，返回错误码（0 表示通过）。
    int _CheckCondition(const CUser* user, const Inven_Item& item,
                        const CItem* itemScript, const Inven_Item& ticket,
                        const upgrade_table_t& table,
                        const upgrade_info_t& info);
    // 0x8546824：广播强化通知包。
    void _NoticeUpgrade(const CUser* user, const Inven_Item& item,
                        bool isAmplify);

    // ---- ProcUpgrade 依赖的升级域内部方法（定义 ItemUpgradeVtableB2.cpp）----
    unsigned short _getUpgradeCountByUpgradeItem(const Inven_Item& target,
                                                 const Inven_Item& material,
                                                 ENUM_ERROR& err);  // T 0x08548e68
    unsigned char _getUpgradeRateByUpgradeItem(const Inven_Item& target,
                                               const Inven_Item& material,
                                               ENUM_ERROR& err);    // T 0x08548ffa
    bool _CheckTargetUsableLv(const Inven_Item& material,
                              const Inven_Item& target) const;      // T 0x08548c3c
    int _ProcPortableUpgradeItem(const Inven_Item& material,
                                 const Inven_Item& target,
                                 upgrade_info_t& info) const;       // T 0x08548d3c
    bool _DoProcUpgrade(CUser* user, Inven_Item* item,
                        const upgrade_info_t* info);                // T 0x0854755a
    int _ProcPenalty(CUser* user, Inven_Item* item, const CItem* itemScript,
                     unsigned short slot, const upgrade_table_t& table,
                     const upgrade_info_t* info, PacketGuard* packet,
                     Inven_Item* ticketCopy, char ticketFlag);      // T 0x08546bba
    void _getAdditionalDisjointItem(
        unsigned int up, const CItem* itemScript,
        std::vector<std::pair<int, int> >& items,
        const upgrade_table_t& table);                              // T 0x08546938
    // ---- G7 miscB 纯追加（定义 WongWorkMiscB_A.cpp）----
    // ORIG T 0x085491d6 _ZN8WongWork12CItemUpgrade20ProcUpgradeChronicleEP5CUsersjsjcRSt6vectorIiSaIiEE
    int ProcUpgradeChronicle(CUser* user, short slotA, unsigned int a3,
                             short slotB, unsigned int a5, char chanceGrade,
                             std::vector<int>& items);
    // ORIG T 0x08546a96 _getAdditionalDisjointItemEjPK5CItemRSt6vectorISt4pairIiiESaIS6_EERK15upgrade_table_tb（bool 重载版）
    void _getAdditionalDisjointItem(
        unsigned int up, const CItem* itemScript,
        std::vector<std::pair<int, int> >& items,
        const upgrade_table_t& table, bool flag);
    // ORIG T 0x0854a916 _ZN8WongWork12CItemUpgrade27find3rdChronicleEnchantDataE21STEnchantSystemMapKeyi
    // （STEnchantSystemMapKey 完整类型：STStackableScript.h，4 字节）。
    STEnchantSystemMapData* find3rdChronicleEnchantData(
        STEnchantSystemMapKey key, int type);
    CMTRand* m_rand;                         // +0x4e8（ORIG ctor new CMTRand）
    int m_randMax;                           // +0x4ec（ORIG ctor 写 100000 热身上界）
};
}
namespace WongWork { class CItemUpgrade_Separate; }  // ORIG _ZN8WongWork21CItemUpgrade_Separate…（权威声明 CItemUpgrade_Separate.h）

// ---- ChattingEmoticonBase（ORIG vtable 0x8b30748；虚函数槽由 ReCalc 调用点
//      +0x0c/+0x10/+0x14 与派生类 vtable 0x8b30680 验证）----
class ChattingEmoticonBase
{
public:
    ChattingEmoticonBase() : m_index(0) {}   // ORIG Base::C1 0x80e5c2a 含 m_index=0
    virtual ~ChattingEmoticonBase() {}
    virtual bool isUsable(const CUser& user) const = 0;        // vtable+0x08
    virtual int GetStartArrayIndex() const = 0;                // vtable+0x0c
    virtual int GetEndArrayIndex() const = 0;                  // vtable+0x10
    virtual bool isCommonEmoticon(const CUser& user) const;    // vtable+0x14
    virtual void AddPremiumIndex(int index);                   // vtable+0x18

    int GetIndex() const { return m_index; }
    void SetIndex(int index) { m_index = index; }

protected:
    int m_index;             // +0x04
    std::set<int> m_set;     // +0x08
};

class CChattingEmoticonConfig;  // CGameOption.h 完整定义
namespace EmoticonType { struct EmoticonScript; }  // 前向：Factory/Load_list 形参（完整定义见 CChattingEmoticonList.h）
class CChattingEmoticonList
{
public:
    CChattingEmoticonList();   // ORIG _ZN21CChattingEmoticonListC1Ev（init new；定义待专属批次）
    ~CChattingEmoticonList();
    ChattingEmoticonBase* find_emoticon(int idx);
    void ReCalc(CChattingEmoticonConfig& config, const CUser& user);
    ChattingEmoticonBase* Factory(const EmoticonType::EmoticonScript& script);  // ORIG 0x80e53a8
    bool Load_list();                                            // ORIG 0x80e54d2

private:
    std::map<int, ChattingEmoticonBase*> m_map;            // +0x00
    std::map<int, std::vector<ChattingEmoticonBase*> > m_mapVec;  // +0x18
    std::map<int, int> m_mapInt;                           // +0x30
};
class CNPCScript;  // 前向：m_npcMap 值类型
class CNPCScriptList
{
public:
    CNPCScriptList();
    ~CNPCScriptList();
    // ORIG _ZN14CNPCScriptList10initScriptEPcS0_S0_（init 调用，0=成功）
    int initScript(char* dir, char* list, char* common);
    // ORIG T 0x08581680：hash_map 命中返回 CNPCScript*，未命中返回 0。
    CNPCScript* find(unsigned int idx) const;
    int getMaxFavorValue() const;                       // W 0x8582364
    int getFavorLevel(unsigned short favorExp) const;   // T 0x85816e4
    unsigned int getFavorLevelValue(unsigned int level) const;  // T 0x858174e
    // ORIG _ZN14CNPCScriptList8_destroyEv 0x8581784 T（D1 0x83762a8 调用；
    // 定义见 CDataManagerScripts.cpp：delete 各 CNPCScript* 后 clear()）
    void _destroy();

    // ORIG D1 0x83762a8 / _destroy 0x8581784：+0x00
    // hash_map<uint,CNPCScript*> +0x14 stNPCCommonData_t（0x50），sizeof 0x64
    __gnu_cxx::hash_map<unsigned int, CNPCScript*> m_npcMap;  // +0x00（ORIG 0x8581680）
    stNPCCommonData_t m_npcCommon;                            // +0x14
};

// ============================================================================
// ItemShopScript（NPC 物品商店脚本，G4 商城/交易域批次，ORIG 布局 0x4c）
// 布局依据 ItemShopScript::C1 (0x85140c8) / clear (0x8514046) /
// importItemShopScript (0x89dc5b4) / CItemShop 各方法位移推导：
//   +0x00 int shopId             +0x04 int limitType
//   +0x08 int limitCount         +0x0c char field_c
//   +0x10 std::string shopName   +0x14 std::vector<int> curItemList
//   +0x20 std::vector<std::string>  +0x2c std::vector<int> limitNpcBuyList
//   +0x38 std::vector<std::vector<int> > dailyList
//   +0x44 char isOneADay         +0x48 int lastUpdateTime
// 注：本工具链 COW std::string 尺寸 4，vector 12，map 12。
// ============================================================================
struct ItemShopScript
{
    void clear()                                     // ORIG 0x8514046
    {
        m_shopId = 0;
        m_limitType = 0;
        m_limitCount = 0;
        m_field0c = 0;
        m_shopName = "";
        m_curItemList.clear();
        m_strList.clear();
        m_limitNpcBuyList.clear();
        m_dailyList.clear();
        m_isOneADay = 0;
    }
    ItemShopScript() { clear(); }                   // ORIG C1 0x85140c8
    int m_shopId;                                   // +0x00
    int m_limitType;                                // +0x04
    int m_limitCount;                               // +0x08
    char m_field0c;                                 // +0x0c
    // Compiler padding from +0x0d to the aligned string at +0x10.
    std::string m_shopName;                         // +0x10
    std::vector<int> m_curItemList;                 // +0x14
    std::vector<std::string> m_strList;             // +0x20
    std::vector<int> m_limitNpcBuyList;             // +0x2c
    std::vector<std::vector<int> > m_dailyList;     // +0x38
    char m_isOneADay;                               // +0x44
    // Compiler padding from +0x45 to the aligned time field at +0x48.
    int m_lastUpdateTime;                           // +0x48
};

// ============================================================================
// CItemShop（NPC 物品商店管理，G4 商城/交易域批次）
// 布局依据 CItemShop::C1 (0x8374e76) / GetOneADayItemList (0x8513076)：
//   +0x00 std::map<int, ItemShopScript> m_shopMap
//   +0x18 std::vector<std::pair<int,int> > m_oneADayList（总尺寸 0x24）
// ============================================================================
class CItemShop
{
public:
    CItemShop() {}                                  // ORIG C1 0x8374e76

    // ---- 装载 ----
    bool loadItemShopFiles();

    // ---- 查询 ----
    bool checkValidShopItem(unsigned int itemIdx, unsigned long count);
    bool checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count);
    char IsOneADayShop(int shopId) const;
    std::vector<std::pair<int, int> >& GetOneADayItemList() const;

    // ---- 每日商店 ----
    unsigned int setOneADayItemListNo(ItemShopScript& script);
    void updateOneADayItemList();             // ORIG 0x8512e98
    void testupdateOneADayItemList();         // ORIG 0x8512f88

    std::map<int, ItemShopScript> m_shopMap;                        // +0x00
    std::vector<std::pair<int, int> > m_oneADayList;                // +0x18
};
class CAICharacter;
// CAICharacterList（ORIG 布局依据 dtor 0x08373d9e / destroy 0x0834a59a /
// init 0x0834a28c：map@0x00、vector 数组@0x18(0x4a4 区间)、int@0x4bc(init 写
// 0x7fffffff)、map@0x4c0、vector[6]@0x4d8、map@0x520）。
class CAICharacterList
{
public:
    ~CAICharacterList();
    void destroy();   // ORIG 0x0834a59a：遍历 m_charMap delete 全部 CAICharacter
    // ORIG 均为成员函数（this=CAICharacterList，mangled 无指针参数）：
    CAICharacter* get(unsigned int idx);            // ORIG T 0x0834a9d2
    CAICharacter* getByLevel(unsigned int level);   // ORIG T 0x0834aa60
    CAICharacter* getByAP(unsigned int ap);          // ORIG T 0x0834aaf4
    int getMinAP();                                  // ORIG W 0x08150872：返回 m_maxAP(+0x4bc)
    std::map<int, int> const& getAPCount() const;    // ORIG W 0x08150880：返回 m_apStatMap(+0x520) 引用
    bool init();   // ORIG _ZN16CAICharacterList4initEv（init 调用）
    bool _add(CAICharacter* ai);   // ORIG T 0x0834a61c（init 每条目调用；报告 _add.md）

    std::map<int, CAICharacter*> m_charMap;               // +0x00
    std::vector<CAICharacter*> m_byLevelList[99];         // +0x18（99×0xc=0x4a4，止于 +0x4bc；ORIG dtor 析构区间 +0x4a4）
    int m_maxAP;                                          // +0x4bc（init 置 0x7fffffff）
    std::map<int, std::vector<CAICharacter*> > m_groupMap; // +0x4c0
    std::vector<CAICharacter*> m_byAPList[6];             // +0x4d8（6×0xc=0x48，止于 +0x520）
    std::map<int, int> m_apStatMap;                       // +0x520
};
class SlangFilter;
namespace WongWork
{
class CItemGeneratorMgr;  // 权威声明见 CItemGeneratorMgr.h（ORIG 位于 namespace WongWork）
}
struct SIG_AUTO_MARKET_CONDITIONS_CONTROL;
class CAutoMarketConditionsControl
{
public:
    CAutoMarketConditionsControl();              // ORIG T 0x082f8108（CDataManager.cpp）
    ~CAutoMarketConditionsControl();             // ORIG T 0x082f8144
    // ORIG T 0x082f8684 _ZN28CAutoMarketConditionsControl12LoadDatabaseEP34SIG_AUTO_MARKET_CONDITIONS_CONTROL
    int LoadDatabase(SIG_AUTO_MARKET_CONDITIONS_CONTROL* msg);
    // ORIG T 0x082f815c：脚本加载（CDataManager.cpp 已实现）。
    int LoadScript(const char* path);
    // ORIG T 0x082f8540（int）/ T 0x082f85a8（无参）：耐久度比率包广播。
    void SendNotiSetDurability(int value);
    void SendNotiSetDurability(CUser* user);
    void SendNotiSetDurability();
    // ORIG T 0x082f89bc _ZN28CAutoMarketConditionsControl21GetGoldDropAmountRateEii
    int GetGoldDropAmountRate(int level, int gold);
    // ORIG T 0x082f8ae8 / T 0x082f8c8c（LinkClosureB_Misc.cpp 实现；bizB 清单批）
    float GetItemDropRate(int levelRange);
    bool ChangeItemRarity(int* itemRarity, int* outItemRarity, int raritySize,
                          double controlRarity);

    int m_field00;              // +0x00（ctor 置 0；非 0 启用折算）
    double m_goldRate[10];      // +0x04 [control of gold drop amount rate]（fstpl 0x4(%eax,%ebx,8)）
    double m_goldBaseRate;      // +0x54 GetGoldDropAmountRate fldl 0x54(%eax)
    int m_goldPerLevel[201];    // +0x5c..+0x380 [.. per level]（按等级直接下标；memset 0x324）
    int m_field380;             // +0x380（ctor 置 0）
    double m_equipProb[10];     // +0x384 [control of equipment drop prob]（(i+0x70)*8+4）
    double m_equipProbBase;     // +0x3d4 GetItemDropRate fldl 0x3d4(%eax)
    std::vector<int> m_vec3dc;  // +0x3dc [control of equipment drop prob per level range]
    int m_field3e8;             // +0x3e8（ctor 置 0；LoadDatabase 与 DB +0x38 比对）
    double m_durRate[10];       // +0x3ec [control of durability decrease rate]（(i+0x7c)*8+0xc）
    double m_durCurrent;        // +0x43c SendNotiSetDurability() fldl 0x43c(%eax)
};
class CNPCDynamicInfoManager;
class CItemLimitEditionMgr;
class StrikerScript;
class CharacManageScript;
class BlueMarbleScriptManager;
class CExpertJobList;
class CEventScriptMng;
class CLevelRewardTableMgr;
// ===================== CDataManager =====================

// ---- ENUM_BLOOD_TYPE（ORIG 命名枚举，mangled 15ENUM_BLOOD_TYPE） ----
// 取值以调用点常量（set/get_limit_inout_count 的 1/2）为准【推断】。
#ifndef DNF_ENUM_ENUM_BLOOD_TYPE_DEFINED
#define DNF_ENUM_ENUM_BLOOD_TYPE_DEFINED
enum ENUM_BLOOD_TYPE { ENUM_BLOOD_TYPE_0 = 0, ENUM_BLOOD_TYPE_1 = 1, ENUM_BLOOD_TYPE_2 = 2 };
#endif

class CDataManager
{
public:
    CDataManager();
    ~CDataManager();
    // ORIG 0x0812268c W：bGM=false→m_actionPoint1（Chn_ActionPointSystem.etc）
    // true→m_actionPoint2（_GM.etc）。实现见 SyncScriptDeps.cpp。
    APSystem::CActionPointEtcParameter* GetActionPointEtcParameter(bool bGM);
    // ---- 查询（find_*）----
    CItem* find_item(int idx) const;
    CItem* find_item(const char* name) const;
    CItem* FindGoods(int goodsIdx) const;
    // ORIG W 0x0830e6d2 _ZNK12CDataManager11getDropRateEv：读 m_dropRate（+0xb3f0）。
    float getDropRate() const;
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost);
    CSkill* find_skill(int skillId, int level) const;
    Quest* find_quest(int idx) const;
    CDungeon* find_dungeon(int idx) const;
    void* get_dungeon(int idx);       // ORIG W 0x8111036（Village::set_village 引用）
    void* find_map(int idx) const;
    void* find_monster(int idx) const;
    void* find_mission(int idx) const;
    int find_npc(unsigned int idx) const;
    bool find_lottery_use_cost(unsigned int money) const;
    int* find_hellparty_group(int idx);

    // ---- 等级/经验 ----
    int set_level_exp();
    int get_level_exp(int level) const;
    int get_level_section_exp(int level) const;
    int get_level_exp_percent(int level, int percent) const;
    int get_exp_level(int exp) const;
    int get_creature_level_exp(int level) const;
    int get_creature_exp_level(int exp) const;
    unsigned int GetSpAtLevelUp(int level) const;
    unsigned int GetMoneyLimitPerLevel(int level, const char* serverGroup) const;
    void get_mob_reward(int level, unsigned int& out) const;
    int set_mob_reward();
    float BaseExpPenalty(int levelA, int levelB);
    void SetExpRate(float rate);
    void SetItemDropRate(float rate);

    // ---- PvP/排行 ----
    int set_pvp_grade();
    int set_reward_sp();
    unsigned short getRankIndex(int idx);
    unsigned int getNewRankIndex(int idx) const;
    int getRankBonusIndex(int idx) const;
    int calRankPoint(int a, int b);
    unsigned int getDungeonClearAverageTime(int a, int b, ENUM_DUNGEON_DIFFICULTY difficulty);
    int getClearTimeBonusPoint(int a, int b);
    int getPvPChannelGrade(int channel);

    // ---- 任务 ----
    std::vector<int>* getDailyTrainingQuest(int idx);
    int isThereDailyTrainingQuestList(int idx, int questIdx);
    bool reselectDailyTrainingQuest();
    int get_WithinMissionIndex(int idx) const;
    std::list<CMission*>* get_kind_mission_list(int idx) const;
    void get_New_DailyMission(std::vector<std::pair<unsigned int, unsigned int> >& out);
    int get_BaseMissionExp_byRank(unsigned short rank) const;
    std::bitset<32> get_daily_mission_kind() const;

    // ---- 事件/道具 ----
    int getDecoEventItemIdx();
    bool getVaildEventItem(int idx);
    int getMatchEventPoint(int idx);
    int getMaxEventItemCount();
    int getFatigue_contidion();
    int getRewardItem_MailExpireDate();
    int get_GiftGrowthWeaponBox();
    int get_GrowthWeaponMaterialItem();
    unsigned short get_GrowthWeaponMaterialMax();
    unsigned short get_GrowthWeaponEventBubbleMax();
    unsigned short get_GrowthWeaponEventSandMax();
    int get_GrowthWeaponEventReward(int idx) const;
    int getChangeAbleCreatureLev();
    int getClientChangeCreatureIndex(int idx);
    int getInfinityChangeCreatureIndex(int idx);
    int getGiveGrowCreatureEventJobCount(int level);
    bool isAvailableGiveGrowCreatureEventJob(int job, int level);
    int getSeriaRoomAniDecoItem(int idx);
    int getSeriaRoomAniDecoRewardItem(int idx);
    unsigned int maxAniDecoItem();
    bool invalidAniDecoItem(int idx, int value);
    int get_useJobFatigueMatchGift(char job, int a, int b);
    void get_useJobFatigueMatchGiftMsg(int job, std::string& msg1, std::string& msg2);
    void get_createCharacterGift(int idx, createChracScript::rewardInfoList& out) const;
    void* get_hellparty_script_values();
    std::vector<stVendingMachine_t>* getVendingMachineTable() const;
    void* get_hellparty_groupOrder_map(int idx);

    // ---- 情感/过滤 ----
    void ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user);
    void rewriteUsableEmoticon(const CUser& user, char* text);
    std::vector<int> IsUsableEmoticons(const CUser& user,
                                       const std::vector<int>& emoticons);
    bool hasPreventString(const char* name);
    bool hasPreventStringName(const char* name);
    int hasPreventPrefixCharacName(const char* name, int unused);
    bool hasSpecialCharacter(const char* name);
    bool hasSpecialCharacterExceptSpace(const char* name);
    char* _subPunctuationMark(const char* src, char* dst);
    bool _isOneByteCharacter(unsigned char c);
    bool getCharacNameFilterPass() const;
    void setCharacNameFilterPass(bool flag);

    // ---- 其它 ----
    bool InitLoginState(ENUM_SERVER_GROUP serverGroup, int channelNo);  // ORIG _ZN12CDataManager14InitLoginStateE17ENUM_SERVER_GROUPi（0x8360306，返回 bool）
    static int loadAdvanceAltarTimeLineParameter(advancealtar::StageTimeLineParameter& param);  // ORIG 静态：调用点仅压 ref 实参（0x8359eee add $0x62d0 后单 push）
    int init();
    void destroy();
    int initRandomOptionItem();
    int initFairPVPSkillPoint();
    int load_map_files(std::vector<void*>& maps);
    int Verify() const;
    int VerifyMap(const CMap& map, int idx) const;
    int VerifyDungeon(const CDungeon& dungeon) const;
    const char* GetScriptPacks() const;
    void SetScriptPacks(const char* path);
    const char* get_server_name(int idx) const;
    void* GetItemMakingSkill() const;
    void* GetImageCommnicationData();   // ORIG _ZN12CDataManager24GetImageCommnicationDataEv（非 const）
    int load_map_files(std::vector<PVPMAP_DATA>& pvpMaps);  // ORIG _ZN12CDataManager14load_map_filesERSt6vectorI11PVPMAP_DATASaIS1_EE
    // ORIG premium_helper_function::cal_premium_term 读取 CDataManager+0x877e（字节）作
    // premium 期限倍率（flag 时 term *= 该字节，再 *extra）。+0x877e 位于 m_boosterGage
    // (+0x8770) 内偏移 0x0e；本区域为 m_pad 未具名，按 ORIG 偏移读取。
    unsigned char getPremiumTermMultiplier() const;
    CItemLimitEditionMgr* getItemLimitEditionMgr();    // ORIG 0x08179fa6：返回 m_itemLimitEditionMgr (+0x18 of m_npcRelation)
    CItemList* getItemList() { return m_itemList; }   // ORIG 布局 +0x0c：返回 m_itemList（具名访问器，替代 *(CItemList**)((char*)G_CDataManager()+0xc) 裸偏移）
    WongWork::CItemGeneratorMgr* getItemGenerator();  // ORIG W 0x0814a6ea：返回 m_itemGeneratorMgr (+0x8cc0)；定义见 CDataManagerScripts.cpp
    std::set<int>* getUltimateDungeonList() const;
    CDungeonList* getDungeonIndexList();   // ORIG T 0x08365434 _ZN12CDataManager19getDungeonIndexListEv（透传 CDungeonList::getDungeonIndexList 返回值；非 const）
    void setChoiceItem(bool flag, unsigned int idx);
    bool checkChoiceItem();        // ORIG _ZN12CDataManager15checkChoiceItemEv（非 const）
    unsigned int getChoiceItem();  // ORIG _ZN12CDataManager13getChoiceItemEv（非 const）
    void set_lottery_use_cost(unsigned int money, unsigned int cost);
    void set_original_dimensionInout(int idx, char value);
    void reset_dimensionInout();
    void set_dimensionInout(int idx, char value);        // ORIG 080eed82 W（实现于 CDungeon.cpp）
    void set_limit_inout_count(char value, ENUM_BLOOD_TYPE type);  // ORIG 08374c82 W（实现于 CDungeon.cpp）
    void testActionType();
    void testBingo();
    void* GetExpertJobScript(int job);
    void* GetExpertJobEtcScript();   // ORIG CDataManager::GetExpertJobEtcScript（CUser 使用）
    void* GetChannelScript() const;
    stDeathTower_t* GetTowerScript(unsigned int dungeonIdx);   // ORIG W 0x081455d8：查 m_deathTowerMap，返回 it->second+4（m_rateA）
    void* getBlueMarbleScript();
    void* get_event_script_mng();
    char get_dimensionInout(int idx);   // ORIG 0x0822b612 返回 char：movzbl 单条读取（无 32 位扩展指令）
    int get_limit_inout_count(ENUM_BLOOD_TYPE type);
    void GetPvPChannelGrade();               // ORIG _ZN12CDataManager18GetPvPChannelGradeEv
    std::vector<stResetItemInfo::ResetItem>* GetResetItemInfo() const;  // ORIG W 0x8693f24
    std::vector<stRefillItemInfo::RefillItem>* GetRefillItemInfo() const;  // ORIG W 0x8693f32
    stLimitItemUsageInfoEx* GetLimitItemUsageInfo();   // ORIG W 0x8693f16：返回 m_limitItemUsage(+0x4cf8)
    bool is_AuctionRegFreeCeraItem(unsigned int itemIdx) const;  // ORIG W 0x84e92a4：m_choiceSet(+0xb4ac).find(itemIdx)!=end
    int GetMinLevelPvPChannel();   // ORIG W 0x0819ee2e（ARAD pvp_grade_check）
    int GetMaxLevelPvPChannel();   // ORIG W 0x0819ee3c（ARAD pvp_grade_check）
    bool IsAvailablePvPChannel(int grade);  // ORIG W 0x0819ede0（ARAD pvp_grade_check）
    int GetMaxGradePvPChannel();
    int GetPvPChannelType();   // ORIG W 0x85e68a8：返回 m_pvpChannel.m_channelType
    int GetPvPMode();          // ORIG W 0x844d4ae（fair_pvp/online_preliminary RequestMap 调用；producer 待 CDataManager 域批次，符号暂缺）
    int GetPvPTeamCount();     // ORIG T 0x08568ce0（online_preliminary GetPlayCountPerOneTeam/TeamInfoSecond::refresh 调用；producer 归 CDataManager 域批次，符号暂缺）
    int GetUpgradeRevisionPvPChannel();
    int IsDBLogItem(unsigned int itemIdx);
    float getUpgradeItemRepairCostRate(int idx, bool flag);   // ORIG _ZN12CDataManager28getUpgradeItemRepairCostRateEib（非 const；WongWork::CItemUpgrade 同名成员为 const 另一符号）
    bool checkValidShopItem(unsigned int itemIdx, unsigned long count);
    bool checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count);
    const char* GetCeraShopGoodsName();
    const char* SuddenShopFileName1();
    const char* SuddenShopFileName2();
    const char* SuddenShopFileName3();

    // ---- 纹章合成表（ORIG Dispatcher_CompoundEmblem 0x8220bb2 实证布局）----
    // 成本表：m_serverParameter.m_emblemCompound[grade] 于 CDataManager+0x828+grade*0x18，
    //         cost==-1 → throw ENUM_ERROR 7。等级范围 0..6。
    // 结果表：*(int*)(CDataManager+0x82c+(grade*6+jewelType)*4) → 结果道具 idx。
    int getEmblemCompoundCost(int grade) const;              // cost==-1 由调用方判定
    int getEmblemResultItem(int grade, int jewelType) const; // 返回结果道具 idx
    CAutoMarketConditionsControl* GetAutoMarketContitionsControl();  // ORIG W 0x084e92fc：返回 m_autoMarket(+0x8cc4)；定义 DropLayoutDeps.cpp
    // ---- 成员布局（ORIG 0x8355b30 构造顺序）----
    CDungeonList* m_dungeonList;        // +0x00（find_dungeon 0x835fa01 mov (%eax) 实证）
    CMapList* m_mapList;                // +0x04
    CMonsterManager* m_monsterManager;  // +0x08
    CItemList* m_itemList;              // +0x0c
    CSkillList* m_skillList;            // +0x10
    CCharacList* m_characList;          // +0x14
    QuestList* m_questList;             // +0x18
    DisJoint* m_disJoint;               // +0x1c
    WongWork::CItemUpgrade* m_itemUpgrade;  // +0x20
    WongWork::CItemUpgrade_Separate* m_itemUpgradeSeparate;  // +0x24（ORIG _ZN8WongWork 命名空间）
    CChattingEmoticonList* m_emoticonList;          // +0x28

    reseal_table_t m_resealTable;                 // +0x2c
    ServerParameterScript m_serverParameter;      // +0x68
    InGameAdvertisementScript m_inGameAd;         // +0x4a5c
    PcBangItemRentarData m_pcBangRentar;          // +0x4af4
    DimensionActivationData m_dimensionAct;       // +0x4b18
    seriaRoomDecoEventScript m_seriaRoomDeco;     // +0x4b70
    growthEquipmentScript m_growthWeapon;         // +0x4bac
    createChracScript m_createChrac;              // +0x4bd4
    useCharacterJobFatigue m_useJobFatigue;       // +0x4bec
    CharacterCreateCreatureGiftScript m_createCreatureGift;  // +0x4c04
    seriaRoomDecoAniEventScript m_seriaAniDeco;   // +0x4c50
    stUnlimitChallengeInfo m_unlimitChallenge;    // +0x4ca8
    CLevelRewardTableMgr* m_levelRewardTableMgr;  // +0x4cf4
    stLimitItemUsageInfoEx m_limitItemUsage;      // +0x4cf8
    std::vector<stDBLogItem> m_dbLogItems; // +0x4d1c
    CNPCRelationEventManager m_npcRelation;       // +0x4d28
    stEmotionTypeTagsInfo m_emotionTags;          // +0x4d44
    stEquipmentGradeSample m_equipGrade;          // +0x4d5c
    stConditionEventInfo m_conditionEvent;        // +0x4d98
    ConditionLevelChkDungeon m_conditionDungeon;  // +0x4de8
    FairPvPStatScript m_fairPvPStat;              // +0x4df8
    RandomOptionScript m_randomOption;            // +0x4e10
    RegenerationROI m_regeneration;               // +0x5040
    stSeriaBlessingScript m_seriaBlessing;        // +0x5094
    stLevelUpRewardItemScript m_levelUpReward;    // +0x50ac
    stGrowthCapsuleScript m_growthCapsule;        // +0x50c4
    char m_securityReward[0x30];                   // +0x50d8（SecurityReward 0x30 字节，ImportSecurityReward 写入）
    stRankSystemInfo m_rankSystem;                // +0x5108
    std::map<unsigned int, stDeathTower_t> m_deathTowerMap;  // +0x5180
    stDeathTower_t m_deathTower;                  // +0x5198
    stBloodDungeon_t m_bloodDungeon;              // +0x5e30
    stVillageAttacked_t m_villageAttacked;        // +0x61a0
    APSystem::CActionPointEtcParameter m_actionPoint1;   // +0x61ac
    APSystem::CActionPointEtcParameter m_actionPoint2;   // +0x6224
    stCharacLinkSystem_t m_characLink;            // +0x629c
    advancealtar::StageTimeLineParameter m_stageTimeLine; // +0x62d0
    advancealtar::AdvanceAltarShopParameter m_altarShop;  // +0x631c
    advancealtar::RewardParameter m_altarReward;          // +0x634c
    advancealtar::ConfigParameter m_altarConfig;          // +0x6364
    stQuestShop m_questShop;                      // +0x6398
    int m_moneyLimit[201];                        // +0x6410
    WorldDropInfo m_worldDrop1[201];              // +0x6734
    WorldDropInfo m_worldDrop2[201];              // +0x76e8
    CAICharacterList* m_aiCharacterList;          // +0x869c
    SlangNameData m_slangName;                    // +0x86a0
    InvalidCharData m_invalidChar;                // +0x86c4
    std::map<int, std::vector<std::vector<STStackablePackageData> > > m_stackableMap;  // +0x86f8
    STGrowthPowerData m_growthPower;              // +0x8710
    AvatarVariation::AvatarColorInfo m_avatarColor;  // +0x8740
    BoosterGageData m_boosterGage;                // +0x8770
    CWorldMapList m_worldMapList;                 // +0x8780
    StageMapList m_stageMapList;                  // +0x8798
    WarAreaCounter m_warAreaCounter;              // +0x87b4
    std::string m_scriptPackPath;                 // +0x8cb8
    CItemShop* m_itemShop;                        // +0x8cbc
    WongWork::CItemGeneratorMgr* m_itemGeneratorMgr;  // +0x8cc0
    CAutoMarketConditionsControl* m_autoMarket;   // +0x8cc4
    QuestParameterScript m_questParameter;        // +0x8cc8
    TrainingQuestScript m_trainingQuest;          // +0x8d34
    std::vector<int> m_dailyTrainingQuest[70];    // +0xa1c4
    GuildParameterScript m_guildParameter;        // +0xa50c
    PowerParameterScript m_powerParameter;        // +0xa628
    SecretShopScript m_secretShop;                // +0xa700
    stTitleBookInfo m_titleBook;                  // +0xa788
    AccountCargoScript m_accountCargo;            // +0xa7e0
    PvPSkillTreeParameterScript m_pvpSkillTree;   // +0xa800
    EventCharacterParameterScript m_eventCharacter;  // +0xa86c
    IndependentDropParameterScript m_independentDrop;  // +0xa884
    std::string m_packName1;                      // +0xa8c0
    std::string m_packName2;                      // +0xa8c4
    std::string m_packName3;                      // +0xa8c8
    std::string m_packName4;                      // +0xa8cc
    CNPCScriptList* m_npcScriptList;              // +0xa8d0
    CNPCDynamicInfoManager* m_npcDynamicInfo;     // +0xa8d4
    bool m_characNameFilterPass;                  // +0xa8d8
    char m_fieldA8d9[3];                          // +0xa8d9
    stMercenarySystemInfo m_mercenarySystem;      // +0xa8dc
    std::map<int, int> m_mapA928;                 // +0xa928
    std::map<int, int> m_mapA940;                 // +0xa940
    char m_fieldA958[0x11c];                      // +0xa958
    union
    {
        struct
        {
            char m_dimensionInout[6];            // +0xaa74（ORIG get_dimensionInout movzbl 读取实证）
            char m_originalDimensionInout[6];     // +0xaa7a
            char m_limitInoutCount[4];            // +0xaa80（set/get_limit_inout_count）
            int m_expTable[200];                  // +0xaa84
        };
        char m_rawAA74[0x328];
    };                                            // +0xaa74
    int m_mobReward[201];                         // +0xada4
    int m_mobRewardBackup[201];                   // +0xb0c8
    float m_expRate;                              // +0xb3ec
    float m_dropRate;                             // +0xb3f0
    channel_script_t m_channelScript;             // +0xb3f4
    std::vector<stVendingMachine_t> m_vendingMachine;  // +0xb424
    pvp_channel_info_t m_pvpChannel;              // +0xb430（sizeof 36，至 +0xb454）
    PvP_MissionSystem* m_pvpMissionSystem;        // +0xb454（ORIG find_mission 实证）
    stItemMakingSkill m_itemMakingSkill;          // +0xb458
    std::set<int> m_ultimateDungeonList;          // +0xb488
    ChoiceItemInfo m_choiceItem;                  // +0xb4a0
    std::set<unsigned int> m_choiceSet;           // +0xb4ac
    ImageCommunicationData m_imageCommData;       // +0xb4c4
    ARAD::SCRIPT::AradJumping_Script m_aradJumping;  // +0xb4c8
    CLottery_NeedMoney m_lotteryNeedMoney;        // +0xb4d4
    live_server_info_t m_liveServerInfo;          // +0xb4ec
    EventAvatarListScript m_eventAvatarList;      // +0xb508
    EventEtcScript m_eventEtc;                    // +0xb520
    std::string m_scriptPacks;                    // +0xb544
    stReturnUserRewardScript m_returnUserReward;  // +0xb548
    stBingoScript m_bingo;                        // +0xb568
    stBroadCastItemScript m_broadCastItem;        // +0xb5bc
    stNewAccountLevelUpToJobScript m_newAccountLevelUp;  // +0xb5e0
    eventReward m_eventReward;                    // +0xb628
    DeleteInvalidItemScript m_deleteInvalidItem;  // +0xb658
};

CDataManager* G_CDataManager();

#endif  // GAME_CDATAMANAGER_H_
