// df_game_r 还原 —— CBattle_Field（G2-3 第一批）。
// 逐函数对照 docs/class_func_reports/CBattle_Field.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径，分支/调用目标→<T>、
// 大绝对地址→<A>、去@plt、剥尾部 nop、C2/D2 同址别名良性）。
// 已并入原 CBattle_Field_map.cpp（地图/移动小簇）；草稿文件已删除。
#include <cstring>

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"
#include "PacketGuard.h" // 权威 PacketGuard(0xc)
#include "CQuickPartyRewardManager.h"
#include "GameRand.h"  // get_rand_int

// ============================================================================

// ============================================================================
// 外部方法（权威头已提供声明：MazeScript::getGridR 见 CDungeon.h，
// CSystemTime::getCurTickCount 见 CSystemTime.h，CRidable::CheckAppearRidableObject
// 见 CRidable.h；GlobalData::s_systemTime_ 见 CUserCharacInfo.h）
// ============================================================================
CDataManager* G_CDataManager();

// CDungeon 局部布局代理（真实 CDungeon.h 已含 +0x8a0 字段布局的完整类型）：
// 本代理仅用于 +0x8a0 字段直接位移访问，保持 ORIG 直接位移加载形态。
struct CDungeon_8a0_proxy
{
    char m_pad[0x8a0];
    int m_8a0;
};

// G_CDataManager() +0x6170 处的 int 表视图（incUltimateRoundClearPoint 08307096
// 实证 add $0x185c,%edx + (%eax,%edx,4) 寻址 = 元素索引 v+0x185c 的成员数组
// 折算形态；视图仅本 TU 使用）
struct CDataManager_UltimateArr_proxy
{
    char m_pad[0x6170];
    int m_arr[1];
};

// ============================================================================
// 小访问器（ORIG 弱符号，直接读反汇编实现）
// ============================================================================

void CBattle_Field::SetParty(CParty* party)
{
    m_party = party;
}

void CBattle_Field::SetCurSeatNo(int seatNo)
{
    m_curSeatNo = seatNo;
}

int CBattle_Field::GetMapInfoKeyValue(int x, int y)
{
    return x * 100 + y;
}

MapInfo* CBattle_Field::GetCurrentMapInfo()
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    return GetMapInfoFromPos(x, y, false);
}

MapInfo* CBattle_Field::GetMapInfoFromPos(int x, int y, bool create)
{
    int key = GetMapInfoKeyValue(x, y);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        if (create)
        {
            return &it->second;
        }
        if (m_gridMaze != 0)
        {
            const GridScript* grid = &m_gridMaze->getGridR(x, y);
            if (grid != 0 && grid->m_layeredMapIndexes.size() != 0)
            {
                if (getCurrentLayeredMapInfo()->m_layeredMap)
                {
                    return getCurrentLayeredMapInfo();
                }
            }
        }
        return &it->second;
    }

    MapInfo info;
    key = GetMapInfoKeyValue(x, y);
    m_mapInfo.insert(std::make_pair(key, info));
    it = m_mapInfo.find(key);
    return &it->second;
}

int CBattle_Field::getMapWidth()
{
    return m_mapWidth;
}

int CBattle_Field::getMapHeight()
{
    return m_mapHeight;
}

bool CBattle_Field::check_start_point()
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    if (m_startX == x && m_startY == y)
    {
        return true;
    }
    return false;
}

bool CBattle_Field::check_end_point()
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    if (m_pos[0].first == x && m_pos[0].second == y)
    {
        return true;
    }
    return false;
}

int CBattle_Field::get_cur_map()
{
    return GetCurrentMapInfo()->m_mapIndex;
}

MapInfo* CBattle_Field::GetMapInfo()
{
    return GetCurrentMapInfo();
}

MazeScript* CBattle_Field::getMaze()
{
    return m_gridMaze;
}

int CBattle_Field::GetCurrentMapIndex()
{
    if (m_pMap != 0)
    {
        return m_pMap->get_index();
    }
    return 0;
}

bool CBattle_Field::IsClearDungeon()
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.IsClearCondition())
        {
            return true;
        }
        return false;
    }
    return check_end_point();
}

int CBattle_Field::get_dungeon_index()
{
    return m_dungeonIndex;
}

int CBattle_Field::get_dungeon_diff()
{
    return m_dungeonDiff;
}

CDungeon* CBattle_Field::GetDungeon()
{
    return m_pDungeon;
}

std::map<int, map_monster>* CBattle_Field::GetMonsterMap()
{
    return &GetCurrentMapInfo()->m_monsterMap;
}

int CBattle_Field::get_map_index(int x, int y)
{
    return GetMapInfoFromPos(x, y, false)->m_mapIndex;
}

CMap* CBattle_Field::GetCurrentMap()
{
    return m_pMap;
}

bool CBattle_Field::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.ClearCondition(type, idx))
        {
            return true;
        }
    }
    return false;
}

unsigned char CBattle_Field::IsVisitedCurrentMap()
{
    return GetCurrentMapInfo()->m_visited;
}

void CBattle_Field::setWorldMapTowerArea(bool flag)
{
    m_worldMapTowerArea = flag;
}

bool CBattle_Field::isWorldMapTowerArea()
{
    return m_worldMapTowerArea;
}

int CBattle_Field::GetCurPosX()
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // ORIG 0x085bf364-0x373 索引段 5 条：`mov 0x114(%eax),%edx`（值直落
        // edx）+ `add $0x2,%edx` 原地自增。4.4 恒 `mov mem,%eax` 落点 +
        // `lea` 合成（实测 P168 值绑定 register asm("edx") 反产 mov%eax,%edx
        // +回读 6 条，负结论已回报），合法源码无法复现直落形——记 N81 附族
        // （4.1 索引 regmove/load 落点），行集仍 5 条。
        return m_pos[m_curSeatNo + 2].first;
    }
    else
    {
        return m_curX;
    }
}

int CBattle_Field::GetCurPosY()
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        // 同上（ORIG 0x085bf3ba 区同构，N81 附族标注）。
        return m_pos[m_curSeatNo + 2].second;
    }
    else
    {
        return m_curY;
    }
}

// N81 附族：m_pos[m_curSeatNo+6] 索引段同上（add vs lea 直落，不可复现）。
MapInfo* CBattle_Field::GetLastMapInfo()
{
    int x = -1;
    int y = -1;
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
    {
        x = m_pos[m_curSeatNo + 6].first;
        y = m_pos[m_curSeatNo + 6].second;
    }
    else
    {
        x = m_lastX;
        y = m_lastY;
    }
    return GetMapInfoFromPos(x, y, false);
}

// N81 附族：m_pos[m_curSeatNo+2] 索引段 `mov mem,%eax`+`lea` 合成 vs ORIG
// `mov mem,%edx`+`add` 直落（同 GetCurPosX/Y，4.1 索引 regmove 落点，合法源码
// 不可复现——负面结论同列）。
void CBattle_Field::getCurPosXY(int& x, int& y)
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_pos[m_curSeatNo + 2].first;
        y = m_pos[m_curSeatNo + 2].second;
    }
    else
    {
        x = m_curX;
        y = m_curY;
    }
}

// N81 附族：m_pos[m_curSeatNo+6] 索引段同上（add vs lea 直落，不可复现）。
void CBattle_Field::getLastPosXY(int& x, int& y)
{
    if (m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        x = m_pos[m_curSeatNo + 6].first;
        y = m_pos[m_curSeatNo + 6].second;
    }
    else
    {
        // ORIG 0x8300e44 原版笔误：两次均写 &x（mov 0xc(%ebp)），y 形参未写。
        x = m_lastX;
        x = m_lastY;
    }
}

// N81 附族：m_pos[...+6]/[...+2] 索引段 + make_pair 传参同理（add vs lea 直落，
// 不可复现）。
void CBattle_Field::swap_curpos_xy(int x, int y)
{
    if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap() &&
        m_curSeatNo >= 0 && m_curSeatNo < 4)
    {
        m_pos[m_curSeatNo + 6] = m_pos[m_curSeatNo + 2];
        m_pos[m_curSeatNo + 2] = std::make_pair<int&, int&>(x, y);
    }
    else
    {
        m_lastX = m_curX;
        m_lastY = m_curY;
        m_curX = x;
        m_curY = y;
    }
}

// ============================================================================
// 小型访问器（读 ORIG 反汇编逐个实现）
// ============================================================================

int CBattle_Field::getBloodState()
{
    return m_bloodState2;
}

char* CBattle_Field::GetEnableHellDungeon()
{
    // ORIG 0822cf26：mov $0x93701ec,%eax; ret（POD 无 guard 的函数局部
    // static，V 符号 _ZZN13CBattle_Field20GetEnableHellDungeonEvE19enable_hell_dungeon）。
    static char enable_hell_dungeon;
    return &enable_hell_dungeon;
}

void CBattle_Field::SetEnableHellDungeon(bool flag)
{
    char* p = GetEnableHellDungeon();
    *p = flag;
}

void CBattle_Field::setUltimateDifficulty(char diff)
{
    m_ultimateDifficulty = diff;
}

char CBattle_Field::getUltimateDifficylty()
{
    return m_ultimateDifficulty;
}

unsigned char CBattle_Field::IsAssignNPCByPotion()
{
    return GetCurrentMapInfo()->m_assignNPCByPotion;
}

unsigned char CBattle_Field::IsEnableHellDungeon()
{
    char* p = GetEnableHellDungeon();
    return *p;
}

void CBattle_Field::set_dungeon_diff(register char diff)
{
    // ORIG 0x830ed21：movzbl 0xc → cmp $0x4 → jbe(写) → push/pop 设 4 ——
    // clamp 到 ≤4 的 if 形态（直接在 diff 参数上，无局部 v；比较无符号）。
    // （已改善 ndiff 18→11；残差为 ISel `movzbl mem` vs `mov32+copy`、
    // `push/pop` vs `mov` 常量装载，合法源码不可复现。）
    if ((unsigned char)diff > 4)
        diff = 4;
    m_dungeonDiff = (unsigned char)diff;
}

bool CBattle_Field::setBloodState(ENUM_BLOOD_STATE state)
{
    if (state <= m_bloodState2)
        return false;
    m_bloodState2 = state;
    return true;
}

int CBattle_Field::GetTournamentCurRound()
{
    return m_tournamentCurRound;
}

void CBattle_Field::SetTournamentCurRound(int round)
{
    m_tournamentCurRound = round;
}

char CBattle_Field::IsFirstEnterDungeon()
{
    return m_firstEnterDungeon;
}

void CBattle_Field::ResetFirstEnterDungeon()
{
    m_firstEnterDungeon = 0;
}

int CBattle_Field::getLayeredMapIndex()
{
    return m_layeredMapIndex;
}

void CBattle_Field::setLayeredMapIndex()
{
    m_layeredMapIndex = m_layeredMapIndex + 1;
}

int CBattle_Field::getLayeredMapKeyValue(int idx)
{
    return idx * 100 + 10000;
}

MapInfo* CBattle_Field::getLayeredMapInfoIndex(int idx)
{
    int key = getLayeredMapKeyValue(idx);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        return &it->second;
    }

    MapInfo info;
    key = getLayeredMapKeyValue(idx);
    m_mapInfo.insert(std::make_pair(key, info));
    it = m_mapInfo.find(key);
    return &it->second;
}

MapInfo* CBattle_Field::getCurrentLayeredMapInfo()
{
    return getLayeredMapInfoIndex(getLayeredMapIndex());
}

unsigned char CBattle_Field::IsKilledAllHellGruoups()
{
    return m_killedAllHellGroups;
}

bool CBattle_Field::SetRidableObjectState(int idx, eRidableObjectState state)
{
    return m_ridable.SetRidableObjectState(idx, state);
}

void CBattle_Field::CheckAppearRidableObject(PacketGuard* packet)
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    m_ridable.CheckAppearRidableObject(m_party, x, y, packet);
}

// CBattle_Field::GetFieldMob —— ORIG 0x0830a708（0x10a 字节）
//   报告：docs/class_func_reports/CBattle_Field/GetFieldMob.md
//   迭代器先默认构造（P28：it = find(...) 经临时中转拷贝）；两次独立
//   GetCurrentMapInfo()（P48 无 CSE）；找到则经块作用域临时 map_monster
//   拷贝给 out（tmp 析构先于 return true，对齐 ORIG 的 mov $1 在 dtor 后）。
bool CBattle_Field::GetFieldMob(int uid, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
        return false;
    {
        map_monster tmp(it->second);
        out = tmp;
    }
    return true;
}

int CBattle_Field::getBloodMaxRound()
{
    if (m_pDungeon != 0)
    {
        return ((CDungeon_8a0_proxy*)m_pDungeon)->m_8a0;
    }
    return 0;
}

// CBattle_Field::BloodRoundMaxTimePacket —— ORIG 0x08305dc4（0xd8 字节）
//   报告：docs/class_func_reports/CBattle_Field/BloodRoundMaxTimePacket.md
//   header 0xd6 = 大轮+1、最大时间、秒数；经 m_party 广播。
void CBattle_Field::BloodRoundMaxTimePacket(short round, int a, int b)
{
    PacketGuard packet;
    packet.put_header(0, 0xd6);
    packet.put_byte(0);
    packet.put_byte((int)round + 1);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    m_party->send_to_party(packet);
}

// CBattle_Field::getNextRoundRandomDifficulty —— ORIG 0x08306156（0x5f 字节）
//   报告：docs/class_func_reports/CBattle_Field/getNextRoundRandomDifficulty.md
//   大轮序==最大轮-1 → 1；否则掷 100，<=49 → 2，否则 1。
int CBattle_Field::getNextRoundRandomDifficulty()
{
    if ((short)m_bloodRound.getBloodRoundOrder() == getBloodMaxRound() - 1)
    {
        return 1;
    }
    else
    {
        int r = get_rand_int(100);
        if (r > 49)
            return 1;
        return 2;
    }
}

// ============================================================================
// 嵌套类型方法（ORIG 弱符号）
// ============================================================================

CBattle_Field::TournamentInfo::TournamentInfo()
{
    Clear();
}

void CBattle_Field::TournamentInfo::Clear()
{
    m_roundOrder = -1;
    for (int i = 0; i <= 1; ++i)
    {
        m_monster[i].Clear();
    }
    m_group = 0;
}

bool CBattle_Field::TournamentInfo::operator<(const TournamentInfo& other) const
{
    if (m_group < other.m_group)
        return true;
    return false;
}

void CBattle_Field::CBloodRound::setBloodRoundOrder()
{
    m_roundOrder = m_preMapRoundOrder;
}

void CBattle_Field::CBloodRound::setPreMapBloodRoundOrder()
{
    m_preMapRoundOrder = m_roundOrder;
}

void CBattle_Field::CBloodRound::incBloodRoundOrder()
{
    ++m_roundOrder;
}

unsigned short CBattle_Field::CBloodRound::getBloodRoundOrder()
{
    return m_roundOrder;
}

void CBattle_Field::CBloodRound::setBloodSmallRoundOrder(short v)
{
    m_smallRoundOrder = v;
}

void CBattle_Field::CBloodRound::incBloodSmallRoundOrder()
{
    ++m_smallRoundOrder;
}

unsigned short CBattle_Field::CBloodRound::getBloodSmallRoundOrder()
{
    return m_smallRoundOrder;
}

void CBattle_Field::CBloodRound::setBloodLastSmallRoundOrder(int v)
{
    m_lastSmallRoundOrder = v;
}

unsigned short CBattle_Field::CBloodRound::getBloodLastSmallRoundOrder()
{
    return m_lastSmallRoundOrder;
}

void CBattle_Field::CBloodRound::reset_blood_values()
{
    m_lastSmallRoundOrder = 0;
    m_roundOrder = 0;
    m_smallRoundOrder = 0;
}

// CBattle_Field::reset_blood_values @ 0x08306d86
// 清空血条分配向量、重生计数/时间数组并复位血条状态字段。
void CBattle_Field::reset_blood_values()
{
    m_bloodSystems.clear();
    m_bloodPhaseTimes.clear();
    m_bloodPhaseOrder.clear();
    m_bloodPhaseClear.clear();
    memset(m_bloodSpawnCnt, 0, sizeof(m_bloodSpawnCnt));
    memset(m_bloodSpawnTime, 0, sizeof(m_bloodSpawnTime));
    m_bloodCrazyRound = -1;
    m_bloodSpawnCntCur = 0;
    m_bloodCrazyType = 0;
}

CBattle_Field::CBloodClearRewardData::stRewardItem_t::stRewardItem_t()
{
}

void CBattle_Field::CBloodClearRewardData::stRewardItem_t::reset()
{
    m_count = 0;
    for (int i = 0; i <= 9; ++i)
    {
        m_item[i].reset();
        m_item[i].m_addInfo = -1;
    }
}

CBattle_Field::CBloodClearRewardData::CBloodClearRewardData()
{
}

void CBattle_Field::CBloodClearRewardData::reset()
{
    m_bloodRound = 0;
    m_playTime = 0;
    m_prevPlayTime = 0;
    m_isBloodClear = 0;
    for (int i = 0; i <= 3; ++i)
    {
        m_rewardExp[i] = 0;
        m_reward[i].reset();
    }
    m_ultimateClearPoint = 0;
}

void CBattle_Field::CBloodClearRewardData::onStartBloodRound()
{
    m_startTick = GlobalData::s_systemTime_.getCurTickCount();
}

void CBattle_Field::CBloodClearRewardData::addPlayTime(unsigned int t)
{
    m_prevPlayTime = m_playTime;
    m_playTime = m_playTime + t;
}

unsigned int CBattle_Field::CBloodClearRewardData::getPlayTime()
{
    return m_playTime;
}

unsigned int CBattle_Field::CBloodClearRewardData::getBloodRoundClearTime()
{
    return m_endTick - m_startTick;
}

int CBattle_Field::CBloodClearRewardData::getCurrentBloodRound()
{
    return m_bloodRound;
}

void CBattle_Field::CBloodClearRewardData::setCurrentBloodRound(int v)
{
    m_bloodRound = v;
}

int CBattle_Field::CBloodClearRewardData::getRewardItemCount(int idx)
{
    return m_reward[idx].m_count;
}

Inven_Item* CBattle_Field::CBloodClearRewardData::getRewardItem(int idx)
{
    return &m_reward[idx].m_item[0];
}

int CBattle_Field::CBloodClearRewardData::getRewardExp(int idx)
{
    return m_rewardExp[idx];
}

// N18a 物化：m_rewardExp[idx] 地址计算 ORIG `lea 0x4(%edx),%ecx` vs 4.4
// `add $0x4,%ecx`（ISel add/lea，合法源码不可复现）。
void CBattle_Field::CBloodClearRewardData::setRewardExp(int idx, unsigned int exp)
{
    m_rewardExp[idx] = exp;
}

void CBattle_Field::CBloodClearRewardData::setRewardItem(int idx, int itemIdx)
{
    m_reward[idx].m_count = itemIdx;
}

void CBattle_Field::CBloodClearRewardData::setBloodClear(bool v)
{
    m_isBloodClear = v;
}

unsigned char CBattle_Field::CBloodClearRewardData::isBloodClear()
{
    return m_isBloodClear;
}

int CBattle_Field::CBloodClearRewardData::getUltimateRoundClearPoint()
{
    return m_ultimateClearPoint;
}

void CBattle_Field::CBloodClearRewardData::incUltimateRoundClearPoint(int v)
{
    // ORIG 08307096：m_9c8 += dm_intTable[v+0x185c]；视图 m_arr[v] 折算同址。
    m_ultimateClearPoint =
        m_ultimateClearPoint
        + ((CDataManager_UltimateArr_proxy*)G_CDataManager())->m_arr[v];
}

CBattle_Field::CTowerOfDespairRewardData::CTowerOfDespairRewardData()
{
}

CBattle_Field::CTowerOfDespairRewardData::~CTowerOfDespairRewardData()
{
}

void CBattle_Field::CTowerOfDespairRewardData::reset_tod()
{
    m_todMap.clear();
}

CBattle_Field::stConditionEnterBossRoomStatus_t::stConditionEnterBossRoomStatus_t()
{
}

CBattle_Field::stConditionEnterBossRoomStatus_t::stConditionEnterBossRoomStatus_t(
    const stConditionEnterBossRoomStatus_t& other)
    : m_monsterIdx(other.m_monsterIdx),
      m_pos(other.m_pos),
      m_curKillCount(other.m_curKillCount),
      m_maxKillCount(other.m_maxKillCount)
{
}

CBattle_Field::stConditionEnterBossRoomStatus_t::~stConditionEnterBossRoomStatus_t()
{
}

CBattle_Field::stConditionEnterBossRoomStatus_t&
CBattle_Field::stConditionEnterBossRoomStatus_t::operator=(
    const stConditionEnterBossRoomStatus_t& other)
{
    m_monsterIdx = other.m_monsterIdx;
    m_pos = other.m_pos;
    m_curKillCount = other.m_curKillCount;
    m_maxKillCount = other.m_maxKillCount;
    return *this;
}

CBattle_Field::stEventMonsterStatus_t::stEventMonsterStatus_t()
{
}

CBattle_Field::stEventMonsterStatus_t::~stEventMonsterStatus_t()
{
}

// ============================================================================
// 外部依赖（CGameManager.h 与本头组合存在重定义冲突，按 ORIG 符号做最小声明，
// 链接解析到真实定义 —— 与 QuickParty.cpp 同一模式）
// ============================================================================
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

// WongWork::CDeathTower 最小完成类型（CUser.h 仅前置声明；非虚方法，仅用 this）。
namespace WongWork
{
class CDeathTower
{
public:
    int genTimerKey(TIMER_MESSAGE msg);   // ORIG 0x8142c46 域内（见 CGameManager.h 声明）
    int getIdx();
};
}

// ============================================================================
// CBattle_Field::getRandomBuffDungeonType —— ORIG 0x0830aaa4（0x1d5 字节）
//   报告：docs/class_func_reports/CBattle_Field/getRandomBuffDungeonType.md
// ============================================================================
int CBattle_Field::getRandomBuffDungeonType(int dungeonIdx)
{
    if (dungeonIdx == 0)
    {
        if (m_pDungeon == 0)
            return 7;
        dungeonIdx = m_pDungeon->get_index();
    }

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
        return 7;

    // quick party 奖励副本表命中 → 类型 7
    std::vector<int>& idxList =
        G_CGameManager()->GetQuickPartyRewardManager()->get_dungeon_idx_list();
    for (std::vector<int>::const_iterator it = idxList.begin();
         it != idxList.end(); ++it)
    {
        if (*it == dungeonIdx)
            return 7;
    }

    if (dungeon->get_dimension_possible() > 0)
        return 2;
    if (dungeon->m_89c != 0)                       // +0x89c 直读（ORIG movzbl 0x89c）
        return 1;

    int blood = dungeon->get_blood_dungeon_type();
    if (blood == 1)
        return 4;
    if (blood == 2)
        return 5;

    // kind==1 且塔内堆叠物品限制非 0 → 3；kind==1 且限制==0 → 6；否则 0。
    // （ORIG 分支顺序如此：!=0 → 3 先判，再判 !1 → 6。）
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 0)
        return 3;
    if (dungeon->getDungeonKind() == 1 && dungeon->limitOfStackableItemInTower() != 1)
        return 6;
    return 0;
}

// ============================================================================
// CBattle_Field::setEplpWaitingTimer —— ORIG 0x0830ac7a（0x17b 字节）
//   报告：docs/class_func_reports/CBattle_Field/setEplpWaitingTimer.md
//   type 3/6 → 绝望之塔 timer(0x4d)；4/5 → party timer(0x25)；其余 → party
//   timer(0x15)。秒数均 0x1e。
//   （N 封顶：ndiff 93 残差为分支判定调度（cmp $4,jge vs $3,jle）+ 三段
//   InsertTimer 链 esi/ebx 跨调用保存与帧差异，源码已按 ORIG 比较边界写 >=4，
//   GCC 仍规范化，无法逐字节复现——见报告）
// ============================================================================
void CBattle_Field::setEplpWaitingTimer(register QuickParty::RandomBuffDungeon type)
{
    if (m_party == 0)
        return;

    if (type <= 5)
    {
        // ORIG 0x0830ac7a：cmp $0x5,jg + cmp $0x4,jge（type∈{4,5} party
        // timer），比较边界写 >=4 而非 >3 以复现 jge。
        if (type >= 4)
        {
            register unsigned int key = static_cast<unsigned int>(
                m_party->gen_timer_key((TIMER_MESSAGE)0x25));
            int idx = m_party->GetPartyIndex();
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, idx,
                                        (TIMER_MESSAGE)0x25, 0x1e, key, 0x25);
            return;
        }
        if (type != 3)
        {
            goto timer15;
        }
    }
    else if (type != 6)
    {
        goto timer15;
    }
    {
        WongWork::CDeathTower* tower = m_party->getManager()->getDeathTower();
        if (tower == 0)
            return;
        register unsigned int key = static_cast<unsigned int>(
            tower->genTimerKey((TIMER_MESSAGE)0x4d));
        int idx = tower->getIdx();
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, idx,
                                    (TIMER_MESSAGE)0x4d, 0x1e, key, 0x4d);
        return;
    }
timer15:
    register unsigned int key = static_cast<unsigned int>(
        m_party->gen_timer_key((TIMER_MESSAGE)0x15));
    int idx = m_party->GetPartyIndex();
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, idx,
                                (TIMER_MESSAGE)0x15, 0x1e, key, 0x15);
}
