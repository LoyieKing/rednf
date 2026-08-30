#include "GameRand.h"
// df_game_r 还原 —— WongWork::CDeathTower（死亡之塔）
// 基于 ORIG 反汇编逐函数实现语义。
#include "CGameManager.h"
#include "CUser.h"
#include "CParty.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "TimerQueue.h"
#include "LogManager.h"
#include "CInventory.h"
#include "CCirculationStatistic.h"
#include "CDataManager.h"
#include "CMap.h"
#include "GameWorld.h"
#include "CBossStage.h"
#include "CDungeonEntranceLog.h"
#include "CDeathTowerRanking.h"
#include "CMonster.h"
#include "CItemGenerator.h"    // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDeathTowerWinPointEvent.h"
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义
#include "CStatisticServerProxy.h"    // statistc_proxy::add（handlePickupItem 随机选项统计）
#include "CMonitorServerProxy.h"      // updateTowerRank（_beginTowerClearProcess）
#include "GlobalData.h"               // s_monitor_proxy_mgr
#include "CServerProxyMgr.h"          // GetServerProxy 完整定义
#include "MonsterIndependentDrop.h"        // CAICharacter 完整镜像（destroy 需 delete）

// ---- 死亡塔统计/记录接口（定义见 Store.cpp / OnlyOrigBatch14.cpp / W8LT_DBTask.cpp）----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_1 = 1,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};
class CDeathTowerValueStatistic
{
public:
    // ORIG 0x0860f342 返回 bool（PRETTY_FUNCTION "bool ..."，true/false 分支）
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                      char a, char b, unsigned int c);
};
extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic();
#include "CDeathTowerPlayDataStatistic.h"          // SendDeathTowerPlayInfo 完整类型
extern CDeathTowerPlayDataStatistic* GetInstanceDeathTowerPlaydataStatistic();
class DB_UpdateDeathTowerBestRecord
{
public:
    static void makeRequest(unsigned int characNo, unsigned int accId,
                            unsigned int memberCount,
                            const stDeathTowerRecordMemberInfo_t* members,
                            unsigned int stage, unsigned int playTime);
};

// ---- CAICharacter / CAICharacterList ----
// 布局依据 ORIG set 0x0834a066 / ctor 0x08373a9a / 各访问器：
//   +0x00 int  idx             （getIdx 0x08150850：mov 0(%eax),eax）
//   +0x04 int  level           （getLevel 0x0815085a：mov 4(%eax),eax）
//   +0x08 int  m_field8        （set 写 script+0xd80）
//   +0x0c int  appearancePoint （getAppearancePoint 0x08150866：mov c(%eax),eax）
//   +0x10 vector<pair<int,int> > m_dropItems （deathTowerSpecifyItemDrop 遍历 +0x10）
//   +0x1c int  m_pvpWinLow     （getPvPWinPoint 0x0834a240 读取 +0x1c）
//   +0x20 int  m_pvpWinHigh    （getPvPWinPoint 读取 +0x20）
//   +0x24 vector<stMonsterIndependentDrop_t>
//   +0x30 stEnamyDropAvatarInfo_t
//   +0x6c std::string m_name
//   +0x70 unsigned char m_rarity
#ifndef DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
#define DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED
class CAICharacter
{
public:
    int getIdx() const { return m_idx; }
    int getLevel() const { return m_level; }
    int getAppearancePoint() const { return m_appearancePoint; }
    // ORIG 0x0834a19c _ZNK12CAICharacter25deathTowerSpecifyItemDropERSt6vectorImSaImEE：
    // 遍历 m_dropItems（vector<pair<int,int>>），对每项 r=get_rand_int(10000)；
    // 若 pair.second > r 则把 pair.first（itemId）push_back 到 dropIds。
    void deathTowerSpecifyItemDrop(std::vector<unsigned long>& dropIds) const
    {
        for (std::vector<std::pair<int, int> >::const_iterator it =
                 m_dropItems.begin();
             it != m_dropItems.end(); ++it)
        {
            int r = get_rand_int(10000);
            if (it->second > r)
                dropIds.push_back((unsigned long)it->first);
        }
    }
    // ORIG 0x0834a240 _ZNK12CAICharacter14getPvPWinPointEv：
    // 返回 PvP 胜点。+0x1c=最小胜点，+0x20=最大胜点；等则返回该值，
    // 否则返回 min + get_rand_int(max - min + 1)。
    int getPvPWinPoint() const
    {
        if (m_pvpWinLow == m_pvpWinHigh)
            return m_pvpWinLow;
        return m_pvpWinLow + get_rand_int(m_pvpWinHigh - m_pvpWinLow + 1);
    }

private:
    int m_idx;                                      // +0x00
    int m_level;                                    // +0x04
    int m_field8;                                   // +0x08
    int m_appearancePoint;                          // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;  // +0x10
    int m_pvpWinLow;                                // +0x1c
    int m_pvpWinHigh;                               // +0x20
    char m_pad24[0x70 - 0x24];                      // +0x24..+0x6f（独立掉/头像/名字区，本 TU 未用）
};
#endif  // DNF_GAME_CAI_CHARACTER_MIRROR_DEFINED

CAICharacter* CAICharacterList::get(unsigned int idx)
{
    std::map<int, CAICharacter*>::iterator it = m_charMap.find((int)idx);
    if (it == m_charMap.end())
        return 0;
    return it->second;
}
// ORIG dtor 0x08373d9e：destroy() 后按逆序析构各子对象（成员析构由编译器生成）。
CAICharacterList::~CAICharacterList() { destroy(); }

// ORIG destroy 0x0834a59a：遍历 m_charMap，对每个 CAICharacter 调析构并 delete。
void CAICharacterList::destroy()
{
    for (std::map<int, CAICharacter*>::iterator it = m_charMap.begin();
         it != m_charMap.end(); ++it)
    {
        // ORIG 此处调用 CAICharacter::~CAICharacter()（0x08373b32 W）后
        // operator delete；CAICharacter 在本工程尚未建模（仅前向声明），
        // 其析构当前无可执行语义，先直接释放【推断】。
        if (it->second != 0)
            delete it->second;   // 镜像类成员（vector/string）随析构释放
    }
    m_charMap.clear();
}

// ---- stMapMonsterKillChecker_t / map_item / map_monster ----
// 权威定义见 WarField.h（经 CGameManager.h → PvP_Room.h → PvP_deps.h 链包含），
// 本地重复定义已删除（§7 单声明点）。方法定义保留在下方（归属 WarField.h 声明）。

unsigned int stMapMonsterKillChecker_t::getUseSkillMaterialTime() { return m_useSkillMaterialTime; }

int stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
{
    if (m_lastMobDieTime == 0 || tick - m_lastMobDieTime > limit)
    {
        m_uncontinuallyMobDieCnt = 0;
    }
    else
    {
        m_uncontinuallyMobDieCnt++;
    }
    m_lastMobDieTime = tick;
    m_dieCnt++;
    return m_uncontinuallyMobDieCnt;
}

void stMapMonsterKillChecker_t::initMonsterInfo()
{
    // ORIG 0x830e71c：仅清零 +0x08
    m_uncontinuallyMobDieCnt = 0;
}

class TimerQueue;
TimerQueue* G_TimerQueue();

// CDungeonEntranceLog（CDungeonEntranceLog.h 权威声明）/ WongWork::CDeathTowerRanking
// （CDeathTowerRanking.h 权威声明）直接调用。

namespace WongWork {

// CBossTower 布局位于 CGameManager.h 的 `WongWork::CBossTower`（未命名 m_pad[0xb18]），
// 且派生于不透明 CBossStage（经 CBossStage::reset 调用，CBossStage.h 权威声明）。
// 以下各偏移均为 CBossStage 基类/内嵌子对象字段，尚无具名成员【推断】，因此保留地址形式并按 ORIG
// （0x81429b4 ctor / 0x8142a74 附近 dtor）逐条注明：
//   +0xb00  CBossDungeonEntranceLog 子对象（ORIG ctor 调其 ctor；此处按 0x18 零填）
//   +0xaf0  CBossStage 基类字段（ctor 置 0）
//   +0x9f0  CBossStage 基类字段（reset 之外 ctor 额外置 0）
//   +0xd    4 字节 CBossStage 字段（ctor memset 0）
//   +8 / +4 CBossPlay 基类首部字段（vtable 之后）；+0xaf0 = CBossTower::m_idx；
//   +0xb00 = CBossTower::m_dungeonLog（CBossDungeonEntranceLog，0x18）。
// 继承：CBossTower : CBossStage : CBossPlay（隐式基类构造/析构，编译器处理 vptr）。
CBossTower::CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    *(int*)((char*)this + 8) = 0;
    memset((char*)this + 0xd, 0, 4);
    memset((char*)this + 0x9f0, 0, 4);
    *(int*)((char*)this + 4) = 0;
    // m_dungeonLog（CBossDungeonEntranceLog）由编译器在进入函数体前按权威
    // ctor（ORIG 0x081456c2：map C1 + reset）构造，此处不再 memset。
}

CBossTower::~CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    *(int*)((char*)this + 8) = 0;
    memset((char*)this + 0xd, 0, 4);
    memset((char*)this + 0x9f0, 0, 4);
    *(int*)((char*)this + 4) = 0;
    // m_dungeonLog 由编译器在函数体结束后按权威 dtor（ORIG 0x08145704：
    // reset + map D1）析构，此处不再 memset。
}


// CDeathTowerRanking 方法实现（权威声明见 CDeathTowerRanking.h；本 TU 为唯一定义点，
// CUser.cpp 的本地同名类/桩已删除）。rankTable 为 getRankTable 返回的外部数据区
// （每塔 0x390 记录）；以下 +0x98/+0x128/+0x7058/+0x70ec 等均为该外部记录字段偏移，
// 非本类成员，保留地址形式。

unsigned int CDeathTowerRanking::makeDungeonIdx2TowerIdx(unsigned int dungeonIdx)
{
    // ORIG 08469d69 W：dungeonIdx∈[0x2af8,0x2afd] → idx-0x2af8（塔序号 0..5），其余 -1。
    if (dungeonIdx > 0x2af7 && dungeonIdx <= 0x2afd)
        return dungeonIdx - 0x2af8;
    return 0xffffffff;
}

void* CDeathTowerRanking::getRankTable(unsigned int type)
{
    // ORIG 082a774c W：4 张静态表（BSS 0x941f8c0，每表 0x70f4；Init 循环
    // type=1..4 调 getRankTable）懒构造（__cxa_guard + 逐表 C1）；
    // type==0 返回首表，否则 (type-1)*0x70f4 偏移。
    static CDeathTowerRanking s_tables[4];  // 静态初始化由编译器 guard 保护
    if (type == 0)
        return &s_tables[0];
    return &s_tables[type - 1];
}

int CDeathTowerRanking::checkRenewMyRecord(unsigned int towerIdx,
                                           unsigned int key,
                                           unsigned int stage,
                                           unsigned int playTime)
{
    // ORIG 084682fe T：m_bestTables[towerIdx].find(key)；无记录 → -1；
    // 否则构造临时纪录(stage,playTime)，临时 > 已有 → 1，否则 0。
    if (towerIdx >= 5)
        return -1;  // 越界保护（ORIG 无检查【推断】）
    stBestRecord_t tmp;
    tmp.m_stage = stage;
    tmp.m_playTime = playTime;
    BestMap::const_iterator it(m_bestTables[towerIdx].find(key));
    if (it == BestMap::const_iterator(m_bestTables[towerIdx].end()))
        return -1;
    return tmp > it->second ? 1 : 0;  // ORIG: stBestRecord_t::operator>(tmp, existing)
}

int CDeathTowerRanking::checkRenewTopRecord(unsigned int towerIdx,
                                            unsigned int clearTime,
                                            unsigned int playTime)
{
    char* base = (char*)this + towerIdx * 0x390;
    if (base[0x128] != 1)
    {
        return 0;
    }

    unsigned int* existingRecord = (unsigned int*)(base + 0x98);
    unsigned int existingClearTime = existingRecord[0];
    unsigned int existingPlayTime = existingRecord[1];

    if (clearTime < existingClearTime)
    {
        return 1;
    }
    else if (clearTime == existingClearTime && playTime < existingPlayTime)
    {
        return 1;
    }
    return 0;
}

// 与 ORIG fistpll（默认 round-to-nearest，ties-to-even）一致的 32→64 取整。
static long long roundHalfEven(long double x)
{
    long long r = (long long)x;         // 向零截断（此处 x≥0，即向下取整）
    long double frac = x - (long double)r;
    if (frac > 0.5L)
        return r + 1;
    if (frac < 0.5L)
        return r;
    return (r & 1) ? r + 1 : r;         // 恰为 .5：就近取偶
}

int CDeathTowerRanking::getVirtualRank(unsigned int towerIdx,
                                       unsigned int stage,
                                       unsigned int playTime)
{
    // 候选新纪录（stage, playTime）。ORIG 0x08468de0：本类无 towerIdx 越界检查。
    // 调用方（rankTable 槽位）保证 towerIdx 在 [0,4]，与 ORIG 保持一致不加防御。

    stBestRecord_t newRec;
    newRec.m_stage = stage;
    newRec.m_playTime = playTime;

    // 1) 表级最优（+0x7058 m_tableBest）严格优于新纪录 → -1。
    if (m_tableBest > newRec)
        return -1;
    // 2) 新纪录 == 表级最优 → 返回表级纪录存故名次（+0x70ec = m_tableBest.m_field94）。
    if (newRec == m_tableBest)
        return m_tableBest.m_field94;
    // 3) 新纪录严格优于该塔当前榜首（+0x98 槽 0）→ 名次 1。
    if (newRec > m_towers[towerIdx].m_slots[0])
        return 1;

    // ---- 层内 rank map（键=通关时间，值=名次）插值 ----
    RankMap& m = m_rankMap[towerIdx][stage];

    unsigned int result = 0xffffffff;  // ORIG local_20（初值 -1）
    RankMap::const_iterator ref;       // ORIG local_88（末次返回的基准迭代器）
    bool inMap = false;                // ORIG local_28 != 0

    if (m.size() >= 2)
    {
        // playTime 须落在 [首键, 末键] 内；否则跳至“层邻插值”分支。
        RankMap::const_iterator first = m.begin();
        RankMap::const_iterator last = m.end();
        --last;
        if (playTime >= first->first && last->first >= playTime)
        {
            // lower_bound(playTime)；若命中键 > playTime 且非 begin，退一位使
            // ref 恒为键 < playTime 的最近元素（= 插值下界）。
            ref = m.lower_bound(playTime);
            if (playTime < ref->first && ref != m.begin())
                --ref;
            RankMap::const_iterator nxt = ref;
            ++nxt;

            if (ref->first == playTime)
                return (int)ref->second;
            if (nxt->first == playTime)
                return (int)nxt->second;

            // 两相邻键线性插值：
            //   keySpan = nxt.key - ref.key（≥1）
            //   slope = |nxt.value - ref.value| / keySpan
            //   result = max(1, round((playTime - ref.key) * slope))
            unsigned int keySpan = nxt->first - ref->first;
            if (keySpan < 1)
                keySpan = 1;
            long long valDiff = (long long)nxt->second - (long long)ref->second;
            long long absDiff = valDiff < 0 ? -valDiff : valDiff;
            float slope = (float)((long double)absDiff / (long double)keySpan);
            long long scaled = roundHalfEven(
                (long double)(playTime - ref->first) * (long double)slope);
            unsigned int r = (unsigned int)scaled;
            result = r < 1 ? 1 : r;
            inMap = true;
        }
    }

    if (!inMap)   // ORIG if (local_28 == 0)：map 元素 <2 或 playTime 超出键范围
    {
        unsigned int lowerStage = 0;   // ORIG local_18
        unsigned int upperStage = 0;   // ORIG local_14

        // 当前层 map 仅 1 元素时先判其末元素。
        if (!m.empty())
        {
            RankMap::const_iterator last = m.end();
            --last;
            if (last->first == playTime)
                return (int)last->second;
            if (last->first < playTime)
                upperStage = stage;
            else
                lowerStage = stage;
        }

        // 向下 / 向上找最近的、rank map 非空的相邻层（下界 ≥2，上界 <200）。
        if (lowerStage == 0)
        {
            lowerStage = stage;
            do
            {
                lowerStage = lowerStage - 1;
                if (lowerStage < 2 ||
                    !m_rankMap[towerIdx][lowerStage].empty())
                    break;
            } while (true);
        }
        if (upperStage == 0)
        {
            upperStage = stage;
            do
            {
                upperStage = upperStage + 1;
                if (upperStage >= kRankStageCount ||
                    !m_rankMap[towerIdx][upperStage].empty())
                    break;
            } while (true);
        }

        // 下界层无纪录（或已下探至 <2 仍空）或上界层无纪录（或已上探至 200 仍空）→ -1。
        // （ORIG 对上界 local_14==200 会无条件读 map[200] 越界；此处用
        //   upperStage>=kRankStageCount 短路护栏避免真实数组越界，语义同“无邻层”。）
        if (m_rankMap[towerIdx][lowerStage].empty() ||
            upperStage >= kRankStageCount ||
            m_rankMap[towerIdx][upperStage].empty())
            return -1;

        // 跨层插值：ref = 上界层末元素（最大键），基准 = 下界层首元素；
        //   stageSpan = upperStage - lowerStage（≥1）
        //   slope = |lower.first.value - upper.last.value| / stageSpan
        //   result = max(1, round((upperStage - stage) * slope))
        RankMap::const_iterator lowerFirst =
            m_rankMap[towerIdx][lowerStage].begin();
        ref = m_rankMap[towerIdx][upperStage].end();
        --ref;

        unsigned int stageSpan = upperStage - lowerStage;
        if (stageSpan < 1)
            stageSpan = 1;
        long long valDiff = (long long)lowerFirst->second -
                            (long long)ref->second;
        long long absDiff = valDiff < 0 ? -valDiff : valDiff;
        float slope = (float)((long double)absDiff / (long double)stageSpan);
        long long scaled = roundHalfEven(
            (long double)(upperStage - stage) * (long double)slope);
        unsigned int r = (unsigned int)scaled;
        result = r < 1 ? 1 : r;
    }

    return (int)(result + ref->second);
}

void CDeathTowerRanking::unregistBestRecord(unsigned int characNo)
{
    // ORIG 08468b4c T：for i in 0..4: _getBestRecordTable(i).erase(characNo)。
    for (unsigned int i = 0; i < 5; ++i)
        m_bestTables[i].erase(characNo);
}

void CDeathTowerRanking::unregistRanking(unsigned int characNo)
{
    // ORIG 08468552 T：for i in 0..4: _getRankingTable(i).erase(characNo)。
    for (unsigned int i = 0; i < 5; ++i)
        m_rankingTables[i].erase(characNo);
}

// ORIG _ZN8WongWork18CDeathTowerRanking4initEj @ 0x084680a4（0x1ec 字节）。
// this = getRankTable(type) 表；对 5 座塔 × 5 槽位重置 stBestRecord_t：
//   +0x90 清字节；槽 1..5 的 +0x04 memcpy 默认名 "AAAAAA"/"BBBBBB"/"CCCCCC"/
//   "DDDDDD"/"EEEEEE"（ORIG 0x8c63370..0x8c6338d，7 字节含 NUL）；
//   +0x94=槽序号、+0x8c=0、+0x88=200-槽序号；
// 最后表尾 +0x70e0=200、+0x70f0=type。
void CDeathTowerRanking::init(unsigned int type)
{
    void* table = getRankTable(type);
    // ORIG 0x8c63370 实证每名 6 字符 + NUL（"AAAAAA\0" 等，7 字节；
    // memcpy 7 字节 = 6 字母 + NUL，与 ORIG 逐字节一致）
    static const char* kDefaultNames[5] = {
        "AAAAAA", "BBBBBB", "CCCCCC", "DDDDDD", "EEEEEE" };
    char* base = reinterpret_cast<char*>(table);
    for (unsigned int tower = 0; tower <= 4; ++tower)
    {
        for (unsigned int slot = 1; slot <= 5; ++slot)
            base[tower * 0x390 + slot * 0x98 + 0x90] = 0;
        for (unsigned int slot = 1; slot <= 5; ++slot)
        {
            unsigned int off = tower * 0x390 + slot * 0x98;
            std::memcpy(base + off + 0x04, kDefaultNames[slot - 1], 7);
            *reinterpret_cast<int*>(base + off + 0x94) = static_cast<int>(slot);
            *reinterpret_cast<unsigned int*>(base + off + 0x8c) = 0;
            *reinterpret_cast<unsigned int*>(base + off + 0x88) =
                200 - slot;
        }
    }
    *reinterpret_cast<unsigned int*>(base + 0x70e0) = 200;
    *reinterpret_cast<unsigned int*>(base + 0x70f0) = type;
}

CDeathTower::CDeathTower()
{
    // ORIG 0x82a75da：CDungeonMgr/CStage/CPlayData 子对象 ctor 由编译器生成；
    // 函数体仅将 m_timerKeys[TIMER_MESSAGE 0x48..0x4e] 清零。
    for (int i = 0x48; i <= 0x4e; ++i)
        m_timerKeys[i - 0x48] = 0;
}

CDeathTower::~CDeathTower() {}

int CDeathTower::getIdx()
{
    return m_idx;
}

int CDeathTower::getTowerState()
{
    return m_towerState;
}

void CDeathTower::setIdx(int idx)
{
    m_idx = idx;
}
int CDeathTower::setTowerState(ENUM_TOWER_STATE state)
{
    // ORIG 0x8469c34：state <= m_towerState 时返回 0；否则写入并返回 1。
    if (state <= (ENUM_TOWER_STATE)m_towerState)
    {
        return 0;
    }
    m_towerState = state;
    return 1;
}

void CDeathTower::reset()
{
    m_dungeonMgr.reset();
    m_stage.reset();
    m_playData.reset();
    m_party = 0;
    m_towerState = 0;
}

void CDeathTower::_destroy()
{
    setTowerState((ENUM_TOWER_STATE)0);
    G_CGameManager()->returnDeathTower(this);
}

CDeathTower::CDungeonMgr* CDeathTower::getCDungeonMGr() const
{
    return (CDungeonMgr*)&m_dungeonMgr;
}

CDeathTower::CStage* CDeathTower::getCStage()
{
    return &m_stage;
}

void CDeathTower::attachParty(CParty* party)
{
    m_party = party;
    for (int i = 0; i < 4; ++i)
    {
        if (party->checkValidUser(i))
        {
            short idx = (short)m_idx;
            CUser* user = party->get_user(i);
            user->setDeathTowerIndex(idx);
        }
    }
    setTowerState((ENUM_TOWER_STATE)1);
}

int CDeathTower::genTimerKey(TIMER_MESSAGE msg)
{
    return _genTimerKey(msg);
}

int CDeathTower::_genTimerKey(TIMER_MESSAGE msg)
{
    // ORIG 0x830edec：返回 ++m_timerKeys[msg - 0x48]。
    return ++m_timerKeys[(int)msg - 0x48];
}

bool CDeathTower::_checkTimerKey(TIMER_MESSAGE msg, unsigned int key)
{
    // ORIG 0x8469c68：key == m_timerKeys[msg - 0x48]。
    return key == (unsigned int)m_timerKeys[(int)msg - 0x48];
}

int CDeathTower::_checkMemberDie()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            if (m_playData.isMemberAlive(i))
            {
                return 0;
            }
        }
    }
    return 1;
}

int CDeathTower::_checkMemberReady()
{
    m_playData.resetMemberReady();
    int count = m_party->get_member_count();
    unsigned int timeout;
    if (count == 1)
    {
        timeout = 0x78;  // CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT
    }
    else
    {
        timeout = 0x5a;  // CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT
    }
    int key = _genTimerKey((TIMER_MESSAGE)0x49);
    int idx = m_idx;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, idx, (TIMER_MESSAGE)0x49, timeout, key, 0x49);
    return 0;
}

int CDeathTower::onLeaveUser(CUser* user)
{
    user->setDeathTowerIndex((short)-1);
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    inven->RemoveFPItem();
    char memberNames[0x100];
    memset(memberNames, 0, sizeof(memberNames));

    if (m_party)
    {
        // ORIG 0x084636f2：m_party 存在时记 LeaveDungeon 历史（state=m_party+0xcd8 字节）。
        char state = *(const char*)((const char*)m_party + 0xcd8);
        m_party->getMemberNames(memberNames);   // char* 返回（ORIG W 转发 _getMemberNames）
        const char* dungeonName = m_dungeonMgr.getDungeon()->GetDungeonName();
        user->m_historyLog.LeaveDungeon(dungeonName, 0, memberNames, state);
    }

    if (m_party)
    {
        CDungeon* dungeon = m_dungeonMgr.getDungeon();
        if (dungeon)
        {
            int dungeonIdx = dungeon->get_index();
            CDungeonEntranceLog* entranceLog = GetInstanceDungeonEntranceLog();
            entranceLog->DecrementDungeonEntrance(dungeonIdx, false);
        }
    }

    if (m_party)
    {
        int cnt = m_party->get_member_count();
        if (cnt <= 0)
        {
            _destroy();
        }
    }
    return 1;
}

void CDeathTower::_beginTowerClearProcess(bool isLast)
{
    if (getTowerState() > 2)
        return;

    unsigned int memberCnt = m_playData.getStartMemberCnt();
    void* rankTable = WongWork::CDeathTowerRanking::getRankTable(memberCnt);

    _onClear(isLast);

    int partyState = m_party->m_partyType;
    if (partyState == 1)
    {
        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);

        int key = _genTimerKey((TIMER_MESSAGE)0x4d);
        int mgrIdx = m_idx;
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, mgrIdx, (TIMER_MESSAGE)0x4d, 0x3c, key, 0x4d);

        PacketGuard pkt;
        pkt.put_header(0, 0xfe);
        pkt.finalize(true);
        m_party->send_to_party(pkt);
    }
    else
    {
        setTowerState((ENUM_TOWER_STATE)3);

        int key = _genTimerKey((TIMER_MESSAGE)0x4b);
        int mgrIdx = m_idx;
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, mgrIdx, (TIMER_MESSAGE)0x4b, 1, key, 0x4b);
    }

    int curStage = m_stage.getCurrentStage();
    unsigned int playTime;
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
        int stageMinus1 = curStage - 1;
        int one = 1;
        curStage = std::max(stageMinus1, one);
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (!user || !m_party->checkValidUser(i))
            continue;

        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);

        int characNo = user->getCurCharacNo();
        unsigned int dungeonIdx = m_dungeonMgr.getDungeon()->get_index();

        int result = _checkRenewMyRecord(dungeonIdx, memberCnt, characNo, curStage, playTime);
        int isNewRecord = (isLast && result == 1) ? 1 : 0;

        int topResult = _checkRenewTopRecord(dungeonIdx, memberCnt, characNo, playTime);
        if (isLast && topResult)
            isNewRecord += 2;

        unsigned int towerIdx = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
        int virtualRank = ((WongWork::CDeathTowerRanking*)rankTable)->getVirtualRank(
            towerIdx, curStage, playTime);

        // ORIG 0x08463f5：notTower = (dungeonIdx != 11000)；随后统计/记录上报
        char notTower = (char)(dungeonIdx != 0x2af8);
        GetInstanceDeathTowerPlaydataStatistic()->SendDeathTowerPlayInfo(
            i, notTower,
            (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
            curStage, memberCnt);

        int level = user->get_charac_level();
        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)1, notTower, (char)level, curStage);
        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)0, notTower, (char)level, 1);

        if (result != 0)
        {
            DB_UpdateDeathTowerBestRecord::makeRequest(
                towerIdx, characNo, memberCnt,
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                curStage, playTime);
            if (virtualRank > 0)
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)0)->updateTowerRank(
                        characNo, towerIdx, memberCnt, virtualRank);
                ((WongWork::CDeathTowerRanking*)rankTable)->updateRanking(
                    towerIdx, characNo, virtualRank);
            }
            ((WongWork::CDeathTowerRanking*)rankTable)->registBestRecord(
                towerIdx, (unsigned int)characNo,
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                (unsigned int)curStage, playTime);
        }

        CPacketHandler handler;
        handler.makeDisplayRangking(
            memberCnt, characNo, curStage, playTime, (result == 1), dungeonIdx,
            virtualRank);
        user->Send(handler);

        // 奖励数量：ORIG 0x08466f5 两套表（limitOfStackableItemInTower 决定）
        // G_CDataManager + ((0x152c + curStage)*4 + 0xc) / ((0x16bc + curStage)*4 + 0xc)
        int rewardNumBase;
        if (m_dungeonMgr.getDungeon()->limitOfStackableItemInTower())
            rewardNumBase = *(int*)((char*)G_CDataManager() + ((0x152c + curStage) * 4 + 0xc));
        else
            rewardNumBase = *(int*)((char*)G_CDataManager() + ((0x16bc + curStage) * 4 + 0xc));
        int rewardItemNum = rewardNumBase + isNewRecord;
        int maxReward = m_dungeonMgr.getDungeon()->getTowerMaxClearRewardItemNum();
        rewardItemNum = std::min(rewardItemNum, maxReward);
        rewardItemNum = std::min(rewardItemNum, 10);

        int level2 = user->get_charac_level();
        m_playData.generateRewardItem(i, rewardItemNum, m_dungeonMgr, level2,
                                      std::min(curStage, 10), m_party);

        unsigned int mobReward = 0;
        G_CDataManager()->get_mob_reward(level2, mobReward);
        QuickParty::RewardData_Param rewardParam(m_party);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = mobReward;
        m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                   rewardResult);
        mobReward = rewardResult.m_value00;

        m_playData.generateRewardExp(i, curStage, mobReward, m_party, m_dungeonMgr);
    }
}
int CDeathTower::handleKillMonster(CUser* user, unsigned short monsterIdx, unsigned short a3, const MSG_MONSTER_DIE& msg, int* a5)
{
    if (m_stage.getState() != 1)
    {
        if (user)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2a, 0x13);
        }
        return 0;
    }

    // Monster kill checker (skill material time)
    stMapMonsterKillChecker_t* checker = (stMapMonsterKillChecker_t*)&m_timerKeys[7];
    bool bSkillMaterial = false;
    if (checker->getUseSkillMaterialTime() == 0)
    {
        unsigned int curTick = GlobalData::s_systemTime_.getCurTickCount();
        int result = checker->dieMonster(curTick, 1000);
        if (result > 2)
        {
            bSkillMaterial = true;
        }
    }

    if (bSkillMaterial)
    {
        bool isPremium = false;
        if (user && user->get_charac_level() >= 0x1e)
        {
            isPremium = true;
        }
        else if (user && user->get_charac_level() >= 0x19)
        {
            CUserPremium* premium = user->GetPremiumInfo();
            if (premium)
            {
                std::vector<SUserPremiumInfo> premiumList;
                premium->GetPremiumInfoList(premiumList, 0x270f);
                for (size_t i = 0; i < premiumList.size(); i++)
                {
                    int type = premiumList[i].m_type;
                    if (type >= 0x17 && type <= 0x1b && premiumList[i].m_state == 1)
                    {
                        isPremium = true;
                        break;
                    }
                }
            }
        }

        if (isPremium)
        {
            checker->initMonsterInfo();
            CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            if (hackAnalyzer)
            {
                hackAnalyzer->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0xd6, 1, 0, 0);
            }
        }
    }

    // Kill monster in stage
    map_monster monster;
    int killResult = m_stage.killMonster(monsterIdx, monster);
    if (killResult == 0)
    {
        return 0;
    }

    int memberLevelGap = m_party->getMemberLevelGap();
    int winPoint = 0;
    std::vector<unsigned long> dropItemIds;

    if (monster.m_roleType < 5)
    {
        // Boss monster drop
        CMonster* monsterPtr = (CMonster*)G_CDataManager()->find_monster(monster.m_mobIndex);
        if (monsterPtr)
        {
            CDungeon* dungeon = m_dungeonMgr.getDungeon();
            if (dungeon && dungeon->hasTowerItemDrop())
            {
                long dropId = monsterPtr->deathTowerSpecifyItemDrop();
                if (dropId != 0xfffffffe)
                {
                    dropItemIds.push_back(dropId);
                }
            }
        }
    }
    else
    {
        // Normal monster drop
        CAICharacterList* aiCharList = G_CDataManager()->m_aiCharacterList;
        CAICharacter* aiChar = 0;
        if (aiCharList)
        {
            aiChar = aiCharList->get(monster.m_mobIndex);
        }
        if (aiChar)
        {
            CDungeon* dungeon = m_dungeonMgr.getDungeon();
            if (dungeon && dungeon->hasTowerItemDrop())
            {
                aiChar->deathTowerSpecifyItemDrop(dropItemIds);
            }
            if (monster.m_dropCnt == 100 || monster.m_dropCnt == 200)
            {
                winPoint = aiChar->getPvPWinPoint();
            }
        }
    }

    // Create items from drop ids
    std::vector<Inven_Item> rewardItems;
    for (size_t i = 0; i < dropItemIds.size(); i++)
    {
        int itemId = dropItemIds[i];
        CItem* item = G_CDataManager()->m_itemList->find_item(itemId);
        if (item)
        {
            Inven_Item invenItem;
            invenItem.m_addInfo = itemId;
            G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)1, invenItem, 1);
            int memberCnt = m_party->get_member_count();
            for (int j = 0; j < memberCnt; j++)
            {
                rewardItems.push_back(invenItem);
            }
        }
    }

    // Exp rewards for party members
    for (int i = 0; i < 4; i++)
    {
        CUser* partyUser = m_party->get_user(i);
        if (!partyUser || !m_party->checkValidUser(i))
        {
            continue;
        }

        if (m_party->get_party_type() != 1)
        {
            unsigned int mobExp = 0;
            int charLevel = partyUser->get_charac_level();
            G_CDataManager()->get_mob_reward(charLevel, mobExp);
            // ORIG 0x08464e69：getMonsterTotalExp 后经 set_random_reward 取回
            mobExp = m_party->getMonsterTotalExp(mobExp, false, 0, 1.0f, 1.0f, false);
            QuickParty::RewardData_Param rewardParam(m_party);
            QuickParty::RewardData_Result rewardResult;
            rewardResult.m_value00 = mobExp;
            m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                       rewardResult);
            mobExp = rewardResult.m_value00;

            if (monster.m_roleType == 5)
            {
                // ORIG 0x08464f2c：G_CDataManager+0x5198 float 倍率（魔王怪 EXP）
                float* ratePtr = (float*)((char*)G_CDataManager() + 0x5198);
                mobExp = (unsigned int)(*ratePtr * (float)mobExp);
            }

            // ORIG 0x08464f80：G_CDataManager+0x5e1c float 倍率（无条件）
            float* ratePtr2 = (float*)((char*)G_CDataManager() + 0x5e1c);
            mobExp = (unsigned int)(*ratePtr2 * (float)mobExp);

            // ORIG 0x08465000：STGainedEachExpData(m_exp=mobExp) 后
            // sumGainedEachExp 取回
            STGainedEachExpData expData;
            expData.m_exp = mobExp;
            mobExp = m_party->sumGainedEachExp(partyUser, expData);

            if (winPoint != 0)
            {
                // ORIG 0x08465017：GetRepeatEvent(0x27) 后直接调 getBonusRate
                // （不判空，与 ORIG 一致）；rate==0 → 原值，否则乘率取整
                CDeathTowerWinPointEvent* event =
                    (CDeathTowerWinPointEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x27);
                float bonusRate = event->getBonusRate();
                if (bonusRate == 0.0f)
                    partyUser->gainWinPoint(winPoint, (eWPAddReason)2);
                else
                    partyUser->gainWinPoint((int)((float)winPoint * bonusRate),
                                            (eWPAddReason)2);
            }

            // ORIG 0x08465090：calcHelpAbuseRatio(mobExp, false)
            partyUser->calcHelpAbuseRatio(mobExp, false);

            int notiType = 0xb;
            if (monster.m_roleType == 5)
            {
                notiType = 0xc;
            }

            int expGain = 0;
            int spGain = 0;
            if (partyUser->gain_exp_sp(mobExp, expGain, spGain,
                                       (CUser::eExpAddReason)notiType,
                                       monster.m_mobIndex, false))
            {
                partyUser->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
            }

            // ORIG 0x0846515f：cUserHistoryLog（user+0x79700）KillMonster 日志；
            // killValue1=msg+0xa1e / killValue2=msg+0xa1a / a=+0xa24 / b=+0xa26；
            // curExp=getCurCharacExp()
            cUserHistoryLog* historyLog =
                (cUserHistoryLog*)((char*)partyUser + 0x79700);
            historyLog->KillMonster(monster.m_mobIndex, msg.m_killValue1,
                                    msg.m_killValue2, msg.m_killPairA24,
                                    msg.m_killPairA26, msg,
                                    static_cast<char>(monster.m_roleType),
                                    monster.m_roleType == 5,
                                    partyUser->getCurCharacExp(),
                                    (int)mobExp, (CUser::eExpAddReason)notiType);

            // ORIG 0x08465244：CheckQuestMonster(dungeonIndex, mobIndex, type)
            // type：roleType 5..8 → 2，否则 1
            {
                int questType = 1;
                if (monster.m_roleType > 4 && monster.m_roleType < 9)
                    questType = 2;
                CDungeon* dungeon = m_dungeonMgr.getDungeon();
                if (dungeon)
                    partyUser->CheckQuestMonster(dungeon->get_index(),
                                                 monster.m_mobIndex,
                                                 (ENUM_QUEST_ENEMY_TYPE)questType);
            }
        }
    }

    // Drop items to stage and send packet
    std::list<map_item> dropList;
    for (size_t i = 0; i < rewardItems.size(); i++)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = a3;
        item.m_createTick = GlobalData::s_systemTime_.getCurSec();
        item.m_item = rewardItems[i];
        m_stage.dropItem(item);
        dropList.push_back(item);
    }

    // Send drop item packet to party
    PacketGuard packet;
    packet.put_header(0, 0x26);
    packet.put_short(monsterIdx);
    int itemCountPos = packet.get_index();
    packet.put_byte(0);  // placeholder for count

    int itemCount = 0;
    for (std::list<map_item>::iterator it = dropList.begin(); it != dropList.end(); ++it)
    {
        map_item& item = *it;
        packet.put_short(item.m_itemIndex);
        packet.put_int(item.m_item.m_addInfo);
        unsigned char itemAttr = item.m_item.GetItemAttr();
        packet.put_byte(itemAttr);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_int(item.m_item.get_add_info());
        }
        else
        {
            CEquipItem* equipItem = (CEquipItem*)G_CDataManager()->find_item(item.m_item.m_addInfo);
            if (!equipItem)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // Avatar period: getAvatarPeriod not yet implemented, use add_info
                packet.put_int(item.m_item.get_add_info());
                stAvatarEmblemInfo_t emblemInfo;
                emblemInfo.init();
                equipItem->getAvatarSocket(item.m_item.m_fieldb, emblemInfo);
            }
        }

        packet.put_short(item.m_item.m_fieldb);
        unsigned char amplifyType = item.m_item.m_amp.getAbilityType();
        packet.put_byte(amplifyType);
        unsigned short amplifyValue = item.m_item.m_amp.getAbilityValue();
        packet.put_short(amplifyValue);
        packet.put_packet(item.m_item);

        if (!item.m_item.isAvatarItemType())
        {
            packet.put_byte(0);
        }
        else
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            stAvatarEmblemInfo_t emblemInfo;
            emblemInfo.init();
            packet.put_binary((char*)&emblemInfo, 0x1e);
        }

        packet.put_short(item.m_dropIndex);
        itemCount++;
    }

    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(itemCountPos, itemCount);
    packet.finalize(true);
    m_party->send_to_party(packet);

    // Check stage clear
    if (m_stage.checkClearStage())
    {
        _onFinishStage();
        int curStage = m_stage.getCurrentStage();
        int endStage = m_dungeonMgr.getEndStage();
        if (curStage == endStage)
        {
            _onPrepareFinishTower();
        }
    }

    return 1;
}
void CDeathTower::_onClear(bool isLast)
{
    unsigned int playTime;
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
    }

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            user->setChattingMessageCount(0);
            // CUser +0x79700：cUserHistoryLog 子对象（未建模，CUser.h 无具名成员）【推断】
            cUserHistoryLog* log = (cUserHistoryLog*)((char*)user + 0x79700);
            log->DungeonClearInfo((int)isLast, playTime / 1000);

        }
    }
}
void CDeathTower::_onFinishDeathTower()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            onLeaveUser(user);
        }
    }
    m_party->ReturnToVillage();
    if (getTowerState() > 0)
    {
        _destroy();
    }
}

void CDeathTower::_onFinishStage()
{
    m_stage.onFinishStage();
    unsigned int clearTime = m_stage.getStageClearTime();
    m_playData.addPlayTime(clearTime);

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            if (clearTime <= 0x176f)   // ORIG cmp $0x176f,setbe（5999，非 6000）
            {
                CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0x19a, 1u, 0u, 0u);
            }
            int stageMapIdx = m_stage.getStageMapIndex();
            UserQuest* quest = user->getCurCharacQuestW();
            quest->set_authen_data((QUEST_CONDITION)1, stageMapIdx, 0);
        }
    }
}

void CDeathTower::_onPrepareFinishTower()
{
    int key = _genTimerKey((TIMER_MESSAGE)0x4a);
    int idx = m_idx;
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, idx, (TIMER_MESSAGE)0x4a, 5, key, 0x4a);
}

void CDeathTower::_onStartDeathTower()
{
    char memberNames[0x40];
    // ORIG 0x08467b83：getMemberNames 填充（转发 _getMemberNames）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (user == 0)
            continue;
        if (!m_party->checkValidUser(i))
            continue;
        CDungeon* dungeon = m_dungeonMgr.getDungeon();
        const char* dungeonName = dungeon->GetDungeonName();
        // CUser +0x79700：cUserHistoryLog 子对象（同 _onClear）【推断】
        cUserHistoryLog* log = (cUserHistoryLog*)((char*)user + 0x79700);
        // ORIG：EnterDungeon(dungeonName, 0, memberNames, m_party->m_partyType)
        log->EnterDungeon(dungeonName, 0, memberNames, m_party->get_party_type());

        UserQuest* quest = user->getCurCharacQuestW();
        // UserQuest +0x75d8：_Quest_Authen_Data 内嵌子对象（UserQuest 未建模）【推断】
        _Quest_Authen_Data* authData = (_Quest_Authen_Data*)((char*)quest + 0x75d8);
        authData->reset();

        int dungeonIdx = dungeon->get_index();
        quest->set_authen_data((QUEST_CONDITION)0, dungeonIdx, -1);
    }
}

void CDeathTower::_onStartStage()
{
    m_stage.onStartStage();
}

// ---- CStage ----
// ORIG W 0x82a74ca：MapInfo ctor（编译器生成）后 resetCurrentStage()。
CDeathTower::CStage::CStage()
{
    resetCurrentStage();
}

// ORIG W 0x82a74ec：仅 MapInfo dtor（编译器生成）。
CDeathTower::CStage::~CStage()
{
}

void CDeathTower::CStage::reset()
{
    // ORIG 0x84614ec：MapInfo::Clear(m_mapInfo)；m_assignItemIdx/m_dropItemCnt
    // = get_rand_int(60000)；m_state = 0。
    m_mapInfo.Clear();
    m_mapInfo.m_assignItemIdx = get_rand_int(60000);
    m_mapInfo.m_dropItemCnt = get_rand_int(60000);
    m_state = 0;
}

void CDeathTower::CStage::onStartStage()
{
    m_startTick = GlobalData::s_systemTime_.getCurTickCount();
}

void CDeathTower::CStage::onFinishStage()
{
    m_finishTick = GlobalData::s_systemTime_.getCurTickCount();
}

int CDeathTower::CStage::getStageClearTime()
{
    return m_finishTick - m_startTick;
}

int CDeathTower::CStage::getStageMapIndex()
{
    return m_mapInfo.m_mapIndex;
}

int CDeathTower::CStage::getCurrentStage()
{
    return m_currentStage;
}

int CDeathTower::_pickupItemMoney(CUser* user, int memberCnt, const map_item& item, int* money1, int* money2)
{
    int money = item.m_item.get_add_info();
    int base = money / memberCnt;
    int remainder = money % memberCnt;
    int total = base + remainder;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (!member || !m_party->checkValidUser(i))
            continue;

        int amount = (member == user) ? total : base;
        int taxed = amount;                                // ORIG -0xc（可能翻倍）
        if (item.m_count != 0)
        {
            money2[i] = member->getCurCharacInvenR()
                            ->GetCreatureMgrR()->GetExtraGold(amount);
            taxed = amount * 2;
        }

        money1[i] = member->getCurCharacInvenW()->gain_money(
            taxed, (eMoneyAddReason)4, true, 0);
        if (item.m_count != 0)
            member->saveTaxMoneyForUpperMember(taxed);
        if (money1[i] < taxed)
        {
            if (money1[i] < 0)
                money1[i] = 0;
            member->SendMoneyFullReason(ENUM_MONEY_FULL_REASON_0,
                                        (unsigned long)taxed,
                                        (unsigned long)money1[i]);
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)1, member, (unsigned int)money1[i]);
    }

    return 0;
}
CUser* CDeathTower::_pickupItem(CUser* user, char* countBuf, int slot, const map_item& item, bool a5, char a6, const CItem* citem)
{
    // ORIG T 0x08466cfe（Ghidra 反编译语义逐条转写；countBuf = 调用点 nameBuf[4] 计数）
    CUser* result = user;
    if (slot > 1)
    {
        char isFromMap = (item.m_count == 1) ? 1 : 0;          // mVar1 == 1
        int rarity = citem->get_rarity();
        if (rarity > 3)
        {
            isFromMap = a5 ? 1 : 0;
        }
        bool bEpic = false;
        if (item.m_count == 0 || !citem->isEpicRoutingItem())
        {
            bEpic = false;
        }
        else
        {
            bEpic = true;
        }
        if (bEpic)
        {
            isFromMap = a5 ? 1 : 0;
        }
        if (isFromMap && citem->check_item_routing())
        {
            result = NULL;
            int routedTurn = m_playData.getLastRoutedTurn() + 1;
            if (slot <= routedTurn)
            {
                routedTurn = 0;
            }
            int local_24 = 0;
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (!member || !m_party->checkValidUser(i))
                    continue;
                if (!m_playData.isMemberAlive(i))
                    continue;

                CInventory* invenR = (CInventory*)member->getCurCharacInvenR();
                int ins = invenR->tryInsertItemIntoInventory(item.m_item);
                if (ins != -1)
                {
                    countBuf[i] += 1;
                    if (local_24 == routedTurn)
                    {
                        countBuf[i] += '(';
                        m_playData.setLastRoutedTurn(local_24);
                    }
                    bool bAdd = false;
                    if (!citem->is_stackable() && citem->GetItemType() == 10)
                    {
                        bAdd = citem->check_job_type(member->get_charac_job());
                    }
                    if (bAdd)
                    {
                        countBuf[i] += 0x1e;
                    }
                    countBuf[i] += get_rand_int(100 - countBuf[i]);
                    local_24 += 1;
                }
            }
            char maxC = 0;
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (!member || !m_party->checkValidUser(i))
                    continue;
                if (!m_playData.isMemberAlive(i))
                    continue;

                if (maxC < countBuf[i])
                {
                    maxC = countBuf[i];
                    result = member;
                }
                else if (countBuf[i] == maxC && countBuf[i] > 0)
                {
                    countBuf[i] -= 1;
                }
            }
        }
    }
    return result;
}

// ORIG T 0x0846966c：卡片统计（每卡 AddValueStatistic 0xb/0xc + 死亡之塔
// ValueStatistic 上报，字段 8/2/7/10；isNotTower/level 参与上报参数）。
void CDeathTower::deathTowerCardStatistic(int count, CUser* user, const Inven_Item* items)
{
    unsigned int dungeonIdx = m_dungeonMgr.getDungeon()->get_index();
    bool isNotTowerOfDespair = (dungeonIdx != 0x2af8);
    int level = user->get_charac_level();

    for (int i = 0; i < count; ++i)
    {
        const Inven_Item* item = &items[i];   // Inven_Item 打包尺寸 0x3d（等价于 items+i*0x3d）
        int itemId = item->m_addInfo;
        if (itemId == 0)
        {
            int addInfo = item->get_add_info();
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0xb, user, (unsigned int)addInfo);
            GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                (DEATHTOWER_VALUE_STATISTIC_FIELD)8, (char)isNotTowerOfDespair,
                (char)level, (unsigned int)addInfo);
        }
        else
        {
            CItem* citem = G_CDataManager()->find_item(itemId);
            if (citem)
            {
                int price = 0;
                Inven_Item itemCopy = *item;
                G_Store()->GetSellItemPrice(itemCopy, (const CItem*)citem, (short)count, false, price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0xc, user, (unsigned int)price);
                if (citem->GetAttachType() == 3)     // ORIG vptr 虚调用 ==3
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)2, (char)isNotTowerOfDespair,
                        (char)level, 1);
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
                else
                {
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)10, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)citem->get_rarity());
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)7, (char)isNotTowerOfDespair,
                        (char)level, (unsigned int)price);
                }
            }
        }
    }
}
bool CDeathTower::_makeDropItem(CUser* user, char type, int slot, int count, map_item& item)
{
    // ORIG T 0x0846638c（Ghidra 反编译语义逐条转写；参数槽：0x8=this,0xc=user,
    // 0x10=type,0x14=slot,0x18=count,0x1c=item&）
    Inven_Item invenItem;   // ORIG 调 _ZN10Inven_ItemC1Ev（0x80cb854）

    switch (type)
    {
    case 0: // money or equip（ORIG 判断 slot==0 为金钱）
        if (slot == 0)
        {
            // money item（ORIG +0x4d：m_count=0 / m_field1=2 / m_addInfo=0 /
            // set_add_info(count) / m_fieldb=0 / ResetItemAttr）
            item.m_count = 0;
            item.m_item.m_field1 = 2;
            item.m_item.m_addInfo = 0;
            item.m_item.set_add_info(count);
            item.m_item.m_fieldb = 0;
            item.m_item.ResetItemAttr();
        }
        else
        {
            CInventory* inven = (CInventory*)user->getCurCharacInvenR();
            inven->GetInvenSlotByRef(1, slot, invenItem);
        }
        break;
    case 2: // cargo
    {
        const CCargo* cargo = user->getCurCharacCargoR();
        invenItem = cargo->get_cargo_slot(slot);
        break;
    }
    case 3: // avatar（ORIG GetInvenSlot space=0）
    {
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        inven->GetInvenSlotByRef(0, slot, invenItem);
        break;
    }
    case 7: // creature（ORIG GetInvenSlot space=3）
    {
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        inven->GetInvenSlotByRef(3, slot, invenItem);
        if (invenItem.m_field1 == 5)          // ITEM_TYPE_CREATURE_EQUIP → 拒绝
        {
            return false;
        }
        break;
    }
    default:
        return false;
    }

    // 公共校验 + 字段填充（ORIG +0x377 起）
    int sellPrice = 0;
    if (invenItem.m_addInfo != 0)             // +0x37e：itemId 非 0 才校验
    {
        CItem* citem = G_CDataManager()->find_item(invenItem.m_addInfo);
        if (!citem)
        {
            return false;
        }
        int attachType = citem->GetAttachType();
        switch (attachType)
        {
        case 1:
        case 2:
        case 5:
            return false;
        case 3:
            if (invenItem.m_field0 != 1)
            {
                return false;
            }
            break;
        }
        if (invenItem.m_upgradeSep.IsTradeRestriction())
        {
            return false;
        }
        if (invenItem.isEquipableItemType())
        {
            int itemType = *(int*)((char*)citem + 0x234);   // ORIG 直读数据 +0x234
            if (itemType == 0xb || itemType < 10)
            {
                return false;
            }
        }
        // 字段填充（ORIG +0x44c）
        item.m_item.m_field0 = 0;
        item.m_item.m_field1 = invenItem.m_field1;
        item.m_item.m_addInfo = invenItem.m_addInfo;
        item.m_item.m_fieldd = invenItem.m_fieldd;
        item.m_item.m_amp = invenItem.m_amp;
        item.m_item.m_fieldb = invenItem.m_fieldb;
        item.m_item.m_field0 = invenItem.m_field0;
        if (invenItem.isEquipableItemType())
        {
            item.m_item.set_add_info(count);
        }
        else
        {
            item.m_item.set_add_info(invenItem.m_addInfo2);
        }
        item.m_item.SetItemAttr(invenItem.GetItemAttr());
        G_Store()->GetSellItemPrice(invenItem, citem, (short)count, false, sellPrice);
    }

    // 删除/扣费 + 统计（ORIG +0x535 起）
    switch (type)
    {
    case 0:
        if (slot == 0)
        {
            CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
            if (invenW->use_money(count, (eMoneySubReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        }
        else
        {
            CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
            if (invenW->delete_item((INVEN_TYPE)1, slot, count,
                                    (eItemDelReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        }
        break;
    case 2:
    {
        CCargo* cargoW = user->getCurCharacCargoW();
        if (cargoW->delete_item(slot, count, (eItemDelReason)0x10) != 1)
        {
            return false;
        }
        break;
    }
    case 3:
    {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (invenW->delete_item((INVEN_TYPE)0, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    case 7:
    {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (invenW->delete_item((INVEN_TYPE)3, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    default:
        return false;
    }
    return true;
}

int CDeathTower::_checkRenewMyRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b, unsigned int c)
{
    unsigned int towerIdx = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    void* rankTable = WongWork::CDeathTowerRanking::getRankTable(memberCnt);
    return ((WongWork::CDeathTowerRanking*)rankTable)->checkRenewMyRecord(towerIdx, a, b, c);
}

int CDeathTower::_checkRenewTopRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b)
{
    unsigned int towerIdx = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    void* rankTable = WongWork::CDeathTowerRanking::getRankTable(memberCnt);
    return ((WongWork::CDeathTowerRanking*)rankTable)->checkRenewTopRecord(towerIdx, a, b);
}

int CDeathTower::_checkStartGameCondition(unsigned char& flag)
{
    Inven_Item items[4];
    int params[4];
    memset(items, 0, sizeof(items));
    memset(params, 0, sizeof(params));

    CDungeon* dungeon = m_dungeonMgr.getDungeon();
    if (m_party->CheckEnterAncientDungeon(dungeon, flag, items, params))
    {
        dungeon = m_dungeonMgr.getDungeon();
        if (m_party->UseAncientDungeonItems(dungeon, items, params))
        {
            return 0;0;
        }
        return 1;1;
    }
    return 0x11;
}
int CDeathTower::handleStartGame(int dungeonIdx, char flag, ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == (ENUM_DUNGEON_TYPE)1)
    {
        throw 8;
    }

    m_dungeonMgr.reset();
    if (!m_dungeonMgr.initDungeonMgr(dungeonIdx))
    {
        throw 0x16;
    }

    unsigned char checkFlag = 0;
    int cond = _checkStartGameCondition(checkFlag);
    if (cond != 0)
    {
        throw cond;
    }

    m_stage.reset();
    m_stage.resetCurrentStage();
    CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
    if (!map)
    {
        throw 0x15;
    }
    if (!m_stage.consistMap(m_dungeonMgr, map, this))
    {
        throw 1;
    }

    m_playData.reset();
    m_playData.makeStartMemberInfo(m_party);

    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            CUser* user = m_party->get_user(i);
            int minLevel = m_dungeonMgr.getDungeon()->get_min_level();
            if (user->get_charac_level() < minLevel)
            {
                throw 0xe;
            }
            user->set_state((ch_state)10);
            m_playData.setMemberAlive(i, true);
            int partyState = m_party->get_party_type();
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                dungeonIdx, partyState == 1);
        }
    }

    m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
    m_party->SetSelectedEPLPCmd(-1);
    PacketGuard packet;
    packet.put_header(0, 0x8e);

    packet.put_int(dungeonIdx);
    packet.put_short(m_dungeonMgr.getEndStage());
    if (m_party == NULL)
    {
        packet.put_byte(0xb);
    }
    else
    {
        packet.put_byte(m_party->get_random_buff_type());
    }
    packet.finalize(true);
    m_party->send_to_party(packet);

    packet.clear();
    packet.put_header(0, 0x8f);
    m_stage.makeStagePacket(packet);
    packet.finalize(true);
    m_party->send_to_party(packet);

    _checkMemberReady();
    setTowerState((ENUM_TOWER_STATE)2);
    _onStartDeathTower();

    return 1;
}

// ---- CDungeonMgr ----
void CDeathTower::CDungeonMgr::reset()
{
    // ORIG W 0x82a7498：m_pDungeon = 0；m_endStage = 0。
    m_pDungeon = 0;
    m_endStage = 0;
}

bool CDeathTower::CDungeonMgr::initDungeonMgr(int dungeonIdx)
{
    // ORIG 0x084605a2：m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx)；
    m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (m_pDungeon == 0)
        return false;
    m_endStage = m_pDungeon->getDeathTowerMaxStage();
    return true;
}

CMap* CDeathTower::CDungeonMgr::getStageMap(unsigned int stage)
{
    // ORIG 0x84605ee：mapIdx = m_pDungeon->getDeathTowerMapIndex(stage)；
    // map = G_CDataManager()->find_map(mapIdx)；为空返回 0，否则返回 map。
    int mapIdx = m_pDungeon->getDeathTowerMapIndex(stage);
    CMap* map = (CMap*)G_CDataManager()->find_map(mapIdx);
    if (map == 0)
        return 0;
    return map;
}

int CDeathTower::CDungeonMgr::getEndStage()
{
    // ORIG W 0x8469ab4：返回 m_endStage（+0x04）。
    return m_endStage;
}

int CDeathTower::handleFinishLoading(CUser* user)
{
    int slot = m_party->GetMemberSlotNo(user);
    if (slot == -1 || !m_party->checkValidUser(slot))
    {
        return 0;
    }

    if (m_playData.checkEnterStartMap(slot))
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
    }
    m_playData.setMemberReady(slot, true);

    int memberCnt = m_party->get_member_count();
    if (m_playData.checkAllMemberReady(memberCnt))
    {
        CPacketHandler packet;
        packet.makeFinishLoading();    // ORIG：全员 ready 后广播加载完成包
        m_party->send_to_party(packet);
    }
    return 1;
}

int CDeathTower::handleDieUser(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            if (!m_playData.isMemberAlive(i))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                return 0;
            }
            m_playData.setMemberAlive(i, false);

            CPacketHandler packet;
            unsigned short uid = user->get_unique_id();
            packet.makeDieState(uid);    // ORIG：死亡状态包
            m_party->send_to_party(packet);

            if (_checkMemberDie())
            {
                _onFinishStage();
                _onPrepareFinishTower();
            }
            return 1;
        }
    }
    return 1;
}

int CDeathTower::handleDropItem(CUser* user, int a2, int a3, char a4, int a5, int a6)
{
    if (a6 < 0)
    {
        return 0;
    }

    map_item item;
    CPacketHandler packet;

    if (!_makeDropItem(user, a4, a5, a6, item))
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x11, a4);
        user->Send(packet);
        return 0;
    }

    int dropIdx = m_stage.dropItem(item);
    if (dropIdx == -1)
    {
        packet.makeDropItemError((ENUM_CMDPACKET)0x32, (ENUM_ERROR)0x16, a4);
        user->Send(packet);
        return 0;
    }

    unsigned short idx16 = (unsigned short)dropIdx;
    packet.makeDropItemNotify((unsigned short)user->get_unique_id(), a2, a3, idx16, item);
    m_party->send_to_party(packet);

    packet.clear();
    packet.makeDropItem(a4, a5, a6);
    user->Send(packet);
    return 1;
}

int CDeathTower::CStage::killMonster(int idx, map_monster& monster)
{
    // ORIG 0x8461708：find(idx)；命中则 monster = it->second、取
    // m_level(+0x10) 作返回值并 erase；未命中返回 0。
    // 注：ORIG movzbl 0x14(%pair) = pair+0x14 = map_monster+0x10 = m_level
    //（map_monster 为 pack(1)，second @ pair+4）。
    std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.find(idx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    monster = it->second;
    int roleType = (unsigned char)it->second.m_level;
    m_mapInfo.m_monsterMap.erase(it);
    return roleType;
}

bool CDeathTower::CStage::peekItem(int idx, map_item& item)
{
    // ORIG 0x8461534：m_mapInfo.m_itemMap.find(idx)；命中拷贝 0x54 字节返回 true。
    std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return false;
    }
    item = it->second;
    return true;
}

int CDeathTower::handleLeaveUser(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) && m_party->get_user(i) == user)
        {
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            m_playData.setMemberAlive(i, false);
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    onLeaveUser(user);

    bool allDead = false;
    if (m_party->get_member_count() > 0)
    {
        allDead = _checkMemberDie() != 0;
    }
    if (allDead)
    {
        _onFinishStage();
        _onPrepareFinishTower();
    }
    return 1;
}

int CDeathTower::handleMoveMap()
{
    try
    {
        if (_checkMemberDie())
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
            m_party->send_to_party(packet);
            return 0;
        }

        if (m_stage.getState() < 2)
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x13);
            m_party->send_to_party(packet);
            return 0;
        }

        unsigned int endStage = m_dungeonMgr.getEndStage();
        if (!m_stage.moveNextStage(endStage))
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
            m_party->send_to_party(packet);
            return 0;
        }

        CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
        if (!map)
        {
            throw 0x15;
        }

        m_stage.reset();
        if (!m_stage.consistMap(m_dungeonMgr, map, this))
        {
            throw 1;
        }

        PacketGuard packet;
        packet.put_header(0, 0x8f);
        m_stage.makeStagePacket(packet);
        packet.finalize(true);
        m_party->send_to_party(packet);

        _checkMemberReady();
        return 1;
    }
    catch (int e)
    {
        // ORIG：catch 后广播 makeCommonError(0x30, e)
        CPacketHandler packet;
        packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)e);
        m_party->send_to_party(packet);
        return 0;
    }
}

int CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
{
    map_item item;
    CPacketHandler packet;

    if (!m_stage.peekItem(idx, item))
    {
        if (a4 > 3)
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x51);
        else
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    int memberCnt = m_party->get_member_count();
    if (memberCnt < 1)
    {
        packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    // ORIG：map_item+0x12 = Inven_Item.m_addInfo（item idx）判定金钱物品。
    if (item.m_item.m_addInfo == 0)
    {
        // Money item
        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }
        int moneyA[4] = {0};
        int moneyB[4] = {0};
        _pickupItemMoney(user, memberCnt, item, moneyA, moneyB);
        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupMoney(moneyA, moneyB, m_party);
    }
    else
    {
        CItem* citem = G_CDataManager()->find_item(item.m_item.m_addInfo);
        if (!citem)
        {
            return 0;
        }

        int itemType = 0;
        if (citem->is_stackable())
        {
            if (citem->GetItemType() == 9)
            {
                itemType = 7;
            }
        }
        else
        {
            if (citem->IsCreatureItem())
            {
                itemType = 7;
            }
            else
            {
                if (citem->IsAvatarItem())
                {
                    itemType = 1;
                }
            }
        }

        char ipgNo[0x18] = {0};
        int nameBuf = 0;              // ORIG -0x4c：_pickupItem/appendPickupItem 的 char*
        CUser* targetUser = _pickupItem(
            user, (char*)&nameBuf, memberCnt, item, a3, a4, citem);
        if (!targetUser)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x16);
            user->Send(packet);
            return 0;
        }

        int slot = -1;
        Inven_Item* invenItem = &item.m_item;
        if (invenItem->isAvatarItemType())
        {
            int characNo = targetUser->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5, characNo, ipgNo);
            slot = targetUser->addAvatarItem(
                item.m_item.m_addInfo, 0, (char)item.m_item.m_fieldb,
                ipgNo, (eAvatarItemAddReason)0x10);
        }
        else
        {
            CInventory* inven = (CInventory*)targetUser->getCurCharacInvenW();
            slot = inven->insertItemIntoInventory(*invenItem, (eItemAddReason)4, true, true);
        }

        if (slot == -1)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)4);
            user->Send(packet);
            return 0;
        }

        if (citem->IsRandomOption())
        {
            statistc_proxy::add(
                targetUser, "log_random_option", "pick_cnt", 1, "%d,'%s'",
                targetUser->get_charac_10_level_section(),
                GlobalData::s_systemTime_.getCurDate());
        }

        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }

        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupItem((char*)&nameBuf, targetUser->get_unique_id(),
                                slot, itemType);
    }

    m_party->send_to_party(packet);
    return 1;
}

int CDeathTower::handleSelectEPLPCommand(CUser* user, int cmd)
{
    int slot = m_party->GetMemberSlotNo(user);
    if (slot == -1 || !m_party->checkValidUser(slot))
    {
        return 0;
    }

    m_party->SetEPLPState(3);
    int key = _genTimerKey((TIMER_MESSAGE)0x4e);
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4e, cmd, key, 0x4e);
    return 1;
}
int CDeathTower::handleStageCommand(CUser* user, unsigned char cmd)
{
    if (m_party->getManager() != user)
    {
        return 0;
    }
    if (cmd == 1)
    {
        if (!m_stage.setState((ENUM_STAGE_STATE)1))
        {
            return 1;
        }
        if (_checkMemberDie())
        {
            return 1;
        }
        _onStartStage();
    }
    else if (cmd == 2)
    {
        if (!m_stage.setState((ENUM_STAGE_STATE)2))
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int CDeathTower::CStage::getState() { return m_state; }
bool CDeathTower::CStage::checkClearStage()
{
    // ORIG 0x8461b3e：遍历 m_mapInfo.m_monsterMap，若 m_dropCnt(+0x2c)==100 且
    // m_field13(+0x13)==0 则未清台返回 false；全部通过返回 true。
    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); ++it)
    {
        map_monster& mob = it->second;
        if (mob.m_dropCnt == 100 && mob.m_field13 == 0)
        {
            return false;
        }
    }
    return true;
}


bool CDeathTower::CStage::setState(ENUM_STAGE_STATE state)
{
    if (state == 1)
    {
        if (m_state != 0)
        {
            return false;
        }
    }
    else if (state == 2)
    {
        if (m_state != 1)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    m_state = state;
    return true;
}

bool CDeathTower::CStage::moveNextStage(unsigned int endStage)
{
    // ORIG 0x84617da：checkClearStage() 且 m_currentStage < endStage 时
    // ++m_currentStage 返回 true，否则 false。
    if (checkClearStage() && (unsigned int)m_currentStage < endStage)
    {
        ++m_currentStage;
        return true;
    }
    return false;
}

bool CDeathTower::CStage::pickupItem(int idx, map_item& item)
{
    // ORIG 0x84615e4：peek 语义 + erase。
    std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return false;
    }
    item = it->second;
    m_mapInfo.m_itemMap.erase(it);
    return true;
}

int CDeathTower::CStage::dropItem(map_item& item)
{
    // ORIG 0x84616a8：idx = m_mapInfo.m_dropItemCnt++；item.m_itemIndex = idx；
    // MapInfo::Add_Item(m_mapInfo, item)（按值拷贝入参）；返回 idx。
    int idx = m_mapInfo.m_dropItemCnt++;
    item.m_itemIndex = idx;
    m_mapInfo.Add_Item(item);
    return idx;
}

bool CDeathTower::CStage::consistMap(const CDungeonMgr& dungeonMgr, const CMap* map,
                                     CDeathTower* tower)
{
    // ORIG 0x8460634：mobIdVec + apMap（拷贝自 aiList->getAPCount()）+
    // map_monster mob 局部，随后按三段填充：AI 布点 → AP 随机怪 → 脚本怪列表
    // → 特殊被动物品生成。tower 参数 ORIG 未使用。
    std::vector<unsigned int> mobIdVec;
    CAICharacterList* aiList = G_CDataManager()->m_aiCharacterList;
    std::map<int, int> apMap(aiList->getAPCount());

    int mobIdx = 0;
    map_monster mob;
    const std::vector<STAICharacterMapArrangeData>* aiChars = map->getAICharacters();

    // ---- 第一段：图上 AI 布点 ----
    if (!aiChars->empty())
    {
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 aiChars->begin(); it != aiChars->end(); ++it)
        {
            const STAICharacterMapArrangeData& data = *it;
            CAICharacter* aiChar = aiList->get(data.m_type);
            if (!aiChar)
                return false;

            unsigned int aiIdx = aiChar->getIdx();
            mobIdVec.push_back(aiIdx);

            int ap = aiChar->getAppearancePoint();
            if (ap != 0)
            {
                std::map<int, int>::iterator it2 = apMap.find(ap);
                if (it2 != apMap.end())
                    it2->second = std::max(it2->second - 1, 0);
            }

            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = mobIdx;
            mob.m_mobIndex = data.m_type;
            mob.m_instId = m_mapInfo.m_assignItemIdx;
            m_mapInfo.m_assignItemIdx++;
            mob.m_roleType = 5;
            mob.m_level = aiChar->getLevel();
            mob.m_dropCnt = data.m_camp;
            m_mapInfo.Add_Mob(mob);
            mobIdx++;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减；ORIG 0x8460940..0x8460d28）----
    int apRandomPoint = map->getAPCRandomPoint();
    if (apRandomPoint != 0)
    {
        mobIdx = 0x40;
        for (;;)
        {
            CAICharacter* aiChar = aiList->getByAP(
                static_cast<unsigned int>(get_rand_int(apRandomPoint) + 1));
            if (!aiChar)
                return false;

            std::vector<unsigned int>::iterator fi = std::find(
                mobIdVec.begin(), mobIdVec.end(), aiChar->getIdx());
            if (fi == mobIdVec.end())
            {
                // 未用过：统计 ≤ 剩余点数的可用 AP 总量，为 0 则跳出
                int total = 0;
                for (std::map<int, int>::const_iterator ci = apMap.begin();
                     ci != apMap.upper_bound(apRandomPoint); ++ci)
                    total += ci->second;
                if (total == 0)
                    break;
            }
            else
            {
                mobIdVec.push_back(aiChar->getIdx());
                int ap = aiChar->getAppearancePoint();
                std::map<int, int>::iterator it2 = apMap.find(ap);
                if (it2 != apMap.end())
                    it2->second = std::max(it2->second - 1, 0);
                apRandomPoint -= ap;

                memset(&mob, 0, sizeof(mob));
                mob.m_mobId = mobIdx;
                mob.m_mobIndex = aiChar->getIdx();
                mob.m_instId = m_mapInfo.m_assignItemIdx;
                m_mapInfo.m_assignItemIdx++;
                mob.m_roleType = 5;
                mob.m_level = aiChar->getLevel();
                mob.m_dropCnt = 100;   // ORIG 立即数
                m_mapInfo.Add_Mob(mob);
                mobIdx++;
            }
            if (aiList->getMinAP() > apRandomPoint)
                break;
        }
    }

    // ---- 第三段：脚本怪列表（ORIG 0x8460d28..0x8460f74）----
    mobIdx = 0;
    std::vector<int> namedVec;
    const std::list<_mapMonster> mons = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mons.begin();
         it != mons.end(); ++it)
    {
        const _mapMonster& src = *it;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = mobIdx;

        if (src.m_flag04 != 0)
        {
            int lv = dungeonMgr.getStandardLevel() + src.m_field08;
            if (lv > 0xc7)
                lv = 0xc8;
            else if (lv <= 0)
                lv = 1;
            mob.m_level = static_cast<unsigned char>(lv);
        }
        else
        {
            mob.m_level = static_cast<unsigned char>(src.m_field08);
        }

        if (src.m_mobId == -1)
            mob.m_mobIndex =
                G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                    mob.m_level);
        else
            mob.m_mobIndex = src.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        mob.m_field11 = static_cast<short>(
            (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_levelAdd)) |
             (static_cast<unsigned short>(
                static_cast<unsigned char>(src.m_field14)) << 8)));
        mob.m_roleType = static_cast<char>(
            static_cast<unsigned char>(src.m_roleType));
        mob.m_instId = m_mapInfo.m_assignItemIdx;
        m_mapInfo.m_assignItemIdx++;

        if (mob.m_roleType == 0 && !mob.isNamedMonster())
            namedVec.push_back(mob.m_instId);

        mob.m_field13 = static_cast<unsigned char>(src.m_champion);
        mob.m_field14 = static_cast<unsigned char>(src.m_posX);
        mob.m_dropCnt = src.m_hpPct;
        m_mapInfo.Add_Mob(mob);
        mobIdx++;
    }

    // ---- 第四段：特殊被动物品生成（ORIG 0x8460fdd..0x846133f）----
    std::vector<stDungeonAssignItem_t>* specialItems =
        dungeonMgr.getSpecialPassiveObjectItems();
    stGenerateRefData_t refData1;
    stGenerateRefData_t refData2;
    dungeonMgr.getObjectDropItems();   // ORIG 取出未用（忠实保留）
    const std::list<_mapItem> items = map->get_item_list();

    for (std::list<_mapItem>::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const _mapItem& src = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at(static_cast<unsigned int>(src.m_objectIdx));

        if (assign.m_assignItemIdx != -1)
        {
            *(char*)((char*)&refData1 + 0x38) =
                static_cast<char>(assign.m_assignItemIdx);
        }
        else
        {
            int stdLevel = dungeonMgr.getStandardLevel();
            if (stdLevel <= 0 || stdLevel > 0xc8)
                continue;
            *(char*)((char*)&refData1 + 0x38) =
                static_cast<char>(stdLevel);
        }
        // ORIG 0x84610fb..0x846111b：refData1+0x39 = src.m_field04 低字节
        //（+0x38/+0x39 即 m_rankBonusIndex 的低两字节）
        *(char*)((char*)&refData1 + 0x39) =
            static_cast<char>(src.m_field04);

        stGenerateResult_t result;
        WongWork::CItemGeneratorMgr* genMgr =
            G_CDataManager()->getItemGenerator();
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateRandomItem(refData1, result);
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refData2, result);

        for (std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
             d != result.m_dropItems.end(); ++d)
        {
            map_item drop;
            drop.m_count = 1;
            drop.m_itemIndex = m_mapInfo.m_dropItemCnt;
            m_mapInfo.m_dropItemCnt++;
            drop.m_standardLevel = static_cast<char>(src.m_field0c);
            drop.m_item = *d;
            m_mapInfo.Add_Item(drop);
        }
    }

    return true;
}

int CDeathTower::handleUseStackable(CUser* user, ENUM_ITEMSPACE itemSpace, unsigned short slot)
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace);
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    Inven_Item* item = inven->GetInvenRef(invenType, slot);
    if (!item)
    {
        return 0;
    }

    CDungeon* dungeon = m_dungeonMgr.getDungeon();
    if (dungeon->limitOfStackableItemInTower())
    {
        // ORIG：Inven_Item +0x02 即 m_addInfo（itemId）
        unsigned int itemId = (unsigned int)item->m_addInfo;
        if (!((itemId < 0x1964 || itemId > 6999) && itemId != 0x18))
        {
            return 1;
        }
        if (itemId < 0x19)
        {
            if (itemId != 10)
            {
                return 0;
            }
        }
        else if (itemId != 0x33 && itemId != 0x28978a)
        {
            return 0;
        }
    }
    return 1;
}

int CDeathTower::onTimer(TIMER_MESSAGE msg, unsigned int key)
{
    if (!_checkTimerKey(msg, key))
    {
        return 0;
    }

    if (getTowerState() == 0)
    {
        return 0;
    }

    switch (msg)
    {
    case 0x49:
    {
        int memberCnt = m_party->get_member_count();
        if (memberCnt == 0)
        {
            _destroy();
            return 1;
        }

        bool someoneLeft = false;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                if (!m_playData.isMemberReady(i))
                {
                    PacketGuard packet;
                    user->Send(packet);
                    handleLeaveUser(user);
                    someoneLeft = true;
                }
            }
        }

        memberCnt = m_party->get_member_count();
        if (!someoneLeft || memberCnt < 1)
        {
            if (memberCnt == 0)
            {
                _destroy();
            }
        }
        else
        {
            for (int i = 0; i < 4; ++i)
            {
                m_playData.setMemberReady(i, true);
            }
            PacketGuard packet;
            m_party->send_to_party(packet);
        }
        break;
    }
    case 0x4b:
    {
        if (getTowerState() < 3)
        {
            return 0;
        }
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                PacketGuard packet;
                Inven_Item* item3 = m_playData.getRewardItem(3);
                int cnt3 = m_playData.getRewardItemCount(3);
                Inven_Item* item2 = m_playData.getRewardItem(2);
                int cnt2 = m_playData.getRewardItemCount(2);
                Inven_Item* item1 = m_playData.getRewardItem(1);
                int cnt1 = m_playData.getRewardItemCount(1);
                Inven_Item* item0 = m_playData.getRewardItem(0);
                int cnt0 = m_playData.getRewardItemCount(0);
                int exp = m_playData.getRewardExp(i);
                Inven_Item* myItem = m_playData.getRewardItem(i);
                int myCnt = m_playData.getRewardItemCount(i);
                deathTowerCardStatistic(myCnt, user, myItem);
                user->Send(packet);
            }
        }
        setTowerState((ENUM_TOWER_STATE)4);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4c);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4c, 5, tkey, 0x4c);
        break;
    }
    case 0x4c:
    {
        if (getTowerState() < 4)
        {
            return 0;
        }
        bool allHaveItem = true;
        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                Inven_Item item;
                const CInventory* inven = (const CInventory*)user->getCurCharacInvenR();
                if (inven->GetInvenData(0x1057, item) < 0)
                {
                    allHaveItem = false;
                    break;
                }
            }
        }

        PacketGuard packet;
        m_party->send_to_party(packet);

        for (int i = 0; i < 4; ++i)
        {
            if (m_party->checkValidUser(i))
            {
                CUser* user = m_party->get_user(i);
                int rewardCnt = m_playData.getRewardItemCount(i);
                for (int j = 0; j < rewardCnt; ++j)
                {
                    Inven_Item* rewardItem = m_playData.getRewardItem(i);
                    // ORIG：rewardItem 为成员奖励 Inven_Item 数组；`+ j*0x3d + 2` = rewardItem[j].
                    // m_addInfo（Inven_Item +0x02，打包 0x3d 字节）。
                    int itemId = rewardItem[j].m_addInfo;
                    if (itemId != -1)
                    {
                        int slot = -1;
                        if (itemId == 0)
                        {
                            int addInfo = rewardItem->get_add_info();
                            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                            int gained = inven->gain_money(addInfo, (eMoneyAddReason)5, true, 0);
                            if (gained < addInfo)
                            {
                                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, addInfo, gained);
                            }
                            slot = 0;
                            CDungeon* dungeon = m_dungeonMgr.getDungeon();
                            int dungeonIdx = dungeon->get_index();
                            if (dungeonIdx == 11000)
                            {
                                GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)4, user, (unsigned int)gained);
                            }
                            else
                            {
                                GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)5, user, (unsigned int)gained);
                            }
                        }
                        else
                        {
                            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                            slot = inven->insertItemIntoInventory(*rewardItem, (eItemAddReason)0xb, true, true);
                        }

                        if (slot >= 0)
                        {
                            PacketGuard updatePacket;
                            user->Send(updatePacket);
                        }
                    }
                }

                int exp = m_playData.getRewardExp(i);
                user->calcHelpAbuseRatio(exp, false);
                int a = 0, b = 0;
                if (user->gain_exp_sp(exp, a, b, (CUser::eExpAddReason)2, 0, false))
                {
                    user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
        }

        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c, tkey, 0x4d);
        break;
    }
    case 0x4d:
    {
        if (m_party->GetEPLPState() != 2)
        {
            return 0;
        }
        if (m_party->GetSelectedEPLPCmd() == 3)
        {
            return 0;
        }
        _onFinishDeathTower();
        break;
    }
    case 0x4e:
    {
        if (m_party->GetEPLPState() != 3)
        {
            return 0;
        }
        _onFinishDeathTower();
        break;
    }
    case 0x4a:
    {
        if (getTowerState() < 2)
        {
            return 0;
        }
        if (m_party->get_member_count() < 1)
        {
            _destroy();
            return 1;
        }
        if (_checkMemberDie())
        {
            _beginTowerClearProcess(false);
        }
        else
        {
            int curStage = m_stage.getCurrentStage();
            int endStage = m_dungeonMgr.getEndStage();
            if (curStage == endStage)
            {
                _beginTowerClearProcess(true);
            }
        }
        break;
    }
    default:
        break;
    }

    return 1;
}

Inven_Item* CDeathTower::CPlayData::getRewardItem(int idx)
{
    // ORIG 0x8469c06：&m_rewards[idx].m_items[0]
    return m_rewards[idx].m_items;
}

int CDeathTower::CPlayData::getRewardItemCount(int idx)
{
    // ORIG 0x8469bec：m_rewards[idx].m_itemCnt
    return m_rewards[idx].m_itemCnt;
}

int CDeathTower::CPlayData::getRewardExp(int idx)
{
    // ORIG 0x8469c1e：m_rewardExp[idx]（+0x9b8 起）
    return m_rewardExp[idx];
}

int CDeathTower::CPlayData::getLastRoutedTurn()
{
    return m_lastRoutedTurn;
}

void CDeathTower::CPlayData::setLastRoutedTurn(int turn)
{
    // ORIG 0x8469b4c：m_lastRoutedTurn（+0x00）= turn
    m_lastRoutedTurn = turn;
}

char* CDeathTower::CPlayData::getStartMemberInfo()
{
    return m_startMemberInfo;
}

CDungeon* CDeathTower::CDungeonMgr::getDungeon() const
{
    return m_pDungeon;
}

void CDeathTower::CStage::resetCurrentStage()
{
    m_currentStage = 1;
}

void CDeathTower::CStage::makeStagePacket(PacketGuard& packet)
{
    packet.put_short(getCurrentStage());
    packet.put_int(get_rand_int(0));
    packet.put_short(m_mapInfo.m_mapIndex);
    packet.put_byte(m_mapInfo.m_monsterMap.size());

    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); ++it)
    {
        map_monster mob(it->second);   // ORIG 栈拷贝（C1ERKS）后逐字段写包
        packet.put_int(mob.m_mobId);
        packet.put_short(mob.m_instId);
        packet.put_int(mob.m_mobIndex);
        packet.put_byte(mob.m_level);
        packet.put_byte(mob.m_roleType);
        packet.put_byte(mob.m_field13);
        packet.put_byte(mob.m_field14);
    }
    // ORIG 0x84619ed 起：m_mapInfo.m_itemMap（+0x30）遍历。序列化格式与
    // CBossStage::SendMapInfoPacket 不同（此处 itemIndex 仅 short、m_addInfo
    // 为 int、endurance word），按 ORIG 逐字段转写。
    packet.put_byte((int)m_mapInfo.m_itemMap.size());
    for (std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.begin();
         it != m_mapInfo.m_itemMap.end(); ++it)
    {
        map_item item = it->second;        // ORIG 21 dword 栈拷贝
        packet.put_byte((int)(signed char)item.m_standardLevel);   // +0x01
        packet.put_short((int)(short)item.m_itemIndex);            // +0x04 低 2 字节
        packet.put_int(item.m_item.m_addInfo);                     // m_item+0x02
        packet.put_int(item.m_item.get_add_info());                // m_item+0x07
        packet.put_short((int)item.m_item.m_fieldb);               // m_item+0x0b（endurance）
        packet.put_byte((int)item.m_item.m_amp.getAbilityType());  // m_item+0x11
        packet.put_short((int)item.m_item.m_amp.getAbilityValue());
    }
}

void CDeathTower::CPlayData::reset()
{
    // ORIG 0x08461f18：m_playTime/m_lastPlayTime/m_lastRoutedTurn 清零；
    // 4 成员 alive/ready 清 0；m_rewards[4]（步长 0x268）逐个
    // stRewardItem_t::reset；m_rewardExp[4] 清 0；m_enterStartMap[4] 清 0。
    m_playTime = 0;
    m_lastPlayTime = 0;
    m_lastRoutedTurn = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberAlive[i] = 0;
        m_memberReady[i] = 0;
        m_rewards[i].reset();
        m_rewardExp[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
        m_enterStartMap[i] = 0;
}

// ORIG 0x08461cb4：m_itemCnt=0；10 个 Inven_Item reset 后 itemIdx(+0x02) 置 -1。
void CDeathTower::CPlayData::stRewardItem_t::reset()
{
    m_itemCnt = 0;
    for (int i = 0; i < 10; ++i)
    {
        m_items[i].reset();
        m_items[i].m_addInfo = -1;
    }
}

int CDeathTower::CPlayData::getStartMemberCnt()
{
    return m_startMemberCnt;
}

void CDeathTower::CPlayData::makeStartMemberInfo(CParty* party)
{
    // ORIG 0x08461d02：memset(m_startMemberInfo,0,0x5c)；遍历 4 个队伍槽，
    // 有效成员（get_user 非空且 checkValidUser==1）把名字 strcpy 到
    // m_startMemberInfo+count*0x17，level/job/growType 写到条目 +0x14/+0x15/+0x16；
    // 结束时 m_startMemberCnt = get_member_count()。
    memset(m_startMemberInfo, 0, sizeof(m_startMemberInfo));
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        CUserCharacInfo* info = (CUserCharacInfo*)party->get_user(i);
        bool invalid;
        if (info == 0)
            invalid = true;
        else
            invalid = party->checkValidUser(i) != 1;
        if (!invalid)
        {
            strcpy(m_startMemberInfo + count * 0x17,
                   info->getCurCharacName());
            m_startMemberInfo[count * 0x17 + 0x14] =
                (char)info->get_charac_level();
            m_startMemberInfo[count * 0x17 + 0x15] =
                (char)info->get_charac_job();
            m_startMemberInfo[count * 0x17 + 0x16] =
                (char)info->getCurCharacGrowType();
            ++count;
        }
    }
    m_startMemberCnt = party->get_member_count();
}

void CDeathTower::CPlayData::resetMemberReady()
{
    m_allReady = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberReady[i] = 0;
    }
}

void CDeathTower::CPlayData::setMemberAlive(int idx, bool alive)
{
    m_memberAlive[idx] = alive;
}

bool CDeathTower::CPlayData::isMemberAlive(int idx)
{
    return m_memberAlive[idx];
}

void CDeathTower::CPlayData::setMemberReady(int idx, bool ready)
{
    m_memberReady[idx] = ready;
}

bool CDeathTower::CPlayData::isMemberReady(int idx)
{
    return m_memberReady[idx];
}

bool CDeathTower::CPlayData::checkEnterStartMap(int idx)
{
    // ORIG 0x08461e4a：!m_enterStartMap[idx] 时置 true 并返回 true。
    if (!m_enterStartMap[idx])
    {
        m_enterStartMap[idx] = true;
        return true;
    }
    return false;
}

bool CDeathTower::CPlayData::checkAllMemberReady(int memberCnt)
{
    // ORIG 0x08461e7e：m_allReady 已置位直接 true；否则统计 m_memberReady，
    // 计满 memberCnt 时置 m_allReady=1 并返回 true（循环固定 4 槽）。
    if (m_allReady != 0)
        return true;
    int readyCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_memberReady[i])
        {
            ++readyCnt;
            if (readyCnt == memberCnt)
            {
                m_allReady = 1;
                return true;
            }
        }
    }
    return false;
}

void CDeathTower::CPlayData::addPlayTime(unsigned int time)
{
    m_lastPlayTime = m_playTime;
    m_playTime += time;
}

unsigned int CDeathTower::CPlayData::getPlayTime()
{
    return m_playTime;
}

unsigned int CDeathTower::CPlayData::getLastPlayTime()
{
    return m_lastPlayTime;
}

} // namespace WongWork
