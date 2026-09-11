// ============================================================================
// df_game_r MapInfo 还原（G2-5 批次，尺寸 0xec）。
// 无独立报告，按 ORIG 反汇编逐函数转录：
//   C1/C2 08151394 / copy 08235d40 / D1/D2 081518bc / operator= 0830eea2
//   Clear 081514ae / Add_Mob(map_monster) 08151612 / Add_Item(map_item) 081517e0
//   SelectDonsterItemDropLimit 0830e800
// 布局以 CBattle_Field.h:176 权威布局为准（map/multimap 0x18、vector 0xc、
// killChecker 0x18、playInfo 0x10）。
// map_monster（0x34c）/map_item（0x54）为全局结构，本 TU 内最小复刻：
// 复制/析构等由 ORIG 对应 TU 提供（跨类调用保持同名外部符号）。
// ============================================================================

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <cstring>
#include "WarField.h"
#include "LogManager.h"
// 日志：使用真实 LogManager.h 的 5 参 logFormat(level,file,func,line,fmt,...)。
// 原 ORIG 4 参符号（_ZN10LogManager9logFormatEiPKciS1_z）为旧签名，桥接已随迁移删除。


// ============================================================================
// map_item / map_monster 生命周期（ORIG 081512cc / 081512dc / 081511c8 /
// 081515ae / 08151aea / 08151d3a；布局以 WarField.h 为权威）
// ============================================================================

// ORIG 081512cc：仅调 Inven_Item ctor（+0x10）并清 m_ownerId（+0x50）；
// +0x00~+0x0c（m_count/m_standardLevel/m_itemIndex/m_dropIndex/m_createTick）
// 保持未初始化。
map_item::map_item()
    : m_item(), m_ownerId(0) {}

// [b28d-solo-MapInfo] AE（ORIG 0x081511c8，orig=63/our=63）：修法=P239 反用——
// 初始化列表只留类成员（m_active/m_mobList 隐式默认构造），标量赋值全部移入
// ctor body 并按 ORIG 发射序书写（0x334,0x336 → 0x33a..0x342 → 0x344,0x345,
// 0x346,0x348,0x349,0x34a → 0x30 → 0x8 → 0x18(float=0.0f 走 edx 中转) → 0x34b）；
// 删除 ORIG 未初始化的字段（0x00..0x2c 多数、0x338/0x339）。p167 CLEAR 4/4。
map_monster::map_monster()
{
    m_hellPartyGroupIdx = 0;
    m_hellPartyGroupOrder = 0;
    m_bloodSmallRoundOrder = 0;
    m_bloodPhaseTime = 0;
    m_bloodRoundOrder = 0;
    m_bloodSpawnIdx = 0;
    m_bloodPhaseIdx = 0;
    m_bloodKillFlag = 0;
    m_bloodDifficulty = 0;
    m_bloodRoundOrder2 = 0;
    m_bloodSpawned = 0;
    m_isBoss = 0;
    m_bloodRound = 0;
    m_dropRate = 100;
    m_roleType = 0;
    m_field18 = 0.0f;
    m_bloodRoundFlag = 0;
}

// [b28d-solo-MapInfo] AE（ORIG 0x08151aea，162=162；.o 侧未解析重定位，body 逐条
// 同形；p167 3/3 仅 C1/C2-D1/D2 同址别名差 P93）：
// ① m_active(+0x38,0x2f0) 的 ORIG `rep movsl` 已由 WarField.h 的「map_monster
//    自然对齐 + 仅 m_field11 packed」方案复现（此前 pack(1) 使 4.4 恒走对齐剥离
//    慢路 +30 条）。
// ② 旧判「packed short m_field11 store 装载序 N76 不可翻」已被推翻：标量成员
//    `m_field11(o.m_field11)` 4.4 恒 RHS-first，但改用同 union 的 2 字节 POD
//    字节视图**整体赋值** `m_field11b(o.m_field11b)`（P308 族：POD 子对象整体
//    拷贝 ⇒ LHS-first 双基址装载）后，4.4.7 逐条复现 ORIG
//    `mov this,%eax; mov other,%edx; movzwl 0x11(%edx),%edx; mov %dx,0x11(%eax)`
//    （探针 /tmp/mapinfo_probe/p3.cpp C2ERKS_）。union 成员/布局/偏移/sizeof 零变化。
// ③ P171 照抄：ORIG 不拷 m_field15/16/17 与 m_pad339，本实现同步不拷。
map_monster::map_monster(const map_monster& other)
    : m_mobId(other.m_mobId), m_instId(other.m_instId),
      m_roleType(other.m_roleType), m_mobIndex(other.m_mobIndex),
      m_level(other.m_level), m_field11b(other.m_field11b),
      m_field13(other.m_field13), m_field14(other.m_field14),
      m_field18(other.m_field18), m_field1c(other.m_field1c),
      m_field20(other.m_field20),
      m_field24(other.m_field24), m_field28(other.m_field28),
      m_dropCnt(other.m_dropCnt), m_dropRate(other.m_dropRate),
      m_flag34(other.m_flag34), m_active(other.m_active),
      m_mobList(other.m_mobList),
      m_hellPartyGroupIdx(other.m_hellPartyGroupIdx),
      m_hellPartyGroupOrder(other.m_hellPartyGroupOrder),
      m_hellPartyScriptIdx(other.m_hellPartyScriptIdx),
      m_bloodSmallRoundOrder(other.m_bloodSmallRoundOrder),
      m_bloodPhaseTime(other.m_bloodPhaseTime),
      m_bloodRoundOrder(other.m_bloodRoundOrder),
      m_bloodSpawnIdx(other.m_bloodSpawnIdx),
      m_bloodPhaseIdx(other.m_bloodPhaseIdx),
      m_bloodKillFlag(other.m_bloodKillFlag),
      m_bloodDifficulty(other.m_bloodDifficulty),
      m_bloodRoundOrder2(other.m_bloodRoundOrder2),
      m_bloodSpawned(other.m_bloodSpawned), m_isBoss(other.m_isBoss),
      m_bloodRound(other.m_bloodRound), m_bloodRoundFlag(other.m_bloodRoundFlag) {}

map_monster::~map_monster() {}

// [b28d-solo-MapInfo] AE（ORIG 0x08151d3a，153=153，p167 CLEAR 1/1）：删除
// `if (this != &other)` 守卫（ORIG 无守卫、无条件逐字段赋值）+ P171 删
// m_field15/16/17、m_pad339；m_field11 改字节视图整体赋值（同 C1ERKS_ 修法，
// 2 字节 POD 拷贝 ⇒ ORIG LHS-first）。
map_monster& map_monster::operator=(const map_monster& other)
{
    m_mobId = other.m_mobId;
    m_instId = other.m_instId;
    m_roleType = other.m_roleType;
    m_mobIndex = other.m_mobIndex;
    m_level = other.m_level;
    m_field11b = other.m_field11b;
    m_field13 = other.m_field13;
    m_field14 = other.m_field14;
    m_field18 = other.m_field18;
    m_field1c = other.m_field1c;
    m_field20 = other.m_field20;
    m_field24 = other.m_field24;
    m_field28 = other.m_field28;
    m_dropCnt = other.m_dropCnt;
    m_dropRate = other.m_dropRate;
    m_flag34 = other.m_flag34;
    m_active = other.m_active;
    m_mobList = other.m_mobList;
    m_hellPartyGroupIdx = other.m_hellPartyGroupIdx;
    m_hellPartyGroupOrder = other.m_hellPartyGroupOrder;
    m_hellPartyScriptIdx = other.m_hellPartyScriptIdx;
    m_bloodSmallRoundOrder = other.m_bloodSmallRoundOrder;
    m_bloodPhaseTime = other.m_bloodPhaseTime;
    m_bloodRoundOrder = other.m_bloodRoundOrder;
    m_bloodSpawnIdx = other.m_bloodSpawnIdx;
    m_bloodPhaseIdx = other.m_bloodPhaseIdx;
    m_bloodKillFlag = other.m_bloodKillFlag;
    m_bloodDifficulty = other.m_bloodDifficulty;
    m_bloodRoundOrder2 = other.m_bloodRoundOrder2;
    m_bloodSpawned = other.m_bloodSpawned;
    m_isBoss = other.m_isBoss;
    m_bloodRound = other.m_bloodRound;
    m_bloodRoundFlag = other.m_bloodRoundFlag;
    return *this;
}

MapInfo::MapInfo()
{
    Clear();
}

// [b28d-solo-MapInfo] AE（ORIG 0x08235d40，188=188，p167 CLEAR 12/12）：
// 修法=P239——把标量赋值从 ctor body 全部移入初始化列表并按声明序排列，
// 4.4 才与 4.1 同为「标量↔类成员按声明序混合发射」（此前 4.4 把 6 个类成员
// meminit 集中提前，标量全落 body ⇒ 发射序整段错位）。
MapInfo::MapInfo(const MapInfo& other)
    : m_key(other.m_key),
      m_visited(other.m_visited),
      m_visitedGoto(other.m_visitedGoto),
      m_mapIndex(other.m_mapIndex),
      m_gridValue(other.m_gridValue),
      m_monsterMap(other.m_monsterMap),
      m_itemMap(other.m_itemMap),
      m_assignItemIdx(other.m_assignItemIdx),
      m_dropItemCnt(other.m_dropItemCnt),
      m_mapType(other.m_mapType),
      m_dummyBossMap(other.m_dummyBossMap),
      m_mapBasisLevel(other.m_mapBasisLevel),
      m_killChecker1(other.m_killChecker1),
      m_killChecker2(other.m_killChecker2),
      m_playInfo(other.m_playInfo),
      m_hellPartyGroup(other.m_hellPartyGroup),
      m_assignNPCByPotion(other.m_assignNPCByPotion),
      m_mapA8(other.m_mapA8),
      m_vecC0(other.m_vecC0),
      m_cc(other.m_cc),
      m_tournamentMob(other.m_tournamentMob),
      m_layeredMap(other.m_layeredMap)
{
}

MapInfo::~MapInfo()
{
}

MapInfo& MapInfo::operator=(const MapInfo& other)
{
    m_key = other.m_key;
    m_visited = other.m_visited;
    m_visitedGoto = other.m_visitedGoto;
    m_mapIndex = other.m_mapIndex;
    m_gridValue = other.m_gridValue;
    m_monsterMap = other.m_monsterMap;
    m_itemMap = other.m_itemMap;
    m_assignItemIdx = other.m_assignItemIdx;
    m_dropItemCnt = other.m_dropItemCnt;
    m_mapType = other.m_mapType;
    m_dummyBossMap = other.m_dummyBossMap;
    m_mapBasisLevel = other.m_mapBasisLevel;
    m_killChecker1 = other.m_killChecker1;
    m_killChecker2 = other.m_killChecker2;
    m_playInfo = other.m_playInfo;
    m_hellPartyGroup = other.m_hellPartyGroup;
    m_assignNPCByPotion = other.m_assignNPCByPotion;
    m_mapA8 = other.m_mapA8;
    m_vecC0 = other.m_vecC0;
    m_cc = other.m_cc;
    m_tournamentMob = other.m_tournamentMob;
    m_layeredMap = other.m_layeredMap;
    return *this;
}

void MapInfo::Clear()
{
    m_key = 0;
    m_visited = 0;
    m_visitedGoto = 0;
    m_mapIndex = 0;
    m_gridValue = 0;
    m_monsterMap.clear();
    m_itemMap.clear();
    m_assignItemIdx = 0;
    m_dropItemCnt = 0;
    m_mapType = 1;
    m_dummyBossMap = 0;
    m_mapBasisLevel = 0;
    m_killChecker1.init();
    m_killChecker2.init();
    m_playInfo.init();
    m_hellPartyGroup.clear();
    m_assignNPCByPotion = 0;
    m_mapA8.clear();
    m_vecC0.clear();
    m_cc = 0;
    m_tournamentMob.clear();
    m_layeredMap = 0;
}

// [b28d-solo-MapInfo] AE（ORIG 0x08151612，120/120，p167 CLEAR 14/14）：修法=
// 具名 iterator（P50，C1Ev + find sret 临时拷贝）+ m_mapA8 用 make_pair 嵌套形
// `std::pair<const int,int>(std::make_pair(m_instId, m_mobIndex))`（P192/P178：
// 复现 ORIG make_pair 0x81112b2 + pair<const int,int> 转换构造 0x80c6d20 的双
// sret 槽 -0x10/-0x18，帧 0x3b0 对齐）。
void MapInfo::Add_Mob(map_monster mob)
{
    std::map<int, map_monster>::iterator it;
    it = m_monsterMap.find(mob.m_instId);
    if (it != m_monsterMap.end())
    {
        // ORIG 08151693..081516c4 实证：vararg=mob.m_instId(+0x4)；三串取自
        // .rodata 0x8b750eb / 0x8b75ee2 / 0x8b750d5，即
        // __FILE__="localglobal/../battle_field.h"、__FUNCTION__="Add_Mob"、
        // 格式串 "monster.find(%u) fail"、行号 0x1f2。
        // [Fn28Gap 2026-09-11] 0x081516ad: movl $0x8b75ee2,0x8(%esp) =
        // _ZZN7MapInfo7Add_MobE11map_monsterE12__FUNCTION__（r,8B="Add_Mob\0"）
        // ⇒ 原码是宏 __FUNCTION__，手写串只留匿名 .rodata、不发射该 local。
        LogManager::logFormat(1, "localglobal/../battle_field.h", __FUNCTION__, 0x1f2,
                              "monster.find(%u) fail", mob.m_instId);
        return;
    }

    m_monsterMap.insert(
        std::pair<const int, map_monster>(mob.m_instId, mob));
    m_mapA8.insert(std::pair<const int, int>(
        std::make_pair(mob.m_instId, mob.m_mobIndex)));
    m_vecC0.push_back(mob.m_instId);

    if (mob.m_bloodRound != 0)
    {
        ++m_cc;
    }
}

// [b28d-solo-MapInfo] AE（ORIG 0x081517e0，60/60，p167 CLEAR 7/7）：修法=
// P50 复活/P314——具名 iterator 默认构造（ORIG 0x8152524 C1Ev）+ find 结果经
// sret 临时(-0x7c)拷贝到 it(-0x70) 两条 mov；此前直接 `find()!=end()` 少 C1Ev
// 与临时槽、帧 0x98 vs ORIG 0xa8。
void MapInfo::Add_Item(map_item item)
{
    std::map<int, map_item>::iterator it;
    it = m_itemMap.find(item.m_itemIndex);
    if (it != m_itemMap.end())
    {
        // ORIG 0815184a..08151878 实证：vararg=item.m_itemIndex(+0x4)；三串取自
        // .rodata 0x8b750eb / 0x8b75e10 / 0x8b75109，即
        // "localglobal/../battle_field.h" / "Add_Item" / "item.find(%u) fail"，
        // 行号 0x206。
        // [Fn28Gap 2026-09-11] 0x08151861: movl $0x8b75e10,0x8(%esp) =
        // _ZZN7MapInfo8Add_ItemE8map_itemE12__FUNCTION__（r,9B="Add_Item\0"）。
        LogManager::logFormat(1, "localglobal/../battle_field.h", __FUNCTION__, 0x206,
                              "item.find(%u) fail", item.m_itemIndex);
        return;
    }

    m_itemMap.insert(
        std::pair<const int, map_item>(item.m_itemIndex, item));
}

void MapInfo::SelectDonsterItemDropLimit(unsigned int limit)
{
    if (limit == 0)
    {
        return;
    }

    if (m_vecC0.size() - (unsigned int)(unsigned short)m_cc <= limit)
    {
        return;
    }

    std::random_shuffle(m_vecC0.begin(), m_vecC0.end());

    for (std::map<int, map_monster>::iterator it = m_monsterMap.begin();
         it != m_monsterMap.end(); ++it)
    {
        if (it->second.m_bloodRound != 1)
        {
            it->second.m_isBoss = 1;
        }
    }

    for (unsigned int i = 0; i < limit; ++i)
    {
        std::map<int, map_monster>::iterator it =
            m_monsterMap.find(m_vecC0.at(i));
        if (it != m_monsterMap.end())
        {
            if (it->second.m_bloodRound != 0)
            {
                ++limit;
            }
            it->second.m_isBoss = 0;
        }
    }
}

// ORIG _ZN11map_monster14isNamedMonsterEv @ 0x08151182 (W)：
// m_mobIndex 落在命名怪区间 (0xc34f,0xc738] 或 (0xdabf,0xea60] → true。
// b28d-Main：`throw()` 与 WarField.h:145 声明一致（依据见该处注释：ORIG 调用点
// checkClearedNamedMonsterMap 无 EH landing pad / 无 _Unwind_Resume）。
bool map_monster::isNamedMonster() throw()
{
    return (m_mobIndex > 0xc34f && m_mobIndex <= 0xc738)
        || (m_mobIndex > 0xdabf && m_mobIndex <= 0xea60);
}
