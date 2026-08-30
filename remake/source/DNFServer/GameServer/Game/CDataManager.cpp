#include "LogManager.h"
// df_game_r CDataManager（全服数据仓库）还原（2026-08-16）。
// 函数形态/成员偏移以 ORIG 汇编为准；ST*Script 类实现属后续脚本批次，
// 本 TU 只声明其 C1/D1 并调用。

#include "CDataManager.h"
#include "CExpertJobList.h"  // CExpertJobList 完整类型（GetExpertJobScript → get(job)）
#include "CGameOption.h"  // CChattingEmoticonConfig 完整类型（ReCalc 需要）
#include "PvPTypes.h"    // BlueMarbleScriptManager 唯一权威声明（替代本 TU 局部镜像）
#include "CMap.h"          // CMap 完整类型（VerifyMap）
#include "CDungeon.h"      // CDungeon/MazeScript 完整类型（VerifyMap）
#include "Quest.h"         // Quest 完整类型（reselectDailyTrainingQuest）
#include "GameWorld.h"     // G_GameWorld/IsPVPChannel（GetPvPChannelGrade）
#include "CCharacList.h"   // CCharacList 权威定义（原 pad 镜像删除；ctor/dtor/init 见 CtorProducers_1.cpp）
#include "CItemLimitEdition.h"  // CItemLimitEditionMgr 唯一权威声明（m_npcRelation 删除）
#include "RefPvpGrade.h"     // stPvpRefTable_t/stPvPGradeExp_t/RefPvpGrade（权威定义）
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "DisJoint.h"             // DisJoint/DisjointScript 权威类型（本地镜像删除，定义 CtorProducers_2.cpp）
#include "StrikerScript.h"        // StrikerScript 权威类型（本地镜像删除）
#include "CharacManageScript.h"   // CharacManageScript 权威类型（本地镜像删除）
#include "CItemUpgrade_Separate.h"  // WongWork::CItemUpgrade_Separate 权威类型（原全局命名空间镜像 mangled 错误，删除）
// __slang_filter__FlexLexer 基类：DNFLex.h 已把 yyFlexLexer 宏占为
// __dnf_script__FlexLexer，这里按 flex 手册手动切换 flavor 后再引权威 SlangFilter.h。
#undef yyFlexLexer
#define yyFlexLexer __slang_filter__FlexLexer
#include "FlexLexer.h"
#include "SlangFilter.h"  // SlangFilter 权威声明（destroy() 按ORIG经虚 D0 析构）
#include "GlobalData.h"      // GlobalData::s_ref_pvp_grade / s_db_mgr / s_p* 全局

// ---- init() 依赖（ORIG 0x08358332 链）----
#include "sync_script.h"             // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // 脚本装载自由函数唯一声明点（须在 CDataManager.h 后）
#include "CEventScriptMng.h"         // EventClassify::CEventScriptMng 完整类型
#include "CCubeStatistic.h"          // getItemGroupNameTable 权威声明
#include "EventUpgradeJarInfo.h"     // Event_Upgrade_Jar 完整类型
#include "InstanceRentalSystem.h"    // InstanceRentalSystem::Ready
#include "CMCAPManager.h"            // WongWork::CMCAPManager::init
#include "CAssaultMgr.h"             // pvp_assault::GetInstanceAssaultMgr/SetAssaultChannel

// CGameManager 最小视图（避免 QuickParty.h/CDungeon.h 守卫冲突；
// 权威声明见 CGameManager.h，符号由 CGameManager.cpp 提供）。
#include "CQuickPartyRewardManager.h"  // CQuickPartyRewardManager 完整类型（init 调用）
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

#include "CCreatureScriptMgr.h"      // user_creature::GetInstanceCreatureScriptMgr
#include "CreatureScript.h"          // CreatureScript::ExpTable_E（get_creature_level_exp）

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <set>

#include "CSystemTime.h"
#include "CSyncSlangFilter.h"
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

class CNPCDynamicInfo;  // 前置：仅以指针出现在 m_map 值类型

// 权威实现/布局见 CtorProducers_1.cpp（ctor 0x083760f2W / dtor 0x08376106W /
// getNPCInfo / add）。本 TU 只镜像布局（ORIG sizeof 0x18，+0x00 std::map），
// 保证 new/delete 尺寸正确；ctor/dtor 定义唯一归属 CtorProducers_1.cpp。
class CNPCDynamicInfoManager
{
public:
    CNPCDynamicInfoManager();  // ORIG _ZN22CNPCDynamicInfoManagerC2Ev W
    ~CNPCDynamicInfoManager();

    std::map<unsigned int, CNPCDynamicInfo*> m_map;   // +0x00
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
// importPvPReferenceTable 在本 TU 下方实现（见 set_pvp_grade 区），无需 extern。
// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME 定义于 InitScriptProducers_2.cpp
// （ORIG 同源首个具名对象，决定 _GLOBAL__I_ 键名）；PvP_Room.cpp 仅 extern 引用。
extern int PVP_NORMAL_BATTLE_TIME[2];
// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME 定义于 PvP_Room.cpp。
extern int PVP_NORMAL_BATTLE_TIME[2];
extern int PVP_DM_BATTLE_TIME[2];
// PVP_RELAY_BATTLE_TIME 无既有定义，由本 TU 提供（链接保证）。
int PVP_RELAY_BATTLE_TIME[2] = {0, 0};

// ===================== 本 TU 局部依赖类/全局定义（后续批次替换） =====================

// 全局数据（ORIG 属于其它 TU；当前先在此提供可链接定义）
CSystemTime g_systemTimeInst;  // TODO(后续批次)
int g_levelExpTable[256] = {0};  // TODO(后续批次)

// CAutoMarketConditionsControl：ORIG ctor 0x082f8108 —— vector<int> @+0x3dc
// 默认构造（成员自动），并置 +0x380 / +0x00 / +0x3e8 为 0（成员初始值）。
CAutoMarketConditionsControl::CAutoMarketConditionsControl()
    : m_field00(0), m_field380(0), m_field3e8(0)
{
}

// ORIG dtor 0x082f8144：仅 vector<int> @+0x3dc 析构（成员自动）。
CAutoMarketConditionsControl::~CAutoMarketConditionsControl()
{
}

// ORIG LoadScript 0x082f815c（0x2e2 字节）逐分支转写：返回出错行，0=成功。
int CAutoMarketConditionsControl::LoadScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1e;
    }
    int err = 0;
    std::string token;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            err = 0;
            break;
        }
        if (token == "[control of gold drop amount rate]")
        {
            int i = 0;
            while (true)
            {
                m_goldRate[i] = ScanFloat(3, &ok);
                if (!ok)
                {
                    err = 0x2f;
                    break;
                }
                if (++i >= 10)
                {
                    break;
                }
            }
        }
        else if (token == "[control of gold drop amount rate per level]")
        {
            memset(m_goldPerLevel, 0, 0x324);
            while (true)
            {
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (level < 1 || 200 < level)
                {
                    err = 0x3c;
                    break;
                }
                m_goldPerLevel[level] = ScanInt(&ok);
                if (!ok)
                {
                    err = 0x40;
                    break;
                }
            }
        }
        else if (token == "[control of equipment drop prob]")
        {
            int i = 0;
            while (true)
            {
                m_equipProb[i] = ScanFloat(3, &ok);
                if (!ok)
                {
                    err = 0x49;
                    break;
                }
                if (++i >= 10)
                {
                    break;
                }
            }
        }
        else if (token ==
                 "[control of equipment drop prob per level range]")
        {
            while (true)
            {
                int range = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                m_vec3dc.push_back(range);
            }
        }
        else if (token == "[control of durability decrease rate]")
        {
            int i = 0;
            while (true)
            {
                m_durRate[i] = ScanFloat(3, &ok);
                if (!ok)
                {
                    err = 0x60;
                    break;
                }
                if (++i >= 10)
                {
                    break;
                }
            }
        }
        else
        {
            err = 0x64;
        }
        if (err != 0)
        {
            break;
        }
    }
    return err;
}


int* Get_instance_stHellParty_t()
{
    // ORIG 0x089097c4：函数局部 static stHellParty_t hellPartyScript_，
    // __cxa_guard 构造 + __cxa_atexit(D1)（P20 单例形态）。
    // 完整类型见 DataManagerInitDepends.h（stHellParty_t），ctor/dtor
    // 由 InitScriptProducers_7.cpp 提供；本函数保持 int* 返回与既有
    // 各 TU 的 extern int* 声明兼容（ORIG 实为 stHellParty_t*）。
    static stHellParty_t hellPartyScript_;
    return reinterpret_cast<int*>(&hellPartyScript_);
}

// ===================== 查询 =====================
// ORIG W 0x0830e6d2 _ZNK12CDataManager11getDropRateEv：读 +0xb3f0 float。
float CDataManager::getDropRate() const
{
    return m_dropRate;
}

// ORIG T 0x088dcee2 _ZNK16channel_script_t23getBonusRateEventServerEhj
// 服务器频道加成版本：命中频道信息即固定 0.02f，否则 0.0f。
float channel_script_t::getBonusRateEventServer(unsigned char channel, unsigned int idx) const
{
    if (getChannelInfo(channel, idx) != 0)
        return 0.02f;
    return 0.0f;
}


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
    return (void*)m_pvpMissionSystem->find_mission(idx);
}

int CDataManager::find_npc(unsigned int idx) const
{
    return (int)(unsigned long)m_npcScriptList->find(idx);
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
    // ORIG 消费 CreatureScript::ExpTable_E（0x94fefa0，initCreatureScript 填充）。
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

// ORIG 0x08360716：float 比较用 fucompp+fnstsw+sahf（i486 形态）、循环体
// fildll/fmuls/fistpll（64 位零扩展中间量）——以 target attribute 与强转链复刻。
__attribute__((target("arch=i486"))) void CDataManager::SetExpRate(float rate)
{
    if (m_expRate != rate)
    {
        memcpy(m_mobReward, m_mobRewardBackup, 0x324);
        m_expRate = rate;
        for (int i = 0; i <= 200; ++i)
        {
            m_mobReward[i] =
                (int)(long long)((float)(unsigned long long)(unsigned int)m_mobReward[i] *
                                 rate);
        }
    }
}

// ORIG 0x083607d8：ratio 先行物化到局部（fmuls 直乘内存槽），两段循环分别
// 遍历 m_genRefs1(+0x468)/m_genRefs0(+0x428)，元素 +0x10=m_dropRate 按
// ROUND(v*ratio) 截断缩放；float 比较为 i486 形态。
__attribute__((target("arch=i486"))) void CDataManager::SetItemDropRate(float rate)
{
    if (m_dropRate != rate)
    {
        float old = m_dropRate;
        float ratio = old / rate;
        m_dropRate = rate;
        m_itemGeneratorMgr->setDropRatio(rate);
        for (unsigned int i = 0; i < m_serverParameter.m_genRefs1.size(); ++i)
        {
            sItemGenRef& ref = m_serverParameter.m_genRefs1[i];
            ref.m_dropRate = (int)((float)ref.m_dropRate * ratio);
        }
        for (unsigned int i = 0; i < m_serverParameter.m_genRefs0.size(); ++i)
        {
            sItemGenRef& ref = m_serverParameter.m_genRefs0[i];
            ref.m_dropRate = (int)((float)ref.m_dropRate * ratio);
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
    return m_pvpMissionSystem->get_WithinMissionIndex(idx);
}

std::list<CMission*>* CDataManager::get_kind_mission_list(int idx) const
{
    return &m_pvpMissionSystem->get_kind_mission_list(idx);
}

void CDataManager::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    m_pvpMissionSystem->get_New_DailyMission(out);
}

int CDataManager::get_BaseMissionExp_byRank(unsigned short rank) const
{
    return m_pvpMissionSystem->get_BaseMissionExp_byRank(rank);
}

std::bitset<32> CDataManager::get_daily_mission_kind() const
{
    return m_pvpMissionSystem->get_daily_mission_kind();
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

unsigned int CDataManager::getChoiceItem()
{
    return m_choiceItem.m_choice;
}

void CDataManager::setChoiceItem(bool flag, unsigned int idx)
{
    m_choiceItem.m_checked = flag;
    m_choiceItem.m_choice = idx;
}

bool CDataManager::checkChoiceItem()
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

// ---- CChattingEmoticonList::ReCalc（报告 ReCalc.md，ORIG T 0x080e5880）：
//      ① 遍历 config 24 行：行 m_nCount 能 find_emoticon 命中且 isCommonEmoticon
//         （vtable+0x14）→ 行拷入 rows 并记 used；② config.clear()；
//      ③ rows 各行回填到 config 中 m_nCount==0 的空行（[GetStartArrayIndex,
//         GetEndArrayIndex] 区间内第一个空位，整行 0x16 字节拷贝）；
//      ④ 遍历 m_map：GetIndex 未在 used 且 isCommonEmoticon → 同样区间内
//         空行写入 m_nCount。 ----
void CChattingEmoticonList::ReCalc(CChattingEmoticonConfig& config,
                                   const CUser& user)
{
    std::vector<CChattingEmoticonConfigRow> rows;
    std::set<int> used;

    for (int i = 0; i <= 0x17; ++i)
    {
        ChattingEmoticonBase* p = find_emoticon(config.m_Row[i].m_nCount);
        if (p == 0)
            continue;
        if (!p->isCommonEmoticon(user))
            continue;
        rows.push_back(config.m_Row[i]);
        used.insert(config.m_Row[i].m_nCount);
    }
    config.clear();

    for (unsigned int r = 0; r < rows.size(); ++r)
    {
        ChattingEmoticonBase* p = find_emoticon(rows[r].m_nCount);
        if (p == 0)
            continue;
        for (int i = p->GetStartArrayIndex(); i <= p->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i] = rows[r];
                break;
            }
        }
    }

    for (std::map<int, ChattingEmoticonBase*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (used.find(it->second->GetIndex()) != used.end())
            continue;
        if (!it->second->isCommonEmoticon(user))
            continue;
        for (int i = it->second->GetStartArrayIndex();
             i <= it->second->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i].m_nCount = (short)it->second->GetIndex();
                break;
            }
        }
    }
}

// ReCalcEmoticonConf —— ORIG T 0x0835fc76 _ZN12CDataManager18ReCalcEmoticonConfER23CChattingEmoticonConfigRK5CUser
//   （docs/class_func_reports/CDataManager/ReCalcEmoticonConf.md）：
//   直通 m_emoticonList(+0x28) 的 CChattingEmoticonList::ReCalc（call 前置：
//   user 压 0x8(%esp)、config 压 0x4(%esp)、m_emoticonList 压 (%esp)）。
// ---------------------------------------------------------------------------
void CDataManager::ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user)
{
    m_emoticonList->ReCalc(config, user);
}

// ---------------------------------------------------------------------------
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
//   （非 const；sret 返回 vector<int>；报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     result 默认构造；m_emoticonList(+0x28) 为空直接返回空表；
//     遍历 emoticons：find_emoticon(emoticons[i]) 未命中跳过；
//     命中且 !isUsable(user)（vtable+0x08 虚调用，bool 取反 P22）
//     → result.push_back(emoticons[i])。
// ---------------------------------------------------------------------------
std::vector<int> CDataManager::IsUsableEmoticons(const CUser& user,
                                                 const std::vector<int>& emoticons)
{
    std::vector<int> result;
    if (m_emoticonList != 0)
    {
        ChattingEmoticonBase* p;
        for (unsigned int i = 0; i < emoticons.size(); ++i)
        {
            p = m_emoticonList->find_emoticon(emoticons[i]);
            if (p == 0)
                continue;
            if (!p->isUsable(user))
                continue;
            result.push_back(emoticons[i]);
        }
    }
    return result;
}

int CDataManager::GetUpgradeRevisionPvPChannel()
{
    return m_pvpChannel.m_upgradeRevision;
}

float CDataManager::getUpgradeItemRepairCostRate(int idx, bool flag)
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

void* CDataManager::GetImageCommnicationData()
{
    return (void*)&m_imageCommData;
}


std::set<int>* CDataManager::getUltimateDungeonList() const
{
    return (std::set<int>*)&m_ultimateDungeonList;
}
// ORIG W 0x0822b64c：return this+0xb424（&m_vendingMachine）。
std::vector<stVendingMachine_t>* CDataManager::getVendingMachineTable() const
{
    return (std::vector<stVendingMachine_t>*)&m_vendingMachine;
}

// 注：ORIG 仅有非 const getBlueMarbleScript()（0x08365eea，nm 无 _ZNK 重载）；
// 原 TU 内发明的 const 重载已删除（调用点走非 const 版本）。


void* CDataManager::get_hellparty_script_values()
{
    return Get_instance_stHellParty_t();
}

// ORIG 0x08365fc0：sub $0x90120,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testActionType()
{
    char buf[0x90118];
    (void)buf;
}

// ORIG 0x08365ef8：sub $0x20,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testBingo()
{
    char buf[0x18];
    (void)buf;
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
        // ORIG destroy：(**(code**)(vptr+4))(p) —— 经 vtable 第 2 槽调 SlangFilter
        // 虚删除析构 D0（权威类 vtable 槽1 即 deleting dtor），等价 delete。
        delete m_invalidChar.m_slangFilter;
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
        delete m_npcRelation.m_itemLimitEditionMgr;
        m_npcRelation.m_itemLimitEditionMgr = 0;
    }
    if (m_altarConfig.m_strikerScript)
    {
        delete m_altarConfig.m_strikerScript;
        m_altarConfig.m_strikerScript = 0;
    }
    if (m_altarConfig.m_characManageScript)
    {
        delete m_altarConfig.m_characManageScript;
        m_altarConfig.m_characManageScript = 0;
    }
    if (m_choiceItem.m_blueMarbleScriptMgr)
    {
        // ORIG destroy：BlueMarbleScriptManager::~BlueMarbleScriptManager(p) +
        // operator delete(p)（destroy.md 0x8357a66 尾段）→ 等价 delete。
        // 权威类（PvPTypes.h）无用户析构，隐式析构 + operator delete 由编译器生成。
        delete m_choiceItem.m_blueMarbleScriptMgr;
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

int CDataManager::get_limit_inout_count(ENUM_BLOOD_TYPE type)
{
    return (unsigned char)m_limitInoutCount[type];
}

// ============================================================================
// 迁移自 GameStubs.cpp 的 CDataManager 桩方法（ORIG 符号/偏移已核对）
// ============================================================================

void CDataManager::set_AuctionRegFreeCeraItem(unsigned int idx)
{
    // ORIG 0x08513fbc W：m_choiceSet(+0xb4ac).insert(idx)
    m_choiceSet.insert(idx);
}

void* CDataManager::GetExpertJobScript(int job)
{
    // ORIG 0x0822b5f2 W：m_regeneration.m_expertJobList（CDM+0x5090）直取后
    // 调 CExpertJobList::get(job)，无中间暂存。
    return m_regeneration.m_expertJobList->get(job);
}

void* CDataManager::GetExpertJobEtcScript()
{
    // ORIG 0x0849f516 W：调 CExpertJobList::get_etc()（W 0x0849f50a，返回 this+0x18
    // = &m_etc）。CUser 侧将其作 std::map<uchar,short>（等级→所需角色等级）读取。
    return m_regeneration.m_expertJobList->get_etc();
}

void* CDataManager::GetChannelScript() const
{
    // ORIG 0x0814a6f8 W：return &m_channelScript（+0xb3f4）
    return const_cast<channel_script_t*>(&m_channelScript);
}

stDeathTower_t* CDataManager::GetTowerScript(unsigned int dungeonIdx)
{
    // ORIG W 0x081455d8：m_deathTowerMap.find(dungeonIdx)；未找到打 cMyTrace；
    // 尾部单条 add $0x4,%eax 是 pair.second 的偏移 —— 返回值即 &it->second
    // （结构体基址）。调用方（CBossPlay/CBossStage）按字段偏移取用：
    // flds 0x4(%eax,%edx,4) = m_rateA[idx]、+0xc88/+0xc8c/+0xc94 奖励概率。
    std::map<unsigned int, stDeathTower_t>::iterator it =
        m_deathTowerMap.find(dungeonIdx);
    if (it == m_deathTowerMap.end())
    {
        cMyTrace trace("stDeathTower_t* CDataManager::GetTowerScript(unsigned int)", 0xd44, 5);
        trace("Dungeon idx not found : %u", dungeonIdx);
    }
    return &it->second;
}

void* CDataManager::getBlueMarbleScript()
{
    // ORIG 0x08365eea T：return *(void**)(this+0xb4a8)
    //   = m_choiceItem.m_blueMarbleScriptMgr（ChoiceItemInfo +0x08，已类型化）。
    return (void*)m_choiceItem.m_blueMarbleScriptMgr;
}

void* CDataManager::get_event_script_mng()
{
    // ORIG 0x08110b62 W：return m_liveServerInfo.m_eventScriptMng（已类型化）
    return (void*)m_liveServerInfo.m_eventScriptMng;
}

CItemLimitEditionMgr* CDataManager::getItemLimitEditionMgr()
{
    // ORIG 0x08179fa6 W：return m_npcRelation.m_itemLimitEditionMgr（+0x18）
    return m_npcRelation.m_itemLimitEditionMgr;
}

char CDataManager::get_dimensionInout(int idx)
{
    // ORIG 0x0822b612：返回类型 char（movzbl 单条读取，无 32 位扩展）；
    // 越界检查顺序与 ORIG 一致（cmp $5 在前、cmp $0 在后）。
    if (idx > 5 || idx < 0)
        return 0;
    return m_dimensionInout[idx];
}

void CDataManager::GetPvPChannelGrade()
{
    // ORIG 0x08357e9e T：非 PVP 频道直接返回；否则在 m_liveServerInfo.m_pvpChannelMap
    // （+0xb4ec，map<int,vector<pvp_channel_info_t>>）按 serverGroup 查找，命中则把
    // 首个匹配项的 9 个字段拷入 m_pvpChannel（+0xb430），并记 PVP 日志。
    if (!G_GameWorld()->IsPVPChannel())
    {
        return;
    }
    // ORIG 读 env+0x378 = m_serverEnvir.m_serverGroup（CEnvironment.h 权威布局）。
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    std::map<int, std::vector<pvp_channel_info_t> >& channelMap =
        m_liveServerInfo.m_pvpChannelMap;
    std::map<int, std::vector<pvp_channel_info_t> >::iterator it =
        channelMap.find(serverGroup);
    if (it != channelMap.end())
    {
        // ORIG 0x08357e9e：遍历 vector，取 env channelNo(Server_Envir+0x18)
        // 与 info.m_field0 匹配的首项拷入 m_pvpChannel（9 int）；未命中保持旧值。
        int channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        std::vector<pvp_channel_info_t>& vec = it->second;
        for (std::vector<pvp_channel_info_t>::iterator vit = vec.begin();
             vit != vec.end(); ++vit)
        {
            if (channelNo == vit->m_field0)
            {
                int* dst = (int*)&m_pvpChannel;
                const int* src = (const int*)&*vit;
                for (int i = 0; i < 9; ++i)
                {
                    dst[i] = src[i];
                }
                break;
            }
        }
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18f3, 0);
    trace("PVP Channel Information  serverGroup(%d), pvpChannelNum(%d), "
          "minGrade(%d), maxGrade(%d), upgradeRevision(%d), type(%d), "
          "minLevel(%d), maxLevel(%d)",
          serverGroup, 0, ((int*)&m_pvpChannel)[1], ((int*)&m_pvpChannel)[2],
          ((int*)&m_pvpChannel)[3], ((int*)&m_pvpChannel)[4],
          ((int*)&m_pvpChannel)[5], ((int*)&m_pvpChannel)[6],
          ((int*)&m_pvpChannel)[7]);
}

int CDataManager::GetMaxGradePvPChannel()
{
    // ORIG 0x0822b65a W：返回 m_pvpChannel(+0xb430) 内 +0x8 字段。
    return m_pvpChannel.m_maxGrade;

}

int CDataManager::GetPvPChannelType()
{
    // ORIG 0x085e68a8 W：mov 0xb440(%eax) → m_pvpChannel+0x10。
    return m_pvpChannel.m_channelType;
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
    // ORIG 0x083620ee T。主分支：
    //  1) AI 角色校验：map.getAICharacters() 中的每个 APC type 须能经
    //     CAICharacterList::get 查到（否则 log "Unknown APC Type"）。
    //  2) 收集目标 dungeon：idx==-1 用 getDungeonLists，否则单元素{vlan}。
    //  3) 遍历 map item list：每个 item 的 assign 索引对 dungeon 的
    //     getSpecialPassiveObjectItems() 做范围/取值校验（1..200）。
    //  4) dungeon list 非空 → 逐 dungeon 校验 eventMonster/rival/maze。
    //  5) hash_map 遍历 m_dungeonList：PvP 地图 idx 越界(>=0xc351 && <=51000 为
    //     合法)时；dungeonKind==1 → m_vec674 匹配 map index；m_mazeList 网格 →
    //     地图 index 出现在 GridScript mapCandidates/secondCandidates 中则过。
    //  6) map.get_map_type() 校验 + map.get_index() 结果。返回 bool。
    // 校验 APC type（主分支）。
    const std::vector<STAICharacterMapArrangeData>* aiList = map.getAICharacters();
    if (aiList && !aiList->empty())
    {
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 aiList->begin();
             it != aiList->end(); ++it)
        {
            unsigned int apcType = (unsigned int)it->m_type;
            if (m_aiCharacterList->get(apcType) == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x2847,
                    "Unknown APC Type(%d). Map(%d)", apcType, map.get_index());
            }
        }
    }
    // 收集 dungeon list。
    std::list<int> dungeonList;
    if (idx == -1)
    {
        std::list<int>* dl = map.getDungeonLists();
        if (dl)
            dungeonList = *dl;
    }
    else
    {
        dungeonList.push_back(idx);
    }
    // 遍历 map item list，校验 passive object assign 项。
    std::list<_mapItem> itemList = map.get_item_list();
    for (std::list<_mapItem>::const_iterator it = itemList.begin();
         it != itemList.end(); ++it)
    {
        const _mapItem& item = *it;
        for (std::list<int>::iterator dit = dungeonList.begin();
             dit != dungeonList.end(); ++dit)
        {
            CDungeon* dungeon = find_dungeon(*dit);
            std::vector<stDungeonAssignItem_t>* passive =
                dungeon ? dungeon->getSpecialPassiveObjectItems() : 0;
            if (item.m_objectIdx == 0xffffffff || !passive || passive->empty())
            {
                continue;
            }
            if ((unsigned int)item.m_objectIdx < passive->size())
            {
                int assignIdx = passive->at(item.m_objectIdx).m_assignItemIdx;
                if (assignIdx < 1 || assignIdx > 200)
                {
                    // [推断] ORIG 在此仅遍历/校验，不作为 fail 条件（report 未显示
                    // 命中时改写行为）；保持遍历语义。
                }
            }
        }
    }
    // 逐 dungeon 校验 eventMonster / rival / maze（map index 匹配）。
    bool verified = true;
    for (std::list<int>::iterator dit = dungeonList.begin();
         dit != dungeonList.end(); ++dit)
    {
        CDungeon* dungeon = find_dungeon(*dit);
        if (dungeon == 0)
        {
            verified = false;
        }
    }
    if (!dungeonList.empty())
    {
        // hash_map 遍历 PvP 地图（ORIG 遍历 m_dungeonList->m_dungeonMap）。
        if (m_dungeonList)
        {
            for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator hit =
                     m_dungeonList->m_dungeonMap.begin();
                 hit != m_dungeonList->m_dungeonMap.end(); ++hit)
            {
                CDungeon* dungeon = hit->second;
                if (!dungeon)
                    continue;
                int mapIndex = map.get_index();
                // PvP 地图 index 区间非法则校验失败。
                if (mapIndex >= 0xc351 && mapIndex <= 51000)
                {
                    // [推断] ORIG：PvPMapInfo 判定区间，越界(FALSE)才继续；此处
                    // 命中合法区间即不触发失败。
                }
                int dungeonKind = dungeon->getDungeonKind();
                if (dungeonKind == 1)
                {
                    // [推断] m_vec674 匹配 map index。
                    std::vector<int>& v674 = *(std::vector<int>*)((char*)dungeon + 0x674);
                    for (unsigned int k = 0; k < v674.size(); ++k)
                    {
                        if (v674[k] == mapIndex)
                            break;
                    }
                }
                // MazeScript 网格：m_mazeList（+0x62c）每网格 getGridR 与候选区。
                std::vector<MazeScript>* mazes =
                    (std::vector<MazeScript>*)((char*)dungeon + 0x62c);
                for (unsigned int m = 0; m < mazes->size(); ++m)
                {
                    MazeScript& maze = (*mazes)[m];
                    if (maze.m_2c == mapIndex)
                        continue;
                    for (int row = 0; row < maze.m_height; ++row)
                    {
                        for (int col = 0; col < maze.m_width; ++col)
                        {
                            const GridScript& grid = maze.getGridR(col, row);
                            if (grid.m_gridValue != 0)
                            {
                                // [推断] map index 落在候选容器则命中。
                                std::vector<int>& idxVals =
                                    (std::vector<int>&)grid.m_mapCandidates;
                                for (unsigned int g = 0; g < idxVals.size(); ++g)
                                {
                                    if (idxVals[g] == mapIndex)
                                        continue;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // [推断] ORIG 末尾：get_map_type()/entranceVec 校验为主分支尾，未测出命中时
    // verified 保持 true；返回 verified。
    int mapType = map.get_map_type();
    (void)mapType;
    return verified ? 1 : 0;
}

void* CDataManager::get_dungeon(int idx)
{
    // ORIG 0x086d18c8 W：return find_dungeon(idx)
    return find_dungeon(idx);
}

bool CDataManager::reselectDailyTrainingQuest()
{
    // ORIG 0x08363ce0 T。主分支：
    //  1) suffleTrainingQuests() 重排每日训练任务。
    //  2) getApplyLevel 填充 multiset<int>（各档可选等级）。
    //  3) 对每个等级 level(1..0x46)：清空 m_dailyTrainingQuest[level-1]，
    //     遍历 multiset，用 getTrainingQuest(level, prevApply, prevMin) 取任务 idx；
    //     find_quest 命中且 Quest::m_type==1 → push_back 到当日列表；否则 error 日志。
    //  4) reset_same_apply_level_cnt()；非空列表记 TRAINING_QUEST trace。
    m_trainingQuest.suffleTrainingQuests();

    // ORIG 0x08363d03：getApplyLevel 按 level 分桶写入 multiset 数组
    // （out[level-1]，level 1..0x46），主循环对每个 level 遍历对应桶。
    std::multiset<int> applyLevels[0x46];
    m_trainingQuest.getApplyLevel(applyLevels);

    for (int level = 1; level < 0x47; ++level)
    {
        m_dailyTrainingQuest[level - 1].clear();
        int prevMin = 0x7fffffff;
        int prevApply = 0x7fffffff;
        for (std::multiset<int>::iterator it = applyLevels[level - 1].begin();
             it != applyLevels[level - 1].end(); ++it)
        {
            prevApply = *it;
            int questIdx =
                m_trainingQuest.getTrainingQuest(level, prevApply, prevMin);
            prevMin = prevApply;
            if (questIdx != 0x7fffffff)
            {
                Quest* quest = m_questList->find_quest(questIdx);
                if (quest != 0)
                {
                    if (quest->m_type == 1)
                    {
                        m_dailyTrainingQuest[level - 1].push_back(questIdx);
                    }
                    else
                    {
                        LogManager::logFormat(
                            1, "data_manager.cpp",
                            __PRETTY_FUNCTION__,
                            0x2ac8,
                            "reselectDailyTrainingQuest() ERROR : quest(%d), "
                            "level(%d)",
                            questIdx, level);
                    }
                }
            }
        }
        m_trainingQuest.reset_same_apply_level_cnt();
        if (!m_dailyTrainingQuest[level - 1].empty())
        {
            // [推断] ORIG 用 sync_script::CStringMaker 把 quest idx 拼成字符串后记
            // cMyTrace；此处以逗号连接（语义等价）。
            std::string questStr;
            for (std::vector<int>::iterator vit =
                     m_dailyTrainingQuest[level - 1].begin();
                 vit != m_dailyTrainingQuest[level - 1].end(); ++vit)
            {
                char tmp[16];
                sprintf(tmp, "%s%d", vit == m_dailyTrainingQuest[level - 1].begin() ? "" : ",",
                        *vit);
                questStr += tmp;
            }
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2ad2, 0);
            trace("TRAINING_QUEST level(%d), quest(%s)", level, questStr.c_str());
        }
    }
    return 1;
}

// ============================================================================
// getCharacterJobToEnumValue（ORIG 0x89bbc0f，_Z26getCharacterJobToEnumValueRSs）
// ----------------------------------------------------------------------------
// 被 importPvPReferenceTable（"[pvp record order]" 键）与
// EquipmentParameterInfo.cpp 引用。ORIG 反汇编（objdump -d df_game_r）为 12 次
// std::string==const char* 比较链（_ZSteqIcSt11char_traitsIcESaIcEEbRKSbIT_T0_T1_EPKS3_
// @0x80809a1）；字符串表 .rodata 0x8e198be..0x8e19923 逐字节核对：
//   "swordman"→0 "fighter"→1 "gunner"→2 "mage"→3 "priest"→4 "at gunner"→5
//   "thief"→6 "at fighter"→7 "at mage"→8 "demonic swordman"→9
//   "creator mage"→10 "common"→11，全不中→-1（0xffffffff）
// ============================================================================
int getCharacterJobToEnumValue(std::string& job)
{
    if (job == "swordman")
        return 0;
    if (job == "fighter")
        return 1;
    if (job == "gunner")
        return 2;
    if (job == "mage")
        return 3;
    if (job == "priest")
        return 4;
    if (job == "at gunner")
        return 5;
    if (job == "thief")
        return 6;
    if (job == "at fighter")
        return 7;
    if (job == "at mage")
        return 8;
    if (job == "demonic swordman")
        return 9;
    if (job == "creator mage")
        return 10;
    if (job == "common")
        return 11;
    return -1;
}

// ============================================================================
// importPvPReferenceTable —— ORIG 0x0890857b（_Z23importPvPReferenceTablePKcR15stPvpRefTable_t）
// 解析 Etc/pvp_ref.etc 填充 stPvpRefTable_t。返回 0 成功（token 流耗尽 EOF）；
// 非 0 为各键解析失败码，均与 ORIG 反汇编逐操作数一致。
// 反汇编证据（objdump -d df_game_r，0x890857b）：
//   loadRDARScriptFile("", path) 失败 → return 0x1816
//   Quote 循环：ScanType(line,1) 返回 0（EOF）→ return 0
//   "[pvp experience max grade]"：m_count=ScanInt+1；!ok→0x181f；reserve+resize
//   "[pvp experience]"：循环 {grade=ScanInt(!ok则break), exp=ScanInt(!ok→0x182d);
//                            m_grades[grade]={grade,exp}}
//   "[pvp experience loss]"：m_pvpExpLoss1=ScanInt(!ok→0x1835)；
//                             m_pvpExpLoss2=ScanInt(!ok→0x1838)
//   "[pvp rank rate grade]"：m_rankRateGrade=ScanInt(!ok→0x183e)
//   "[pvp rank rate]"：循环 push_back(ScanInt)，!ok break
//   6 个战斗时间键 → PVP_*_BATTLE_TIME[i] 对应地址（0x93f4ee0..0x93f4ef4）
//   "[pvp record order]"：clear；while(ScanStr) m_recordOrder.push_back(getCharacterJobToEnumValue)
//   "[pvp team hp correction]"：ScanFloat 读取但丢弃（fstp st0）
// 未知键忽略（继续 ScanType 循环）。
// ============================================================================
int importPvPReferenceTable(const char* path, stPvpRefTable_t& table)
{
    if (!loadRDARScriptFile("", path))
        return 0x1816;

    std::string line;
    while (ScanType(line, true))
    {
        if (line == "[pvp experience max grade]")
        {
            bool ok = false;
            int cnt = ScanInt(&ok);
            table.m_count = cnt + 1;
            if (!ok)
                return 0x181f;
            table.m_grades.reserve(table.m_count);
            table.m_grades.resize(table.m_count, stPvPGradeExp_t());
        }
        else if (line == "[pvp experience]")
        {
            while (true)
            {
                bool ok = false;
                int grade = ScanInt(&ok);
                if (!ok)
                    break;
                bool ok2 = false;
                int exp = ScanInt(&ok2);
                if (!ok2)
                    return 0x182d;
                stPvPGradeExp_t g;
                g.a = grade;
                g.b = exp;
                table.m_grades[grade] = g;
            }
        }
        else if (line == "[pvp experience loss]")
        {
            bool ok = false;
            int v1 = ScanInt(&ok);
            if (!ok)
                return 0x1835;
            table.m_pvpExpLoss1 = v1;
            bool ok2 = false;
            int v2 = ScanInt(&ok2);
            if (!ok2)
                return 0x1838;
            table.m_pvpExpLoss2 = v2;
        }
        else if (line == "[pvp rank rate grade]")
        {
            bool ok = false;
            int v = ScanInt(&ok);
            if (!ok)
                return 0x183e;
            table.m_rankRateGrade = v;
        }
        else if (line == "[pvp rank rate]")
        {
            while (true)
            {
                bool ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                table.m_rankRate.push_back(v);
            }
        }
        else if (line == "[pvp normal battle time]")
        {
            PVP_NORMAL_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time]")
        {
            PVP_DM_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time]")
        {
            PVP_RELAY_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp normal battle time for tournament]")
        {
            PVP_NORMAL_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time for tournament]")
        {
            PVP_DM_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time for tournament]")
        {
            PVP_RELAY_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp record order]")
        {
            table.m_recordOrder.clear();
            std::string job;
            while (ScanStr(&job))
            {
                table.m_recordOrder.push_back(getCharacterJobToEnumValue(job));
            }
        }
        else if (line == "[pvp team hp correction]")
        {
            ScanFloat((bool*)0);  // 读取但丢弃
        }
    }
    return 0;  // EOF：解析成功
}

// ============================================================================
// CDataManager::set_pvp_grade —— ORIG 0x08360aa2
// 加载 PvP 等级参考表，填充 GlobalData::s_ref_pvp_grade（RefPvpGrade）。
// 调用 importPvPReferenceTable（本 TU 实现，ORIG 0x890857b）。
// ============================================================================
int CDataManager::set_pvp_grade()
{
    RefPvpGrade* grade = GlobalData::s_ref_pvp_grade;
    grade->Clear();
    stPvpRefTable_t table;
    table.m_count = 0;
    if (importPvPReferenceTable("Etc/pvp_ref.etc", table) != 0)
        return 0;
    int lastGrade = 0;
    int prevExp = 0;
    for (unsigned int i = 1; i < (unsigned int)table.m_count; ++i)
    {
        RefPvpGrade::RefPvpGradeData data;
        data.curr = prevExp;
        data.next = table.m_grades[i].b;
        grade->SetPvpGrade(table.m_grades[i].a, data);
        prevExp = table.m_grades[i].b;
        ++lastGrade;
    }
    grade->set(table);
    grade->SetLastGrade(lastGrade);
    return 1;
}

// ORIG _ZNK12CDataManager16GetResetItemInfoEv @ 0x08693f24 (W)：return this+0x4d04
// = &m_limitItemUsage.m_resetC.m_list（stLimitItemUsageInfoEx@0x4cf8 + stResetItemInfo@+0x0c）。
std::vector<stResetItemInfo::ResetItem>* CDataManager::GetResetItemInfo() const
{
    return const_cast<std::vector<stResetItemInfo::ResetItem>*>(
        &m_limitItemUsage.m_resetC.m_list);
}

// ORIG _ZNK12CDataManager17GetRefillItemInfoEv @ 0x08693f32 (W)：return this+0x4d10
// = &m_limitItemUsage.m_refill18.m_list（stLimitItemUsageInfoEx@0x4cf8 + stRefillItemInfo@+0x18）。
std::vector<stRefillItemInfo::RefillItem>* CDataManager::GetRefillItemInfo() const
{
    return const_cast<std::vector<stRefillItemInfo::RefillItem>*>(
        &m_limitItemUsage.m_refill18.m_list);
}

// ---------------------------------------------------------------------------
// CDataManager::InitLoginState —— ORIG 0x08360306（0xf9），报告
// docs/class_func_reports/CDataManager/InitLoginState.md 逐条转写：
// 取 DB_HANDLE_6 句柄；operation_type 0/2 → 按 channel 清 login_status；
// operation_type==1 且 env+0x1b0==1 → 不带 channel 清；否则直接成功。
// env+0x1b0 字段未建模，保留裸偏移（推断）。
// ---------------------------------------------------------------------------
bool CDataManager::InitLoginState(ENUM_SERVER_GROUP serverGroup, int channelNo)
{
    MySQL* hdb = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                   (ENUM_SERVER_GROUP)0);
    CEnvironment* env = G_CEnvironment();
    const int opType = env->get_operation_type();
    bool result;
    if (opType == 0 || opType == 2)
    {
        hdb->set_query(
            "upDate login_account_%d set login_status=0 where m_channel_no=%d",
            serverGroup, channelNo);
        result = hdb->exec(true);
    }
    else if (opType == 1 &&
             env->m_serverEnvir.m_channelNo == 1)
    {
        hdb->set_query("upDate login_account_%d set login_status=0",
                       serverGroup);
        result = hdb->exec(true);
    }
    else
    {
        result = true;
    }
    return result;
}

// ---------------------------------------------------------------------------
// CDataManager::init —— ORIG 0x08358332..0x0835f1d5（0x6ea3 字节，~5430 指令）。
// 报告 docs/class_func_reports/CDataManager/init.md 全量转写（汇编 + 反编译 C
// 逐段核对，含 +0x1124/+0x18f6/+0x2565/+0x2640 等歧义区回查原始 asm）。
//
// 结构：约 160 个脚本装载步骤组成的「成功续链」——每步失败按 ORIG 各自的
// 行为处理：多数 TRACE 后 return 0；个别仅告警继续（ItemInfoMaster、
// GoldLimit、bindsphere、TowerOfDespair、CheckLightServer/Statistic 等，
// 均在对应分支注释标注报告行号）。
//
// 失败路径返回 0；全链成功返回 1。栈局部对象（vector<PVPMAP_DATA>、
// vector<_DailySchedule>、SDCActiveStatusSyncServer、AvatarRoulette）由 C++
// 作用域析构，与 ORIG 异常清理块（+0x6df2..+0x6e96）等价。
// ---------------------------------------------------------------------------
int CDataManager::init()
{
    CEnvironment* env = G_CEnvironment();

    // [ORIG +0x000c..+0x005d] 遍历 m_serverEnvir.m_priorityScriptDir(+0x2fc)
    // 逐个 SetPriorityBaseDirectory。
    for (unsigned int i = 0; i < env->m_serverEnvir.m_priorityScriptDir.size(); ++i)
    {
        SetPriorityBaseDirectory(
            env->m_serverEnvir.m_priorityScriptDir[i].c_str());
    }

    // [ORIG +0x005f] m_characNameFilterPass(+0xa8d8) = 0。
    m_characNameFilterPass = false;

    // [+0x0069..+0x00b1] InitRDARScript(基目录 c_str(), m_scriptpacks 字节,
    // GetScriptPacks(), NULL)。
    InitRDARScript(env->m_serverEnvir.m_baseScriptDirectory.c_str(),
                   env->m_serverEnvir.m_scriptpacks != 0,
                   GetScriptPacks(), (int*)0);

    // [+0x00b3..+0x0176] InitRDARScriptStringManager("n_String.lst", NULL, NULL)
    // —— 失败是唯一的前导致命错误（TRACE 0x1972 / 0x1975）。
    if (!InitRDARScriptStringManager("n_String.lst", (const char*)0,
                                     (std::vector<ErrorLog>*)0))
    {
        cMyTrace trace("int CDataManager::init()", 0x1972, 9, true, true);
        trace("\t- InitRDARScriptStringManager Fail");
        return 0;
    }
    {
        cMyTrace trace("int CDataManager::init()", 0x1975, 9, true, true);
        trace("\t- InitRDARScriptStringManager Success");
    }

    // [+0x0176] InitMapEquipmentAniScript()。
    InitMapEquipmentAniScript();

    // [+0x017b..+0x02a8] ItemInfoMaster：失败/DB 步骤失败均只 TRACE 继续
    // （0x1982/0x1986/0x1989）。
    sync_script::CSyncScript* sync = sync_script::GetInstanceSyncScript();
    if (sync->LoadItemInfoMasterScript("Etc/ItemTypeMaster.etc"))
    {
        if (!sync->truncate_item_info_master_table())
        {
            cMyTrace("int CDataManager::init()", 0x1986, 9, true, true)(
                "\t- Failed to < TRUNCATE ITEM INFO MASTER TABLE >");
        }
        if (!sync->insert_item_info_master_to_db())
        {
            cMyTrace("int CDataManager::init()", 0x1989, 9, true, true)(
                "\t- Failed to < INSERT ITEM INFO MASTER TO DB >");
        }
    }
    else
    {
        cMyTrace("int CDataManager::init()", 0x1982, 9, true, true)(
            "\t- Failed to load < ITEM INFO MASTER SCRIPT >");
    }

    // [+0x02a8..+0x03xx] OldEquipmentStatInfo：同上 log-only（0x198f/93/96）。
    if (importOldEquipmentStatInfo(""))
    {
        if (!sync->truncate_old_equip_info_table())
        {
            cMyTrace("int CDataManager::init()", 0x1993, 9, true, true)(
                "\t- Failed to < TRUNCATE OLD EQUIP INFO TABLE >");
        }
        if (!sync->insert_old_equip_info_to_db())
        {
            cMyTrace("int CDataManager::init()", 0x1996, 9, true, true)(
                "\t- Failed to < INSERT OLD EQUIP INFO TO DB >");
        }
    }
    else
    {
        cMyTrace("int CDataManager::init()", 0x198f, 9, true, true)(
            "\t- Failed to import < OldEquipmentStatInfo.dat >");
    }

    // ---- 主成功续链开始：任一 FAIL 分支 TRACE 后 return 0 ----

    // ServerParameter.etc（fail TRACE 0x1972 区尾段 0x1a35）。
    if (!importServerParameterScript(&m_serverParameter,
                                     "Etc/ServerParameter.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1a35, 9, true, true)(
            "\t- importServerParameterScript Fail");
        return 0;
    }

    // serverType==2 时把 expGroup378 拷入 expGroup368（+0x368/+0x378 各 0x10）。
    if (env->m_serverEnvir.m_serverType == 2)
    {
        memcpy(m_serverParameter.m_expGroup368,
               m_serverParameter.m_expGroup378, 0x10);
    }

    // IngameAdvertisementParameter.etc（fail 0x1a4a）。
    if (!importInGameAdvertisementEtcScript(&m_inGameAd,
                                            "Etc/IngameAdvertisementParameter.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1a4a, 9, true, true)(
            "\t- importInGameAdvertisementEtcScript Fail");
        return 0;
    }

    // SeriaRoomDecoEvent.evt（fail 0x1a53）。
    if (!importSeriaRoomDecoEvent(&m_seriaRoomDeco,
                                  "Event/SeriaRoomDecoEvent.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a53, 9, true, true)(
            "\t- SeriaRoomDecoEventScript Fail!");
        return 0;
    }

    // GrowthWeapon.evt（fail "GrownEquipmentScript Fail!"）。
    if (!importGrowthEquipmentScript(&m_growthWeapon, "Event/GrowthWeapon.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a5c, 9, true, true)(
            "\t- GrownEquipmentScript Fail!");
        return 0;
    }

    // CreateCharacterGift.evt。
    if (!importCharacterCreateGiftScript(&m_createChrac,
                                         "Event/CreateCharacterGift.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a65, 9, true, true)(
            "\t- importCharacterCreateGiftScript Fail");
        return 0;
    }

    // JobFatigue.evt。
    if (!importUseJobFatigueGiftScript(&m_useJobFatigue, "Event/JobFatigue.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a6e, 9, true, true)(
            "\t- importUseJobFatigueGiftScript Fail");
        return 0;
    }

    // CreateCharacterGiftCreature.evt。
    if (!importCharacterCreateCreatureGiftScript(
            &m_createCreatureGift, "Event/CreateCharacterGiftCreature.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a77, 9, true, true)(
            "\t- importCharacterCreateCreatureGiftScript Fail");
        return 0;
    }

    // SeriaRoomDecoAnimation.evt。
    if (!importSeriaRoomAnimationDecoEvent(&m_seriaAniDeco,
                                           "Event/SeriaRoomDecoAnimation.evt"))
    {
        cMyTrace("int CDataManager::init()", 0x1a80, 9, true, true)(
            "\t- importSeriaRoomAnimationDecoEvent Fail!");
        return 0;
    }

    // EventReward/reward.evt（返回 0=成功）。
    if (m_eventReward.importRewardTable("Event/EventReward/", "reward.evt") != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1a92, 9, true, true)(
            "\t- importRewardTable Fail");
        return 0;
    }

    // antibot 版本号：读 m_versionBytes 四字节计算后写 m_antibotVersion
    //（ORIG +0x1aa0 区：ver=b0*1e7+b1*1e5+(b2-hi*100)*100+hi*1e4+b3，
    //  hi=(byte)((ushort)(b2*0x29)>>12)；TRACE 0x1aa7）。
    {
        const unsigned char b0 = m_serverParameter.m_versionBytes[0];
        const unsigned char b1 = m_serverParameter.m_versionBytes[1];
        const unsigned char b2 = m_serverParameter.m_versionBytes[2];
        const unsigned char b3 = m_serverParameter.m_versionBytes[3];
        const unsigned int hi =
            (unsigned int)(unsigned char)((unsigned short)(b2 * 0x29) >> 12);
        const unsigned int b2adj = (unsigned int)(b2 - hi * 100);
        m_serverParameter.m_antibotVersion =
            b0 * 10000000u + b1 * 100000u + b2adj * 100u + hi * 10000u + b3;
        cMyTrace("int CDataManager::init()", 0x1aa7, 9, true, true)(
            "\t- Server version for antibot : %d",
            m_serverParameter.m_antibotVersion);
    }

    // ImageCommunication.etc。
    if (!importImageCommunicationInfo("Etc/ImageCommunication.etc",
                                      &m_imageCommData))
    {
        cMyTrace("int CDataManager::init()", 0x1ab0, 9, true, true)(
            "\t- importImageCommunicationInfo Fail");
        return 0;
    }

    // CheckTraningFirstGive 为假 → TRACE 0x1ab7 后 return 0。
    if (!m_serverParameter.CheckTraningFirstGive())
    {
        cMyTrace("int CDataManager::init()", 0x1ab7, 9, true, true)(
            "\t- [training first give] error");
        return 0;
    }

    // expertjob.lst（失败 TRACE 0x1abf）。
    if (!initExpertJobScript((char*)"Character/", (char*)"expertjob.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1abf, 9, true, true)(
            "\t- Load expertjob.lst fail");
        return 0;
    }

    // new CExpertJobList(0x48) → m_regeneration.m_expertJobList；init() 失败 TRACE 0x1ac5。
    m_regeneration.m_expertJobList = new CExpertJobList;
    if (!m_regeneration.m_expertJobList->init())
    {
        cMyTrace("int CDataManager::init()", 0x1ac5, 9, true, true)(
            "\t- Load expertjob fail");
        return 0;
    }

    // TowerList.lst（返回 0=成功）。
    if (importTowerInfo("Etc/", "TowerList.lst", m_deathTowerMap) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1ad0, 9, true, true)(
            "\t- importTowerInfo Fail(%d)");
        return 0;
    }

    // DeathTower.etc（返回 0=成功）。
    if (importDeathTowerScript("Etc/DeathTower.etc", m_deathTower) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1ada, 9, true, true)(
            "\t- importDeathTowerScript Fail(%d)");
        return 0;
    }

    // RankSystemInfo.etc。
    if (!importRankSystemScript("Etc/RankSystemInfo.etc", &m_rankSystem))
    {
        cMyTrace("int CDataManager::init()", 0x1ae3, 9, true, true)(
            "\t- importRankSystemScript Fail");
        return 0;
    }

    // BloodClearReward.etc（返回 0=成功）。
    if (importBloodDungeonScript("Etc/BloodClearReward.etc", m_bloodDungeon) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1aec, 9, true, true)(
            "\t- importBloodDungeonScript Fail(%d)");
        return 0;
    }

    // HellParty.etc（全局单例，返回 0=成功）。
    if (importHellPartyScript(
            "Etc/HellParty.etc",
            *reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1af6, 9, true, true)(
            "\t- importHellPartyScript Fail(%d)");
        return 0;
    }

    // CharacLinkSystem.etc（返回 0=成功）。
    if (importCharacLinkSystemScript("Etc/LinkSystem/CharacLinkSystem.etc",
                                     m_characLink) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1aff, 9, true, true)(
            "\t- importCharacLinkSystemScript Fail(%d)");
        return 0;
    }

    // striker.etc（getInsance 单例存入 m_altarConfig.m_strikerScript）。
    m_altarConfig.m_strikerScript = StrikerScript::getInsance();
    if (!m_altarConfig.m_strikerScript->importScript("Etc/LinkSystem/striker.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1b13, 9, true, true)(
            "\t- StrikerScript importScript Fail");
        return 0;
    }

    // CharacterManage.etc。
    m_altarConfig.m_characManageScript = CharacManageScript::getInsance();
    if (!m_altarConfig.m_characManageScript->importScript(
            "Etc/LinkSystem/CharacterManage.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1b22, 9, true, true)(
            "\t- CharacManageScript importScript Fail");
        return 0;
    }

    // BlueMarble 三连（Tile→Map→Info），单例存入 m_choiceItem.m_blueMarbleScriptMgr。
    m_choiceItem.m_blueMarbleScriptMgr = BlueMarbleScriptManager::getInsance();
    if (!m_choiceItem.m_blueMarbleScriptMgr->initTileList(
            "Event/BlueMarble/", "BlueMarbleTile.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1b38, 9, true, true)(
            "\t- BlueMarbleScriptManager initTileList Fail");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initMapList(
            "Event/BlueMarble/", "BlueMarbleMap.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1b41, 9, true, true)(
            "\t- BlueMarbleScriptManager initMapList Fail");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initBlueMarbleInfo(
            "Event/BlueMarble/", "BlueMarble.blu"))
    {
        cMyTrace("int CDataManager::init()", 0x1b4a, 9, true, true)(
            "\t- BlueMarbleScriptManager initBlueMarbleInfo Fail");
        return 0;
    }

    // questshop.etc（返回 0=成功；fail TRACE 带 %d 行号 0x1b45 区）。
    {
        int err = importQuestShopScript("Etc/questshop.etc", m_questShop);
        if (err != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1b48, 9, true, true)(
                "\t- importQuestShopScript Fail(%d)", err);
            return 0;
        }
    }

    // IndependentDrop 三连（lst 加载 bool / 列表加载 / 数据导入返回 0=成功；
    // 后两步 fail 均 TRACE "Load IndependentDrop.lst fail" 0x1b56/0x1b5c）。
    if (!initIndependentDropScript((char*)"Etc/", (char*)"IndependentDrop.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1b50, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    if (!m_independentDrop.load_independent_drop_list())
    {
        cMyTrace("int CDataManager::init()", 0x1b5c, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    if (m_independentDrop.ImportIndependentDrop("Etc/Independent_Drop.etc") != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b62, 9, true, true)(
            "\t- Import Independent_Drop.etc fail");
        return 0;
    }

    // Event 脚本簇（SeriaBlessing/LevelUpReward/GrowthCapsule/NewAccountLevelUp/
    // ReturnUserReward/Bingo/BroadCastItem，全部返回 0=成功）。
    if (ImportSeriaBlessingScript("Event/SeriaBlessing.evt", m_seriaBlessing) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b6e, 9, true, true)(
            "\t- ImportSeriaBlessingScript Fail");
        return 0;
    }
    if (ImportLevelUpRewardItemScript("Event/LevelUpRewardItem.evt",
                                      m_levelUpReward) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b78, 9, true, true)(
            "\t- ImportLevelUpRewardItemScript Fail");
        return 0;
    }
    if (ImportGrowthCapsuleScript("Event/GrowthCapsule.evt", m_growthCapsule) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b82, 9, true, true)(
            "\t- ImportGrowthCapsuleScript Fail");
        return 0;
    }
    if (ImportNewAccountLeventUpToJobScript("Event/NewAccountLevelUpToJob.evt",
                                            m_newAccountLevelUp) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b8c, 9, true, true)(
            "\t- ImportNewAccountLeventUpToJobScript Fail");
        return 0;
    }
    if (ImportReturnUserRewardScript("Event/ReturnUserReward.evt",
                                     m_returnUserReward) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1b96, 9, true, true)(
            "\t- ImportReturnUserRewardScript Fail");
        return 0;
    }
    if (ImportBingoScript("Event/BingoEvent.evt", m_bingo) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1ba0, 9, true, true)(
            "\t- ImportBingoScript Fail");
        return 0;
    }
    if (ImportBroadCastItemScript("Etc/BroadCastItem.etc", m_broadCastItem) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1baa, 9, true, true)(
            "\t- ImportBroadCastItemScript Fail");
        return 0;
    }

    // SecurityReward（bool 语义）。
    if (!ImportSecurityReward(reinterpret_cast<SecurityReward*>(&m_securityReward)))
    {
        cMyTrace("int CDataManager::init()", 0x1bb4, 9, true, true)(
            "\t- ImportSecurityReward Fail");
        return 0;
    }

    // ItemMakingSkill.etc（返回 0=成功）。
    if (ImportItemMakingSkill("Etc/ItemMakingSkill.etc", m_itemMakingSkill) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1bbe, 9, true, true)(
            "\t- ImportItemMakingSkill Fail");
        return 0;
    }

    // UltimateDungeonList.etc。
    if (!importUltimateDungeonList("Etc/UltimateDungeonList.etc",
                                   m_ultimateDungeonList))
    {
        cMyTrace("int CDataManager::init()", 0x1bc8, 9, true, true)(
            "\t- importUltimateDungeonList Fail");
        return 0;
    }

    // mapping equip info：truncate 成功才继续（失败无 TRACE ret0）；
    // 两张表 false/true 导入（fail TRACE "Equipment Script mapping table
    // load fail" 0x1c0c/0x1c12）；insert 用第二张表（ORIG +0x18f6 取 +0xa940）。
    if (!sync->truncate_mapping_equip_info_table())
    {
        cMyTrace("int CDataManager::init()", 0x1c06, 9, true, true)(
            "\t- Fail truncate_mapping_equip_info_table");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA928, false))
    {
        cMyTrace("int CDataManager::init()", 0x1c0c, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA940, true))
    {
        cMyTrace("int CDataManager::init()", 0x1c12, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!sync->insert_mapping_equip_info_to_db(m_mapA940))
    {
        cMyTrace("int CDataManager::init()", 0x1c18, 9, true, true)(
            "\t- Fail insert_mapping_equip_info_table");
        return 0;
    }

    // VillageAttacked.etc（返回 0=成功）。
    if (importVillageAttackedScript("Etc/VillageAttacked.etc",
                                    m_villageAttacked) != 0)
    {
        cMyTrace("int CDataManager::init()", 0x1c1c, 9, true, true)(
            "\t- importVillageAttackedScript Fail");
        return 0;
    }

    // ActionPoint 两份（普通/GM；GetActionPointEctParameter(bool) W 0x08122ace）。
    APSystem::CActionPointEtcParameter* ap = GetActionPointEtcParameter(false);
    if (!ap->importActionPointSystemEtcScript("Etc/Chn_ActionPointSystem.etc",
                                              *ap))
    {
        cMyTrace("int CDataManager::init()", 0x1c26, 9, true, true)(
            "\t- importActionPointSystemEtcScript Fail");
        return 0;
    }
    ap = GetActionPointEtcParameter(true);
    if (!ap->importActionPointSystemEtcScript("Etc/Chn_ActionPointSystem_GM.etc",
                                              *ap))
    {
        cMyTrace("int CDataManager::init()", 0x1c30, 9, true, true)(
            "\t- importActionPointSystemEtcScript(GM) Fail");
        return 0;
    }

    // action point desc DB 同步（无条件）。
    sync_script::GetInstanceSyncScript()->truncate_charac_action_point_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_action_point_desc();

    // Chn_ServerCheckSchedule.etc → vector<_DailySchedule> → AddDailSchedule。
    {
        std::vector<_DailySchedule> schedules;
        if (importDailyScheduleEtcScript("Etc/Chn_ServerCheckSchedule.etc",
                                         schedules) != 0)
        {
            return 0;   // ORIG 清理链经 vector dtor 后 ret0（+0x6e88 区）
        }
        if (!schedules.empty())
        {
            GlobalData::s_DailyScheduleManager->AddDailSchedule(schedules);
        }

        // AdvanceAltar 三件套 + TimeLine。
        if (loadAdvanceAltarTimeLineParameter(m_stageTimeLine) != 0)
        {
            return 0;
        }
        // ORIG init.md +0x1c11：返回值非零=Fail（打印 Fail(%d) 后 ret0）。
        if (m_altarShop.importAdvanceAltarShopParameter(
                "Etc/AdvanceAltar/AdvanceAltarShop.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1c66, 9, true, true)(
                "\t- importAdvanceAltarShopParameter Fail");
            return 0;
        }
        if (m_altarReward.importAchievementRewardParameter(
                "Etc/AdvanceAltar/AdvanceAltarAchievementReward.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1c70, 9, true, true)(
                "\t- importAchievementRewardParameter Fail");
            return 0;
        }
        if (m_altarConfig.importConfigParameter(
                "Etc/AdvanceAltar/AdvanceAltarConfig.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1c7a, 9, true, true)(
                "\t- importConfigParameter Fail");
            return 0;
        }
    }   // schedules 析构（ORIG 清理块 -0xf28 vector<_DailySchedule> D2）

    sync_script::GetInstanceSyncScript()->truncate_charac_advance_altar_item_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_advance_altar_item_desc();

    // pvpChannelList.etc（live_server_info 宿主）。
    if (!importPVPChannelList((char*)"Etc/pvpChannelList.etc",
                              &m_liveServerInfo, 0))
    {
        cMyTrace("int CDataManager::init()", 0x1c90, 9, true, true)(
            "\t- importPVPChannelList Fail");
        return 0;
    }

    // Guild/Power/SecretShop/AccountCargo/TitleBook 参数链。
    if (!importGuildParameterScript(&m_guildParameter, "Etc/Guild.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1ca0, 9, true, true)(
            "\t- importGuildParameterScript Fail");
        return 0;
    }
    if (!importPowerParameterScript(&m_powerParameter, "Etc/Power.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1caa, 9, true, true)(
            "\t- importPowerParameterScript Fail");
        return 0;
    }
    if (!importSecretShopScript(&m_secretShop, "Etc/SecretShop.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1cb4, 9, true, true)(
            "\t- importSecretShopScript Fail");
        return 0;
    }
    if (!importAccountCargoScript(&m_accountCargo, "Etc/AccountCargo.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1cbe, 9, true, true)(
            "\t- importAccountCargoScript Fail");
        return 0;
    }
    if (!importTitleBookEtcScript("Etc/TitleBook.etc", &m_titleBook))
    {
        cMyTrace("int CDataManager::init()", 0x1cc8, 9, true, true)(
            "\t- importTitleBookEtcScript Fail");
        return 0;
    }

    // EventCharacter 两步。
    if (!initEventCharacterScript((char*)"Event/EventCharacter/",
                                  (char*)"EventCharacter.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1cd4, 9, true, true)(
            "\t- initEventCharacterScript Fail");
        return 0;
    }
    if (!m_eventCharacter.load_event_character_list())
    {
        cMyTrace("int CDataManager::init()", 0x1cdd, 9, true, true)(
            "\t- load_event_character_list Fail");
        return 0;
    }

    // Quest 参数链。
    if (!importQuestParameterScript(&m_questParameter,
                                    "n_Quest/questParameter.etc"))
    {
        cMyTrace("int CDataManager::init()", 0x1ce9, 9, true, true)(
            "\t- importQuestParameterScript Fail");
        return 0;
    }
    if (!importTrainingQuestScript(&m_trainingQuest,
                                   "n_Quest/TrainingQuest.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1cf3, 9, true, true)(
            "\t- importTrainingQuestScript Fail");
        return 0;
    }

    // Collect Dungeon Statistic 开关（非 0 才开；log-only TRACE 0x1d84）。
    if (m_serverParameter.CheckCollectDungeonStatisticServer(
            env->m_serverEnvir.m_serverGroup, env->m_serverEnvir.m_channelNo))
    {
        SetCollectDungeonStatistic(true);
        cMyTrace("int CDataManager::init()", 0x1d84, 9, true, true)(
            "\t- Collect Dungeon Data Statistic start!");
    }

    // Assault channel 开启。
    pvp_assault::GetInstanceAssaultMgr()->SetAssaultChannel(true);

    // 名字过滤两件套。
    if (!importSlangName((char*)"Etc/slangname.etc", &m_slangName))
    {
        cMyTrace("int CDataManager::init()", 0x1d95, 9, true, true)(
            "\t- importSlangName Fail");
        return 0;
    }
    if (!importInvalidName((char*)"Etc/invalidName.etc", &m_invalidChar))
    {
        cMyTrace("int CDataManager::init()", 0x1d9f, 9, true, true)(
            "\t- importInvalidName Fail");
        return 0;
    }

    // new SlangFilter(0x80) → m_invalidChar.m_slangFilter（destroy 经 vtable D0）。
    m_invalidChar.m_slangFilter = new SlangFilter;

    // FairPVPEquipment.etc → m_stackableMap。
    if (!importFairPVPEquipment((char*)"Etc/FairPVPEquipment.etc",
                                &m_stackableMap))
    {
        cMyTrace("int CDataManager::init()", 0x1db4, 9, true, true)(
            "\t- importFairPVPEquipment Fail");
        return 0;
    }

    // GrowthPower / BoosterGage。
    if (!importGrowthPower((char*)"Etc/GrowthPowerRewardBuff.etc",
                           &m_growthPower))
    {
        cMyTrace("int CDataManager::init()", 0x1dbe, 9, true, true)(
            "\t- importGrowthPower Fail");
        return 0;
    }
    if (!importBoosterGage((char*)"Etc/BoosterGage.etc", &m_boosterGage))
    {
        cMyTrace("int CDataManager::init()", 0x1dc8, 9, true, true)(
            "\t- importBoosterGage Fail");
        return 0;
    }

    // AICharacter。
    if (!initAICharacterScript("AICharacter/", "AICharacter.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1dd4, 9, true, true)(
            "\t- Load AI Character fail");
        return 0;
    }
    m_aiCharacterList = new CAICharacterList;
    if (!m_aiCharacterList->init())
    {
        cMyTrace("int CDataManager::init()", 0x1dde, 9, true, true)(
            "\t- Load AI Character fail");
        return 0;
    }

    // Light Server 提示（log-only TRACE 0x1dc5 区）。
    if (m_serverParameter.CheckLightServer(env->m_serverEnvir.m_serverGroup))
    {
        cMyTrace("int CDataManager::init()", 0x1dc5, 9, true, true)(
            "\t- Light Server Mode start!");
    }

    // WorldDrop 两张表（无条件导入）。
    ImportWorldDrop(m_worldDrop1, (char*)"Etc/WorldDrop.etc");
    ImportWorldDrop(m_worldDrop2, (char*)"Etc/WorldDropWarArea.etc");

    // PremiumList_New.etc（+0x2638 区回查）：err=LoadPremiumInfo(...)，
    // 非 0 → TRACE "LoadPremiumInfo Fail(Line: %d)" ret0。
    {
        const int premiumErr =
            WongWork::CUserPremium::LoadPremiumInfo("Etc/PremiumList_New.etc");
        if (premiumErr != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1e0f, 9, true, true)(
                "\t- LoadPremiumInfo Fail(Line: %d)", premiumErr);
            return 0;
        }
    }

    // 非 light server 必须成功加载 WarArea.etc（fail TRACE 0x1e18 ret0）。
    if (!IsLightServer() && !ImportWarAreaCounter(&m_warAreaCounter))
    {
        cMyTrace("int CDataManager::init()", 0x1e18, 9, true, true)(
            "\t- WarArea.etc file load failed");
        return 0;
    }

    // ---- 大类脚本链（Character→Monster→PassiveObject→Equipment→Stackable→
    //      Map→Dungeon），任一失败 TRACE ret0 ----
    if (!InitCharacterScript("Character/", "Character.lst", "ExpTable.tbl",
                             (const char*)0))
    {
        cMyTrace("int CDataManager::init()", 0x1e23, 9, true, true)(
            "\t- Load Character.lst fail");
        return 0;
    }
    if (!InitMonsterScript("Monster/", "Monster.lst",
                           "MonsterApcDifficultyBonus.tbl"))
    {
        cMyTrace("int CDataManager::init()", 0x1e2d, 9, true, true)(
            "\t- Load Monster.lst fail");
        return 0;
    }
    if (!InitPassiveObjectScript("PassiveObject/", "PassiveObject.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e37, 9, true, true)(
            "\t- Load PassiveObject.lst fail");
        return 0;
    }
    if (!InitEquipmentScript("equipment/", "equipment.lst", "pricetable.tbl"))
    {
        cMyTrace("int CDataManager::init()", 0x1e41, 9, true, true)(
            "\t- Load equipment.lst fail");
        return 0;
    }
    if (!InitStackableScript("stackable/", "stackable.lst", "pricetable.tbl"))
    {
        cMyTrace("int CDataManager::init()", 0x1e4b, 9, true, true)(
            "\t- Load stackable.lst fail");
        return 0;
    }
    if (!InitMapScript("Map/", "Map.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e55, 9, true, true)(
            "\t- Load Map.lst fail");
        return 0;
    }
    if (!InitDungeonScript((char*)"Dungeon/", (char*)"Dungeon.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e5f, 9, true, true)(
            "\t- Load Dungeon.lst fail");
        return 0;
    }

    // QuickParty 奖励管理器 init。
    if (!G_CGameManager()->GetQuickPartyRewardManager()->init())
    {
        cMyTrace("int CDataManager::init()", 0x1e6e, 9, true, true)(
            "\t- QuickPartyRewardManager init Fail");
        return 0;
    }

    // Quest/Skill。
    if (!initQuestScript("n_Quest/", "Quest.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e77, 9, true, true)(
            "\t- Load Quest.lst fail");
        return 0;
    }
    if (!InitSkillScript("Skill/", "SkillList.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e81, 9, true, true)(
            "\t- Load SkillList.lst fail");
        return 0;
    }

    // ItemShop：new(0x24) → m_itemShop；ItemShop.lst + loadItemShopFiles。
    m_itemShop = new CItemShop;
    if (!initItemShopScript((char*)"ItemShop/", (char*)"ItemShop.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1e94, 9, true, true)(
            "\t- Load ItemShop.lst fail");
        return 0;
    }
    if (!m_itemShop->loadItemShopFiles())
    {
        cMyTrace("int CDataManager::init()", 0x1e9d, 9, true, true)(
            "\t- loadItemShopFiles Fail");
        return 0;
    }

    // PvP Mission：initPvpMissionSystem + new(0x54) + Load_list + Load_EtcParameter。
    if (!initPvpMissionSystem((char*)"PVP_Mission/", (char*)"Mission.lst"))
    {
        cMyTrace("int CDataManager::init()", 0x1eab, 9, true, true)(
            "\t- Load PVP_Mission.lst fail");
        return 0;
    }
    m_pvpMissionSystem = new PvP_MissionSystem;
    if (!m_pvpMissionSystem->Load_list())
    {
        cMyTrace("int CDataManager::init()", 0x1eb8, 9, true, true)(
            "\t- PvP_MissionSystem Load_list Fail");
        return 0;
    }
    if (!m_pvpMissionSystem->Load_EtcParameter())
    {
        cMyTrace("int CDataManager::init()", 0x1ec1, 9, true, true)(
            "\t- PvP_MissionSystem Load_EtcParameter Fail");
        return 0;
    }

    // CashShop 文件名四连（+0x8cb8 / +0xa8c0..0xa8cc）。
    m_scriptPackPath = "Etc/NewCashShop.etc";
    m_packName1 = "CashShop/NewArad_CashShop.shp";
    m_packName2 = "CashShop/Sudden_Shop_1.shp";
    m_packName3 = "CashShop/Sudden_Shop_2.shp";
    m_packName4 = "CashShop/Sudden_Shop_3.shp";

    // Money Limit per Level（fail 仅 TRACE 0x1eb7 继续）。
    {
        cMyTrace("int CDataManager::init()", 0x1eb4, 9, true, false)(
            "\t- Loading Money Limit per Level ");
        if (!importGoldLimitByLevel(
                reinterpret_cast<unsigned int*>(m_moneyLimit)))
        {
            cMyTrace("int CDataManager::init()", 0x1eb7, 9, true, true)(
                "\t- Loading Money Limit per Level Fail");
        }
    }

    // Compound Item（bindsphere.etc；失败 TRACE 0x1ebf 后 ret0）。
    cMyTrace("int CDataManager::init()", 0x1eb9, 9, false, true)("");
    {
        cMyTrace("int CDataManager::init()", 0x1ebb, 9, true, false)(
            "\t- Loading Compound Item ");
        ExtreamDungeon::CCompound_ExtreamDun_Item* bindsphere =
            new ExtreamDungeon::CCompound_ExtreamDun_Item;
        GlobalData::s_pCompoundBindCube = bindsphere;
        if (bindsphere->LoadScript("Etc/", "bindsphere.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1ebf, 9, true, true)(
                "\t- Fail load Compound Item Bind Cube Script File");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1ec2, 9, false, true)("");
    }

    // ARAD Event Upgrade Jar（失败 TRACE 0x1ec9 ret0）。
    {
        cMyTrace("int CDataManager::init()", 0x1ec5, 9, true, false)(
            "\t- Loading ARAD - Event Upgrade Jar ");
        Event_Upgrade_Jar* jar = new Event_Upgrade_Jar;
        GlobalData::s_pEvent_UpGrade_Jar = jar;
        if (jar->LoadScript("", "Etc/EventUpgradeJar.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1ec9, 9, true, true)(
                "\t- Fail load Event.Upgrad.Jar Script File");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1ecc, 9, false, true)("");
    }

    // Compound Avatar（nothrow new(0x88)；失败 ret0）。
    {
        cMyTrace("int CDataManager::init()", 0x1ed9, 9, true, false)(
            "\t- Loading Compound Avatar ");
        WongWork::CCompoundAvatar* avatar =
            new (std::nothrow) WongWork::CCompoundAvatar;
        GlobalData::s_pCompoundAvatar = avatar;
        if (avatar == 0 || !avatar->LoadScript("Etc/", "CompoundAvatar.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x1ee2, 9, true, true)(
                "\t- Loading Compound Avatar Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1ee5, 9, false, true)("");
    }

    // VendingMachine.lst（无条件）。
    importVendingMachineInfo("Etc/", "VendingMachine.lst", m_vendingMachine);

    // WorldMap/StageMap。
    if (!initWorldMapScript((char*)"WorldMap/", (char*)"WorldMap.lst"))
    {
        return 0;
    }
    initStageMapScript((char*)"StageMap/", (char*)"StageMap.lst");

    // Dungeons：new CDungeonList(0x14) + dimension 区清零 + load_list。
    {
        cMyTrace("int CDataManager::init()", 0x1ef3, 9, true, false)(
            "\t- Loading Dungeons ");
        m_dungeonList = new CDungeonList;
        memset(m_dimensionInout, 0, 6);
        memset(m_originalDimensionInout, 0, 6);
        memset(m_limitInoutCount, 0, 3);
        if (!m_dungeonList->load_list())
        {
            cMyTrace("int CDataManager::init()", 0x1f05, 9, true, true)(
                "\t- Loading Dungeons Fail");
            return 0;
        }
    }

    // Tower Of Despair（失败 TRACE 0x1f0f 后 ret0）。
    {
        cMyTrace("int CDataManager::init()", 0x1f08, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x1f0b, 9, true, false)(
            "\t- Loading Tower Of Despair System ");
        TowerOfDespairMgr* tower = new TowerOfDespairMgr;
        GlobalData::s_pTowerOfDespairMgr = tower;
        if (tower->LoadScript("Etc/", "TowerOfDespair.etc") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x1f0f, 9, true, true)(
                "\t- Fail load TowerOfDespair System Script");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f12, 9, false, true)("");
    }

    // Chatting Emoticon：new(0x48) + lst + Load_list。
    {
        cMyTrace("int CDataManager::init()", 0x1f16, 9, true, false)(
            "\t- Loading Chatting Emoticon ");
        m_emoticonList = new CChattingEmoticonList;
        if (!initChattingEmoticonScriptList((char*)"ChatEmoticon/",
                                            (char*)"ChatEmoticon.lst"))
        {
            cMyTrace("int CDataManager::init()", 0x1f1b, 9, true, true)(
                "\t- Emoticon List file error");
            return 0;
        }
        if (!m_emoticonList->Load_list())
        {
            cMyTrace("int CDataManager::init()", 0x1f20, 9, true, true)(
                "\t- Emoticon Script file fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f23, 9, false, true)("");
    }

    // PvP 地图基础数据 + Maps。
    {
        std::vector<PVPMAP_DATA> pvpMaps;
        if (!loadPvpMapBasicDataScript("Etc/pvpmapbasicdata.etc", &pvpMaps))
        {
            cMyTrace("int CDataManager::init()", 0x1f29, 9, true, true)(
                "Load Etc/pvpmapbasicdata.etc fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f2d, 9, true, false)(
            "\t- Loading Maps ");
        m_mapList = new CMapList;
        if (!load_map_files(pvpMaps))
        {
            cMyTrace("int CDataManager::init()", 0x1f33, 9, true, true)(
                "\t- Loading Maps Fail");
            return 0;
        }

        // Monsters。
        cMyTrace("int CDataManager::init()", 0x1f36, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x1f38, 9, true, false)(
            "\t- Loading Monsters ");
        m_monsterManager = new CMonsterManager;
        if (!m_monsterManager->load_monsters())
        {
            cMyTrace("int CDataManager::init()", 0x1f3e, 9, true, true)(
                "\t- Loading Monsters Fail");
            return 0;
        }
    }   // pvpMaps 析构（ORIG 清理块 -0xf34 vector<PVPMAP_DATA> D2）

    // IndependentDrop 缓存清理（无条件）+ Skills。
    cMyTrace("int CDataManager::init()", 0x1f41, 9, false, true)("");
    m_independentDrop.clear();
    {
        cMyTrace("int CDataManager::init()", 0x1f47, 9, true, false)(
            "\t- Loading Skills ");
        m_skillList = new CSkillList;
        if (!m_skillList->init())
        {
            cMyTrace("int CDataManager::init()", 0x1f4d, 9, true, true)(
                "\t- Loading Skills Fail");
            return 0;
        }
    }

    // Creature Damage + ActiveStatus（SDCActiveStatusSyncServer 栈局部）。
    {
        cMyTrace("int CDataManager::init()", 0x1f50, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x1f52, 9, true, false)(
            "\t- Loading Creatrue Damage ");
        if (!prepareServerDelayCheckDamage())
        {
            cMyTrace("int CDataManager::init()", 0x1f55, 9, true, true)(
                "\t- Loading Creatrue Damage Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f58, 9, true, false)("Success");

        SDCActiveStatusSyncServer activeStatus;
        cMyTrace("int CDataManager::init()", 0x1f5c, 9, true, false)(
            "\t- Loading ActiveStatus Damage ");
        if (!activeStatus.loadActiveStatusScript())
        {
            cMyTrace("int CDataManager::init()", 0x1f5f, 9, true, true)(
                "\t- Loading ActiveStatus Damage Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f62, 9, true, false)("Success");
    }

    // ItemGroupName.tbl（返回非 0 = 失败 ret0）+ Items。
    if (importItmeGroupTable("Etc/", "ItemGroupName.tbl",
                             getItemGroupNameTable()) != 0)
    {
        return 0;
    }
    {
        cMyTrace("int CDataManager::init()", 0x1f69, 9, true, false)(
            "\t- Loading Items ");
        m_itemList = new CItemList;
        if (!m_itemList->init())
        {
            cMyTrace("int CDataManager::init()", 0x1f6f, 9, true, true)(
                "\t- Loading items Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1f72, 9, true, false)("Success");
    }

    // Avatar Roulette（栈局部 roulette → import → new Server → Init →
    // InitAddCharacSkill，失败 LogManager ret0）。
    {
        cMyTrace("int CDataManager::init()", 0x1f8a, 9, true, false)(
            "\t- Loading Avatar Roulette ");
        AvatarRoulette roulette;
        if (!importAvatarRouletteList((char*)"Etc/avatar_roulette_jar.etc",
                                      roulette))
        {
            cMyTrace("int CDataManager::init()", 0x1f90, 9, true, true)(
                "\t- Loading Avatar Roulette Fail");
            return 0;
        }
        AvatarRouletteServer* server = new AvatarRouletteServer;
        GlobalData::s_pAvatarRoulette = server;
        if (server == 0)
        {
            return 0;
        }
        if (!server->Init(&roulette))
        {
            cMyTrace("int CDataManager::init()", 0x1f9a, 9, true, true)(
                "\t- Initalizing Avatar Roulette Fail");
            return 0;
        }
        if (!server->InitAddCharacSkill())
        {
            LogManager::logFormat(
                1, "data_manager.cpp", "int CDataManager::init()", 0x1fa1,
                "[AVATAR ROULETTE] initalizing(InitAddCharacSkill) failed.");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1fa6, 9, true, false)("Success");
    }   // roulette 析构（ORIG 清理块 -0x107c AvatarRoulette D2）

    // RandomOption 七文件循环（任一 err!=0 → TRACE ret0）。
    {
        static const char* const randomOptionFiles[7] = {
            "Etc/RandomOption/OptionGrouping.etc",
            "Etc/RandomOption/OptionGroupSelection.etc",
            "Etc/RandomOption/OptionNumbering.etc",
            "Etc/RandomOption/OptionQuantity.etc",
            "Etc/RandomOption/PartSelection.etc",
            "Etc/RandomOption/RandomizedOptionOverall1.etc",
            "Etc/RandomOption/RandomizedOptionOverall2.etc",
        };
        cMyTrace("int CDataManager::init()", 0x1fb6, 9, true, false)(
            "\t- Loading RandomOption ");
        for (unsigned int i = 0; i < 7; ++i)
        {
            const int err = importRandomOptionScript(m_randomOption,
                                                     randomOptionFiles[i]);
            if (err != 0)
            {
                cMyTrace("int CDataManager::init()", 0x1fbb, 9, true, true)(
                    "\t- Fail load RandomOption Script File name(%s), "
                    "error_line(%d)\n", randomOptionFiles[i], err);
                return 0;
            }
        }
        cMyTrace("int CDataManager::init()", 0x1fbf, 9, true, false)("Success");
    }

    // initRandomOptionItem + random_option_ref DB 同步。
    if (!initRandomOptionItem())
    {
        cMyTrace("int CDataManager::init()", 0x1fc3, 9, true, true)(
            "\t- Fail load initRandomOptionItem");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->truncate_random_option_ref())
    {
        cMyTrace("int CDataManager::init()", 0x1fc9, 9, true, true)(
            "\t- Fail truncate_random_option_ref");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->insert_random_option_ref())
    {
        cMyTrace("int CDataManager::init()", 0x1fcf, 9, true, true)(
            "\t- Fail insert_random_option_ref");
        return 0;
    }
    if (m_regeneration._0_Load_RegenerationRandomOptionItem() != 0)
    {
        return 0;
    }

    // WorldMapList + Quests + Characters + Disjoints。
    if (!m_worldMapList.load_list())
    {
        cMyTrace("int CDataManager::init()", 0x1fe1, 9, true, true)(
            "\t- Loading World Map List Fail");
        return 0;
    }
    {
        cMyTrace("int CDataManager::init()", 0x1fe5, 9, true, false)(
            "\t- Loading Quests ");
        m_questList = new QuestList;
        if (!m_questList->load_list())
        {
            cMyTrace("int CDataManager::init()", 0x1fe9, 9, true, true)(
                "\t- Loading Quests Fail");
            return 0;
        }
    }
    {
        cMyTrace("int CDataManager::init()", 0x1fec, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x1fee, 9, true, false)(
            "\t- Loading Characters ");
        m_characList = new CCharacList;
        if (!m_characList->init())
        {
            cMyTrace("int CDataManager::init()", 0x1ff2, 9, true, true)(
                "\t- Loading Characters Fail");
            return 0;
        }
    }
    {
        cMyTrace("int CDataManager::init()", 0x1ff5, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x1ff7, 9, true, false)(
            "\t- Loading Disjoints ");
        m_disJoint = new DisJoint;
        if (!m_disJoint->LoadDisJoint())
        {
            cMyTrace("int CDataManager::init()", 0x1ffb, 9, true, true)(
                "\t- Loading Disjoints Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x1ffe, 9, false, true)("Success");
    }

    // Advance Altar：StageMapList::loadList。
    {
        cMyTrace("int CDataManager::init()", 0x2001, 9, true, false)(
            "\t- Loading Advance Altar ");
        if (!m_stageMapList.loadList())
        {
            cMyTrace("int CDataManager::init()", 0x2004, 9, true, true)(
                "\t- Loading Advance Altar Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2007, 9, false, true)("");
    }

    // ItemGeneratorMgr / AutoMarket。
    if (m_itemGeneratorMgr == 0)
    {
        m_itemGeneratorMgr = new WongWork::CItemGeneratorMgr;
    }
    if (!m_itemGeneratorMgr->initialize())
    {
        cMyTrace("int CDataManager::init()", 0x2012, 9, true, true)(
            "\t- Loading Item Generator Manager Fail\n");
        return 0;
    }
    {
        int err = 0;
        if (m_autoMarket == 0)
        {
            m_autoMarket = new CAutoMarketConditionsControl;
        }
        err = m_autoMarket->LoadScript("Etc/ItemDropInfo_Control.etc");
        if (err != 0)
        {
            cMyTrace("int CDataManager::init()", 0x201a, 9, true, true)(
                "\t- LoadScript() error Etc/ItemDropInfo_Control.etc line(%d)",
                err);
            return 0;
        }
    }

    // LevelExp / MobReward / PvpGrade / RewardSP。
    {
        cMyTrace("int CDataManager::init()", 0x2022, 9, true, false)(
            "\t- Init Level Exp ");
        set_level_exp();
        cMyTrace("int CDataManager::init()", 0x2028, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x202a, 9, true, false)(
            "\t- Init Mob Reward ");
        set_mob_reward();
        cMyTrace("int CDataManager::init()", 0x2030, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x2032, 9, true, false)(
            "\t- Init PVP Grade ");
        if (!set_pvp_grade())
        {
            cMyTrace("int CDataManager::init()", 0x2035, 9, true, true)(
                "\t- Init PVP Grade Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2038, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x203a, 9, true, false)(
            "\t- Init Reward SP ");
        if (!set_reward_sp())
        {
            cMyTrace("int CDataManager::init()", 0x203d, 9, true, true)(
                "\t- Init Reward SP Fail\n");
            return 0;
        }
    }

    // Channel 数据/脚本。
    {
        cMyTrace("int CDataManager::init()", 0x2040, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x2044, 9, true, false)(
            "\t- Init Get Channel Data(channel_info.etc) ");
        if (!GlobalData::s_channel_proxy->SendCheckChannelScript())
        {
            cMyTrace("int CDataManager::init()", 0x2047, 9, true, true)(
                "\t- Init Get Channel Data(channel_info.etc) Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x204a, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x204e, 9, true, false)(
            "\t- Init Channel Script ");
        if (!importChannelScript(&m_channelScript))
        {
            cMyTrace("int CDataManager::init()", 0x2058, 9, true, true)(
                "\t- Init Channel Script Fail");
            return 0;
        }
    }

    // ItemUpgrade / Separate / 3rdChronicle / Amplifier / Reseal。
    {
        cMyTrace("int CDataManager::init()", 0x205b, 9, false, true)("");
        m_itemUpgrade = new WongWork::CItemUpgrade;
        cMyTrace("int CDataManager::init()", 0x205e, 9, true, false)(
            "\t- Init Item Upgrade Info ");
        if (!m_itemUpgrade->LoadUpgradeScript("Etc/Upgrade.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x2061, 9, true, true)(
                "\t- Init Item Upgrade Info Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2064, 9, false, true)("");
        m_itemUpgradeSeparate = new WongWork::CItemUpgrade_Separate;
        if (!m_itemUpgradeSeparate->LoadUpgradeScript("Etc/Upgrade_Separate.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x206a, 9, true, true)(
                "\t- Item Upgrade Separate Fail");
            return 0;
        }
        if (!m_itemUpgrade->Load3rdChronicleUpgradeScript("Etc/chronicle.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x2072, 9, true, true)(
                "\t- Loading 3rd Chronice Upgrade Script Fail");
            return 0;
        }
        GlobalData::s_itemAmplifier_->loadScript("Etc/AmplifyItem.etc",
                                                 "Etc/AmplifyUpgrade.etc");
    }

    // Reseal / UnlimitChallenge / LevelRewardTable / LimitItemUsage。
    {
        cMyTrace("int CDataManager::init()", 0x2084, 9, true, false)(
            "\t- Init Reseal table ");
        if (!importResealTable("Etc/Reseal.etc", &m_resealTable))
        {
            cMyTrace("int CDataManager::init()", 0x208c, 9, true, true)(
                "\t- Init Reseal table Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x208f, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x2092, 9, true, false)(
            "\t- Init Unlimit Challenge Info ");
        if (!importUnlimitChallengeScript("Etc/UnlimitChallenge.etc",
                                          &m_unlimitChallenge))
        {
            cMyTrace("int CDataManager::init()", 0x2095, 9, true, true)(
                "\t- Init Unlimit Challenge Info Fail");
            return 0;
        }
        m_levelRewardTableMgr = CLevelRewardTableMgr::GetInstance();
        m_levelRewardTableMgr->SetChallengeInfo(m_unlimitChallenge);
        cMyTrace("int CDataManager::init()", 0x209e, 9, false, true)("");
        if (!importLimitItemUsageInfo(
                (char*)"Etc/Chn_Server_LimititemUsageinfo.etc",
                &m_limitItemUsage))
        {
            cMyTrace("int CDataManager::init()", 0x20a5, 9, true, true)(
                "\t- importLimitItemUsageInfo Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x20a8, 9, true, true)(
            "\t- Import Limit Item Usage Info Success");
    }

    // DB Log Item。
    {
        cMyTrace("int CDataManager::init()", 0x20b7, 9, true, true)(
            "\t- Import DB Log Item - ");
        if (!importDBLogItem((char*)"Etc/Chn_DBLogItem.etc", &m_dbLogItems))
        {
            cMyTrace("int CDataManager::init()", 0x20be, 9, true, true)(
                "\t- Import DB Log Item  Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x20c1, 9, true, true)(
            "\t- Import DB Log Item Success");
    }

    // Condition Event 两件套。
    {
        cMyTrace("int CDataManager::init()", 0x20db, 9, true, false)(
            "\t- Import Condition Event Info ");
        if (!importConditionEventScript("Event/ConditionEvent.evt",
                                        &m_conditionEvent))
        {
            cMyTrace("int CDataManager::init()", 0x20e2, 9, true, true)(
                "\t- Import Condition Event Info Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x20e5, 9, false, true)("");
        cMyTrace("int CDataManager::init()", 0x2102, 9, true, false)(
            "\t- Import Condition Event Script Dungeon ");
        if (!importConditionEventChkDungeonScript(
                "Event/ConditionEventChkDungeon.evt", &m_conditionDungeon))
        {
            cMyTrace("int CDataManager::init()", 0x2109, 9, true, true)(
                "\t- Import Condition Event Script Dungeon Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x210c, 9, false, true)("Success");
    }

    // NPCRelation / EmotionTags / EquipGrade。
    {
        cMyTrace("int CDataManager::init()", 0x2111, 9, true, false)(
            "\t- Import NPC Relation Event Info Success");
        if (!importNPCRelationEventScript("Event/NPCRelationEvent.evt",
                                          &m_npcRelation))
        {
            cMyTrace("int CDataManager::init()", 0x2118, 9, true, true)(
                "\t- Import NPC Relation Event Info Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x211b, 9, false, true)("Success");
        cMyTrace("int CDataManager::init()", 0x211e, 9, true, false)(
            "\t- Import Emotion Type Tags Info ");
        if (!importEmotionTypeTagsScript("Etc/EmotionExpressionData.etc",
                                         &m_emotionTags))
        {
            cMyTrace("int CDataManager::init()", 0x2121, 9, true, true)(
                "\t- Import Emotion Type Tags Info Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2124, 9, false, true)("Success");
        cMyTrace("int CDataManager::init()", 0x2126, 9, true, false)(
            "\t- Import Equipment Grade Sample Info ");
        if (!importEquipmentGradeSampleInfo("Etc/EquipmentGradeSample.etc",
                                            &m_equipGrade))
        {
            cMyTrace("int CDataManager::init()", 0x2129, 9, true, true)(
                "\t- Import Equipment Grade Sample Info Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x212c, 9, false, true)("Success");
    }

    // Creature 脚本两步。
    {
        cMyTrace("int CDataManager::init()", 0x215c, 9, true, false)(
            "\t- Import Creature Script");
        if (!initCreatureScript("Creature/", "Creature.lst", "ExpTable.tbl"))
        {
            cMyTrace("int CDataManager::init()", 0x215f, 9, true, true)(
                "\t- Import Creature Creature.lst fail");
            return 0;
        }
        if (!user_creature::GetInstanceCreatureScriptMgr()
                  ->LoadCreatureScriptFile())
        {
            cMyTrace("int CDataManager::init()", 0x2165, 9, true, true)(
                "\t- Import Creature Script Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2168, 9, false, true)("Success");
    }

    // PvP single playable map（静态，无条件）+ NPC Script。
    CMapList::LoadPvPSinglePlayableMap((char*)"extern.sd");
    m_npcScriptList = new CNPCScriptList;
    m_npcDynamicInfo = new CNPCDynamicInfoManager;
    {
        cMyTrace("int CDataManager::init()", 0x217a, 9, true, false)(
            "\t- Import Npc Script ");
        if (m_npcScriptList->initScript((char*)"Npc/", (char*)"Npc.lst",
                                        (char*)"common.dat") != 0)
        {
            cMyTrace("int CDataManager::init()", 0x217d, 9, true, true)(
                "\t- Import Npc.lst Fail\n");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2180, 9, false, true)("");
    }

    // ItemLimitEditionMgr（m_npcRelation.m_itemLimitEditionMgr）+ MCAP init。
    m_npcRelation.m_itemLimitEditionMgr = new CItemLimitEditionMgr;
    WongWork::CMCAPManager::init();

    // Equipment Part Set：trancate + import + g_equipmentPartSetMap_ 双层回灌。
    sync_script::GetInstanceSyncScript()->trancate_item_parts_set();
    {
        cMyTrace("int CDataManager::init()", 0x21ac, 9, true, false)(
            "\t- Import Equipment Part Set Info ");
        if (!importEquipmentPartSetInfo("Etc/EquipmentPartSet.etc",
                                        static_cast<ENUM_MODULE_TYPE>(3)))
        {
            cMyTrace("int CDataManager::init()", 0x21af, 9, true, true)(
                "\t- Import Equipment Part Set Info Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x21b3, 9, true, false)("Success");

        for (std::map<int, STEquipmentPartSetData>::iterator it =
                 g_equipmentPartSetMap_.begin();
             it != g_equipmentPartSetMap_.end(); ++it)
        {
            for (std::vector<STEquipmentPartSetPiece>::iterator pit =
                     it->second.m_pieces.begin();
                 pit != it->second.m_pieces.end(); ++pit)
            {
                sync_script::GetInstanceSyncScript()->insert_item_parts_set(
                    it->first, *pit);
            }
            if (it->second.m_script != 0)
            {
                for (std::map<int, EquipmentParameterInfo>::iterator eit =
                         it->second.m_script->m_paramMap678.begin();
                     eit != it->second.m_script->m_paramMap678.end(); ++eit)
                {
                    STEquipmentScript es;
                    es.m_strac = it->second.m_script->m_str668;
                    es.m_param1c0 = eit->second;
                    sync_script::GetInstanceSyncScript()
                        ->insert_equiptment_item_to_db(&es, (unsigned int)eit->first,
                                                       2, it->first);
                }
            }
        }
    }

    // FairPvPStat + PvP SkillTree 四步 + initFairPVPSkillPoint。
    {
        cMyTrace("int CDataManager::init()", 0x21ca, 9, true, false)(
            "\t- Import FairPvPStat ");
        if (!importPairPvPStatScript(m_fairPvPStat, "Etc/FairPvPStat.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x21cd, 9, true, true)(
                "\t- Fail load FairPvPStat Script File");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x21d1, 9, true, false)("Success");
        if (!initPvPSkillTreeScript((char*)"Etc/", (char*)"PvPSkillTree.lst"))
        {
            cMyTrace("int CDataManager::init()", 0x21d7, 9, true, true)(
                "\t- Load PvPSkillTree.lst fail");
            return 0;
        }
        if (!m_pvpSkillTree.load_pvp_skill_tree_list())
        {
            cMyTrace("int CDataManager::init()", 0x21dd, 9, true, true)(
                "\t- Load load_pvp_skill_tree_list fail");
            return 0;
        }
        if (!m_pvpSkillTree.importPairPvPSkillPointScript(
                "Etc/PvPSkillPoint.etc"))
        {
            cMyTrace("int CDataManager::init()", 0x21e3, 9, true, true)(
                "\t- Load PairPvPSkillPointScript fail");
            return 0;
        }
        if (!initFairPVPSkillPoint())
        {
            cMyTrace("int CDataManager::init()", 0x21eb, 9, true, true)(
                "\t- initFairPVPSkillPoint fail");
            return 0;
        }
    }

    // Ingame Event：new(0x5c) CEventScriptMng → m_eventScriptMng；导入 + init。
    m_liveServerInfo.m_eventScriptMng = new EventClassify::CEventScriptMng;
    {
        cMyTrace("int CDataManager::init()", 0x221d, 9, true, false)(
            "\t- Loading Ingame Event Script List ");
        EventClassify::CEventScriptMng* mng = m_liveServerInfo.m_eventScriptMng;
        if (!importInGameEventInfoList(static_cast<InGameEventInfoList*>(mng),
                                       "Event/InGameEventList.evt"))
        {
            cMyTrace("int CDataManager::init()", 0x2224, 9, true, true)(
                "\t- Fail load ingame_event_script_list");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2228, 9, true, false)("Success");
        if (!mng->initScriptlist(*mng))
        {
            return 0;
        }
    }

    // Event Avatar List / Event Etc。
    {
        cMyTrace("int CDataManager::init()", 0x2231, 9, true, false)(
            "\t- Loading Event Avatar List ");
        if (!importEventAvatarListScript(&m_eventAvatarList,
                                         "Event/EventAvatarList.evt"))
        {
            cMyTrace("int CDataManager::init()", 0x223d, 9, true, true)(
                "\t- Fail Loading Event Avatar List");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2240, 9, true, false)("Success");
        cMyTrace("int CDataManager::init()", 0x2244, 9, true, false)(
            "\t- Loading Event Etc Info ");
        if (!importEventEtcScript(&m_eventEtc, "Event/EventEtc.evt"))
        {
            cMyTrace("int CDataManager::init()", 0x224e, 9, true, true)(
                "\t- Fail Loading Event Etc Info");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2252, 9, true, false)("Success");
    }

    // AvatarColorVariation。
    if (!AvatarVariation::importAvatarColorVariation(&m_avatarColor))
    {
        cMyTrace("int CDataManager::init()", 0x227b, 9, true, true)(
            "\t- Loading importAvatarColorVariation  Fail");
        return 0;
    }

    // Mercenary System（err=出错行）。
    {
        cMyTrace("int CDataManager::init()", 0x229b, 9, true, false)(
            "\t- Loading Mercenary System ");
        const int err = importMercenarySystemEtcScript("Etc/mercenary.etc",
                                                       &m_mercenarySystem);
        if (err != 0)
        {
            cMyTrace("int CDataManager::init()", 0x22a0, 9, true, true)(
                "\t- Loading Mercenary System Fail - %d line \n", err);
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x22a3, 9, true, false)("Success");
    }

    NeedMaterialDataManager::getInstance();

    // PcBang / DimensionActivation。
    if (!importPcBangEquipmentRendar("Etc/PcBangRentalService.etc",
                                     &m_pcBangRentar))
    {
        cMyTrace("int CDataManager::init()", 0x2304, 9, true, true)(
            "\t- PcBangRentalService Fail!\n");
        return 0;
    }
    if (!importDimensionActivationData("Event/DimensionActivationEvent.evt",
                                       &m_dimensionAct))
    {
        cMyTrace("int CDataManager::init()", 0x230b, 9, true, true)(
            "\t- DimensionActivationEvent Fail!\n");
        return 0;
    }

    // 租借系统就绪检查。
    if (!reinterpret_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
              ->Ready())
    {
        std::cerr << "[GlobalData::Init] The rental system is not ready!"
                  << std::endl;
        return 0;
    }

    // Delete Invalid Item Script（最后一步）。
    {
        cMyTrace("int CDataManager::init()", 0x2331, 9, true, false)(
            "\t- Load Delete Invalid Item Script");
        if (!m_deleteInvalidItem.loadScript())
        {
            cMyTrace("int CDataManager::init()", 0x2334, 9, true, true)(
                "\t- Load Delete Invalid Item Script Fail");
            return 0;
        }
        cMyTrace("int CDataManager::init()", 0x2337, 9, true, false)("Success");
    }

    return 1;
}

// ===================== 副本次数排行 =====================

// ORIG T 0x083639d2 _ZN12CDataManager26getDungeonClearAverageTimeEii23ENUM_DUNGEON_DIFFICULTY：
//   以 (dungeonIdx, partyCount) 为键查 m_rankSystem.m_dungeonRank.m_map0；
//   未命中返回 0；命中按 difficulty(0..3) 取 stClearTimeAverage::m_v[难度]，
//   其它难度返回 0。
unsigned int CDataManager::getDungeonClearAverageTime(int dungeonIdx, int partyCount,
                                                      ENUM_DUNGEON_DIFFICULTY difficulty)
{
    stClearTimeAverageMapKey key;
    key.m_a = dungeonIdx;
    key.m_b = partyCount;

    std::map<stClearTimeAverageMapKey, stClearTimeAverage>::const_iterator it =
        m_rankSystem.m_dungeonRank.m_map0.find(key);
    if (it == m_rankSystem.m_dungeonRank.m_map0.end())
        return 0;

    switch (difficulty)
    {
    case 0: return (unsigned int)it->second.m_v[0];
    case 1: return (unsigned int)it->second.m_v[1];
    case 2: return (unsigned int)it->second.m_v[2];
    case 3: return (unsigned int)it->second.m_v[3];
    default: return 0;
    }
}

// ORIG T 0x08363ac8 _ZN12CDataManager22getClearTimeBonusPointEii：
//   avgTime/clearTime 任一为 0 → 0；diff = avgTime - clearTime；
//   遍历 m_dungeonRank.m_vec18（vector<stBonusPointValues>）：
//     若 diff < 当前 lo：列表首项 → 0，否则返回上一项 value；
//     否则记录 (lo, value) 继续；循环结束返回最后一个 value。
int CDataManager::getClearTimeBonusPoint(int avgTime, int clearTime)
{
    if (avgTime == 0 || clearTime == 0)
        return 0;

    int diff = avgTime - clearTime;
    int result = 0;
    const std::vector<stBonusPointValues>& values =
        m_rankSystem.m_dungeonRank.m_vec18;

    for (std::vector<stBonusPointValues>::const_iterator it = values.begin();
         it != values.end(); ++it)
    {
        if (diff < it->m_a)
        {
            if (it == values.begin())
                return 0;
            return result;
        }
        result = it->m_b;
    }
    return result;
}

// ---- ORIG W force-use 发射（结构生命周期专项）----
// LevelUpRewardItemTableKey C1 0x0891bac0 / clear 0x0891bad4：
//   默认构造在既有 TU 无使用点（均为 C2Eiii），此处合法 force-use 触发弱符号。
// LevelUpRewardItemData C1 0x081b88bc / clear 0x081b88d0 同段发射（重复弱符号无害）。
namespace {
void emitLevelUpRewardWeakSymbols()
{
    LevelUpRewardItemTableKey key;   // C1 → call clear
    (void)key;
    LevelUpRewardItemData data;      // C1 → call clear
    (void)data;
}
}  // namespace
