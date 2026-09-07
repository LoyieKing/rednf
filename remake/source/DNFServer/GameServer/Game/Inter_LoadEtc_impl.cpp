// Inter_LoadEtc_impl.cpp — Inter_LoadEtc 全类 ORIG 方法实现（独占 TU）
//
// 还原依据：docs/class_func_reports/Inter_LoadEtc/ 全部 17 函数报告
// （## 汇编 + ## 反编译 C 逐段转写）；ORIG 二进制 df_game_r。
// dispatch_sig @0x084c0264（nm -S size 0x240a）。
// ⚠ 需 CMake 登记：source/cmake/game/CMakeLists.txt 增加 Inter_LoadEtc_impl.cpp。
// （Game/InterDispatcher.cpp 原 TODO(ORIG 0x84c0264) 空桩已删除。）

#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "CInventory.h"       // GetInvenRef（LinkE2 Inter_NpcLimitBuyItemInfo::sendPacket）
#include "LogManager.h"       // logFormat（LinkE2 Inter_VillageAttackedReward::_send_reward_item）
#include <string>

#include "TaiwanQueries.h"   // ODR 探针
#include "DBSelectMobileAuthRewardTw.h"  // Garena 手机认证奖励请求（ORIG T 0x0815c586）
#include "InterDispatcher.h"
#include "CAradRyosikaEvent.h"   // LinkClosureB-2 权威声明（DoEvent 定义在本 TU）
#include "SigTypes.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "AttendanceEvent.h"   // 权威叶事件头（原局部类定义已迁移，getRewordItem 同迁）
#include "CDataManager.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "GlobalData.h"
#include "PacketGuard.h"
#include "PacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SkillSlot.h"
#include "CEventScriptMng.h"
#include "CGuildServerProxy.h"          // CEventBase / CEventManager / CDelivery / DeliveryMsg
#include "CPremiumLetheManager.h"
#include "CMailBoxHelper.h"
#include "MoreTypes.h"
#include "Arad_InternalDispatcher.h"    // make_internal_stream_jpn
#include "CFatigueBatteryHandle.h"
#include "RefPvpGrade.h"
#include "CCirculationStatistic.h"
#include "QuestClear.h"                 // WongWork::CQuestClear
#include "CUserCharacInfo.h"
#include "GrowthEquipEvent.h"
#include "W8LT_EventTail.h"             // GiveGrowCreatureEvent / SeriaRoomAniDecoEvent
#include "InterfacePacketBuf.h"
#include "RDARScriptStringManager.h"
#include "CEnvironment.h"
#include "AvatarCoin.h"
#include "BreakAwayPreventSystem.h"     // STBreakAwayRewards
#include "CBoosterGage.h"               // charac_expand::CData
#include "CItemLimitEdition.h"
#include "CGlobalEffectManager.h"
#include "CPowerManager.h"
#include "CItemList.h"                  // Arad_GiftItem_Set::send_mail（LinkClosureD）
#include "CServerEvent.h"
#include "OnlinePreliminary.h"
#include "CCharacList.h"                // SetSkillTree：m_characs[job].m_giveSkillList[0]
#include "CSkill.h"                     // SetSkillTree：skill->get_group()
#include "STExpertJobScript.h"          // SetSkillTree：script->m_growSkillVec（+0x0c）
#include "CHackAnalyzer.h"
#include "CUserProc.h"                  // APSystem::CUserProc
#include "Arad_DataManager.h"
#include "Quest.h"
#include "CAchievement.h"
#include "CTitleBook.h"
   // item_lock::stItemLockRef  // Quest 完整类型（find_quest 调用点）
#include "CSharedServerMessageManager.h"
#include "CHandlePremium.h"
#include "ServiceRestrictManager.h"
#include "HeroMissionEvent.h"
#include "CraneMinigameManager.h"
#include "Arad_ServerStateManager.h"
#include "Arad_EventPeriodDataManager.h"

// ---------------------------------------------------------------------------
// CSkillTreeParam —— ORIG W 全套；本 TU 为唯一权威声明点（四符号全实现）。
// ---------------------------------------------------------------------------

// 本 TU 提供权威最小布局与全部四符号的忠实定义；SetSkillTree 按 ORIG
// 0x084ef4a6（sz 0x7ec）objdump 逐指令转写（字符串常量实证返回类型为 int）。
struct stSPReturnInfo_t;   // SkillSlot.h 已带出完整定义

class CSkillTreeParam
{
public:
    CSkillTreeParam(CUser* user, SIG_LOAD_ETC* sig, PacketGuard& packet);   // W 0x084eef48
    ~CSkillTreeParam();                                                     // W 0x084efc92
    int SetSkillTree(Inter_LoadEtc* owner);                                 // W 0x084ef4a6
    void SendSkillPointReturnInfo();                                        // W 0x084eefd4
    CUser* m_user;                          // +0x00
    SIG_LOAD_ETC* m_sig;                    // +0x04
    PacketGuard* m_packet;                  // +0x08
    int m_curSp;                            // +0x0c..+0x1b（ctor 清零区）
    int m_field10;
    int m_field14;
    int m_field18;
    std::vector<stSPReturnInfo_t> m_returns1;   // +0x1c
    std::vector<stSPReturnInfo_t> m_returns2;   // +0x28
};

CSkillTreeParam::CSkillTreeParam(CUser* user, SIG_LOAD_ETC* sig, PacketGuard& packet)
{
    m_user = user;
    m_sig = sig;
    m_packet = &packet;
    m_curSp = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
}

CSkillTreeParam::~CSkillTreeParam()
{
}

// ---------------------------------------------------------------------------
// SendSkillPointReturnInfo —— ORIG W 0x084eefd4 sz 0x4d2（objdump
// 0x084eefd4..0x084ef4a6 逐指令转写 [报告缺失]）。四个 SP/SFP kind 差值累计
// 后按需对第一/第二技能树各发一个 0x7a 回包：
//   m_curSp  = skill->get_remain_sp_at_index(0)  - m_curSp   （+0x0c）
//   m_field14 = skill->get_remain_sfp_at_index(2) - m_field14（+0x14）
//   if (m_curSp > 0 || m_field14 > 0) && !m_returns1.empty():
//     packet.clear(); put_header(0,0x7a); put_byte(0);
//     put_int(m_field14); put_int(m_curSp); put_short(m_returns1.size());
//     foreach e: put_short(e.m_skillIdx); put_short(e.m_levelDiff);
//                put_int(e.m_returnSP); put_int(e.m_returnSFP);
//     finalize(true); m_user->Send(*m_packet)
//   m_field10 = get_remain_sp_at_index(1)  - m_field10 （+0x10）
//   m_field18 = get_remain_sfp_at_index(3) - m_field18 （+0x18）
//   同型第二包：put_byte(1)、put_int(m_field18)、put_int(m_field10)、
//   遍历 m_returns2。空差值或空列表 → 跳过发包。
// ---------------------------------------------------------------------------
void CSkillTreeParam::SendSkillPointReturnInfo()
{
    SkillSlot* skill = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()));
    m_curSp = skill->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(0))
              - m_curSp;
    skill = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()));
    m_field14 = skill->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2))
                - m_field14;

    bool need1 = false;
    if (m_curSp > 0 || m_field14 > 0)
        need1 = !m_returns1.empty();
    if (need1)
    {
        m_packet->clear();
        m_packet->put_header(0, 0x7a);
        m_packet->put_byte(0);
        m_packet->put_int(m_field14);
        m_packet->put_int(m_curSp);
        m_packet->put_short((int)m_returns1.size());
        for (std::vector<stSPReturnInfo_t>::const_iterator it =
                 m_returns1.begin(), endIt = m_returns1.end();
             it != endIt; ++it)
        {
            m_packet->put_short((int)it->m_skillIdx);
            m_packet->put_short((int)it->m_levelDiff);
            m_packet->put_int((int)it->m_returnSP);
            m_packet->put_int((int)it->m_returnSFP);
        }
        m_packet->finalize(true);
        m_user->Send(*m_packet);
    }

    skill = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()));
    m_field10 = skill->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(1))
                - m_field10;
    skill = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()));
    m_field18 = skill->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(3))
                - m_field18;

    bool need2 = false;
    if (m_field10 > 0 || m_field18 > 0)
        need2 = !m_returns2.empty();
    if (need2)
    {
        m_packet->clear();
        m_packet->put_header(0, 0x7a);
        m_packet->put_byte(1);
        m_packet->put_int(m_field18);
        m_packet->put_int(m_field10);
        m_packet->put_short((int)m_returns2.size());
        for (std::vector<stSPReturnInfo_t>::const_iterator it =
                 m_returns2.begin(), endIt = m_returns2.end();
             it != endIt; ++it)
        {
            m_packet->put_short((int)it->m_skillIdx);
            m_packet->put_short((int)it->m_levelDiff);
            m_packet->put_int((int)it->m_returnSP);
            m_packet->put_int((int)it->m_returnSFP);
        }
        m_packet->finalize(true);
        m_user->Send(*m_packet);
    }
}

// SetSkillTree —— ORIG W 0x084ef4a6 sz 0x7ec（objdump 0x084ef4a6..0x084efc91
// 逐指令转写 [报告缺失]；.rodata 字符串 "int CSkillTreeParam::SetSkillTree
// (Inter_LoadEtc*)" 实证返回类型 int）。流程：
//   1) SaveBackupSkillInfo(sig+0x29fd/0x2b97, kind0/1) 备份技能；
//   2) 非 PvP 技能树通道且 pvp_grade<=9 → ProcessOneDayLethe/2ND（孟婆回包）；
//   3) GetCurCharacSkillTreeIndex()==-1 与 else 两路，按 lethe1/lethe2==1
//      各发 0xb4 回包（byte1,byte1 或 byte1,byte2）；
//   4) 读四档 remain_sp/sfp 到 m_curSp/m_field10/m_field14/m_field18；
//   5) level（等级修订通道 → 0x55）→ checkOverLevelSkill 两树，命中 enableSaveSkill；
//   6) PvP 技能树通道 → 直接返回 0；
//   7) 拷贝 m_characList->m_characs[job].m_giveSkillList[0] 遍历：
//      find_skill 缺失 → logFormat 并返回 0xa00；否则按 get_skillslot_no<0
//      对两树 growtype_skill(job, first, second, kind)；
//   8) 专家职业脚本 m_growSkillVec（+0x0c）遍历：未掌握技能同样 growtype_skill。
//      正常路径最终返回 0。
int CSkillTreeParam::SetSkillTree(Inter_LoadEtc* owner)
{
    m_user->SaveBackupSkillInfo(
        reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(m_sig->m_backupSkill),
        ENUM_SKILL_TREE_KIND(0));
    m_user->SaveBackupSkillInfo(
        reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(m_sig->m_backupSkill2ND),
        ENUM_SKILL_TREE_KIND(1));

    int lethe1 = 0;   // ProcessOneDayLethe 结果
    int lethe2 = 0;   // ProcessOneDayLethe2ND 结果

    if (!G_GameWorld()->IsPvPSkilTreeChannel() && m_user->get_pvp_grade() <= 9)
    {
        owner->ProcessOneDayLethe(m_sig, m_user, lethe1);
        owner->ProcessOneDayLethe2ND(m_sig, m_user, lethe2);
    }

    if (m_user->GetCurCharacSkillTreeIndex() == -1)
    {
        if (lethe1 == 1)
        {
            m_packet->clear();
            m_packet->put_header(0, 0xb4);
            m_packet->put_byte(1);
            m_packet->put_byte(1);
            m_packet->finalize(true);
            m_user->Send(*m_packet);
        }
    }
    else
    {
        if (lethe1 == 1)
        {
            m_packet->clear();
            m_packet->put_header(0, 0xb4);
            m_packet->put_byte(1);
            m_packet->put_byte(1);
            m_packet->finalize(true);
            m_user->Send(*m_packet);
        }
        else
        {
            m_packet->clear();
            if (lethe2 == 1)
            {
                m_packet->put_header(0, 0xb4);
                m_packet->put_byte(1);
                m_packet->put_byte(2);
                m_packet->finalize(true);
                m_user->Send(*m_packet);
            }
        }
    }

    {
        SkillSlot* skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        m_curSp = skill->get_remain_sp_at_index(ENUM_SKILL_TREE_KIND(0));
        skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        m_field10 = skill->get_remain_sp_at_index(ENUM_SKILL_TREE_KIND(1));
        skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        m_field14 = skill->get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND(2));
        skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        m_field18 = skill->get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND(3));
    }

    int level = m_user->get_charac_level();
    if (G_GameWorld()->IsCharacterLevelRevisionChannel())
        level = 0x55;

    {
        SkillSlot* skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        if (skill->checkOverLevelSkill(m_returns1, ENUM_SKILL_TREE_KIND(0), level))
            m_user->enableSaveSkill();
        skill = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(m_user->getCurCharacSkillR()));
        if (skill->checkOverLevelSkill(m_returns2, ENUM_SKILL_TREE_KIND(1), level))
            m_user->enableSaveSkill();
    }

    if (G_GameWorld()->IsPvPSkilTreeChannel())
        return 0;

    {
        std::pair<int, int> p;
        std::vector<std::pair<int, int> > giveSkills(
            G_CDataManager()->m_characList->m_characs[m_user->get_charac_job()]
                .m_giveSkillList[0]);
        int i;
        for (i = 0; i < (int)giveSkills.size(); ++i)
        {
            p = giveSkills[i];
            int first = p.first;
            int second = p.second;
            CSkill* skill = 0;
            skill = G_CDataManager()->find_skill(m_user->get_charac_job(), first);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__, 0x9ff,
                    "G_CDataManager()->find_skill( %d, %d )",
                    m_user->get_charac_job(), first);
                return 0xa00;
            }
            int group = skill->get_group();
            int slot1 = 0;
            int slot2 = 0;
            SkillSlot* skillR = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(m_user->getCurCharacSkillR()));
            slot1 = skillR->get_skillslot_no(first, group,
                                             ENUM_SKILL_TREE_KIND(0), true);
            slot2 = skillR->get_skillslot_no(first, group,
                                             ENUM_SKILL_TREE_KIND(1), true);
            if (slot1 < 0)
            {
                SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                    m_user->getCurCharacSkillW());
                skillW->growtype_skill(m_user->get_charac_job(), first, second,
                                       ENUM_SKILL_TREE_KIND(0));
            }
            if (slot2 < 0)
            {
                SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                    m_user->getCurCharacSkillW());
                skillW->growtype_skill(m_user->get_charac_job(), first, second,
                                       ENUM_SKILL_TREE_KIND(1));
            }
        }
    }

    {
        STExpertJobScript* script = reinterpret_cast<STExpertJobScript*>(
            G_CDataManager()->GetExpertJobScript(
                m_user->GetCurCharacExpertJobType()));
        if (script != 0)
        {
            int j;
            for (j = 0; j < (int)script->m_growSkillVec.size(); ++j)
            {
                int skillIdx = script->m_growSkillVec[j].first;
                int expLevel = m_user->GetCurExpertJobLevel(
                    m_user->GetCurCharacExpertJobExp());
                SkillSlot* skillR = reinterpret_cast<SkillSlot*>(
                    const_cast<char*>(m_user->getCurCharacSkillR()));
                if (!skillR->check_skill_mastered(skillIdx, expLevel, 2,
                                                  ENUM_SKILL_TREE_KIND(0)))
                {
                    SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                        m_user->getCurCharacSkillW());
                    skillW->growtype_skill(m_user->get_charac_job(), skillIdx,
                                           expLevel, ENUM_SKILL_TREE_KIND(0));
                }
                if (!skillR->check_skill_mastered(skillIdx, expLevel, 2,
                                                  ENUM_SKILL_TREE_KIND(1)))
                {
                    SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                        m_user->getCurCharacSkillW());
                    skillW->growtype_skill(m_user->get_charac_job(), skillIdx,
                                           expLevel, ENUM_SKILL_TREE_KIND(1));
                }
            }
        }
    }

    return 0;
}

// CCoinRefillEvent —— ORIG T 事件类（docs/class_func_reports/CCoinRefillEvent/ 全套报告，
// 权威头/TU 归事件域批次）。本 TU 仅忠实落 getLastEventTime（getLastEventTime.md 全文）。
class CCoinRefillEvent : public CEventBase
{
public:
    CCoinRefillEvent();
    ~CCoinRefillEvent();
    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    int getLastEventTime() const;           // ORIG T 0x0832c0b8

    int m_fieldc;                           // +0x0c（时间表条目数）
    int m_times[8];                         // +0x10..（ getLastEventTime 读 +0xc*4 表）
};


void SetPvPSkillTree(CUser*, const _Mastered_skill*, ENUM_SKILL_TREE_KIND);
void RewardItem(CUser*, long, long, long, unsigned int, const char*,
                const char*, unsigned int);        // ORIG T 0x084bfb84（producer 归 Item/Event 域）
bool  CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);
int   ComputeDaysScheduleTime(int hour, long lastTime, long curTime);

// ---- Garena 段 [blocked]：Taiwan::GarenaAuthData 权威视图在 TaiwanQueries.h，
// 但该头与 MoreTypes.h 存在五组 Sig 类 ODR 重定义，include 任一均冲突。
// dispatch_sig 尾部 Taiwan 段调用按 ORIG 保留语义登记为 blocked_dependencies：
//   garena->getGcaType() / getMobileAuth() / GarenaCyberCafe / GarenaAuthPCRoom
bool Taiwan_GarenaCyberCafe(unsigned short cafeFlag);      // T 0x0817313c [INFERENCE 归属名]
void Taiwan_GarenaAuthPCRoom(CUser* user);                 // T 0x0855428e [INFERENCE]

// ---- DB 任务类壳（ORIG makeRequest 均为真实 T；仓库无头无 producer → blocked）----
class DB_SaveUserHandicap
{
public:
    static void makeRequest(unsigned int accId, int a, int b, int c, int d, int e);
        // ORIG _ZN19DB_SaveUserHandicap11makeRequestEjiiiii T 0x0842801a
};
class DB_InsertAttendanceEvent
{
public:
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo);
        // ORIG _ZN24DB_InsertAttendanceEvent11makeRequestEijj T 0x08448afe
};
class DB_LoadBreakAwayRewardData
{
public:
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo,
                            STBreakAwayRewards& out);
        // ORIG _ZN26DB_LoadBreakAwayRewardData11makeRequestEijjR18STBreakAwayRewards T 0x0843915c
};
class DB_LoadBloodBestRecord
{
public:
    static void makeRequest(int uid, unsigned int characNo);
};
class DB_LoadDimension
{
public:
    static void makeRequest(int uid, unsigned int characNo);
};
class DB_LoadEventLevelupSupport
{
public:
    static void makeRequest(int uid, int characNo);   // ORIG _ZN26DB_LoadEventLevelupSupport11makeRequestEii
};
// ===========================================================================
// Inter_LoadEtc 私有方法实现
// ===========================================================================

bool Inter_LoadEtc::SetRankRecord(CUser* user, SIG_LOAD_ETC* sig)
{
    GameResultType* records = sig->m_records;
    for (int i = 0; i < sig->m_recordCount; ++i)
    {
        records[i].m_field0 = 1;
        user->update_record(records[i]);            // ORIG T 0x08676876
    }
    return true;
}

namespace {

inline SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* asLetheResult(void* p)
{
    return reinterpret_cast<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD*>(p);
}

} // namespace

// ===========================================================================
// Inter_LoadEtc 私有方法实现
// ===========================================================================

int Inter_LoadEtc::_fnEnumQuestClear(unsigned int questIdx, char* ctx)
{
    // ORIG：ctx 双槽 (InterfacePacketBuf** buf, int** count)（_fnEnumQuestClear.md）
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(ctx);
    int* count = *reinterpret_cast<int**>(ctx + sizeof(InterfacePacketBuf*));
    Quest* quest = G_CDataManager()->find_quest((int)questIdx);
    if (quest != 0 && quest->check_clear_send() != 0)
    {
        buf->put_short((int)(questIdx & 0xffff));
        ++(*count);
    }
    return 1;
}

bool Inter_LoadEtc::SetFatigue(CUser* user)
{
    const _Charac_info* cur = user->getCurCharacR();
    if (cur == 0)
        return false;

    long now = GlobalData::s_systemTime_.getCurSec();
    if (!CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                cur->m_field7b, now))     // +0x7b lastPlayTick
        return true;

    int days = 0;
    if (cur->m_field7b != 0)
    {
        long t = GlobalData::s_systemTime_.getCurSec();
        days = ComputeDaysScheduleTime(
            G_CEnvironment()->m_serverEnvir.m_fatigueTime,
            cur->m_field7b, t);
    }

    short charging =
        GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, days);
    if (charging > 0)
    {
        user->IncFatigueBatteryCharging(charging);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x139a, 0);
        trace("FATIGUE BATTERY : charno(%d) days(%d), lastplaytick(%d), "
              "curTick(%d), charging(%d), cur charging(%d)",
              (int)user->getCurCharacNo(), days,
              (unsigned)cur->m_field7b,
              (unsigned)GlobalData::s_systemTime_.getCurSec(),
              (int)charging,
              (int)user->GetFatigueBatteryCharging());
    }

    if (user->GetFatigueBatteryCharging() > 0)
    {
        short decAmount = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
        if (decAmount != 0)
        {
            user->DecFatigueBatteryCharging(decAmount);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x13a4, 0);
            trace("FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), "
                  "cur charging(%d)",
                  (int)user->getCurCharacNo(), (int)decAmount,
                  (int)user->GetFatigueBatteryCharging());
        }
    }

    user->RecoverFatigue(0);                                        // ORIG T 0x08657ada
    user->resetCurCharacMemberBonusFatigue();                       // ORIG T（恢复疲劳后清 bonus）
    user->ResetDailyQuest();
    user->ResetTrainingQuest();
    return true;
}

void Inter_LoadEtc::verifyCharacVillage(CUser* user)
{
    if (G_GameWorld()->IsPVPChannel())
        return;

    char vill = (char)user->getCurCharacVill();
    Village* v = G_GameWorld()->GetVillage(vill);
    if (v == 0)
    {
        user->setCurCharacVill(1);
        return;
    }

    bool needMove = user->get_charac_level() < v->m_field2c;   // +0x2c（最低等级门槛）
    if (v->m_questIdx != 0 &&                                  // +0x30
        !user->getCurCharacQuestR()->isClearQuest(v->m_questIdx))
        needMove = true;

    if (!needMove)
        return;

    switch (vill)
    {
    case 2:  user->setCurCharacVill(1); break;
    case 3:  user->setCurCharacVill(2); break;
    case 4:  user->setCurCharacVill(1); break;
    case 5:  user->setCurCharacVill(2); break;
    case 6:  user->setCurCharacVill(3); break;
    case 9:  user->setCurCharacVill(3); break;
    case 11: user->setCurCharacVill(2); break;
    default: break;
    }
}

bool Inter_LoadEtc::RequestDBEventArad(CUser* user)
{
    arad::SigLoadRewardEventItem sigEventItem;
    sigEventItem.set(user->get_acc_id());           // ORIG W 0x081866b6

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x1193),
                       true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard,
        ARAD::ENUM_INTERNALPACKET_JPN(0x26), (int)user->GetUID());
    guard.operator->()->put_binary(&sigEventItem, sizeof(sigEventItem));  // ptEv→0x56
    bool ok = GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return ok != false;
}

bool Inter_LoadEtc::SetPvpRecord(CUser* user, SIG_LOAD_ETC* sig)
{
    PvpResultType* pvp = &sig->m_pvpResult;
    if (!user->SetPvpRecord(*pvp))
    {
        user->disableSavePvP();
        return false;
    }
    user->disableSavePvP();

    int lossGrade = GlobalData::s_ref_pvp_grade->getPvPExpLossGrade();
    int minPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(lossGrade);
    const PvpResultType* cur = user->getPVPResultRefR();
    // ORIG：!(cur+0x14 < lossGrade || cur+8 <= minPoint) 才进日切扣减分支（无 null 检查）
    bool apply = !(cur->m_pvpGrade < lossGrade || cur->m_expPoint <= minPoint);
    if (apply)
    {
        long now = GlobalData::s_systemTime_.getCurSec();
        long lastPlay = user->getCurCharacLastPlayTick();
        if (CheckDailyScheduleTime(4, lastPlay, now))
        {
            PvpResultType* w = user->getPVPResultRefW();
            int dec = GlobalData::s_ref_pvp_grade->getPvPExpLossExp();
            w->m_expPoint = std::max(minPoint, w->m_expPoint - dec);
        }
    }
    return true;
}

bool Inter_LoadEtc::SendResult(CUser* user)
{
    PacketGuard packet;
    packet.put_header(1, 4);
    packet.put_byte(1);
    packet.put_int(user->getRegTime());
    packet.put_int((int)user->getCurCharacCreateTime());
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(user->getCurCharacTotalFatigue());
    packet.put_short(user->getCurCharacTotalMaxFatigue());
    packet.put_short(user->getCurCharacUsedFatigue());

    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    // ORIG 无 null 检查，直接解引用调用 GetPremiumInfoList(0x270f)。
    std::vector<WongWork::SUserPremiumInfo> infos =
        premium->GetPremiumInfoList(0x270f);
    int index = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    long now = GlobalData::s_systemTime_.getCurSec();
    for (size_t i = 0; i < infos.size(); ++i)
    {
        const WongWork::SUserPremiumInfo& info = infos[i];
        if (info.m_state == 2)                       // ENUM_PREMIUM_STATE_2 等待开始
            continue;
        if (G_GameWorld()->IsPvPSkilTreeChannel() && user->get_pvp_grade() <= 9 &&
            info.m_type == 0x21)
            continue;
        // ORIG：非 PvP 树通道或 grade>9 或 type!=0x21 → 写入
        if (!(G_GameWorld()->IsPvPSkilTreeChannel() && user->get_pvp_grade() <= 9) ||
            info.m_type != 0x21)
        {
            packet.put_byte(info.m_type & 0xff);
            packet.put_int(info.m_end - (int)now);
            ++count;
        }
    }

    if (user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1323, 5);
        trace("bool Inter_LoadEtc::SendResult()");
        return false;
    }

    user->verifyDoingQuest();
    UserQuest* questR = user->getCurCharacQuestR();
    for (int i = 0; i < 0x14; ++i)
    {
        int idxVal = questR->m_quest[i];
        packet.put_short(idxVal == 0 ? 0xffff : idxVal & 0xffff);
        int stepVal = questR->m_trigger[i];
        packet.put_int(stepVal);
    }
    for (int i = 0; i < 5; ++i)
    {
        int v = *reinterpret_cast<const int*>(&questR->m_authenData.m_authen[i * 4]);
        packet.put_int(v);
    }
    packet.put_byte((int)(signed char)user->getCurCharacVill());
    packet.put_byte(user->GetTutorialSkipable());

    int tIndex = packet.get_index();
    int tCount = 0;
    packet.put_byte(0);
    for (unsigned int j = 0; j < 0x80; ++j)
    {
        if (user->getCurCharacEscaladeTutorialFlag(j) != 0)
        {
            packet.put_byte((int)j);
            ++tCount;
        }
    }
    packet.put_byte(tIndex, tCount);
    packet.put_short(user->GetFatigueBatteryCharging());
    packet.put_short(user->getCurCharacFatigueGrownUpBuff());

    char punishOverThirty = user->isTradePunishOverThirtyDays() ? 1 : 0;
    packet.put_byte(punishOverThirty);
    packet.finalize(true);
    user->Send(packet);
    AvatarCoin::SendSyncPacket(user);
    return true;
}

void Inter_LoadEtc::giveBreakAwayReward(CUser* user)
{
    char title[21];
    char body[256];

    if (user->getUserBreakAwayRewardLuckPoint() < 1)
        user->setBreakAwayRewardData(false);

    int itemId = 0;
    int itemCnt = 0;
    itemId = user->getBreakAwayRewardItemId();
    itemCnt = user->getBreakAwayRewardAItemCnt();
    if (itemId == 0 && itemCnt == 0)
        return;

    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_135", 0),
            0x14);

    switch (user->getBreakAwayRewardItemOrder())
    {
    case 0:
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_136", 0), 0xff);
        break;
    case 2:
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_142", 0), 0xff);
        break;
    case 4:
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_160", 0), 0xff);
        break;
    case 1:
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_158", 0), 0xff);
        break;
    case 3:
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_159", 0), 0xff);
        break;
    default:
        return;
    }

    Inven_Item mailItem;
    int coinCount = 0;
    if (itemId == 0)
    {
        coinCount = itemCnt;
        CCirculationStatistic* stat = GetInstanceCirculationStatistic();
        stat->AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD(0x10), user,
                                       (unsigned int)itemCnt);
    }
    else
    {
        CItem* found = G_CDataManager()->find_item(itemId);
        if (found == 0)
            return;
        (void)found->is_stackable();                 // ORIG 虚槽 +0xc 返回值弃用
        found->make_item(mailItem);                  // ORIG 虚槽 +0x8
        mailItem.set_add_info(itemCnt);
    }

    unsigned int accId = user->get_acc_id();
    unsigned int charNo = user->getCurCharacNo();
    int serverGroup = (int)user->GetServerGroup();
    int sent = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, coinCount, charNo, body, (int)strlen(body),
        7u, (ENUM_SERVER_GROUP)serverGroup, false, false);

    if (sent == 0)
    {
        STBreakAwayRewards* rewards = user->getBreakAwayRewards();
        DB_LoadBreakAwayRewardData::makeRequest((int)user->GetUID(), accId, charNo,
                                                *rewards);
        user->setBreakAwayRewardItemId(0);
        user->setBreakAwayRewardAItemCnt(0);
    }
}

void Inter_LoadEtc::processGrowthWeaponEvent(CUser& user)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(100);
    if (!ev->IsEventing(0))
        return;
    GrowthEquipEvent* growth = static_cast<GrowthEquipEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(100));
    if (growth == 0)
        return;
    growth->GiftGrowthWeaponBox(user);                  // ORIG T 0x080f7050
    growth->SendGrowthWeaponEventInfo(user);            // ORIG T 0x080f7226
}

void Inter_LoadEtc::processGrowthCreatureEvent(CUser& user)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    if (!ev->IsEventing(0))
        return;
    GiveGrowCreatureEvent* growth = static_cast<GiveGrowCreatureEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x67));
    if (growth == 0)
        return;
    growth->SendGrowthCreatureEventInfo(user);          // ORIG T 0x081b6b70
}

void Inter_LoadEtc::processSeriaRoomAniDecoEvent(seriaRoom_AniDeco info, CUser& user)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x69);
    if (ev == 0 || !ev->IsEventing(0))
        return;
    user.setSeriaRoomAniDecoInfo(info);
    SeriaRoomAniDecoEvent* seria = static_cast<SeriaRoomAniDecoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x69));
    if (seria == 0)
        return;
    seria->SendEventInfo(user);                         // ORIG T 0x0827b99a
    long now = GlobalData::s_systemTime_.getCurSec();
    long lastPlay = user.getCurCharacLastPlayTick();
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               lastPlay, now))
        seria->SendAniDecoItem(user);                   // ORIG T 0x0827b75a
}

void Inter_LoadEtc::processCheckInvalidExpPerLevel(CUser& user)
{
    int levelExpLow = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level());
    int levelExpHigh = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level() + 1);
    unsigned exp = (unsigned)user.get_charac_exp();
    if (!(exp < levelExpLow || levelExpHigh < exp))
        return;

    if (user.isTradePunishType(1))
        return;
    user.SetTradePunishType(1);

    DB_SaveUserHandicap::makeRequest(user.get_acc_id(), 0x9c42, 0xb, 0, 3, 0);

    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x6c7d, 5);
    trace("[InvalidExp] Invalid character exp per level. "
          "(mid:%u, charac:%u, level:%d, exp:%d,%d,%d)",
          user.get_acc_id(),
          user.get_charac_no(-1),
          user.get_charac_level(),
          user.get_charac_exp(),
          levelExpLow,
          levelExpHigh);
}

void Inter_LoadEtc::_checkUserDisadvantage(CUser* user)
{
    if (user->IsGameMasterMode())
        return;

    ServiceRestrictManager* restrictMgr =
        ARAD::Singleton<ServiceRestrictManager>::Get();
    if (!restrictMgr || !restrictMgr->isRestricted(RestrictCategory::Enum(3), 1))
        return;
    if (user->getCurCharacR() == 0)
        return;

    bool invalid = false;
    if (user->get_charac_level() < 0x14 || user->getStdDropRate() == 0)
        invalid = true;
    if (invalid)
        return;

    // 遍历 0..29999 任务清除位（CQuestClear 区位于 questR+4，含 m_user 头）
    char hasClearedQuest = 0;
    for (unsigned int i = 0; i < 30000; ++i)
    {
        UserQuest* questR = user->getCurCharacQuestR();
        WongWork::CQuestClear* cleared = &questR->m_cleared;
        if (cleared->isClearedQuest(i))
        {
            hasClearedQuest = 1;
            break;
        }
    }

    int penalty = 0;
    if (hasClearedQuest != 1)
    {
        penalty = 0x28;                                 // 未清任务：+40
        const CCargo* cargo = user->getCurCharacCargoR();
        if (cargo->GetItemCount() == 0)
            penalty += 10;                              // 无货物再 +10
    }
    else
    {
        // ORIG：hasQuest=1 → 基线 40（汇编 add $0x28 无条件后再减）保持
        penalty = 0;
    }

    int hundredBase = 100;
    penalty = std::min(hundredBase, penalty);           // min(penalty, 100)
    penalty = 100 - penalty;                            // 100 - min
    int dropRate = user->getStdDropRate();
    penalty = std::min(penalty, dropRate);              // 再与当前掉率取小

    if (dropRate != penalty)
    {
        DB_SaveUserHandicap::makeRequest(user->get_acc_id(), 0x9c41, 0x65,
                                         penalty, 3, 0);
        user->setStdDropRate(penalty);
    }
}

bool Inter_LoadEtc::ProcessOneDayLethe(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    user->SetOneDayLetheFlag((unsigned char)sig->m_backupSkill[0x198]);  // +0x2b95（backupSkill 尾哨）
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    bool useLethe = false;
    unsigned char flag = (unsigned char)sig->m_backupSkill[0x198];
    if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) ||
        flag == CPremiumLetheManager::USE_LETHE_STATE ||
        flag == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        useLethe = true;

    if (useLethe)
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill), ENUM_SKILL_TREE_KIND(0)))
            return 0x40c6;

        if (flag != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag(false);
        }

        Stream* stream = GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x40d2);
        CStreamGuard guard(stream, true);
        guard << 0x15c;
        guard << (int)user->GetUID();
        SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
            guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
        out->m_accID = user->get_acc_id();
        out->m_characNo = user->getCurCharacNo();
        memset(out->m_skills, 0, 0x198);
        out->m_state2 = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_state = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_treeKind = 0;
        user->SaveBackupSkillInfo(reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
            out->m_skills), ENUM_SKILL_TREE_KIND(0));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        unsigned char flag2 = sig->m_oneDayLetheFlag;
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            flag2 == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            int job = user->get_charac_job();
            _Mastered_skill* skillR = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skillslot_buf(ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSave(static_cast<const _Mastered_skill*>(skillR),
                                        (unsigned)job, 0, eSkillSaveReason(0));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 flag2 == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            int job = user->get_charac_job();
            _Mastered_skill* skillR = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skillslot_buf(ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSave(static_cast<const _Mastered_skill*>(skillR),
                                        (unsigned)job, 0, eSkillSaveReason(0));
        }
    }
    return false;
}

bool Inter_LoadEtc::ProcessOneDayLethe2ND(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    user->SetOneDayLetheFlag2ND((unsigned char)sig->m_backupSkill2ND[0x198]);  // +0x2d2f
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    bool useLethe = false;
    unsigned char flag = (unsigned char)sig->m_backupSkill2ND[0x198];
    if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) ||
        flag == CPremiumLetheManager::USE_LETHE_STATE ||
        flag == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        useLethe = true;

    if (useLethe)
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill2ND), ENUM_SKILL_TREE_KIND(1)))
            return 0x4111;

        if (flag != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag2ND(false);
        }

        Stream* stream = GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x411d);
        CStreamGuard guard(stream, true);
        guard << 0x15c;
        guard << (int)user->GetUID();
        SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
            guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
        out->m_accID = user->get_acc_id();
        out->m_characNo = user->getCurCharacNo();
        memset(out->m_skills, 0, 0x198);
        out->m_state2 = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_state = CPremiumLetheManager::NOT_USE_LETHE_STATE;
        out->m_treeKind = 1;
        user->SaveBackupSkillInfo(reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
            out->m_skills), ENUM_SKILL_TREE_KIND(1));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        unsigned char flag2 = sig->m_oneDayLetheFlag2ND;
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            flag2 == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            int job = user->get_charac_job();
            _Mastered_skill* skillR = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skillslot_buf(ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSave(static_cast<const _Mastered_skill*>(skillR),
                                        (unsigned)job, 1, eSkillSaveReason(0));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 flag2 == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            int job = user->get_charac_job();
            _Mastered_skill* skillR = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skillslot_buf(ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSave(static_cast<const _Mastered_skill*>(skillR),
                                        (unsigned)job, 1, eSkillSaveReason(0));
        }
    }
    return false;
}

void Inter_LoadEtc::processAttendAnceEvent(SIG_ATTENDANCE_EVENT* sig, CUser& user)
{
    if (sig->m_isEnabled != 1)                       // ORIG *param_1 == 1
        return;

    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x60);
    if (ev == 0 || !ev->IsEventing(0))
        return;
    AttendanceEvent* attendance = static_cast<AttendanceEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x60));
    if (attendance == 0)
        return;

    Inven_Item mailItem;
    // ORIG 0x084e8102 直接 call _ZN15AttendanceEvent13getRewordItemEv（2026-08-28
    // OrigEventLeaf8f 实证），返回 (short)m_addInfo.m_info0。
    int rewardIdx = attendance->getRewordItem();
    CItem* item = G_CDataManager()->find_item(rewardIdx);
    if (item == 0)
        return;
    item->make_item(mailItem);                       // ORIG 虚槽 +0x8
    mailItem.set_add_info(1);

    char sender[256];
    memset(sender, 0, sizeof(sender));
    char title[21];
    memset(title, 0, sizeof(title));
    strncpy(sender,
            g_scriptStringManager_.findChangedString(4, "game_server_msg_276", 0),
            0xff);
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_275", 0),
            0x14);

    unsigned int charNo = user.getCurCharacNo();
    int serverGroup = (int)user.GetServerGroup();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, 0u, charNo, sender, (int)strlen(sender), 0u,
        (ENUM_SERVER_GROUP)serverGroup, false, false);

    DB_InsertAttendanceEvent::makeRequest((int)user.GetUID(),
                                          user.get_acc_id(), charNo);
    sig->m_isEnabled = 0;
}

int Inter_LoadEtc::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_LOAD_ETC* sig = reinterpret_cast<SIG_LOAD_ETC*>(data);

    if (user->get_state() == 0)
        return 0xa7b;
    if (user->getCurCharacR() == 0)
        return 0xa7f;
    if (sig->m_field8 == 1)
        return 0xa96;

    PacketGuard packet;
    {
        int grade = sig->m_pvpResult.m_pvpGrade;
        if (!G_CDataManager()->IsAvailablePvPChannel(grade))
        {
            LogManager::logFormat(1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__, 0xa9b,
                "Inter_LoadEtc::dispatch_sig pvp_grade = %d", grade);
            return 0xa9c;
        }
    }

    // ---- 失败链前置处理（ORIG 各 err 分支统一收口于 LAB_084c2653）----
    int result;
    bool pvpTreeSkip0 = false;
    bool bccBranch = false;
    bool c14Branch = false;
    bool pvpTreeSkip = false;
    do
    {
        if (!SetPvpRecord(user, sig)) { result = 0xaa2; break; }
        if (!SetRankRecord(user, sig)) { result = 0xaa5; break; }

        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xac5, 0);
        trace("CharLastPlayTick char(%d) t(%d)",
              (unsigned)sig->m_characNo,
              (unsigned)user->getCurCharacR()->m_field7b);

        user->ProcPremiumFatigue();
        WongWork::CUserPremium* premium = user->GetPremiumInfo();
        if (premium != 0)
            premium->RecalcAdditionalInfo(user);

        if (!SetFatigue(user)) { result = 0xacf; break; }
        verifyCharacVillage(user);
        if (!SendResult(user)) { result = 0xae6; break; }

        user->SendRestrictedGoods(false, 0);
        // §4.3 具名化：+0x2308 区在 SigTypes_C.h:464 已有 union 具名叠加
        // （m_cooltimeItem @+0x230c = m_pad2308+4、m_effectItem @+0x24a4 =
        //  m_pad2308+0x19c；写点证据 CUserCharacInfo::GetContinuousItemInfo
        //  0x08677f24 / DB_LoadEtc LoadCharacItemStat.md cap 0x194）。
        user->LoadCoolTimeItemInfo(
            reinterpret_cast<CONTINUOUS_ITEM_INFO*>(sig->m_cooltimeItem));
        user->LoadEffectItemInfo(
            reinterpret_cast<CONTINUOUS_ITEM_INFO*>(sig->m_effectItem));

        typedef charac_expand::CData CDataAlias;
        for (;;)
        {
            CDataAlias* exp6 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(6)));
            exp6->loadData(user, sig->m_expandData6);
            CDataAlias* exp8 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(8)));
            // [hc 收] ORIG 0x084bb9e2..：sig->m_mixedRegion 为 SIG_LOAD_ETC 载荷
            // 线格式缓冲（§5.3 序列化缓冲豁免），+0x415 为该 expand-data 的固定布局
            // 偏移；loadData(user, ptr) 取地址直传，偏移为承载格式固有，seek 无漏。
            exp8->loadData(user, sig->m_mixedRegion + 0x415);
            CDataAlias* expD = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xd)));
            // [hc 收] +0xd58 同承载体 0xd 型 expand-data 布局偏移（§5.3 豁免）。
            expD->loadData(user, sig->m_mixedRegion + 0xd58);
            break;
        }

        int tagIdx = user->get_charac_idx(sig->m_fieldC);
        if (sig->m_fieldC != 0 && tagIdx != -1 &&
            user->getTagCharacR() != 0)
        {
            char* tagSkillBuf = user->getTagCharacSkillW();
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(tagSkillBuf);
            (void)skillW;
            			skillW->set_skill_slot((char*)&sig->m_tagEtc.m_skillSlot,
			                       (char*)&sig->m_tagEtc.m_skillCombo, 0x198);
        }
        if (tagIdx == -1)
            sig->m_mixedRegion[0xd3b] = 0;

        {
            CDataAlias* expB = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xb)));
            // [hc 收] +0xbe7 / +0xd3c 同承载体 0xb/0xa 型 expand-data 布局偏移
            // （§5.3 线格式序列化缓冲豁免，ORIG loadData 实参即固定偏移）。
            expB->loadData(user, sig->m_mixedRegion + 0xbe7);
            CDataAlias* expA = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(10)));
            expA->loadData(user, sig->m_mixedRegion + 0xd3c);
            CDataAlias* exp10 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x10)));
            exp10->loadData(user, sig->m_mixedRegion + 0xd7c);
            CDataAlias* expE = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xe)));
            expE->loadData(user, sig->m_expandDataE);
            CDataAlias* expF = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xf)));
            expF->loadData(user, sig->m_expandDataF);
            CDataAlias* exp11 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x11)));
            exp11->loadData(user, sig->m_mixedRegion + 0x2308);
            CDataAlias* exp12 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x12)));
            exp12->loadData(user, sig->m_mixedRegion + 0x2310);
            CDataAlias* exp13 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x13)));
            exp13->loadData(user, reinterpret_cast<char*>(sig->m_attendanceEvent.m_pad + 1));
        }

        // §4.3 具名化：m_expandDataF(+0xb1a4)+0x1804 = +0xc9a8 =
        // SigTypes_C.h:546 具名 m_achievementLastUpdate（DB_LoadEtc
        // LoadAchievement.md：+0xb1a4 解压 0x1800 数据、+0xc9a4 计数、
        // +0xc9a8 last_update get_uint → +0xc9ac 衔接具名字段）。
        if (sig->m_achievementLastUpdate == 0)
        {
            CAchievement* achievement =
                reinterpret_cast<CAchievement*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(0xf)));
            achievement->adjust();
        }

        // ---- 登录历史（CNetwork 视图位于 CUser+0xe0）----
        // ORIG:get_ip()==0 与否则两分支（仅 peerIp 缓冲写法不同，实参序一致），
        // 每分支独立调用 GetServerGroup/GetLogInOutState/G_CEnvironment/get_port/
        // GetPeerIP2/getWebAddress + Login（无缓存局部）。
        {
            CNetwork<4096, 450000>* net = &user->m_network;
            char peerIp[16];
            memset(peerIp, 0, sizeof(peerIp));
            if (net->get_ip() == 0)
            {
                int serverGroup = (int)user->GetServerGroup();
                unsigned char logState = user->GetLogInOutState();
                int channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
                unsigned short port = net->get_port();
                net->GetPeerIP2(peerIp + 0x10, 0x10);
                user->m_historyLog.Login(user->getWebAddress(), peerIp,
                                         (int)(port & 0xffff), channelNo, logState,
                                         serverGroup);
            }
            else
            {
                int serverGroup = (int)user->GetServerGroup();
                unsigned char logState = user->GetLogInOutState();
                int channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
                unsigned short port = net->get_port();
                net->GetPeerIP2(peerIp, 0x10);
                user->m_historyLog.Login(user->getWebAddress(), peerIp,
                                         (int)(port & 0xffff), channelNo, logState,
                                         serverGroup);
            }
        }

        long nowSec = GlobalData::s_systemTime_.getCurSec();
        user->SetLoginTick(nowSec);
        nowSec = GlobalData::s_systemTime_.getCurSec();
        user->SetCharacStartPlayTick(nowSec);

        CSkillTreeParam skillTreeParam(user, sig, packet);

        // ======== 主链（对应 ORIG try/状态码分支）========
        do
        {

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                unsigned char flagByte = (unsigned char)sig->m_mixedRegion[0x11];
                unsigned char refByte = (unsigned char)
                    G_CDataManager()->m_pvpSkillTree.m_field50;  // +0xa850 基线标志
                if (flagByte != refByte)
                {
                    int myGrade = user->get_pvp_grade();
                    // ORIG:基线读取与 getInitSkillTreeGrade 调用各自重取 G_CDataManager()。
                    unsigned char initBase =
                        (unsigned char)G_CDataManager()->m_pvpSkillTree.m_field50;
                    int initGrade =
                        G_CDataManager()->m_pvpSkillTree.getInitSkillTreeGrade(myGrade);
                    if (myGrade < initGrade)
                    {
                        memset(sig->m_mixedRegion, 0, 0x414);
                        pvpTreeSkip = true;
                    }
                }
            }

            if (!user->SetSkill(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion),
                                reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)))
            {
                user->disableSaveSkill();
                result = 0xb91;
                break;   // 跳出主链 → 析构 skillTreeParam / packet
            }
            user->disableSaveSkill();

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                bool grantFlagged = (sig->m_mixedRegion[0x14] == 0 &&
                    *reinterpret_cast<int*>(sig->m_mixedRegion + 0x1df) != 0);
                if (grantFlagged)
                {
                    SetPvPSkillTree(user,
                        reinterpret_cast<const _Mastered_skill*>(
                            sig->m_mixedRegion + 0xc),
                        ENUM_SKILL_TREE_KIND(0));
                    SetPvPSkillTree(user,
                        reinterpret_cast<const _Mastered_skill*>(
                            sig->m_mixedRegion + 0x1a8),
                        ENUM_SKILL_TREE_KIND(1));
                }
                else
                {
                    user->givePvPSkillTree(0, false, 3);
                    user->givePvPSkillTree(0, true, 3);
                    int grade = user->get_pvp_grade();
                    char secondGrow = user->getCurCharSecondGrowType();
                    char firstGrow = user->getCurCharFirstGrowType();
                    int job = user->get_charac_job();
                    int spPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                        job, (int)firstGrow, (int)secondGrow, grade, true);
                    SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                        user->getCurCharacSkillW());
                    skillW->set_remain_sp_at_index(spPoint,
                                                   ENUM_SKILL_TREE_KIND(0));
                    skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
                    skillW->set_remain_sp_at_index(spPoint,
                                                   ENUM_SKILL_TREE_KIND(1));
                }
            }

            if (skillTreeParam.SetSkillTree(this))
            {
                bccBranch = true;
                break;
            }

            {
                item_lock::CItemLock* itemLock = reinterpret_cast<item_lock::CItemLock*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2)));
                std::vector<item_lock::stItemLockRef>& lockVec = itemLock->GetItemLockRefVec();
                CTitleBook* titleBook = reinterpret_cast<CTitleBook*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(0xe)));
                titleBook->getItemLockList(lockVec);
                item_lock::CItemLock* itemLock2 = reinterpret_cast<item_lock::CItemLock*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2)));
                itemLock2->VerifyItemLock(user);
            }

            if (G_CGameManager()->insert_game_world(user) != 1)
            {
                c14Branch = true;
                break;
            }

            user->sendCharacQp();
            user->sendCharacQuestPiece();
            {
                CBoosterGage* booster = reinterpret_cast<CBoosterGage*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(0x11)));
                booster->send_data(user, 0);
            }
            user->SendItemMakingSkillInfo();

            if (G_GameWorld()->IsPvPSkilTreeChannel() &&
                ((sig->m_mixedRegion[0x14] != 0 ||
                  *reinterpret_cast<int*>(sig->m_mixedRegion + 0x1df) == 0 ||
                  pvpTreeSkip)))
            {
                packet.clear();
                packet.put_header(0, 0x154);
                packet.finalize(true);
                user->Send(packet);
            }
            user->CheckMaxLuckyLevel();

            user->SetCurConditionEventStep((short)sig->m_curConditionEventStep);
            user->SetCurConditionEventRewardStep((short)sig->m_curConditionEventRewardStep);
            user->SetCurCharacRevengeData(
                sig->m_curVillageAttackCount,
                sig->m_curRevengeDungeonCount);
            if ((short)sig->m_properDungeonClearCount < 0)
                sig->m_properDungeonClearCount = 0;
            user->SetProperDungeonClearCount((short)sig->m_properDungeonClearCount);
            user->SetCurCharacUsedGiftFatigueQuantity(
                sig->m_curCharacUsedGiftFatigueQuantity);

            if (user->getCurCharacR() != 0)
            {
                user->SetFirstTimeLogin(false);
                int fatigueBase = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
                CCoinRefillEvent* coinEvent = static_cast<CCoinRefillEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0xd));
                if (coinEvent != 0 && coinEvent->IsEventing(0))
                    fatigueBase = coinEvent->getLastEventTime();
                user->setBlueMarbleEnterCount(sig->m_blueMarbleEnterCount);

                long nowTick = GlobalData::s_systemTime_.getCurSec();
                long lastPlay = user->getCurCharacR()->m_field7b;
                if (CheckDailyScheduleTime(fatigueBase, lastPlay, nowTick))
                {
                    user->SetFirstTimeLogin(true);
                    user->RecoverCoin(1);
                    user->AddDailyItem();
                    user->ClearCurConditionEventStep();
                    user->ClearCurConditionEventRewardStep();
                    user->ClearProperDungeonClearCount();
                    user->SetFirstLogin();

                    bool newUserLevelGate =
                        user->IsFirstTimeLogin() &&
                        user->getCurCharacGrowType() == 0;
                    if (newUserLevelGate)
                    {
                        WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                        if (hack != 0)
                            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1a1, 1u, 0, 0);
                    }
                    user->ResetCurCharacUsedGiftFatigueQuantity();

                    int level = user->get_charac_level();
                    bool secondGrowNew =
                        user->IsFirstTimeLogin() &&
                        user->getCurCharSecondGrowType() == 0;
                    if (secondGrowNew)
                    {
                        int loops = 0;
                        int subCode = 0;
                        if (level < 0x3d) { if (level > 0x32) { loops = 1; subCode = 0xb; } }
                        else { loops = 3; subCode = 0xc; }
                        for (int i = 0; i < loops; ++i)
                        {
                            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                            if (hack != 0)
                                hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u,
                                                        subCode, 0);
                        }
                    }
                    bool firstGrowNew =
                        user->IsFirstTimeLogin() &&
                        user->getCurCharFirstGrowType() == 0;
                    if (firstGrowNew)
                    {
                        int loops = 0;
                        int subCode = 0;
                        if (level < 0x33)
                        {
                            if (level < 0x29)
                            {
                                if (level < 0x1f) { if (level > 0x14) { loops = 1; subCode = 1; } }
                                else { loops = 2; subCode = 2; }
                            }
                            else { loops = 3; subCode = 3; }
                        }
                        else { loops = 5; subCode = 4; }
                        for (int i = 0; i < loops; ++i)
                        {
                            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                            if (hack != 0)
                                hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u,
                                                        subCode, 0);
                        }
                    }
                    user->resetBlueMarbleEnterCount();
                }
                user->sendBlueMarbleEnterCount();
                user->SendOpenflag();
                // ORIG:RewardItem 实参与后续判定各自调用 getCurCharacR()。
                RewardItem(user, user->getCurCharacR()->m_field7b,
                           0x4b4515f0, 0x4b578af0, 0x27addd,
                           "game_server_msg_53", "game_server_msg_131", 0);
                if (user->getCurCharacR()->m_field7b != 0)
                    user->initAllSkill();
            }

            user->SendRevengeDungeon();
            user->SendConditionEventInfo();
            user->SendProperDungeonClearCount();
            processSeriaRoomAniDecoEvent(
                *reinterpret_cast<seriaRoom_AniDeco*>(sig->m_comboSkill + 0xb2), *user);
            processCheckInvalidExpPerLevel(*user);
            WongWork::CHandlePremium::handlePremiumEffect(user, ENUM_PREMIUM_TYPE(9999));

            for (int evIdx = 1; evIdx < 2; ++evIdx)
            {
                if (CServerEvent::IsEventing(evIdx))
                {
                    int interval = CServerEvent::GetEventInterval(evIdx);
                    Inter_MonitorServerEventStart::SendEventNotiPacket(
                        true, (short)evIdx, interval, user);
                }
            }
            {
                char winnerSide = GlobalData::s_power_manager->GetWinnerSide();
                packet.clear();
                packet.put_header(0, 0xbc);
                if (G_GameWorld()->IsIntegratedPvPBaseChannel())
                    packet.put_byte(99);
                else
                    packet.put_byte((int)winnerSide);
                packet.finalize(true);
                user->Send(packet);
                if (GlobalData::s_power_manager != 0)
                {
                    if (GlobalData::s_power_manager->CheckCompleteDBLoadStatueInfo())
                        GlobalData::s_power_manager->SendPowerWarUserStatueInfo(user);
                }
            }
            {
                packet.clear();
                GlobalData::s_event_manager->MakeNotiEventInfo(packet, user);
                user->Send(packet);
            }
            skillTreeParam.SendSkillPointReturnInfo();
            user->SendDungeonClearList();
            user->EnableCharacInfo(CACHE_SECTION(0xb));
            _checkUserDisadvantage(user);
            user->onSelectCharacter();

            if (user->NeedSecuReward() && user->getRewardType() != 0)
            {
                unsigned char rewardType = user->getRewardType();
                user->AddSecuRewardItem(rewardType);
                user->setRewardType(0);
            }

            // ORIG:检查与发信各调一次 getPad();判定用 getChangedPwd() 访问器。
            if (user->getPad()->getChangedPwd() != 0)
            {
                user->getPad()->SendChagePwdRewardMail(user);
            }

            user->discard_mouse_register();

            // ORIG:两个判定各自调用 get_charac_level()。
            if (user->get_charac_level() > 0x27)
                DB_LoadBloodBestRecord::makeRequest((int)user->GetUID(),
                                                    user->getCurCharacNo());
            if (user->get_charac_level() > 0x3b)
                DB_LoadDimension::makeRequest((int)user->GetUID(),
                                              user->getCurCharacNo());

            if (user->isBreakAwayRewardData())
                giveBreakAwayReward(user);

            if (user->getBreakAwayDungeonClearCnt() > 0)
            {
                int questTime = user->getBreakAwayQuestTime();
                int remain = questTime -
                             (int)GlobalData::s_systemTime_.getCurSec();
                if (remain < 1)
                {
                    user->setBreakAwayDungeonClearCnt(-1);
                }
                else
                {
                    packet.clear();
                    packet.put_header(0, 0x136);
                    packet.put_int(remain);
                    packet.put_byte(3 - user->getBreakAwayDungeonClearCnt());
                    packet.finalize(true);
                    user->Send(packet);
                }
            }

            {
                // ORIG:判定与双 send 分支各自调用 G_GameWorld()。
                int lvlNow = user->get_charac_level();
                int minLvl = G_GameWorld()->getDungeonMinimumRequiredLevel(-1);
                bool dungeonNotice = false;
                if (lvlNow >= minLvl)
                {
                    if (!G_GameWorld()->IsPVPChannel())
                        dungeonNotice = true;
                }
                if (dungeonNotice)
                {
                    if (!G_CDataManager()->m_serverParameter.isDungeonOpen(-1))
                    {
                        G_GameWorld()->send_user_dungeon_inout_message(
                            user, 0x2aff, false);
                    }
                    else
                    {
                        G_GameWorld()->send_user_dungeon_inout_message(
                            user, 0x2aff, true);
                    }
                }
            }

            {
                CAutoMarketConditionsControl* autoMarket =
                    G_CDataManager()->GetAutoMarketContitionsControl();
                if (autoMarket != 0)
                    autoMarket->SendNotiSetDurability(user);
            }

            CGlobalEffectManager* fx = GlobalData::s_globalEffectManager_;
            if (fx != 0 && fx->getEffectNum() != 0)
            {
                packet.clear();
                fx->makeGlobalEffectInfo(packet);
                packet.finalize(true);
                user->Send(packet);
            }

            {
                CSharedServerMessageManager* sharedMsg =
                    G_CGameManager()->GetSharedServerMessageManager();
                if (sharedMsg != 0)
                {
                    if (!sharedMsg->IsSetServerMessage())
                        sig->m_eventMessageCount = 0;
                    else
                    {
                        EVENT_SERVER_MESSAGE* msg =
                            reinterpret_cast<EVENT_SERVER_MESSAGE*>(
                                sharedMsg->GetCurServerMessage());
                        if (msg != 0)
                        {
                            // ORIG:const_iterator 循环,7 个字段逐一经 operator*() 拷贝
                            // (de 站点 ×6/字节域 ×1;m_count 先整体拷贝)。
                            SIG_EVENT_SERVER_MESSAGE* dstMsg =
                                reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(&sig->m_eventMessageCount);
                            dstMsg->m_count = msg->m_count;
                            if (msg->m_count == msg->m_vecMsg.size())
                            {
                            std::vector<stSERVER_MESSAGE>::const_iterator smi =
                                msg->m_vecMsg.begin();
                            std::vector<stSERVER_MESSAGE>::const_iterator smiEnd =
                                msg->m_vecMsg.end();
                            int idx = 0;
                            while (smi != smiEnd)
                            {
                                // ORIG:7 个字段读各自经 operator*()（de 站点 ×6）。
                                dstMsg->m_msg[idx].m_lenMsg = (*smi).m_lenMsg;
                                dstMsg->m_msg[idx].m_kind = (*smi).m_kind;
                                dstMsg->m_msg[idx].m_endTime = (*smi).m_endTime;
                                memcpy(dstMsg->m_msg[idx].m_msg2, (*smi).m_msg2,
                                       sizeof((*smi).m_msg2));
                                dstMsg->m_msg[idx].m_index = (*smi).m_index;
                                memcpy(dstMsg->m_msg[idx].m_msg, (*smi).m_msg,
                                       sizeof((*smi).m_msg));
                                dstMsg->m_msg[idx].m_lenMsg2 = (*smi).m_lenMsg2;
                                idx = idx + 1;
                                smi++;
                            }
                            }
                        }
                    }
                    sharedMsg->SendServerMessageInfo(
                        user, reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(&sig->m_eventMessageCount), 0);
                }
            }

            // [hc 收] +0x330 为 SIG_LIMIT_ITEM_INFO 在该 SIG_LOAD_ETC 承载体缓冲
            // 中的固定布局偏移；经 view 类型 cast 直读（§5.3 序列化缓冲豁免）。
            user->SetLimitInfo(reinterpret_cast<SIG_LIMIT_ITEM_INFO*>(
                sig->m_pad2308 + 0x330));
            user->SendLoginToPCRoomServer();

            {
                CDataAlias* exp7 = static_cast<CDataAlias*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(7)));
                exp7->loadData(user, sig->m_mixedRegion + 0x55f);
            }

            {
                bool needExpertSync =
                    user->GetCurCharacExpertJobType() != 0 ||
                    user->GetCurCharacExpertJobExp() == -1;
                if (needExpertSync)
                {
                    Stream* stream = GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xed7);
                    CStreamGuard guard(stream, true);
                    guard.operator*()->operator<<(0x180);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
            }

            {
                // ORIG:empty 判定与 makeItemLimitEditionInfo 各自重新调用 getter。
                // ORIG:无 null 判定,empty 与 makeInfo 各调一次 getter。
                if (!G_CDataManager()->getItemLimitEditionMgr()->empty())
                {
                    packet.clear();
                    packet.put_header(0, 0xe1);
                    packet.put_byte(0);
                    G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionInfo(packet);
                    packet.finalize(true);
                    user->Send(packet);
                }
            }

            {
                Stream* stream = GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf20);
                CStreamGuard guard(stream, true);
                guard.operator*()->operator<<(0x1a7);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }

            // ORIG:两个独立 if,各自重调 G_GameWorld()->IsIntegratedPvPBaseChannel()。
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                Stream* stream = GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf2a);
                CStreamGuard guard(stream, true);
                guard.operator*()->operator<<(0x1d5);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((char)user->GetServerGroup());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                Stream* stream2 = GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf36);
                CStreamGuard guard2(stream2, true);
                guard2.operator*()->operator<<(0x1e0);
                guard2.operator*()->operator<<((int)user->GetUID());
                guard2.operator*()->operator<<((char)user->GetServerGroup());
                guard2.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);
            }

            if (G_GameWorld()->GetChannelType() == 0xe)
            {
                unsigned int playCount =
                    online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam();
                if (playCount == 1)
                {
                    Stream* stream = GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf44);
                    CStreamGuard guard(stream, true);
                    guard.operator*()->operator<<(0x1e6);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
                else
                {
                    Stream* stream = GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf4d);
                    CStreamGuard guard(stream, true);
                    guard.operator*()->operator<<(0x1e8);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
            }

            user->send_RedeemInfo();
            if (GlobalData::s_DailyScheduleManager != 0)
                GlobalData::s_DailyScheduleManager->CheckAndSendToOneUserSchedule(*user);
            user->send_MissionList();
            user->processReturnUser();
            APSystem::CUserProc::SendActionListState(user);
            user->setFlagPresentAvengerTitle(sig->m_flagPresentAvengerTitle != 0);
            user->VerifyPresentAvengerTitle();

            {
                unsigned int cur = (unsigned)
                    GlobalData::s_systemTime_.getCurSec();
                // findGameScript 返回 Arad_InterfaceGameScript*，向下转型到 period 数据管理器
                if (reinterpret_cast<ARAD::Arad_EventPeriodDataManager*>(
                        ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                            "Arad_EventPeriodDataManager"))
                    ->isApplied(ENUM_REPEAT_EVENT_CODE(0x8e), cur))
                {
                    AradServerStateMessage msg;
                    ARAD::notifyOpenMessageDialog(user, 0, 0x8e, msg);
                }
            }
            RequestDBEventArad(user);

            {
                std::bitset<32> flags(sig->m_bitset);
                // ORIG:每次 process_account_first_login 前重调 get_event_script_mng()。
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x191, user, flags);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x192, user, flags);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x19c, user, flags);
            }

            {
                CItemShop* shop = G_CDataManager()->m_itemShop;
                if (shop != 0)
                    user->SendOneADayItemShopIndex(shop->GetOneADayItemList());
            }
            user->reset_is_end_lethe_at_login();

            {
                // ORIG:两次调用各自重取 get_event_script_mng()。
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->send_event_init_data(user, false);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->send_event_init_record_data(user);
            }

            {
                unsigned int total = sig->m_totalPcRoomPlayTime;
                unsigned char cnt = sig->m_pcRoomPlayTimeRewardCnt;
                unsigned int cur = (unsigned)
                    GlobalData::s_systemTime_.getCurSec();
                user->setPcRoomPlayTimeData(cur, total, cnt);
            }
            user->set_aura_avatar_option(*reinterpret_cast<stAuraAvatarOption*>(
                sig->m_mixedRegion + 0xd70));
            user->send_aura_avatar_option();
            if (GlobalData::s_serverStateManager_ != 0)
                GlobalData::s_serverStateManager_->sendState(user);
            user->setCharacOption(*reinterpret_cast<CharacterOption*>(
                sig->m_mixedRegion + 0x2108));
            user->sendCharacOption();
            if (sig->m_fieldC != 0)
                user->SendTagCharacInfo();

            const_cast<_Charac_info*>(user->getCurCharacR())->m_field14BC = 0;   // _Charac_info pad 拆分后 +0x14bc 具名字段

            {
                CraneMinigameManager* crane = G_CGameManager()->GetCraneMinigameManager();
                if (crane != 0)
                    crane->SendCraneItemNeedMaterial(user);
            }
            processAttendAnceEvent(&sig->m_attendanceEvent, *user);
            user->SendMaxEquipLevel();
            processGrowthWeaponEvent(*user);
            processGrowthCreatureEvent(*user);
            user->sendBingoAddData();
            user->requestCubePremiumInfoIfContracted();
            CerashopAddRestrict::Manager::LoadRequestBuyRestrictItem(user);

            {
                CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x9e);
                if (ev != 0 && ev->IsEventing(0))
                    DB_LoadEventLevelupSupport::makeRequest((int)user->GetUID(),
                                                            user->getCurCharacNo());
            }
            {
                HeroMissionEvent* hero = ARAD::Singleton<HeroMissionEvent>::Get();
                if (hero != 0)
                    hero->loadEventData(user);
            }

            // ORIG:Garena 段(双 getGarenaAuthData + Taiwan::GarenaAuthData 访问器)。
            // CUser::getGarenaAuthData 返回 char*(GarenaAuthData 视图首址)。
            Taiwan::GarenaAuthData* garenaData =
                reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
            if (Taiwan::GarenaCyberCafe(garenaData->getGcaType()) != 0)
            {
                Taiwan::GarenaAuthPCRoom(user);
            }
            if (reinterpret_cast<Taiwan::GarenaAuthData*>(
                    user->getGarenaAuthData())->getMobileAuth() != 0)
            {
                // ORIG 0x815c586:makeRequest 不消费 this(m_field8ec31 经栈参 user 读,
                // 无 C1 构造站点)——静态语义调用。
                DBSelectMobileAuthRewardTw::makeRequest(user);
            }

            result = user->unlock4DataLoad() ? 0 : 0x1139;
        } while (false);

        if (bccBranch) { result = 0xbcc; break; }
        if (c14Branch) { result = 0xc14; break; }
    } while (false);

    return result;
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：Inter_LoadEtc 生命周期（ORIG objdump 转写，报告缺失标注）
// ---------------------------------------------------------------------------

// ORIG W 0x084e9cb6 sz 0x1c（_ZN13Inter_LoadEtcC1Ev，objdump 直接转写，
// 无独立报告）：call ISigDispatcher ctor + vptr=0x8c8fff0。
// ORIG W 0x084e9cd2 sz 0x30（D1：vptr=0x8c8fff0 → ISigDispatcher D1 → delete
// 条件体）。out-of-line 定义使本 TU 成为 vtable for Inter_LoadEtc 的发出点
// （key function=析构；此前无定义导致链接 undefined vtable）。
Inter_LoadEtc::~Inter_LoadEtc() {}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：CAradRyosikaEvent::DoEvent（ORIG T 0x081a107c sz 0x7d，
// 报告 docs/class_func_reports/CAradRyosikaEvent/DoEvent.md 全文已读）：
// user!=0 时 vptr+0x34（IsEventing(0)）为真且 ApplyDatePeriod(user) 为真
// → SendMail(user, param2, 1, param4)（param3 不透传，ORIG 汇编压栈序实证）。
void CAradRyosikaEvent::DoEvent(CUser* user, int param2, int param3, int param4)
{
    if (user != 0)
    {
        if (IsEventing(0) && ApplyDatePeriod(user))
        {
            SendMail(user, param2, 1, param4);
        }
    }
}

// ORIG T 0x081a0fda sz 0xa（ApplyDatePeriod.md 全文已读）：恒返 true。
bool CAradRyosikaEvent::ApplyDatePeriod(CUser* /*user*/)
{
    return true;
}

// ORIG T 0x081a0fe4 sz 0x97（SendMail.md 全文已读）：
// m_field10 < 0 → send_mail(user,p2,p3,p4,m_field0c)；
// 否则 p4==7 → p3=5；m_field10 == p4 → send_mail(user,p2,p3,p4,m_field0c)。
// 依赖 GlobalInstance<Arad_GiftItem_Set>::inst()（ORIG W 0x081a1155）。
void CAradRyosikaEvent::SendMail(CUser* user, int param2, int param3, int param4)
{
    if (m_field10 < 0)
    {
        GlobalInstance<Arad_GiftItem_Set>::inst()->send_mail(
            user, param2, param3, param4, m_field0c);
        return;
    }
    if (param4 == 7)
        param3 = 5;
    if (m_field10 == param4)
    {
        GlobalInstance<Arad_GiftItem_Set>::inst()->send_mail(
            user, param2, param3, param4, m_field0c);
    }
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：PvP 技能树域两个符号
// ---------------------------------------------------------------------------

// ORIG W 0x084e913a sz 0x6a（_ZNK27PvPSkillTreeParameterScript21getInitSkillTreeGradeEi，
// objdump 逐指令转写，无独立报告）：m_map54（this+0x54 map<int,int>）find(grade)，
// 命中返回 second，否则 0。
int PvPSkillTreeParameterScript::getInitSkillTreeGrade(int grade) const
{
    std::map<int, int>::const_iterator it = m_map54.find(grade);
    if (it != m_map54.end())
    {
        return it->second;
    }
    return 0;
}

// ORIG T 0x084bff8e sz 0x2d5（_Z15SetPvPSkillTreeP5CUserPK15_Mastered_skill20ENUM_SKILL_TREE_KIND，
// 报告 docs/class_func_reports/_lt_global_gt_/SetPvPSkillTree.md 全文已读，
// Ghidra C 与汇编双段对照）。依赖 getGivePvPSkillPoint（定义由 Main 分派）。
void SetPvPSkillTree(CUser* user, const _Mastered_skill* skill,
                     ENUM_SKILL_TREE_KIND kind)
{
    int usedSp = 0;     // calcUsedSP 出参1（ORIG -0x2c）
    int usedSfp = 0;    // calcUsedSP 出参2（ORIG -0x30，后续未读）
    SkillSlot* skillR = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(user->getCurCharacSkillR()));
    skillR->calcUsedSP(skill, usedSp, usedSfp);

    // ORIG：grade/secondGrow/firstGrow/job 三个计算点各自重取（P48 去缓存）；
    // getCurCharacSkillR 亦两处调用（calcUsedSP 后 get_remain_sp_at_index 前重新 GetInBuffer）。
    int pvpPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
        user->get_charac_job(),
        (int)(char)user->getCurCharFirstGrowType(),
        (int)(char)user->getCurCharSecondGrowType(),
        user->get_pvp_grade(), false);

    skillR = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(user->getCurCharacSkillR()));
    int remainSp = skillR->get_remain_sp_at_index(kind);
    int avail = pvpPoint - (usedSp + remainSp);      // ORIG -0x20
    int giveIdx;                                     // ORIG -0x1c
    if (kind == -1 || kind == 0)
    {
        giveIdx = 1;
    }
    else
    {
        giveIdx = 2;
    }

    if (avail > 0)
    {
        // ORIG getGivePvPSkillPoint 实参压栈序：job, first, second, grade
        int givePoint = G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
            user->get_charac_job(),
            (int)(char)user->getCurCharFirstGrowType(),
            (int)(char)user->getCurCharSecondGrowType(),
            user->get_pvp_grade());
        if (givePoint != 0)
        {
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
            skillW->set_remain_sp_at_index(remainSp + avail, kind);
        }
        else
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
    }
    else if (avail < 0)
    {
        SkillSlot* skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        skillW->clear_all_skills(kind);
        skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        skillW->set_parent(user);
        int givePoint = G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
            user->get_charac_job(),
            (int)(char)user->getCurCharFirstGrowType(),
            (int)(char)user->getCurCharSecondGrowType(),
            user->get_pvp_grade());
        if (givePoint == 0)
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
        else
        {
            skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
            skillW->set_remain_sp_at_index(pvpPoint, kind);
        }
    }
    user->givePvPSkillTree(0, true, giveIdx);
}

// ORIG T 0x08a5dc74 sz 0xed（_ZNK27PvPSkillTreeParameterScript20getGivePvPSkillPointEiiii，
// 报告 docs/class_func_reports/PvPSkillTreeParameterScript/getGivePvPSkillPoint.md 全文已读）：
// grade 被 m_field4c(+0x4c) 收敛上限；m_map18(+0x18 map<int, map<int,int>>) 按
// job find；内层按 firstGrow*5+secondGrow find；命中返回 second，否则 0。
int PvPSkillTreeParameterScript::getGivePvPSkillPoint(int job, int firstGrow,
                                                      int secondGrow,
                                                      int grade) const
{
    int key = grade;
    if (m_field4c < key)
    {
        key = m_field4c;
    }
    std::map<int, std::map<int, int> >::const_iterator it = m_map18.find(key);
    if (it != m_map18.end())
    {
        std::map<int, int>::const_iterator it2 =
            it->second.find(firstGrow * 5 + secondGrow);
        if (it2 != it->second.end())
        {
            return it2->second;
        }
    }
    return 0;
}

// ====================================================================
// LinkE2 批：Inter_NpcLimitBuyItemInfo / Inter_RequestCharacSkillInfo /
//            Inter_VillageAttackedReward（逐指令转写自对应报告）
// ====================================================================

// ORIG T _ZN25Inter_NpcLimitBuyItemInfo10sendPacketEP5CUser @0x084e6c42 sz0x1d7
// （docs/class_func_reports/Inter_NpcLimitBuyItemInfo/sendPacket.md 全文已读）。
// 限制购买 NPC 物品信息回执包（0x1d1）：m_data1c==0 时附带 m_param1 与
// m_invenItem1 全字段（含 m_amp 强化选项、put_packet 原始物品体）。
void Inter_NpcLimitBuyItemInfo::sendPacket(CUser* user)
{
    PacketGuard guard;
    guard.put_header(1, 0x1d1);
    guard.put_byte(1);
    guard.put_byte(static_cast<int>(static_cast<char>(m_data1c)));
    if (m_data1c == 0)
    {
        guard.put_short(m_param1);
        guard.put_int(m_invenItem1.m_addInfo);
        guard.put_int(m_invenItem1.m_addInfo2);
        guard.put_short(static_cast<int>(m_invenItem1.m_fieldb));
        guard.put_byte(static_cast<int>(m_invenItem1.m_amp.getAbilityType()) & 0xff);
        guard.put_short(m_invenItem1.m_amp.getAbilityValue() & 0xffff);
        guard.put_packet(m_invenItem1);
    }
    guard.put_int(m_invenItem2.m_addInfo);
    const CInventory* inventory = user->getCurCharacInvenR();
    const Inven_Item* ref = inventory->GetInvenRef(1, m_param2);
    if (ref == 0)
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(ref->m_addInfo2);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ORIG T _ZN28Inter_RequestCharacSkillInfo21_PutSkillInfoToCharacEcPK15_Mastered_skillP9SkillSlot
// @0x084e7820 sz0x64（docs/class_func_reports/Inter_RequestCharacSkillInfo/
// _PutSkillInfoToCharac.md 全文已读）。slotFlag 0/-1 → m_skillSlot[0]，
// 1 → m_skillSlot2[0]；单条 _Mastered_skill（0x198 字节）memcpy 拷贝。
void Inter_RequestCharacSkillInfo::_PutSkillInfoToCharac(
    char slotFlag, const _Mastered_skill* skill, SkillSlot* skillSlot)
{
    if (slotFlag == -1 || slotFlag == 0)
    {
        memcpy(&skillSlot->m_skillSlot[0], skill, 0x198);
    }
    else if (slotFlag == 1)
    {
        memcpy(&skillSlot->m_skillSlot2[0], skill, 0x198);
    }
}

// ORIG T _ZN27Inter_VillageAttackedReward17_send_reward_itemEP5CUsermii
// @0x084e0390 sz0x232（docs/class_func_reports/Inter_VillageAttackedReward/
// _send_reward_item.md 全文已读）。按 itemIdx 现做道具经系统邮件发放：
// -1 → 错误日志直返；1 → 耐久字段 m_fieldb 置 1；第 4 参 ORIG 未消费。
void Inter_VillageAttackedReward::_send_reward_item(CUser* user,
                                                    unsigned long itemIdx,
                                                    int count, int rank)
{
    (void)rank;
    Inven_Item item;
    item.m_addInfo = static_cast<int>(itemIdx);
    G_CDataManager()->m_itemList->create_item(
        ENUM_ITEM_CREATE_TYPE_0, item, count);
    if (item.m_addInfo == -1)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x4dbc,
            "VillageAttackedReward Invalid Item Index [item:%u]",
            item.m_addInfo);
        return;
    }
    if (item.m_addInfo == 1)
    {
        item.m_fieldb = 1;
    }
    char title[30];
    char content[256];
    memset(title, 0, sizeof(title));
    memset(content, 0, sizeof(content));
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_124", 0),
            0x1d);
    strncpy(content, g_scriptStringManager_.findString(4, "game_server_msg_125", 0),
            0xff);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, static_cast<unsigned int>(user->getCurCharacNo()), content,
        static_cast<int>(strlen(content)), 0xc, user->GetServerGroup(), false,
        false);
}
