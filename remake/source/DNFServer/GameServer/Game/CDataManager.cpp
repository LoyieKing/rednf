#include "LogManager.h"
// df_game_r CDataManager（全服数据仓库）还原（2026-08-16）。
// 函数形态/成员偏移以 ORIG 汇编为准；ST*Script 类实现属后续脚本批次，
// 本 TU 只声明其 C1/D1 并调用。

#include "CDataManager.h"
#include "CGameOption.h"  // CChattingEmoticonConfig 完整类型（ReCalc 需要）

#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "CSystemTime.h"
#include "DBMgr.h"
#include "MySQL.h"

// ===================== 外部依赖声明（其它 TU 提供） =====================

template <class T>
class GlobalInstance_CDM
{
public:
    static T* inst_ptr();
};

extern CSystemTime g_systemTimeInst;
extern int g_levelExpTable[256];

class CMapListImpl;
class CMonsterManagerImpl;

class DisJoint
{
public:
    ~DisJoint();
};

class CCharacList
{
public:
    ~CCharacList();
};

class CItemUpgrade_Separate
{
public:
    ~CItemUpgrade_Separate();
};

class CAutoMarketConditionsControl
{
public:
    ~CAutoMarketConditionsControl();
};

class CNPCDynamicInfoManager
{
public:
    ~CNPCDynamicInfoManager();
};

class CItemLimitEditionMgr
{
public:
    ~CItemLimitEditionMgr();
};

class StrikerScript
{
public:
    ~StrikerScript();
};

class CharacManageScript
{
public:
    ~CharacManageScript();
};

class BlueMarbleScriptManager
{
public:
    ~BlueMarbleScriptManager();
};

class SlangFilter
{
public:
    virtual void f0();
    virtual void Destroy();
};

class CSyncSlangFilter
{
public:
    static CSyncSlangFilter* GetInstance();
    bool HasSlang(std::string& name);
    bool HasSlangName(std::string& name);
};

class CreatureScript
{
public:
    static int ExpTable_[256];
};

class CChattingEmoticon
{
public:
    virtual ~CChattingEmoticon();
    virtual void f4();
    virtual bool isUsable(const CUser* user);
};

extern int* Get_instance_stHellParty_t();
extern int* Get_instance_groupOrder_map_pool();

extern int importSpPerLevelReferenceTable(const char* path, void* table);
extern int importPvPReferenceTable(const char* path, void* table);

class RefPvpGrade
{
public:
    static void Clear(void* table);
    static void SetPvpGrade(void* table, int grade, void* data);
    static void set(void* table, void* refTable);
    static void SetLastGrade(void* table, int grade);
};

extern void* GlobalData_ref_pvp_grade;

// ===================== 本 TU 局部依赖类/全局定义（后续批次替换） =====================

// 全局数据（ORIG 属于其它 TU；当前先在此提供可链接定义）
CSystemTime g_systemTimeInst;  // TODO(后续批次)
int g_levelExpTable[256] = {0};  // TODO(后续批次)

// 析构 stub（destroy() 的 delete 目标）
DisJoint::~DisJoint() {}  // TODO(后续批次)
CCharacList::~CCharacList() {}  // TODO(后续批次)
CItemUpgrade_Separate::~CItemUpgrade_Separate() {}  // TODO(后续批次)
CAutoMarketConditionsControl::~CAutoMarketConditionsControl() {}  // TODO(后续批次)
CNPCDynamicInfoManager::~CNPCDynamicInfoManager() {}  // TODO(后续批次)
StrikerScript::~StrikerScript() {}  // TODO(后续批次)
CharacManageScript::~CharacManageScript() {}  // TODO(后续批次)
BlueMarbleScriptManager::~BlueMarbleScriptManager() {}  // TODO(后续批次)

// SlangFilter 虚析构/销毁（vtable 由本 TU 发出；ORIG destroy 经 vtable+4 调 Destroy）
void SlangFilter::f0() {}  // TODO(后续批次)
void SlangFilter::Destroy() {}  // TODO(后续批次)

int CreatureScript::ExpTable_[256] = {0};  // TODO(后续批次)

int* Get_instance_stHellParty_t()
{
    static int v = 0;  // TODO(后续批次)
    return &v;
}

// ===================== 查询 =====================

CItem* CDataManager::find_item(int idx) const
{
    if (idx == 0 || idx == 1 || idx == 2)
    {
        return 0;
    }
    return m_itemList->find_item(idx);
}

CItem* CDataManager::find_item(const char* name) const
{
    return m_itemList->find_item(name);
}

CItem* CDataManager::FindGoods(int goodsIdx) const
{
    return (CItem*)m_itemList->FindGoods(goodsIdx);
}

CSkill* CDataManager::find_skill(int skillId, int level) const
{
    return m_skillList->find_skill(skillId, level);
}

Quest* CDataManager::find_quest(int idx) const
{
    return m_questList->find_quest(idx);
}

CDungeon* CDataManager::find_dungeon(int idx) const
{
    return m_dungeonList->find_dungeon(idx);
}

void* CDataManager::find_map(int idx) const
{
    return m_mapList->find_map(idx);
}

void* CDataManager::find_monster(int idx) const
{
    return m_monsterManager->find_monster(idx);
}

void* CDataManager::find_mission(int idx) const
{
    return (void*)m_pvpChannel.m_pvpMissionSystem->find_mission(idx);
}

int CDataManager::find_npc(unsigned int idx) const
{
    return m_npcScriptList->find(idx);
}

bool CDataManager::find_lottery_use_cost(unsigned int money) const
{
    return m_lotteryNeedMoney.isNeedMoney(money);
}

// ===================== 等级/经验 =====================

int CDataManager::set_level_exp()
{
    for (int i = 0; i <= 199; ++i)
    {
        m_expTable[i] = g_levelExpTable[i];
    }
    return 1;
}

int CDataManager::get_level_exp(int level) const
{
    if (level < 2 || level > 200)
    {
        return 0;
    }
    return m_expTable[level - 2];
}

int CDataManager::get_level_section_exp(int level) const
{
    if (level < 2 || level > 200)
    {
        return 0;
    }
    return m_expTable[level - 1] - m_expTable[level - 2];
}

int CDataManager::get_level_exp_percent(int level, int percent) const
{
    int section = get_level_section_exp(level);
    return section * percent / 100;
}

int CDataManager::get_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 99; level += 1)
    {
        if (get_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
}

int CDataManager::get_creature_level_exp(int level) const
{
    if (level < 2 || level > 99)
    {
        return 0;
    }
    return CreatureScript::ExpTable_[level - 2];
}

int CDataManager::get_creature_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 98; level += 1)
    {
        if (get_creature_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
}

unsigned int CDataManager::GetSpAtLevelUp(int level) const
{
    if (level < 1 || level > 95)
    {
        return 0;
    }
    // ORIG 0x8360cb8：`*(int*)(this + (level+0x36a2)*4 + 8)` = SP-at-level-up 表
    // （位于本类尾部未建模区 this+0xda94+4*(level-1)）。无对应具名成员，保留
    // 地址形式，用 ORIG 偏移直译（原写法错误地借 &m_monsterManager(+0x08) 伪装）。
    return *(int*)((char*)this + (level + 0x36a2) * 4 + 8);
}

unsigned int CDataManager::GetMoneyLimitPerLevel(int level, const char* serverGroup) const
{
    if (level <= 0 || level > 200)
    {
        LogManager::logFormat(1, "data_manager.cpp",
                              "CDataManager::GetMoneyLimitPerLevel", 0x2705,
                              "Out of range level (%d)", level);
        level = std::max(1, std::min(200, level));
    }
    if (serverGroup && strcmp(serverGroup, "0") == 0)
    {
        return 0x7fffffff;
    }
    return m_moneyLimit[level];
}

void CDataManager::get_mob_reward(int level, unsigned int& out) const
{
    if (level <= 0 || level > 199)
    {
        out = 0;
        return;
    }
    out = m_mobReward[level];
}

int CDataManager::set_mob_reward()
{
    short level;
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect level, exp from monster_reward_ref");
    ok = db->exec(true);
    if (ok != 1)
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows < 1)
    {
        return 0;
    }
    int i = 0;
    while (i < rows)
    {
        ok = db->fetch();
        if (ok != 1)
        {
            break;
        }
        ok = db->get_short(0, level);
        if (ok != 1)
        {
            return 0;
        }
        ok = db->get_short(1, m_mobReward[level]);
        if (ok != 1)
        {
            return 0;
        }
        i = i + 1;
    }
    m_mobReward[0] = 0;
    memcpy(m_mobRewardBackup, m_mobReward, 0x324);
    return 1;
}

float CDataManager::BaseExpPenalty(int levelA, int levelB)
{
    int diff = levelB - levelA;
    float result;
    if (diff < -6)
    {
        result = 0.05f;
    }
    else if (diff < -5)
    {
        result = 0.2f;
    }
    else if (diff == -5)
    {
        result = 0.5f;
    }
    else if (diff == -4)
    {
        result = 0.75f;
    }
    else if (diff == -3)
    {
        result = 1.0f;
    }
    else if (diff == -2)
    {
        result = 1.0f;
    }
    else if (diff == -1)
    {
        result = 1.0f;
    }
    else if (diff == 0)
    {
        result = 1.0f;
    }
    else if (diff == 1)
    {
        result = 1.12f;
    }
    else if (diff == 2)
    {
        result = 1.12f;
    }
    else if (diff == 3)
    {
        result = 1.12f;
    }
    else if (diff == 4)
    {
        result = 1.0f;
    }
    else if (diff == 5)
    {
        result = 1.0f;
    }
    else if (diff == 6)
    {
        result = 0.75f;
    }
    else if (diff == 7)
    {
        result = 0.7f;
    }
    else if (diff == 8)
    {
        result = 0.6f;
    }
    else if (diff == 9)
    {
        result = 0.5f;
    }
    else if (9 < diff)
    {
        result = 0.05f;
    }
    return result;
}

void CDataManager::SetExpRate(float rate)
{
    if (m_expRate != rate)
    {
        memcpy(m_mobReward, m_mobRewardBackup, 0x324);
        m_expRate = rate;
        for (int i = 0; i <= 200; ++i)
        {
            m_mobReward[i] =
                (int)((long long)((double)(unsigned int)m_mobReward[i] * rate));
        }
    }
}

void CDataManager::SetItemDropRate(float rate)
{
    if (m_dropRate != rate)
    {
        float old = m_dropRate;
        m_dropRate = rate;
        m_itemGeneratorMgr->setDropRatio(rate);
        for (unsigned int i = 0; i < m_serverParameter.m_genRefs1.size(); ++i)
        {
            m_serverParameter.m_genRefs1[i].m_dropRate =
                (int)((float)m_serverParameter.m_genRefs1[i].m_dropRate *
                      (rate / old));
        }
        for (unsigned int i = 0; i < m_serverParameter.m_genRefs0.size(); ++i)
        {
            m_serverParameter.m_genRefs0[i].m_dropRate =
                (int)((float)m_serverParameter.m_genRefs0[i].m_dropRate *
                      (rate / old));
        }
    }
}

// ===================== PvP/排行 =====================

unsigned short CDataManager::getRankIndex(int idx)
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[0];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[1];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[2];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[3];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[4];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[5];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[6];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[7];
    return 0;
}


unsigned int CDataManager::getNewRankIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 0;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return 5;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return 6;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return 7;
    return 8;
}


int CDataManager::getRankBonusIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 0;
    return -1;
}


int CDataManager::calRankPoint(int a, int b)
{
    return a + b;
}

// ===================== 任务 =====================

std::vector<int>* CDataManager::getDailyTrainingQuest(int idx)
{
    return &m_dailyTrainingQuest[idx - 1];
}

int CDataManager::isThereDailyTrainingQuestList(int idx, int questIdx)
{
    if (idx <= 0 || idx > 0x46)
    {
        return 0;
    }
    std::vector<int>* list = getDailyTrainingQuest(idx);
    for (std::vector<int>::iterator it = list->begin(); it != list->end(); ++it)
    {
        if (*it == questIdx)
        {
            return 1;
        }
    }
    return 0;
}

int CDataManager::get_WithinMissionIndex(int idx) const
{
    return m_pvpChannel.m_pvpMissionSystem->get_WithinMissionIndex(idx);
}

std::list<CMission*>* CDataManager::get_kind_mission_list(int idx) const
{
    return &m_pvpChannel.m_pvpMissionSystem->get_kind_mission_list(idx);
}

void CDataManager::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out) const
{
    m_pvpChannel.m_pvpMissionSystem->get_New_DailyMission(out);
}

int CDataManager::get_BaseMissionExp_byRank(unsigned short rank) const
{
    return m_pvpChannel.m_pvpMissionSystem->get_BaseMissionExp_byRank(rank);
}

std::bitset<32> CDataManager::get_daily_mission_kind() const
{
    return m_pvpChannel.m_pvpMissionSystem->get_daily_mission_kind();
}

// ===================== 事件/道具 =====================

int CDataManager::getDecoEventItemIdx()
{
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return *it;
    }
    return 0xffffffff;
}

bool CDataManager::getVaildEventItem(int idx)
{
    (void)idx;
    std::vector<int>::iterator it = m_seriaRoomDeco.m_eventItemIdx.begin();
    if (it != m_seriaRoomDeco.m_eventItemIdx.end())
    {
        return true;
    }
    return false;
}

int CDataManager::getMatchEventPoint(int idx)
{
    std::map<int, int>::iterator it = m_seriaRoomDeco.m_matchEventMap.find(idx);
    if (it != m_seriaRoomDeco.m_matchEventMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getMaxEventItemCount()
{
    return m_seriaRoomDeco.m_maxEventItemCount;
}

int CDataManager::getFatigue_contidion()
{
    return m_seriaRoomDeco.m_fatigueCondition;
}

int CDataManager::getRewardItem_MailExpireDate()
{
    return m_seriaRoomDeco.m_rewardMailExpireDate;
}

int CDataManager::get_GiftGrowthWeaponBox()
{
    return m_growthWeapon.m_giftBox;
}

int CDataManager::get_GrowthWeaponMaterialItem()
{
    return m_growthWeapon.m_materialItem;
}

unsigned short CDataManager::get_GrowthWeaponMaterialMax()
{
    return m_growthWeapon.m_materialMax;
}

unsigned short CDataManager::get_GrowthWeaponEventBubbleMax()
{
    return m_growthWeapon.m_eventBubbleMax;
}

unsigned short CDataManager::get_GrowthWeaponEventSandMax()
{
    return m_growthWeapon.m_eventSandMax;
}

int CDataManager::get_GrowthWeaponEventReward(int idx) const
{
    std::map<int, int>::const_iterator it = m_growthWeapon.m_rewardMap.find(idx);
    if (it != m_growthWeapon.m_rewardMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getChangeAbleCreatureLev()
{
    return m_createCreatureGift.m_changeAbleCreatureLev;
}

int CDataManager::getClientChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_infinityChangeMap.find(idx);
    if (it != m_createCreatureGift.m_infinityChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}

int CDataManager::getInfinityChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_clientChangeMap.find(idx);
    if (it != m_createCreatureGift.m_clientChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}

int CDataManager::getGiveGrowCreatureEventJobCount(int level)
{
    if (m_createCreatureGift.m_eventJobMap.empty())
    {
        return 0;
    }
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator it =
        m_createCreatureGift.m_eventJobMap.begin();
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator e =
        m_createCreatureGift.m_eventJobMap.end();
    int count = 0;
    while (it != e)
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            count += 1;
        }
        it++;
    }
    return count;
}

bool CDataManager::isAvailableGiveGrowCreatureEventJob(int job, int level)
{
    std::map<int, CharacterCreateCreatureGiftScript::eventJob>::iterator
        it = m_createCreatureGift.m_eventJobMap.find(job);
    if (it != m_createCreatureGift.m_eventJobMap.end())
    {
        if (it->second.m_minLevel < level && level < it->second.m_maxLevel)
        {
            return true;
        }
    }
    return false;
}

int CDataManager::getSeriaRoomAniDecoItem(int idx)
{
    if (m_seriaAniDeco.m_itemMap.empty())
    {
        return 0;
    }
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        return it->second;
    }
    return 0;
}

int CDataManager::getSeriaRoomAniDecoRewardItem(int idx)
{
    if (m_seriaAniDeco.m_rewardList.empty())
    {
        return 0;
    }
    std::vector<seriaRoomDecoRewardInfo>::iterator it =
        m_seriaAniDeco.m_rewardList.begin();
    std::vector<seriaRoomDecoRewardInfo>::iterator e =
        m_seriaAniDeco.m_rewardList.end();
    while (it != e)
    {
        if (it->m_itemIdx == idx)
        {
            return it->m_reward;
        }
        it++;
    }
    return 0;
}

unsigned int CDataManager::maxAniDecoItem()
{
    return (unsigned int)m_seriaAniDeco.m_itemMap.size();
}

bool CDataManager::invalidAniDecoItem(int idx, int value)
{
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        if (it->second == value)
        {
            return true;
        }
    }
    return false;
}

unsigned int CDataManager::getChoiceItem() const
{
    return m_choiceItem.m_choice;
}

void CDataManager::setChoiceItem(bool flag, unsigned int idx)
{
    m_choiceItem.m_checked = flag;
    m_choiceItem.m_choice = idx;
}

bool CDataManager::checkChoiceItem() const
{
    return m_choiceItem.m_checked;
}

void CDataManager::set_lottery_use_cost(unsigned int money, unsigned int cost)
{
    m_lotteryNeedMoney.add_lottery_needMoney(money, cost);
}

void CDataManager::set_original_dimensionInout(int idx, char value)
{
    if (idx < 6)
    {
        m_originalDimensionInout[idx] = value;
    }
    else
    {
    }
}

void CDataManager::reset_dimensionInout()
{
    memcpy(m_dimensionInout, m_originalDimensionInout, 6);
}

// ===================== 情感/过滤 =====================

// ---- ChattingEmoticonBase 基类虚函数（ORIG 0x80e5f46/0x80e5dde）----
bool ChattingEmoticonBase::isCommonEmoticon(const CUser& user) const
{
    // ORIG 0x80e5f46：vtable+0x08（isUsable）转发。
    return isUsable(user);
}

void ChattingEmoticonBase::AddPremiumIndex(int index)
{
    // ORIG 0x80e5dde：特定 index 映射到 premium 槽位集合。
    if (index < 0x2898bf)
        return;
    if (index <= 0x2898c0)
    {
        m_set.insert(0x53);
        return;
    }
    if (index > 0x2898c2)
        return;
    m_set.insert(0x57);
}

// ---- CChattingEmoticonList（ORIG find_emoticon 0x80e581c / ReCalc 0x80e5880）----
ChattingEmoticonBase* CChattingEmoticonList::find_emoticon(int idx)
{
    std::map<int, ChattingEmoticonBase*>::iterator it = m_map.find(idx);
    if (it == m_map.end())
        return 0;
    return it->second;
}

void CChattingEmoticonList::ReCalc(CChattingEmoticonConfig& config, const CUser& user)
{
    // ORIG 0x80e5880：两轮扫描。
    // 1) 遍历 config 行（0x17+1 行），对每行 +0x14 的 emoticon idx 查表；
    //    命中且 isCommonEmoticon(user) 为真的行收集到 rows，idx 记录到 locked。
    // 2) config.clear() 后，把 rows 按各 emoticon 的 [GetStartArrayIndex,
    //    GetEndArrayIndex] 槽位范围填回 config 空槽（+0x14 == 0）；
    //    最后遍历 m_map，把未锁定且 common 的 emoticon idx 填入空槽。
    std::vector<CChattingEmoticonConfigRow> rows;
    std::set<int> locked;

    for (int i = 0; i <= 0x17; ++i)
    {
        const unsigned short idx =
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&config) + i * 0x16 + 0x14);
        ChattingEmoticonBase* e = find_emoticon(idx);
        if (e == 0)
            continue;
        if (e->isCommonEmoticon(user) == false)
            continue;
        rows.push_back(*reinterpret_cast<const CChattingEmoticonConfigRow*>(
            reinterpret_cast<const char*>(&config) + i * 0x16));
        locked.insert(idx);
    }

    config.clear();

    for (int j = 0; j < static_cast<int>(rows.size()); ++j)
    {
        const unsigned short idx =
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&rows[j]) + 0x14);
        ChattingEmoticonBase* e = find_emoticon(idx);
        if (e == 0)
            continue;
        int k = e->GetStartArrayIndex();
        while (e->GetEndArrayIndex() >= k)
        {
            unsigned short* slot =
                reinterpret_cast<unsigned short*>(
                    reinterpret_cast<char*>(&config) + k * 0x16 + 0x14);
            if (*slot == 0)
            {
                memcpy(reinterpret_cast<char*>(&config) + k * 0x16, &rows[j], 0x16);
                break;
            }
            ++k;
        }
    }

    for (std::map<int, ChattingEmoticonBase*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        ChattingEmoticonBase* e = it->second;
        const int idx = e->GetIndex();
        if (locked.find(idx) != locked.end())
            continue;
        if (e->isCommonEmoticon(user) == false)
            continue;
        int k = e->GetStartArrayIndex();
        while (e->GetEndArrayIndex() >= k)
        {
            unsigned short* slot =
                reinterpret_cast<unsigned short*>(
                    reinterpret_cast<char*>(&config) + k * 0x16 + 0x14);
            if (*slot == 0)
            {
                *slot = static_cast<unsigned short>(idx);
                break;
            }
            ++k;
        }
    }
}

void CDataManager::ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user)
{
    m_emoticonList->ReCalc(config, user);
}

int CDataManager::GetUpgradeRevisionPvPChannel()
{
    return m_pvpChannel.m_upgradeRevision;
}

float CDataManager::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    return m_itemUpgrade->getUpgradeItemRepairCostRate(idx, flag);
}

bool CDataManager::checkValidShopItem(unsigned int itemIdx, unsigned long count)
{
    return m_itemShop->checkValidShopItem(itemIdx, count);
}

bool CDataManager::checkLimitNpcBuyItemList(unsigned int itemIdx, unsigned long count)
{
    return m_itemShop->checkLimitNpcBuyItemList(itemIdx, count);
}

bool CDataManager::hasPreventString(const char* name)
{
    if (getCharacNameFilterPass())
    {
        setCharacNameFilterPass(false);
        return false;
    }
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlang(str);
}

bool CDataManager::hasPreventStringName(const char* name)
{
    std::string str(name);
    return CSyncSlangFilter::GetInstance()->HasSlangName(str);
}

bool CDataManager::getCharacNameFilterPass() const
{
    return m_characNameFilterPass;
}

void CDataManager::setCharacNameFilterPass(bool flag)
{
    m_characNameFilterPass = flag;
}

// ===================== 其它 =====================

const char* CDataManager::GetScriptPacks() const
{
    return m_scriptPacks.c_str();
}

void CDataManager::SetScriptPacks(const char* path)
{
    if (path == 0)
    {
        m_scriptPacks = "Script.pvf";
    }
    else
    {
        m_scriptPacks = path;
    }
}

void* CDataManager::GetItemMakingSkill() const
{
    return (void*)&m_itemMakingSkill;
}

void* CDataManager::GetImageCommnicationData() const
{
    return (void*)&m_imageCommData;
}

std::set<int>* CDataManager::getUltimateDungeonList() const
{
    return (std::set<int>*)&m_ultimateDungeonList;
}

int* CDataManager::getBlueMarbleScript() const
{
    return (int*)m_choiceItem.m_blueMarbleScriptMgr;
}

void CDataManager::getDungeonIndexList() const
{
    m_dungeonList->getDungeonIndexList();
}

void* CDataManager::get_hellparty_script_values()
{
    return Get_instance_stHellParty_t();
}

void CDataManager::testActionType()
{
    char buf[0x90118];
    (void)buf;
}

void CDataManager::testBingo()
{
    char buf[0x18];
    (void)buf;
}

bool CDataManager::_isOneByteCharacter(unsigned char c)
{
    (void)c;
    return true;
}

int CDataManager::IsDBLogItem(unsigned int itemIdx)
{
    int cur = g_systemTimeInst.getCurSec();
    for (std::vector<stDBLogItem>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx && cur <= it->m_time)
        {
            return it->m_value;
        }
    }
    return 0xffffffff;
}

// ===================== 构造/析构 =====================

CDataManager::CDataManager()
{
    m_disJoint = 0;
    m_characList = 0;
    m_questList = 0;
    m_itemList = 0;
    m_skillList = 0;
    m_monsterManager = 0;
    m_mapList = 0;
    m_dungeonList = 0;
    m_itemUpgrade = 0;
    m_itemUpgradeSeparate = 0;
    m_emoticonList = 0;
    m_itemGeneratorMgr = 0;
    m_invalidChar.m_slangFilter = 0;
    m_aiCharacterList = 0;
    m_autoMarket = 0;
    m_npcScriptList = 0;
    m_npcDynamicInfo = 0;
    m_npcRelation.m_itemLimitEditionMgr = 0;
    m_expRate = 1.0f;
    m_dropRate = 1.0f;
    m_altarConfig.m_strikerScript = 0;
    m_altarConfig.m_characManageScript = 0;
    m_choiceItem.m_blueMarbleScriptMgr = 0;
}

CDataManager::~CDataManager()
{
    destroy();
}

void CDataManager::destroy()
{
    if (m_disJoint)
    {
        delete m_disJoint;
        m_disJoint = 0;
    }
    if (m_characList)
    {
        delete m_characList;
        m_characList = 0;
    }
    if (m_questList)
    {
        delete m_questList;
        m_questList = 0;
    }
    if (m_itemList)
    {
        delete m_itemList;
        m_itemList = 0;
    }
    if (m_skillList)
    {
        delete m_skillList;
        m_skillList = 0;
    }
    if (m_monsterManager)
    {
        delete m_monsterManager;
        m_monsterManager = 0;
    }
    if (m_mapList)
    {
        delete m_mapList;
        m_mapList = 0;
    }
    if (m_dungeonList)
    {
        delete m_dungeonList;
        m_dungeonList = 0;
    }
    if (m_itemUpgrade)
    {
        delete m_itemUpgrade;
        m_itemUpgrade = 0;
    }
    if (m_itemUpgradeSeparate)
    {
        delete m_itemUpgradeSeparate;
        m_itemUpgradeSeparate = 0;
    }
    if (m_emoticonList)
    {
        delete m_emoticonList;
        m_emoticonList = 0;
    }
    if (m_itemGeneratorMgr)
    {
        delete m_itemGeneratorMgr;
        m_itemGeneratorMgr = 0;
    }
    if (m_invalidChar.m_slangFilter)
    {
        ((SlangFilter*)m_invalidChar.m_slangFilter)->Destroy();
        m_invalidChar.m_slangFilter = 0;
    }
    if (m_aiCharacterList)
    {
        delete m_aiCharacterList;
        m_aiCharacterList = 0;
    }
    if (m_autoMarket)
    {
        delete m_autoMarket;
        m_autoMarket = 0;
    }
    if (m_npcScriptList)
    {
        delete m_npcScriptList;
        m_npcScriptList = 0;
    }
    if (m_npcDynamicInfo)
    {
        delete m_npcDynamicInfo;
        m_npcDynamicInfo = 0;
    }
    if (m_npcRelation.m_itemLimitEditionMgr)
    {
        delete (CItemLimitEditionMgr*)m_npcRelation.m_itemLimitEditionMgr;
        m_npcRelation.m_itemLimitEditionMgr = 0;
    }
    if (m_altarConfig.m_strikerScript)
    {
        delete (StrikerScript*)m_altarConfig.m_strikerScript;
        m_altarConfig.m_strikerScript = 0;
    }
    if (m_altarConfig.m_characManageScript)
    {
        delete (CharacManageScript*)m_altarConfig.m_characManageScript;
        m_altarConfig.m_characManageScript = 0;
    }
    if (m_choiceItem.m_blueMarbleScriptMgr)
    {
        ((BlueMarbleScriptManager*)m_choiceItem.m_blueMarbleScriptMgr)
            ->~BlueMarbleScriptManager();
        m_choiceItem.m_blueMarbleScriptMgr = 0;
    }
}

// ===================== 单例 =====================

template <class T>
T* GlobalInstance_CDM<T>::inst_ptr()
{
    static T inst;  // TODO(后续批次)
    return &inst;
}

CDataManager* G_CDataManager()
{
    return GlobalInstance_CDM<CDataManager>::inst_ptr();
}

// ---- CDataManager 成员类析构实现（G2/G5 批次细化）----
// CMapList/CMonsterManager/CDungeonList/CSkillList/QuestList 实现在
// CListTables.cpp（G2 第七批）；CChattingEmoticonList 由各自 TU 提供。
CChattingEmoticonList::~CChattingEmoticonList() {}

int CDataManager::get_limit_inout_count(int type)
{
    return (unsigned char)m_padAA80[type];
}

// ============================================================================
// 迁移自 GameStubs.cpp 的 CDataManager 桩方法（ORIG 符号/偏移已核对）
// ============================================================================

void CDataManager::set_AuctionRegFreeCeraItem(unsigned int idx)
{
    // ORIG 0x08513fbc W：m_choiceSet(+0xb4ac).insert(idx)
    m_choiceSet.insert(idx);
}

void CDataManager::set_lottery_use_cost(unsigned int cost)
{
    // 重建便捷 1 参版本（CStackableItem.cpp 调用，script.m_field0）。
    // ORIG 无此 1 参符号；2 参版本见本 TU 上方，语义为 add_lottery_needMoney。
    m_lotteryNeedMoney.add_lottery_needMoney(0, cost);
}

void* CDataManager::GetExpertJobScript(int job)
{
    // ORIG 0x0822b5f2 W：(CExpertJobList*)(this+0x5090)->get(job)。+0x5090 即
    // m_regeneration.m_expertJobList（RegenerationROI +0x50）。CExpertJobList 仅
    // 有前置声明，先返回其成员地址，待类型完整后补 get() 调用。
    return (void*)&m_regeneration.m_expertJobList;

}

void* CDataManager::GetExpertJobEtcScript()
{
    // ORIG 0x0849f516 W：((CExpertJobList*)(this+0x5090))->get_etc()，get_etc() 原子：
    //   return this + 0x18（CExpertJobList 内 ·etc 脚本位于 +0x18）。
    // 与 GetExpertJobScript 保持一致：以 &m_regeneration.m_expertJobList（+0x5090）为基址，
    // get_etc 返回基址 +0x18。CUser 将其作 std::map<uchar,short>（等级→所需角色等级）。
    return (char*)&m_regeneration.m_expertJobList + 0x18;
}

void* CDataManager::GetChannelScript() const
{
    // ORIG 0x0814a6f8 W：return &m_channelScript（+0xb3f4）
    return const_cast<channel_script_t*>(&m_channelScript);
}

void* CDataManager::getBlueMarbleScript()
{
    // ORIG 0x08365eea T（非 const void*）。最小化迁移，后续细化。
    return 0;
}

void* CDataManager::get_event_script_mng()
{
    // ORIG 0x08110b62 W：return (void*)m_liveServerInfo.m_eventScriptMng
    return reinterpret_cast<void*>(m_liveServerInfo.m_eventScriptMng);
}


int CDataManager::get_dimensionInout(int idx)
{
    // ORIG 0x0822b612 W：idx 越界(0..5)返回 0，否则返回 m_dimensionInout(idx)
    if (idx < 0 || idx > 5)
        return 0;
    return m_dimensionInout[idx];
}

void CDataManager::GetPvPChannelGrade()
{
    // ORIG 0x08357e9e T。最小化迁移，后续细化。
}

int CDataManager::GetMaxGradePvPChannel()
{
    // ORIG 0x0822b65a W：返回 m_pvpChannel(+0xb430) 内 +0x8 字段。
    return m_pvpChannel.m_maxGrade;

}

const char* CDataManager::GetCeraShopGoodsName()
{
    // ORIG 0x08513fe6 W：return m_scriptPackPath.c_str()（+0x8cb8）
    return m_scriptPackPath.c_str();
}

const char* CDataManager::SuddenShopFileName1()
{
    // ORIG 0x08513ffe W：return m_packName2.c_str()（+0xa8c4）
    return m_packName2.c_str();
}

const char* CDataManager::SuddenShopFileName2()
{
    // ORIG 0x08514016 W：return m_packName3.c_str()（+0xa8c8）
    return m_packName3.c_str();
}

const char* CDataManager::SuddenShopFileName3()
{
    // ORIG 0x0851402e W：return m_packName4.c_str()（+0xa8cc）
    return m_packName4.c_str();
}

int CDataManager::VerifyMap(const CMap& map, int idx) const
{
    // ORIG 0x083620ee T。最小化迁移（返回 1 表示校验通过），后续细化。
    return 1;
}

void* CDataManager::get_dungeon(int idx)
{
    // ORIG 0x086d18c8 W：return find_dungeon(idx)
    return find_dungeon(idx);
}

int CDataManager::reselectDailyTrainingQuest()
{
    // ORIG 0x08363ce0 T。最小化迁移，后续细化。
    return 0;
}
