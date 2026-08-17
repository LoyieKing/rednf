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

// ---- 支撑结构（最小复刻，布局与 ORIG 一致） ----

// 6 个 int（0x18 字节）；init ORIG 081512ec（CBattle_Field TU）
struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    int m_dieCnt;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14

    void init();
};

// 4 个 int（0x10 字节）；init ORIG 0815132c（CBattle_Field TU）
struct stMapPlayInfo_t
{
    unsigned int m_startTick;   // +0x00
    unsigned int m_clearTick;   // +0x04
    int m_useItemType2Count;    // +0x08
    int m_useItemOtherCount;    // +0x0c

    void init();
};

// map_monster（0x34c 字节；ctor 081511c8，其它 TU）
struct map_monster
{
    int m_mobId;       // +0x00
    int m_instId;      // +0x04
    char m_pad08[4];   // +0x08
    int m_mobIndex;    // +0x0c
    char m_pad10[0x339];  // +0x10..+0x348
    char m_isBoss;     // +0x349
    char m_bloodRound; // +0x34a
    char m_pad34b[1];  // +0x34b

    map_monster();
    map_monster(const map_monster& other);
    ~map_monster();
    map_monster& operator=(const map_monster& other);
};

// map_item（0x54 字节；ctor 081512cc，其它 TU）
struct map_item
{
    int m_count;          // +0x00
    int m_itemIndex;      // +0x04
    char m_pad08[0x4c];   // +0x08..+0x53

    map_item();
    map_item(const map_item& other);
    ~map_item();
    map_item& operator=(const map_item& other);
};

// 日志（ORIG Add_Mob/Add_Item 调用 LogManager::logFormat，本 TU 外部引用）
class LogManager
{
public:
    static void logFormat(int level, const char* section, int line,
                          const char* msg, ...);
};

// ---- MapInfo（0xec 字节） ----
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

    MapInfo();
    MapInfo(const MapInfo& other);
    ~MapInfo();
    MapInfo& operator=(const MapInfo& other);
    void Clear();
    void Add_Mob(map_monster mob);
    void Add_Item(map_item item);
    void SelectDonsterItemDropLimit(unsigned int limit);
};

MapInfo::MapInfo()
{
    Clear();
}

MapInfo::MapInfo(const MapInfo& other)
    : m_monsterMap(other.m_monsterMap),
      m_itemMap(other.m_itemMap),
      m_hellPartyGroup(other.m_hellPartyGroup),
      m_mapA8(other.m_mapA8),
      m_vecC0(other.m_vecC0),
      m_tournamentMob(other.m_tournamentMob)
{
    m_key = other.m_key;
    m_visited = other.m_visited;
    m_visitedGoto = other.m_visitedGoto;
    m_mapIndex = other.m_mapIndex;
    m_gridValue = other.m_gridValue;
    m_assignItemIdx = other.m_assignItemIdx;
    m_dropItemCnt = other.m_dropItemCnt;
    m_mapType = other.m_mapType;
    m_dummyBossMap = other.m_dummyBossMap;
    m_mapBasisLevel = other.m_mapBasisLevel;
    m_killChecker1 = other.m_killChecker1;
    m_killChecker2 = other.m_killChecker2;
    m_playInfo = other.m_playInfo;
    m_assignNPCByPotion = other.m_assignNPCByPotion;
    m_cc = other.m_cc;
    m_layeredMap = other.m_layeredMap;
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

void MapInfo::Add_Mob(map_monster mob)
{
    if (m_monsterMap.find(mob.m_instId) != m_monsterMap.end())
    {
        LogManager::logFormat(1, "MapInfo.cpp", 0x1f2,
                              "duplicated mob instance", mob.m_instId);
        return;
    }

    m_monsterMap.insert(
        std::pair<const int, map_monster>(mob.m_instId, mob));
    m_mapA8.insert(std::pair<const int, int>(mob.m_instId, mob.m_mobIndex));
    m_vecC0.push_back(mob.m_instId);

    if (mob.m_bloodRound != 0)
    {
        ++m_cc;
    }
}

void MapInfo::Add_Item(map_item item)
{
    if (m_itemMap.find(item.m_itemIndex) != m_itemMap.end())
    {
        LogManager::logFormat(1, "MapInfo.cpp", 0x206,
                              "duplicated item index", item.m_itemIndex);
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
