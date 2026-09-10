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

// 判据㉓ 符号绑定：ORIG 本类六符号全 W（C1/C2 0x084eef48、D1/D2 0x084efc92、
// SendSkillPointReturnInfo 0x084eefd4、SetSkillTree 0x084ef4a6；nm 实查）
// ⇒ 本 TU 内定义加 weak（不改头；ORIG 头内定义形即 W）。
__attribute__((weak))
CSkillTreeParam::CSkillTreeParam(CUser* user, SIG_LOAD_ETC* sig, PacketGuard& packet)
    // P29/P55：ORIG 0x084eef50 先写 +0x00/0x04/0x08（列表），再构造 +0x1c/+0x28
    // vector 成员，最后 body 清零 +0x0c..0x18 ⇒ 前三者走初始化列表。
    : m_user(user), m_sig(sig), m_packet(&packet)
{
    m_curSp = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
}

__attribute__((weak)) CSkillTreeParam::~CSkillTreeParam()
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
__attribute__((weak)) void CSkillTreeParam::SendSkillPointReturnInfo()
{
    // [b28d-solo-Inter_LoadEtc_impl] P0/P183 去缓存：ORIG 无 skill 具名局部
    // （`call getCurCharacSkillR; mov %eax,(%esp)` 返回值寄存器直传），原码为内联
    // 右值表达式。删除 `SkillSlot* skill` 缓存槽 ⇒ 帧 0x48→0x38 与 ORIG 对齐。
    m_curSp = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(0)) - m_curSp;
    m_field14 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2)) - m_field14;

    // [b28d-solo-Inter_LoadEtc_impl] P0：ORIG 0x084ef038 段无 bool 栈槽/预清，
    // 条件全程 %eax 直传（`jg`/`jle` + `call empty; xor $1; test; je` + mov$1/$0 汇聚）
    // ⇒ 原码是内联条件表达式，非 `bool need1` 具名局部。
    if ((m_curSp > 0 || m_field14 > 0) && !m_returns1.empty())
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

    m_field10 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(1)) - m_field10;
    m_field18 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(3)) - m_field18;

    if ((m_field10 > 0 || m_field18 > 0) && !m_returns2.empty())
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
__attribute__((weak)) int CSkillTreeParam::SetSkillTree(Inter_LoadEtc* owner)
{
    // [Lane17 I 判词] ORIG 0x084ef4a6：p167 CLEAR（87/87）；判据⑪ 直方图逐值全等
    // {−0xc…−0x54} ⇒ 非槽/帧问题；gap −4 位于函数头部两次 `m_skillSlot` 初始化段
    // （ORIG 84ef4ae..84ef4f3 两段 `mov 0x8(%ebp),%eax; mov 0x4(%eax),%eax;
    // lea 0x29fd/0x2b97(%eax)…` 各 9 条）⇒ 待逐段比对（本轮未完成）。旧判词：DIFF 539/536。
    // b28d-solo 本轮已修：① 4 处 skillR 缓存局部去缓存（P0/P183，帧 0x90→0x70 归位）；
    // ② 4 处 skillW 缓存去缓存（ORIG `call getCurCharacSkillW; mov %eax,(%esp)` 无槽）；
    // ③ **真语义 bug**：通道守卫 `!chan && grade<=9` → `!chan || grade>9`（P196 落点体
    //    互换，探针 /tmp/b28d_ile/probe3.cpp f_or 逐条同构）；④ 专家段循环索引 unsigned
    //    化（seta）。残差 18 块：lea-vs-add ×2（N107）、1 条 4.4 default/尾 jmp、
    //    块内槽序（i/slot1/slot2 各 -0xc 深差，N82附/N98）、`movl $0x0,-0x18` 预清落点。
    m_user->SaveBackupSkillInfo(
        reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(m_sig->m_backupSkill),
        ENUM_SKILL_TREE_KIND(0));
    m_user->SaveBackupSkillInfo(
        reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(m_sig->m_backupSkill2ND),
        ENUM_SKILL_TREE_KIND(1));

    int lethe1 = 0;   // ProcessOneDayLethe 结果
    int lethe2 = 0;   // ProcessOneDayLethe2ND 结果

    // [b28d-solo-Inter_LoadEtc_impl] **真语义 bug 修复（P196）**：ORIG 0x084ef513
    // 段为 `xor $1; test %al,%al; jne <mov $1>; call get_pvp_grade; cmp $9,%eax;
    // jle <mov $0>` ⇒ 落点体互换（ORIG jne 落 true / 我们 je 落 false）⇒ 原码是
    // **or 形** `!chan || grade>9`（= !(chan && grade<=9)），旧写 `&& grade<=9` 使
    // chan==0 且 grade>9、chan==1 且 grade>9 两分支行为与 ORIG 相反（漏调孟婆处理）。
    // 探针 /tmp/b28d_ile/probe3.cpp f_or 逐条同构（f_and 即旧错形）。
    if (!G_GameWorld()->IsPvPSkilTreeChannel() || m_user->get_pvp_grade() > 9)
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

    // [b28d-solo-Inter_LoadEtc_impl] P0/P183：ORIG 无 skillR 具名局部
    // （每处 `call getCurCharacSkillR; mov %eax,(%esp)` 寄存器直传）⇒ 内联右值。
    m_curSp = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(ENUM_SKILL_TREE_KIND(0));
    m_field10 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(ENUM_SKILL_TREE_KIND(1));
    m_field14 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND(2));
    m_field18 = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(m_user->getCurCharacSkillR()))
        ->get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND(3));

    int level = m_user->get_charac_level();
    if (G_GameWorld()->IsCharacterLevelRevisionChannel())
        level = 0x55;

    // 同上：checkOverLevelSkill 两处各自重取（P0，无 skillR 复用局部）。
    if (reinterpret_cast<SkillSlot*>(const_cast<char*>(m_user->getCurCharacSkillR()))
            ->checkOverLevelSkill(m_returns1, ENUM_SKILL_TREE_KIND(0), level))
        m_user->enableSaveSkill();
    if (reinterpret_cast<SkillSlot*>(const_cast<char*>(m_user->getCurCharacSkillR()))
            ->checkOverLevelSkill(m_returns2, ENUM_SKILL_TREE_KIND(1), level))
        m_user->enableSaveSkill();

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
            // [b28d-63] ORIG 0x84ef9c2/0x84ef9f8：两次 get_skillslot_no 各自
            // 重新取 getCurCharacSkillR（无 skillR 复用局部；P167 MISSING ×2）。
            slot1 = reinterpret_cast<SkillSlot*>(
                        const_cast<char*>(m_user->getCurCharacSkillR()))
                        ->get_skillslot_no(first, group, ENUM_SKILL_TREE_KIND(0), true);
            slot2 = reinterpret_cast<SkillSlot*>(
                        const_cast<char*>(m_user->getCurCharacSkillR()))
                        ->get_skillslot_no(first, group, ENUM_SKILL_TREE_KIND(1), true);
            if (slot1 < 0)
            {
                // P0：ORIG `call getCurCharacSkillW; mov %eax,(%esp)` 无 skillW 槽。
                reinterpret_cast<SkillSlot*>(m_user->getCurCharacSkillW())
                    ->growtype_skill(m_user->get_charac_job(), first, second,
                                     ENUM_SKILL_TREE_KIND(0));
            }
            if (slot2 < 0)
            {
                reinterpret_cast<SkillSlot*>(m_user->getCurCharacSkillW())
                    ->growtype_skill(m_user->get_charac_job(), first, second,
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
            // [b28d-solo-Inter_LoadEtc_impl] P2/P310：ORIG 尾测 `seta`（无符号）
            // ⇒ 索引与 size() 同型比较（旧 `(int)size()` cast 生 `setg`）。
            unsigned int j;
            for (j = 0; j < script->m_growSkillVec.size(); ++j)
            {
                int skillIdx = script->m_growSkillVec[j].first;
                int expLevel = m_user->GetCurExpertJobLevel(
                    m_user->GetCurCharacExpertJobExp());
                // [b28d-solo-Inter_LoadEtc_impl] P0/P167：ORIG 0x084efb7c/0x084efbf4
                // 两处各自重取 getCurCharacSkillR（无 skillR 缓存局部）。
                if (!reinterpret_cast<SkillSlot*>(const_cast<char*>(
                        m_user->getCurCharacSkillR()))
                         ->check_skill_mastered(skillIdx, expLevel, 2,
                                                ENUM_SKILL_TREE_KIND(0)))
                {
                    reinterpret_cast<SkillSlot*>(m_user->getCurCharacSkillW())
                        ->growtype_skill(m_user->get_charac_job(), skillIdx,
                                         expLevel, ENUM_SKILL_TREE_KIND(0));
                }
                if (!reinterpret_cast<SkillSlot*>(const_cast<char*>(
                        m_user->getCurCharacSkillR()))
                         ->check_skill_mastered(skillIdx, expLevel, 2,
                                                ENUM_SKILL_TREE_KIND(1)))
                {
                    reinterpret_cast<SkillSlot*>(m_user->getCurCharacSkillW())
                        ->growtype_skill(m_user->get_charac_job(), skillIdx,
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
// [P327/N132 封顶] 本 TU 相对 ORIG 多 1 个
//   `_GLOBAL__I__ZN15CSkillTreeParamC2EP5CUserP12SIG_LOAD_ETCR11PacketGuard`：
//   根因（-H 实测链）CUser.h:85 → TaiwanQueries.h → TaiwanCash.h →
//   ThreadInterface.h:29 `#include <iostream>` ⇒ 本 TU 发射
//   std::ios_base::Init __ioinit（static_init 体仅 InitC1 + __cxa_atexit）。
//   ORIG 本区唯一 GI 为 _GLOBAL__I__Z7getUserj（属 InterDispatcher.cpp 的 TU）；
//   删直接 include 无效（CUser.h 仍传递）。唯一修法 = 删共享头
//   ThreadInterface.h 的 iostream（7 直接 + 12/56 间接消费）⇒ Main 2026-09-10
//   按 GmCmds_F 同判例裁决「不删」⇒ 结构性噪音封顶。
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
    // ORIG 0x84c2a98：无 records 局部槽，sig->m_records[i] 每轮直算（P0/P183）。
    for (int i = 0; i < sig->m_recordCount; ++i)
    {
        sig->m_records[i].m_field0 = 1;
        user->update_record(sig->m_records[i]);     // ORIG T 0x08676876
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
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 40/42：ctx 参数槽自增 ORIG 为
    // `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)` 两段、4.4 恒折 `addl $0x4,0xc(%ebp)`；
    // 另 put_short 实参装载序（ORIG 先载 edx）——跨版本 codegen，语义等价（p167 调用点一致）。
    // [b28c-lane25 判词·N] ORIG 0x084c2b0e 为 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp);
    // mov (%eax),%eax`（先自增写回参数槽再读旧值）；4.4 恒「读值→addl 参数槽」，
    // 且 bool callee 直判恒多 setne（P168a③b 跨版本 bool ABI）。语义等价，封顶。
    // ORIG 0x84c2b08：static（0x8=questIdx 无 this）；ctx 参数槽原地自增
    // （lea 0x4; mov →0xc(%ebp) 后读旧值），源码 = 读旧值 + ctx+=4 两条语句。
    // [L22-L17 L1 已试并回退] P319 整型加法+回 cast 形：4.4 产
    // `mov 0xc(%ebp),%eax; add $0x4,%eax; mov %eax,0xc(%ebp)`（三/四条，`add` 非 `lea`，
    // 第二处再多一条 mov）⇒ 指令数 40→42 但形态仍 DIFF。N107 确认（4.4 恒 add）。
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(ctx);
    ctx += 4;
    int* count = *reinterpret_cast<int**>(ctx);
    ctx += 4;
    Quest* quest = G_CDataManager()->find_quest((int)questIdx);
    // ORIG 两 je 直跳同出口（嵌套 if 消 && 汇聚 bool 岛，P197④ 反向）。
    if (quest != 0)
    {
        // [b28d-53 判词·依赖上报] ORIG 0x084c2b55 调用点 `call; test %al,%al; je`
        // 免归一形（P292/P248 档①）；我方 callee 声明 `char check_clear_send()`
        // （signed char）致 4.4 生 `test %al; setne %al; test %al; je`。
        // ORIG callee 0x084e923c 为 `movzbl 0x44(%eax),%eax; ret` 单字节直返
        // （P9 ⇒ uchar/bool）。已上报 Main 分派 Quest.cpp 属主把返回类型
        // char→unsigned char（Quest.h:403 + Quest.cpp:391），落地后本处裸判归位。
        // 残余：lea-vs-add ×2（N107 封顶）+ put_short 实参装载序（P23）。
        if (quest->check_clear_send())
        {
            buf->put_short(questIdx);   // ORIG 直传完整 32 位（截断在 put_short 内）
            ++(*count);
        }
    }
    return 1;
}

bool Inter_LoadEtc::SetFatigue(CUser* user)
{
    // [Lane22-L17 L6] 判据⑩/P212：ORIG 槽 curCharging=**-0x50**（084c3221 实证，深于
    // days -0x24 / charging -0x1e / decAmount -0x1c 的浅区）⇒ 原码 curCharging 属
    // **函数级**声明（4.1 先声明深）；我方块内声明被 4.4 「浅=后声明」放到 -0x20，
    // 连带 days/charging 深移 4。提出到函数头（未初始化声明，P232 仍占槽）。
    // [Lane17 I 结果] 实测：days/charging 已归位（-0x24/-0x1e 与 ORIG 逐值一致），
    // curCharging 落 -0x28（ORIG -0x50，仍在浅区）；判据⑪ 直方图 ORIG
    // {−0x1c,−0x1e,−0x24,−0x34,−0x44,−0x4c,−0x50} vs 我方
    // {−0x1c,−0x1e,−0x24,−0x28,−0x38,−0x48,−0x4c} ⇒ 残余 = curCharging 区划
    // （4.1 把 trace 前求值缓存放深区 −0x50/−0x4c，4.4 放浅区）+ vararg %esi/%edi
    // 角色互换（N7）⇒ 封顶。
    int curCharging;
    if (user->getCurCharacR() == 0)
        return false;

    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               user->getCurCharacR()->m_field7b,
                               GlobalData::s_systemTime_.getCurSec()))     // +0x7b
    {
        int days = 0;
        if (user->getCurCharacR()->m_field7b != 0)
        {
            days = ComputeDaysScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                user->getCurCharacR()->m_field7b,
                GlobalData::s_systemTime_.getCurSec());
        }

        short charging =
            GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, days);
        if (charging > 0)
        {
            user->IncFatigueBatteryCharging(charging);
            curCharging = user->GetFatigueBatteryCharging();
            cMyTrace(__PRETTY_FUNCTION__, 0x139a, 0)(
                "FATIGUE BATTERY : charno(%d) days(%d), lastplaytick(%d), "
                "curTick(%d), charging(%d), cur charging(%d)",
                  (int)user->getCurCharacNo(), days,
                  (unsigned)user->getCurCharacR()->m_field7b,
                  (unsigned)GlobalData::s_systemTime_.getCurSec(),
                (int)charging,
                curCharging);
        }

        if (user->GetFatigueBatteryCharging() > 0)
        {
            // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8778 `call; cwtl; mov %eax,-0x1c`
            // ⇒ decAmount 为 int 槽（short 声明恒生 `mov %ax,-N; cmpw`）。
            int decAmount = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
            if (decAmount != 0)
            {
                user->DecFatigueBatteryCharging(decAmount);
                cMyTrace(__PRETTY_FUNCTION__, 0x13a4, 0)(
                    "FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), "
                    "cur charging(%d)",
                    (int)user->getCurCharacNo(), (int)decAmount,
                    (int)user->GetFatigueBatteryCharging());
            }
        }

        user->RecoverFatigue(0);                                        // ORIG T 0x08657ada
        user->resetCurCharacMemberBonusFatigue();                       // ORIG T（恢复疲劳后清 bonus）
        user->ResetDailyQuest();
        user->ResetTrainingQuest();
    }
    return true;
}

void Inter_LoadEtc::verifyCharacVillage(CUser* user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 102/101（fresh）：vill int 槽（movsbl→-0x14）、
    // needMove 预清+条件置位、嵌套 if（P225 推广）均已归位；残余为 switch 尾 default 岛
    // （4.4 恒发 `jmp;nop`，P223 边界=4.4 自身 epilogue-threading，四形实测不可消）。
    // [Lane17 I 佐证] 判据⑪ 直方图**逐值全等** {−0x9,−0x10,−0x14} ⇒ 确认非槽问题；
    // 归一化后仅剩尾岛（我方 `1272: nop; jmp 1276` vs ORIG 单 `nop`）与跳转表
    // `mov 0xf4(%eax)`（.o 未重定位假象，链接后 = ORIG `mov 0x8c8a480(%eax)`）⇒ N121 封顶。
    if (G_GameWorld()->IsPVPChannel())
        return;

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8693 `movsbl %al,%eax; mov %eax,-0x14`
    // ⇒ vill 为 int 槽（非 char）。
    int vill = (int)(char)user->getCurCharacVill();
    Village* v = G_GameWorld()->GetVillage(vill);
    if (v == 0)
    {
        user->setCurCharacVill(1);
        return;
    }

    // ORIG 0x084e86ce：`movb $0x0,-0x9` 预清 + 条件置位（P198①/P194）
    char needMove = 0;
    if (user->get_charac_level() < v->m_field2c)   // +0x2c（最低等级门槛）
        needMove = 1;
    // [b28d-solo-Inter_LoadEtc_impl] P225 推广：ORIG 两守卫各自 `je` 直跳同一落点
    // ⇒ 嵌套 if（`&&` 单表达式会被 4.4 物化 bool 岛 +7 条）。
    if (v->m_questIdx != 0)                                    // +0x30
    {
        if (!user->getCurCharacQuestR()->isClearQuest(v->m_questIdx))
            needMove = 1;
    }

    if (needMove == 0)
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
    // [b24ab] ORIG `mov %eax,%ebx` 直接返回 put 结果（32 位），非 bool `ok!=false`
    // （后者多 mov%al/movzbl 规范化 + 帧扩容）
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool Inter_LoadEtc::SetPvpRecord(CUser* user, SIG_LOAD_ETC* sig)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 92/92：P258 正极性 or 形（lossGrade>pvpGrade
    // 短路）+ CheckDaily/RefW 内联（P52/P0）+ CUser.h:3652 SetPvpRecord int→bool（Main 代落）。
    // [b23S3C] P183/P0：ORIG &sig->m_pvpResult 直落实参槽（add $0x340;mov %eax,0x4(%esp)），无 pvp 指针槽
    if (!user->SetPvpRecord(sig->m_pvpResult))
    {
        user->disableSavePvP();
        return false;
    }
    user->disableSavePvP();

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c29b7 正极性 or 形（P258）：第一项
    // `lossGrade > pvpGrade` 短路进 body；P48 双臂各重取 getPvPExpLossGrade/RefR。
    int minPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(
                       GlobalData::s_ref_pvp_grade->getPvPExpLossGrade());
    // [SweepInterLoadEtcImpl2 2026-09-10] 真语义 bug（判据㊲ 条件极性反写；
    // jcc 目标下标交叉验证）：旧写 `lossGrade > pvpGrade || expPoint > minPoint`
    // 与 ORIG 整体反相。ORIG 0x084c29eb `cmp %eax,%ebx; jg +0xa5`（→ mov $0,%eax）
    // 与 0x084c2a00 `cmp %eax,%edx; jle +0xa5` 两跳同落 false 块、$1 走
    // fall-through ⇒ body 条件 = (lossGrade <= pvpGrade) && (expPoint > minPoint)。
    // AE 归一化把跳转目标统一为 <T>，故 92/92 的旧读数是假阴性。
    if (GlobalData::s_ref_pvp_grade->getPvPExpLossGrade() <=
            user->getPVPResultRefR()->m_pvpGrade &&
        user->getPVPResultRefR()->m_expPoint > minPoint)
    {
        // ORIG 0x084c2a12：now/lastPlay 寄存器直传（%ebx/%eax，无槽，P0）
        if (CheckDailyScheduleTime(4, user->getCurCharacLastPlayTick(),
                                   GlobalData::s_systemTime_.getCurSec()))
        {
            // [b28d-solo-Inter_LoadEtc_impl] P52/P0：RefW 结果经 %ebx 跨
            // getPvPExpLossExp 保活（ORIG 0x084c2a4e `mov %eax,%ebx`），无 w 槽。
            user->getPVPResultRefW()->m_expPoint = std::max(
                minPoint,
                user->getPVPResultRefR()->m_expPoint -
                    GlobalData::s_ref_pvp_grade->getPvPExpLossExp());
        }
    }
    return true;
}

bool Inter_LoadEtc::SendResult(CUser* user)
{
    // [Lane17 I 判词] ORIG 0x084eb2a7：p167 CLEAR（72/72）。判据⑪ 直方图显示
    // **相对序差**（ORIG −0x44 ↔ 我方 −0x3c 差 8、ORIG −0x28 ↔ 我方 −0x2c 差 4、
    // −0x50↔−0x48、−0x54↔−0x4c、−0x24↔−0x28 交错）⇒ (a) 类、判据⑩ 可攻；
    // 本轮未做（预算）⇒ 记「待攻」。旧判词：DIFF 402/397。
    // 本轮已修：`for (unsigned j…)` → `int j`（P2：ORIG 尾测 setle，旧 setbe 为 unsigned）。
    // 残差 71 块：packet/it 槽组 -0x3c vs -0x44（N98 首次使用序）+ `m_type==0x21`
    // 的 setcc-fold（ORIG `cmp $0x21; sete; test; jne` vs 4.4 直跳 jne，跨版本 N）。
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
    // [b28d-solo-Inter_LoadEtc_impl] P96/P270：ORIG 调 begin() const/end() const
    // ⇒ 容器为 const 局部（非 const 版会多转换构造 + ne(PKi,Pi) 混合实例）。
    const std::vector<WongWork::SUserPremiumInfo> infos =
        premium->GetPremiumInfoList(0x270f);
    int index = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    long now = GlobalData::s_systemTime_.getCurSec();
    // [b28d-solo-Inter_LoadEtc_impl] P167 判据：ORIG 用 const_iterator + `it->`
    // （operator-> ×4 / operator* ×1 / operator++ ×1），且 PvP 检查只有一处
    // （我方旧 `[]` 循环 + 重复 if 多发 G_GameWorld/IsPvPSkilTreeChannel/get_pvp_grade/size/op[]）。
    for (std::vector<WongWork::SUserPremiumInfo>::const_iterator it = infos.begin();
         it != infos.end(); ++it)
    {
        // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c2cfc `call operator*` + 5 dword
        // 块拷贝到 -0x74（值拷贝具名局部，P0 槽往返证据）；字段读仍走 `it->`（ptEv）。
        WongWork::SUserPremiumInfo info = *it;
        (void)info;
        if (it->m_state == 2)                        // ENUM_PREMIUM_STATE_2 等待开始
            continue;
        if (G_GameWorld()->IsPvPSkilTreeChannel() && user->get_pvp_grade() <= 9 &&
            it->m_type == 0x21)
            continue;
        packet.put_byte(it->m_type & 0xff);
        packet.put_int(it->m_end - (int)now);
        ++count;
    }

    // [b28d-solo-Inter_LoadEtc_impl] P167 补缺口：ORIG 0x084c2e0a `put_byte(int&,int)`
    // （index 计数回填）+ 0x084c2e15 `CUser::GetCera()` + 0x084c2e24 `put_int`。
    packet.put_byte(index, count);
    packet.put_int(user->GetCera());

    if (user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1323, 5);
        trace("bool Inter_LoadEtc::SendResult()");
        return false;
    }

    user->verifyDoingQuest();
    // [b28d-solo-Inter_LoadEtc_impl] P48/P167：ORIG 每处重取 getCurCharacQuestR
    // （0x084c2e9a/0x084c2ed0/0x084c2ef7/0x084c2f3a 四调用点），无 questR 缓存局部。
    for (int i = 0; i < 0x14; ++i)
    {
        if (user->getCurCharacQuestR()->m_quest[i] == 0)
            packet.put_short(0xffff);
        else
            packet.put_short(user->getCurCharacQuestR()->m_quest[i] & 0xffff);
        packet.put_int(user->getCurCharacQuestR()->m_trigger[i]);
    }
    for (int i = 0; i < 5; ++i)
    {
        int v = *reinterpret_cast<const int*>(
            &user->getCurCharacQuestR()->m_authenData.m_authen[i * 4]);
        packet.put_int(v);
    }
    packet.put_byte((int)(signed char)user->getCurCharacVill());
    packet.put_byte(user->GetTutorialSkipable());

    int tIndex = packet.get_index();
    int tCount = 0;
    packet.put_byte(0);
    // [b28d-solo-Inter_LoadEtc_impl] P2：ORIG 尾测 `setle`（有符号）
    // ⇒ 循环变量为 int（`unsigned` 生 `setbe`）。
    for (int j = 0; j < 0x80; ++j)
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
    // [Lane17 I 判词] ORIG 0x084d8e68：判据⑪ 直方图**逐值全等**
    // {−0x1c…−0x182 共 11 值两侧一致} ⇒ 无槽/帧问题；归一化逐行实测**唯一残差**
    // = 实参中转寄存器角色 1 处：ORIG `mov -0x30(%ebp),%ebx; mov %ebx,0x4(%esp)`
    // vs 我方 `mov -0x30(%ebp),%edx; mov %edx,0x4(%esp)` ⇒ **N7 封顶**
    // （跨版本 callee-saved 选择，源形不可择）。旧判词：帧差 0x1ac/0x1b0（已随槽位归位消失）。
    // = N82附/P190 帧分配，语义等价。
    char title[21];
    char body[256];

    if (user->getUserBreakAwayRewardLuckPoint() > 0)
    {
    }
    else
        user->setBreakAwayRewardData(false);   // ORIG test;setg;test;jne

    int itemId = 0;
    int itemCnt = 0;
    itemId = user->getBreakAwayRewardItemId();
    itemCnt = user->getBreakAwayRewardAItemCnt();
    if (itemId == 0 && itemCnt == 0)
        return;

    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_135", 0),
            0x14);

    // ORIG(4.1) 为 if-else 链且每判点重调 getter（无跳转表、无 order 局部槽）
    if (user->getBreakAwayRewardItemOrder() == 0)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_136", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 2)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_142", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 4)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_160", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 1)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_158", 0), 0xff);
    else if (user->getBreakAwayRewardItemOrder() == 3)
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_159", 0), 0xff);
    else
        return;

    Inven_Item mailItem;
    int coinCount = 0;
    if (itemId == 0)
    {
        coinCount = itemCnt;
        GetInstanceCirculationStatistic()->AddCirculationStatistic(
            CIRCULATION_STATISTIC_FIELD(0x10), user, (unsigned int)coinCount);
    }
    else
    {
        CItem* found = G_CDataManager()->find_item(itemId);
        if (found == 0)
            return;
        // [SweepInterLoadEtcImpl] ORIG 0x084d912d `mov %eax,-0x80(%ebp)` 与
        // 0x084d9142 `mov %al,-0x81(%ebp)` 的槽址 = mailItem(-0x82) + 2 / +1
        // ⇒ mailItem.m_addInfo = itemId、mailItem.m_field1 = GetItemType()
        // （旧译文 `(void)found->GetItemType();` 丢两次字段写入 = 真语义 bug）。
        // P282：ORIG 0x84d9135 `add $0xc` ⇒ 虚槽 word[5] = GetItemType()。
        mailItem.m_addInfo = itemId;
        mailItem.m_field1 = (unsigned char)found->GetItemType();
        found->make_item(mailItem);                  // ORIG 虚槽 +0x8
        mailItem.set_add_info(itemCnt);
    }

    unsigned int accId = user->get_acc_id();
    // [SweepInterLoadEtcImpl] ORIG 0x084d9187 `movl $0x0,-0x20(%ebp)`：sent 预清
    // 在 accId 赋值之后、GetServerGroup 之前（P198① 两句式，合写会丢预清行）。
    // GetServerGroup 无栈槽（ORIG 0x084d9199 `mov %eax,%ebx` 直落 callee-saved，
    // P0/P183）⇒ 内联第 8 实参；右→左求值序使它在 strlen 之前，与 ORIG 一致。
    int sent = 0;
    sent = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, coinCount, user->getCurCharacNo(), body, (int)strlen(body),
        7u, (ENUM_SERVER_GROUP)(int)user->GetServerGroup(), false, false);

    // [SweepInterLoadEtcImpl] ORIG 0x084d9207 `jne 084d9280`(nop 岛) 直落 epilogue：
    // 4.4 对 `if (sent==0){...}` 恒把该出口线程化到 add 指令（少 1 岛）⇒ P223 尾空标签形。
    if (sent != 0)
        goto give_break_away_reward_end;
    {
        // [SweepInterLoadEtcImpl] ORIG 0x084d920f 结果直落 %edi 跨后续 3 个 getter
        // 保活（无栈槽往返，P0/P183）⇒ 内联第 4 实参；右→左求值序 = ORIG 的
        // getBreakAwayRewards 先于 GetUID/get_acc_id/getCurCharacNo。
        // ORIG 0x84d921c/0x84d9229：失败分支重调 getCurCharacNo/get_acc_id。
        DB_LoadBreakAwayRewardData::makeRequest((int)user->GetUID(),
                                                user->get_acc_id(),
                                                user->getCurCharacNo(),
                                                *user->getBreakAwayRewards());
        user->setBreakAwayRewardItemId(0);
        user->setBreakAwayRewardAItemCnt(0);
    }
give_break_away_reward_end:;
}

void Inter_LoadEtc::processGrowthWeaponEvent(CUser& user)
{
    // ORIG 0x84e834a：首次 IsEventing 判定全链无槽（GetRepeatEvent 返回值
    // 寄存器直传 vtable 调用，P134/P183），growth 槽仅第二次调用后出现。
    // 正向嵌套消 ! 的 xor 物化（P194 反向）。
    if (GlobalData::s_event_manager->GetRepeatEvent(100)->IsEventing(0))
    {
        GrowthEquipEvent* growth = static_cast<GrowthEquipEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(100));
        if (growth != 0)
        {
            growth->GiftGrowthWeaponBox(user);                  // ORIG T 0x080f7050
            growth->SendGrowthWeaponEventInfo(user);            // ORIG T 0x080f7226
        }
    }
}

void Inter_LoadEtc::processGrowthCreatureEvent(CUser& user)
{
    // 同 processGrowthWeaponEvent（ORIG 0x84e838c 同构）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))
    {
        GiveGrowCreatureEvent* growth = static_cast<GiveGrowCreatureEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x67));
        if (growth != 0)
        {
            growth->SendGrowthCreatureEventInfo(user);      // ORIG T 0x081b6b70
        }
    }
}

void Inter_LoadEtc::processSeriaRoomAniDecoEvent(seriaRoom_AniDeco info, CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 59/59：去 ev 局部（P134/P183 寄存器直传
    // 虚调用）+ 正极性 test;je 尾（P224）+ now/lastPlay 内联（%esi/%ebx 保活）+ 嵌套正向
    // if（消尾部 jmp+nop）。
    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8430：GetRepeatEvent 返回值寄存器直传
    // 虚调用（P134/P183；无 ev 局部、无 null 检查），正极性 test;je 尾（P224）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing(0))
    {
        user.setSeriaRoomAniDecoInfo(info);
        SeriaRoomAniDecoEvent* seria = static_cast<SeriaRoomAniDecoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x69));
        if (seria != 0)
        {
            seria->SendEventInfo(user);                 // ORIG T 0x0827b99a
            // ORIG 0x084e84a3：now/lastPlay 经 %esi/%ebx 跨 G_CEnvironment 保活（无槽，P0）
            if (CheckDailyScheduleTime(
                    G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                    user.getCurCharacLastPlayTick(),
                    GlobalData::s_systemTime_.getCurSec()))
                seria->SendAniDecoItem(user);           // ORIG T 0x0827b75a
        }
    }
}

void Inter_LoadEtc::processCheckInvalidExpPerLevel(CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 97/97：levelExpLow/High 同型化为 unsigned
    // （P211 档③）翻正第二比较装载序 + 首比较方向翻转（levelExpLow>exp）+ `100 - min(...)`
    // 单表达式（消回读槽）。
    unsigned levelExpLow = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level());
    unsigned levelExpHigh = (unsigned)G_CDataManager()->get_level_exp(
        user.get_charac_level() + 1);
    unsigned exp = (unsigned)user.get_charac_exp();
    // [b28d-53] ORIG 0x084e8556 段：`mov levelExpLow; cmp exp; ja body` +
    // `mov exp; cmp levelExpHigh; jbe end` = 双守卫共享尾（P257 正向包裹消
    // 早返形多发的 jmp+nop 岛）。残余：第二比较 4.4 恒折 `levelExpHigh >= exp`
    // 方向（ORIG 为 `exp <= levelExpHigh`）⇒ N18b/N22 比较方向族，97/97 封顶。
    if (levelExpLow > exp || exp > levelExpHigh)
    {
        // [b28d-53] ORIG 0x084e8577 `call; xor $1,%eax; test %al; je end`
        // ⇒ bool callee + 取反形（P168a①）。
        if (!user.isTradePunishType(1))
        {
            user.SetTradePunishType(1);
            DB_SaveUserHandicap::makeRequest(user.get_acc_id(), 0x9c42, 0xb, 0, 3, 0);
            // ORIG 0x084e860b 段：ctor 前求 varargs 并保活（N80/P227 全表达式临时链）。
            cMyTrace(__PRETTY_FUNCTION__, 0x6c7d, 5)(
                "[InvalidExp] Invalid character exp per level. "
                "(mid:%u, charac:%u, level:%d, exp:%d,%d,%d)",
                user.get_acc_id(),
                user.get_charac_no(-1),
                user.get_charac_level(),
                user.get_charac_exp(),
                levelExpLow,
                levelExpHigh);
        }
    }
}

void Inter_LoadEtc::_checkUserDisadvantage(CUser* user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 154/154：P171 照抄 ORIG 恒真 bool 局部
    // （-0xd，第二个惩罚项/货物检查开关）+ `penalty +=` 读改写回 + hundredBase 声明提前
    // （赋值保持原位，P212）+ `100 - min(...)` 单表达式 + cargo 内联（P0）。
    if (user->IsGameMasterMode())
        return;

    // [b23S3C] P48/§4.1：ORIG Get() 结果 %eax 直落 isRestricted 实参槽（+0x2f），
    // 无 restrictMgr 槽、无 null 检查（原 null 检查系伪造行为）
    if (!ARAD::Singleton<ServiceRestrictManager>::Get()
             ->isRestricted(RestrictCategory::Enum(3), 1))
        return;
    if (user->getCurCharacR() == 0)
        return;

    // [b23S3C] P56/P198：ORIG 直测条件内联（xor$1/test/jne），无 invalid bool 槽
    if (user->get_charac_level() < 0x14 || user->getStdDropRate() == 0)
        return;

    // 遍历 0..29999 任务清除位（CQuestClear 区位于 questR+4，含 m_user 头）
    // [b28d-solo-Inter_LoadEtc_impl] P196：ORIG 边界 `cmpl $0x752f; setle`（有符号）
    // ⇒ i 必须 int；P248 档②：ORIG `movzbl slot; xor $1; test; je` ⇒ 标志位 bool。
    bool hasClearedQuest = false;
    for (int i = 0; i < 30000; ++i)
    {
        if ((user->getCurCharacQuestR()->m_cleared).isClearedQuest((unsigned int)i))
        {
            hasClearedQuest = true;
            break;
        }
    }

    // [b28d-solo-Inter_LoadEtc_impl] P171 照抄：ORIG 0x084c2833 存在恒真 bool 局部
    // （-0xd `movb $0x1` 后全程只读、无写 0 点）——第二个惩罚项与货物检查的开关。
    bool penaltyEnabled = true;
    int hundredBase;                                    // 声明提前（赋值保持 ORIG 位置）
    int penalty = 0;
    if (!hasClearedQuest)
        penalty += 0x28;                                // 未清任务：+40
    if (!penaltyEnabled)
        penalty += 0x1e;
    if (!hasClearedQuest || !penaltyEnabled)
    {
        // P0：ORIG 无 cargo 具名槽（getCurCharacCargoR 结果直落实参槽）
        if (user->getCurCharacCargoR()->GetItemCount() == 0)
            penalty += 0xa;                             // 无货物再 +10
    }

    hundredBase = 100;
    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c28ba：min 返回值经 %eax 直入减法
    // （无回读槽）⇒ 单表达式 `100 - min(...)`。
    penalty = 100 - std::min(hundredBase, penalty);     // 100 - min(penalty, 100)
    // [b23S3C] P48：ORIG getStdDropRate 双臂各重取（+0x16a 入 min、+0x18f 入比较，无 dropRate 槽）
    penalty = std::min(penalty, user->getStdDropRate());   // 再与当前掉率取小

    if (user->getStdDropRate() != penalty)
    {
        DB_SaveUserHandicap::makeRequest(user->get_acc_id(), 0x9c41, 0x65,
                                         penalty, 3, 0);
        user->setStdDropRate(penalty);
    }
}

int Inter_LoadEtc::ProcessOneDayLethe(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    // [Lane17 I 判词] ORIG 0x084e8c92：判据⑪ 直方图逐值全等 {−0xc,−0x10,−0x18}。
    // 归一化残差 = 3 处「字节读比较」双段形：ORIG `movzbl 0x2b95(%eax),%edx;
    // movzbl &CPremiumLetheManager::…_STATE,%eax`（2 条）vs 我方 `movzbl 0x2b95(%eax),%eax;
    // mov %eax,%edx; movzbl …,%eax`（3 条，多 eax→edx 中转）⇒ **N18a/N7 封顶**
    // （4.4 对 uchar 比较恒插中转，源码侧 cast/操作数交换均被前端折叠）。
    // 已修：job/skillR 缓存（内联实参）、skill=m_skillSlot(+0x46)、reason=eSkillSaveReason(1)。
    // 残差为 lethe/out/guard 槽深（帧 0x40 vs 0x30）= N82附/N98。
    user->SetOneDayLetheFlag((unsigned char)sig->m_backupSkill[0x198]);  // +0x2b95（backupSkill 尾哨）
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    // ORIG 实证：use = !premium && (f==USE || f==CONFIRM)；flag/use 均无栈槽
    if (!user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
        ((unsigned char)sig->m_backupSkill[0x198] == CPremiumLetheManager::USE_LETHE_STATE ||
         (unsigned char)sig->m_backupSkill[0x198] == CPremiumLetheManager::SKILL_CONFIRM_STATE))
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill), ENUM_SKILL_TREE_KIND(0)))
            return 0x40c6;

        if ((unsigned char)sig->m_backupSkill[0x198] != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag(false);
        }

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x40d2),
                           true);
        // [b28d-63] ORIG 0x84d9602/0x84d962a：`guard << x` 实为 (*guard) << x
        // （每处先 call CStreamGuard::operator*），GetInBuffer 才走 operator->（ptEv）。
        (*guard)->operator<<(0x15c);
        (*guard)->operator<<((int)user->GetUID());
        SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
            guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();  // ORIG 0x84d9641 ptEv
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
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            (unsigned char)sig->m_oneDayLetheFlag == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084d9797 段：无 job/skillR 具名局部
            // （job 经 %ebx 保活）；skill 实参 = SkillSlot::m_skillSlot（`lea 0x46(%eax)`）；
            // 第 4 参 ORIG 立即数 = 1（我方旧写 eSkillSaveReason(0)）。
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 0, eSkillSaveReason(1));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 (unsigned char)sig->m_oneDayLetheFlag == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(0));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 0, eSkillSaveReason(1));
        }
    }
    return false;
}

int Inter_LoadEtc::ProcessOneDayLethe2ND(SIG_LOAD_ETC* sig, CUser* user, int& err)
{
    // [Lane17 I 判词] 同 1 号（ORIG 0x084e8e1e 同构）：判据⑪ 直方图逐值全等；
    // 残差同为 3 处 uchar 比较的 eax→edx 中转（N18a/N7）⇒ 封顶。
    // p167 CLEAR 35/35；去缓存 + m_skillSlot + reason=1 已落地。
    user->SetOneDayLetheFlag2ND((unsigned char)sig->m_backupSkill2ND[0x198]);  // +0x2d2f
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
        return false;

    // ORIG 实证同 1 号：use = !premium && (f==USE || f==CONFIRM)；无 flag/use 槽
    if (!user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
        ((unsigned char)sig->m_backupSkill2ND[0x198] == CPremiumLetheManager::USE_LETHE_STATE ||
         (unsigned char)sig->m_backupSkill2ND[0x198] == CPremiumLetheManager::SKILL_CONFIRM_STATE))
    {
        if (!lethe->BackToPre(user, reinterpret_cast<_Mastered_skill*>(
                sig->m_backupSkill2ND), ENUM_SKILL_TREE_KIND(1)))
            return 0x4111;

        if ((unsigned char)sig->m_backupSkill2ND[0x198] != CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            err = 1;
            user->SetIsInitSkillFlag2ND(false);
        }

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x411d),
                           true);
        // [b28d-63] ORIG 0x84d9602/0x84d962a：`guard << x` 实为 (*guard) << x
        // （每处先 call CStreamGuard::operator*），GetInBuffer 才走 operator->（ptEv）。
        (*guard)->operator<<(0x15c);
        (*guard)->operator<<((int)user->GetUID());
        SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
            guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();  // ORIG 0x84d9641 ptEv
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
        if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
            (unsigned char)sig->m_oneDayLetheFlag2ND == CPremiumLetheManager::NOT_USE_LETHE_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            // [b28d-solo-Inter_LoadEtc_impl] 同 1 号：内联实参 + m_skillSlot(+0x46) + reason=1。
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
        else if (user->isAffectedPremium(ENUM_PREMIUM_TYPE(0x21)) &&
                 (unsigned char)sig->m_oneDayLetheFlag2ND == CPremiumLetheManager::SKILL_CONFIRM_STATE)
        {
            lethe->BackupSkill(user, ENUM_SKILL_TREE_KIND(1));
            user->m_historyLog.SkillSPInfoSave(user, eSkillSPInfoSaveReason(0));
            user->m_historyLog.SkillSave(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(
                    user->getCurCharacSkillR()))->m_skillSlot,
                (unsigned)user->get_charac_job(), 1, eSkillSaveReason(1));
        }
    }
    return false;
}

void Inter_LoadEtc::processAttendAnceEvent(SIG_ATTENDANCE_EVENT* sig, CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 195/197（fresh）：p167 CLEAR（19/19）；已修
    // m_isEnabled bool 化（SigTypes_C.h:405，P248 档②）+ 去 ev 局部负极性守卫；
    // 残差为 attendance/mailItem 槽平移 4（N82附）与 vararg %esi/%ebx 互换（N7）。
    // [b28d-solo-Inter_LoadEtc_impl] P248 档②：ORIG 0x084e8078 `movzbl; xor $1;
    // test %al,%al; jne` ⇒ bool 字段 + 裸 `!field`。
    if (!sig->m_isEnabled)
        return;

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e80c8：GetRepeatEvent 返回值寄存器直传
    // 虚调用（P134/P183；无 ev 局部、无 null 检查），负极性 `xor $1; test; jne` 尾。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x60)->IsEventing(0))
        return;
    AttendanceEvent* attendance = static_cast<AttendanceEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x60));
    if (attendance == 0)
        return;

    Inven_Item mailItem;
    // ORIG 0x084e8102 直接 call _ZN15AttendanceEvent13getRewordItemEv（2026-08-28
    // OrigEventLeaf8f 实证），返回 (short)m_addInfo.m_info0。
    int rewardIdx = attendance->getRewordItem();
    // [SweepInterLoadEtcImpl] ORIG 0x084e810a `movl $0x1,-0x20(%ebp)`：写后从不读的
    // 死局部（P171 照抄；(void) 消警告）。缺它会使后续 item/mailItem 槽整体上移 4。
    int deadFlag = 1;
    (void)deadFlag;
    CItem* item = G_CDataManager()->find_item(rewardIdx);
    if (item == 0)
        return;
    // [SweepInterLoadEtcImpl] ORIG 0x084e8132 `mov -0x24(%ebp),%eax; mov %eax,-0x78(%ebp)`
    // = mailItem.m_addInfo（-0x7a+2）= rewardIdx，在 make_item 之前（旧译文缺此写入）。
    mailItem.m_addInfo = (unsigned int)rewardIdx;
    item->make_item(mailItem);                       // ORIG 虚槽 +0x8
    mailItem.set_add_info(1);

    // [b28d-63] ORIG 无 memset 调用（p167 EXTRA ×2）⇒ 聚合零初始化（P214③）。
    char sender[256] = {0};
    char title[21] = {0};
    strncpy(sender,
            g_scriptStringManager_.findChangedString(4, "game_server_msg_276", 0),
            0xff);
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_275", 0),
            0x14);

    // [b28d-63] ORIG 0x84e82ab/0x84e82fe：getCurCharacNo 两处内联调用（无缓存局部）。
    // [SweepInterLoadEtcImpl] ORIG 0x084e8293 `mov %eax,%ebx`：GetServerGroup 直落
    // callee-saved（无栈槽，P0/P183）⇒ 内联第 8 实参（右→左求值序 = 先于 strlen）。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, 0u, user.getCurCharacNo(), sender, (int)strlen(sender), 0u,
        (ENUM_SERVER_GROUP)(int)user.GetServerGroup(), false, false);

    DB_InsertAttendanceEvent::makeRequest((int)user.GetUID(),
                                          user.get_acc_id(), user.getCurCharacNo());
    sig->m_isEnabled = 0;
}

int Inter_LoadEtc::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [BigInterLoadEtc 2026-09-09] ORIG 2296 / 我方 2325（gap +29，起点 +74）；
    // p167 CLEAR（436/436，差集仅 C1↔C2/D1↔D2 镜像）；TU 四数 65/99/0/12 全程未退。
    // 本轮三件**真缺口修复**（非形态）：
    //  ① getDungeonMinimumRequiredLevel/isDungeonOpen 实参 -1 → 0x2aff
    //     （ORIG 0x084c1279/0x084c1297 `movl $0x2aff,0x4(%esp)`）；
    //  ② peerIp 缓冲 16B→0x20（ORIG local_b4 跨 -0xb0..-0x90，if 分支用 +0x10 段，
    //     原声明越界写 16B）+ else 分支补 16B 清零（ORIG 4×`movl $0x0,disp`）；
    //  ③ getInitSkillTreeGrade 实参 myGrade → initBase
    //     （ORIG 0x084c09e6 `movzbl %al,%esi; mov %esi,0x4(%esp)`）。
    // 形态收敛（P259/P52/P0/P328）：Acquire 内联 ×6、set_skill_slot/skillW 链式、
    // 具名 bool ×5 去槽、premium/CAchievement/autoMarket/crane/hero/ev/itemLock/
    // titleBook 链式、nowSec/nowTick/lastPlay 内联、4 处错误码检查补函数级
    // `bool ok`（ORIG 共用 -0x61 槽）、insert_game_world 具名 uchar（P3 收窄）。
    // 残差 = N82 帧/槽复用（4.1 不复用、4.4 复用 ⇒ 槽号错位）+ N86 迭代器临时槽
    // + P292/N122 类 bool 归一化跨版本差。
    SIG_LOAD_ETC* sig = reinterpret_cast<SIG_LOAD_ETC*>(data);

    if (user->get_state() == 0)
        return 0xa7b;
    if (user->getCurCharacR() == 0)
        return 0xa7f;
    if (sig->m_field8 == 1)
        return 0xa96;

    PacketGuard packet;
    {
        // [BigUserLogic] ORIG 0x084c02??：无 grade 局部（两处各自重取 sig+0x354，
        // logFormat 实参为 43-44 的 mov -0x60;mov 0x354）。原 grade 槽产 store+load。
        if (!G_CDataManager()->IsAvailablePvPChannel(sig->m_pvpResult.m_pvpGrade))
        {
            LogManager::logFormat(1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__, 0xa9b,
                "Inter_LoadEtc::dispatch_sig pvp_grade = %d", sig->m_pvpResult.m_pvpGrade);
            return 0xa9c;
        }
    }

    // ---- 失败链前置处理（ORIG 各 err 分支统一收口于 LAB_084c2653）----
    // [BigUserLogic] 原 do-while+bccBranch/c14Branch 标志形是我方重写产物：ORIG
    // 各失败点为 `mov $code,%ebx; jmp <统一尾>` = 源码直接 return（编译器自动落
    // 析构尾）。改回 return 形，消除 4 个物化 bool 及其清零/读回指令。
    bool pvpTreeSkip = false;
    {
        // [BigInterLoadEtc] ORIG 0x084c0352..0x084c0472 四处检查共用同一具名
        // bool 槽 -0x61（`mov %al,-0x61; movzbl -0x61,%eax; xor $1; test; je`）
        // ⇒ 原码是函数级 `bool ok;` 各分支赋值复用（P328⑪/P320），非内联 !call()。
        bool ok;
        ok = SetPvpRecord(user, sig);
        if (!ok) return 0xaa2;
        ok = SetRankRecord(user, sig);
        if (!ok) return 0xaa5;

        // [BigUserLogic] ORIG 0x084c03xx：先物化 getCurCharacR()->+0x7b 与 sig+4
        // 两实参，**再**构造 cMyTrace，后调 operator()。原写法把实参求值放到
        // ctor 之后，指令序整体错位。
        long playTick = user->getCurCharacR()->m_field7b;
        unsigned charNo = (unsigned)sig->m_characNo;
        cMyTrace trace(__PRETTY_FUNCTION__, 0xac5, 0);
        trace("CharLastPlayTick char(%d) t(%d)", charNo, (unsigned)playTick);

        user->ProcPremiumFatigue();
        // [BigUserLogic] ORIG 0x084c040e 无判空直调。
        // [BigInterLoadEtc] ORIG O106-O110 无 premium 具名槽（eax 直传 this）。
        user->GetPremiumInfo()->RecalcAdditionalInfo(user);

        ok = SetFatigue(user);
        if (!ok) return 0xacf;
        verifyCharacVillage(user);
        ok = SendResult(user);
        if (!ok) return 0xae6;

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
        {
            // [BigUserLogic] ORIG 0x084c04c7+：GetCharacExpandData 结果留 %eax
            // 直发虚调用（不 spill）。命名变量形会让 4.4 多 2 条槽往返，改嵌套单行。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(6)))->loadData(user, sig->m_expandData6);
            // [hc 收] ORIG 0x08660ec4 区：sig->m_mixedRegion 为 SIG_LOAD_ETC 载荷
            // 线格式缓冲（§5.3 序列化缓冲豁免），+0x415 为该 expand-data 的固定布局
            // 偏移；loadData(user, ptr) 取地址直传，偏移为承载格式固有，seek 无漏。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(8)))->loadData(user, sig->m_mixedRegion + 0x415);
            // [hc 收] +0xd58 同承载体 0xd 型 expand-data 布局偏移（§5.3 豁免）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xd)))->loadData(user, sig->m_mixedRegion + 0xd58);
        }

        int tagIdx = user->get_charac_idx(sig->m_fieldC);
        // [BigInterLoadEtc] ORIG 0x084c05ae..0x084c05b4：末条件独立
        // `test %eax,%eax; setne %al; test %al,%al; je`（P292 非 bool 返回直测），
        // 合入 && 链会被 4.4 折成 mov $1/jmp/mov $0 物化 ⇒ 拆嵌套 if。
        if (sig->m_fieldC != 0 && tagIdx != -1)
        {
            if (user->getTagCharacR() != 0)
            {
            // [BigUserLogic] ORIG 0x084c09??：先物化 sig+0x1a8/sig+0x10 两实参，
            // 再调 getTagCharacSkillW，最后一并传 set_skill_slot。原写法在 call 后
            // 又经 tagSkillBuf/skillW 两跳槽写入（多 2 条 store + 求值序颠倒）。
            // [BigInterLoadEtc/P52] 三具名局部各多 2 条槽往返（ORIG 用 %esi/%ebx/%eax
            // 保活，无槽）⇒ 合并为单条链式表达式，让 4.4 复用 callee-saved 寄存器。
            reinterpret_cast<SkillSlot*>(user->getTagCharacSkillW())->set_skill_slot(
                (char*)sig->m_tagEtc.m_skills[0],    // sig+0x10（行 0 = tagSkillSlot）
                (char*)sig->m_tagEtc.m_skills[1],    // sig+0x1a8（行 1 = tagSkillCombo）
                0x198);
            }
        }
        if (tagIdx == -1)
            sig->m_mixedRegion[0xd3b] = 0;

        {
            // [hc 收] +0xbe7 / +0xd3c 同承载体 0xb/0xa 型 expand-data 布局偏移
            // （§5.3 线格式序列化缓冲豁免，ORIG loadData 实参即固定偏移）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xb)))->loadData(user, sig->m_mixedRegion + 0xbe7);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(10)))->loadData(user, sig->m_mixedRegion + 0xd3c);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x10)))->loadData(user, sig->m_mixedRegion + 0xd7c);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xe)))->loadData(user, sig->m_expandDataE);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xf)))->loadData(user, sig->m_expandDataF);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x11)))->loadData(user, sig->m_mixedRegion + 0x2308);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x12)))->loadData(user, sig->m_mixedRegion + 0x2310);
            // [BigUserLogic] ORIG 0x084c09?? 单条 `add $0x14a9a,%edx`：实参直接用
            // sig 基址+常量（m_attendanceEvent(+0x14a99)+1 的写法产两条 add）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x13)))->loadData(
                user, reinterpret_cast<char*>(sig) + 0x14a9a);
        }

        // §4.3 具名化：m_expandDataF(+0xb1a4)+0x1804 = +0xc9a8 =
        // SigTypes_C.h:546 具名 m_achievementLastUpdate（DB_LoadEtc
        // LoadAchievement.md：+0xb1a4 解压 0x1800 数据、+0xc9a4 计数、
        // +0xc9a8 last_update get_uint → +0xc9ac 衔接具名字段）。
        if (sig->m_achievementLastUpdate == 0)
        {
            // [BigInterLoadEtc] ORIG 0x084c0530..0x084c0538：结果留 %eax 直发
            // this（无具名槽）⇒ 链式单语句。
            reinterpret_cast<CAchievement*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xf)))->adjust();
        }

        // ---- 登录历史（CNetwork 视图位于 CUser+0xe0）----
        // ORIG:get_ip()==0 与否则两分支（仅 peerIp 缓冲写法不同，实参序一致），
        // 每分支独立调用 GetServerGroup/GetLogInOutState/G_CEnvironment/get_port/
        // GetPeerIP2/getWebAddress + Login（无缓存局部）。
        {
            CNetwork<4096, 450000>* net = &user->m_network;
            // [BigInterLoadEtc 修复] ORIG local_b4 跨 -0xb0..-0x90（0x20 字节）：
            // if 分支用 +0x10 段（lea -0xa0），else 分支用基址段（lea -0xb0）；
            // 原声明 char[16] 令 if 分支 GetPeerIP2(peerIp+0x10,0x10) 越界写 16B。
            // 声明为 uint[8] 使 else 清零复现 ORIG 的 4×`movl $0x0,disp(%ebp)` 单条形。
            unsigned int peerIp[8];   // [b28d-63] ORIG 0x84c0a23 memset 仅 m_mixedRegion（0x414）
            if (net->get_ip() == 0)
            {
                int serverGroup = (int)user->GetServerGroup();
                // [BigInterLoadEtc] int 版实测反劣 +2/分支（4.4 传 uchar 形参前多
                // 一次 movzbl 截断到临时槽）；保持 uchar 与 ORIG 传参值域一致。
                unsigned char logState = user->GetLogInOutState();
                // [BigInterLoadEtc] ORIG 0x084c07f9..0x084c0862 实参右→左：
                // serverGroup/logState 走具名槽，channelNo/port/peerIp 均为内联
                // 实参（GetPeerIP2 返回值即第二实参；channelNo/port 无具名槽）。
                user->m_historyLog.Login(user->getWebAddress(),
                    net->GetPeerIP2(reinterpret_cast<char*>(peerIp) + 0x10, 0x10),
                                         (int)(net->get_port() & 0xffff),
                                         G_CEnvironment()->m_serverEnvir.m_channelNo,
                                         logState,
                                         serverGroup);
            }
            else
            {
                // [BigInterLoadEtc 修复] ORIG 0x084c086c..0x084c088a：else 分支先
                // 清零前 16 字节（4×`movl $0x0,-0xb0/-0xac/-0xa8/-0xa4(%ebp)`）。
                peerIp[0] = 0;
                peerIp[1] = 0;
                peerIp[2] = 0;
                peerIp[3] = 0;
                int serverGroup = (int)user->GetServerGroup();
                unsigned char logState = user->GetLogInOutState();
                user->m_historyLog.Login(user->getWebAddress(),
                    net->GetPeerIP2(reinterpret_cast<char*>(peerIp), 0x10),
                                         (int)(net->get_port() & 0xffff),
                                         G_CEnvironment()->m_serverEnvir.m_channelNo,
                                         logState,
                                         serverGroup);
            }
        }

        // [BigInterLoadEtc] ORIG 0x084c093d..0x084c096e：getCurSec 结果 eax 直传
        // 实参（无具名槽），两次调用各自重取 ⇒ 内联。
        user->SetLoginTick(GlobalData::s_systemTime_.getCurSec());
        user->SetCharacStartPlayTick(GlobalData::s_systemTime_.getCurSec());

        CSkillTreeParam skillTreeParam(user, sig, packet);

        // ======== 主链（对应 ORIG try/状态码分支）========
        {

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                // [BigUserLogic 修复] 原写 m_mixedRegion[0x11](+0xc9d1)；ORIG
                // 0x084c09b1 为 movzbl 0xcdd1(%eax) = m_scriptVersion。
                unsigned char flagByte = sig->m_scriptVersion;
                unsigned char refByte = (unsigned char)
                    G_CDataManager()->m_pvpSkillTree.m_field50;  // +0xa850 基线标志
                if (flagByte != refByte)
                {
                    int myGrade = user->get_pvp_grade();
                    // ORIG:基线读取与 getInitSkillTreeGrade 调用各自重取 G_CDataManager()。
                    unsigned char initBase =
                        (unsigned char)G_CDataManager()->m_pvpSkillTree.m_field50;
                    // [BigInterLoadEtc 修复] ORIG 0x084c09e6..0x084c09ff 实证：
                    // `movzbl %al,%esi`（initBase）→ `mov %esi,0x4(%esp)` 为
                    // getInitSkillTreeGrade 的实参；`cmp %eax,%ebx` 用 myGrade(%ebx)。
                    // 旧写 `(void)initBase` + 传 myGrade 是参数用错（真语义 bug）。
                    if (myGrade < G_CDataManager()->m_pvpSkillTree
                                           .getInitSkillTreeGrade(initBase))
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
                return 0xb91;   // ORIG 0x08661e?? else 尾：dtor 后返回
            }
            user->disableSaveSkill();

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                // [BigUserLogic 修复] ORIG 0x084c0a8f/0x084c0a9d 布局：give 体为
                // fall-through、Set 体由 jne 跳出 ⇒ 源码是 OR 形判 give。
                if (sig->m_pvpSkillInsertFlag != 0 || sig->m_fairPvpLastPlaySec == 0)
                {
                    user->givePvPSkillTree(0, false, 3);
                    user->givePvPSkillTree(0, true, 3);
                    int grade = user->get_pvp_grade();
                    char secondGrow = user->getCurCharSecondGrowType();
                    char firstGrow = user->getCurCharFirstGrowType();
                    int job = user->get_charac_job();
                    int spPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                        job, (int)firstGrow, (int)secondGrow, grade, true);
                    // [BigInterLoadEtc] ORIG 0x084c0e5d..0x084c0e9?：getCurCharacSkillW
                    // 结果 eax 直传 this（无具名槽，两处各自重调）⇒ 链式内联。
                    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                        ->set_remain_sp_at_index(spPoint, ENUM_SKILL_TREE_KIND(0));
                    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                        ->set_remain_sp_at_index(spPoint, ENUM_SKILL_TREE_KIND(1));
                }
                else
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
            }

            if (skillTreeParam.SetSkillTree(this))
            {
                return 0xbcc;
            }

            {
                // [BigInterLoadEtc] ORIG 0x084c0c??：GetItemLockRefVec 结果在 %ebx、
                // titleBook/两个 itemLock 均 eax 直传（无具名槽）⇒ 全内联。
                reinterpret_cast<CTitleBook*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(0xe)))->getItemLockList(
                    reinterpret_cast<item_lock::CItemLock*>(user->GetCharacExpandData(
                        ENUM_CHARAC_EXPAND_TYPE(2)))->GetItemLockRefVec());
                reinterpret_cast<item_lock::CItemLock*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(2)))->VerifyItemLock(user);
            }

            // [BigInterLoadEtc] ORIG 0x084c0c51..0x084c0c56：`mov %al,-0x56;
            // movzbl -0x56,%eax; xor $1; test; je` ⇒ 具名字节局部（头签名 int 需
            // 调用点 P3 收窄，否则 4.4 插 test/setne 归一化多 1 条）。
            unsigned char inserted =
                (unsigned char)G_CGameManager()->insert_game_world(user);
            if (inserted == 0)
            {
                return 0xc14;
            }

            user->sendCharacQp();
            user->sendCharacQuestPiece();
            {
                CBoosterGage* booster = reinterpret_cast<CBoosterGage*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(0x11)));
                booster->send_data(user, 0);
            }
            user->SendItemMakingSkillInfo();

            // [BigUserLogic] 原 `ch && (a || b || c)` 合写在 4.4 产 bool 物化 $1/$0
            // 汇合（ORIG 0x084c0cf5+ 为逐段 test/je/cmpb/je 直落）⇒ 拆嵌套。
            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                if (sig->m_pvpSkillInsertFlag != 0 ||
                    sig->m_fairPvpLastPlaySec == 0 ||
                    pvpTreeSkip)
                {
                packet.clear();
                packet.put_header(0, 0x154);
                packet.finalize(true);
                user->Send(packet);
                }
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
                // [BigUserLogic] ORIG 无判空：直接 vtable+0x34 调 IsEventing。
                if (coinEvent->IsEventing(0))
                    fatigueBase = coinEvent->getLastEventTime();
                user->setBlueMarbleEnterCount(sig->m_blueMarbleEnterCount);

                // [BigInterLoadEtc] ORIG 0x084c0f50 段：nowTick 在 %ebx、lastPlay 由
                // getCurCharacR()->+0x7b 直读（均无具名槽）⇒ 内联实参（右→左求值）。
                if (CheckDailyScheduleTime(fatigueBase,
                        user->getCurCharacR()->m_field7b,
                        GlobalData::s_systemTime_.getCurSec()))
                {
                    user->SetFirstTimeLogin(true);
                    user->RecoverCoin(1);
                    user->AddDailyItem();
                    user->ClearCurConditionEventStep();
                    user->ClearCurConditionEventRewardStep();
                    user->ClearProperDungeonClearCount();
                    user->SetFirstLogin();

                    // [BigInterLoadEtc] 具名 bool 局部会落槽（+movb/cmpb 各一），
                    // ORIG 0x084c0f6e 区该汇聚值仅在 %al 物化无槽 ⇒ 改匿名条件。
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharacGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 0x084c12??：getHackAnalyzer 结果留
                        // %eax 直传（无槽 spill），单行嵌套形复现。
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0x1a1, 1u, 0, 0);
                    }
                    user->ResetCurCharacUsedGiftFatigueQuantity();

                    int level = user->get_charac_level();
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharSecondGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 0x084c13??(secondGrow) 为降序 jle 链 +
                        // unsigned 比较(setb)：>0x3c→(3,0xc)；>0x32→(1,0xb)。
                        unsigned int loops = 0;
                        unsigned int subCode = 0;
                        if (level > 0x3c) { loops = 3; subCode = 0xc; }
                        else if (level > 0x32) { loops = 1; subCode = 0xb; }
                        for (unsigned int i = 0; i < loops; ++i)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u, subCode, 0);
                        }
                    }
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharFirstGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 降序 jle 链：>0x32→(5,4)；>0x28→(3,3)；
                        // >0x1e→(2,2)；>0x14→(1,1)（与旧嵌套形逐区间等价）。
                        unsigned int loops = 0;
                        unsigned int subCode = 0;
                        if (level > 0x32) { loops = 5; subCode = 4; }
                        else if (level > 0x28) { loops = 3; subCode = 3; }
                        else if (level > 0x1e) { loops = 2; subCode = 2; }
                        else if (level > 0x14) { loops = 1; subCode = 1; }
                        for (unsigned int i = 0; i < loops; ++i)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u, subCode, 0);
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
                // [BigUserLogic] ORIG 无判空。
                if (GlobalData::s_power_manager->CheckCompleteDBLoadStatueInfo())
                    GlobalData::s_power_manager->SendPowerWarUserStatueInfo(user);
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
                // [BigInterLoadEtc 修复] ORIG 0x084c1279 压栈立即数 0x2aff（非 -1）：
                // `movl $0x2aff,0x4(%esp)` 后 call getDungeonMinimumRequiredLevel。
                int minLvl = G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff);
                if (lvlNow >= minLvl)
                {
                    if (!G_GameWorld()->IsPVPChannel())
                    {
                        // [BigInterLoadEtc] ORIG 0x084c1297/0x084c1301：正条件
                        // `test %al; je <false 体>` ⇒ open 真值体在前（fall-through）。
                        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
                        {
                            G_GameWorld()->send_user_dungeon_inout_message(
                                user, 0x2aff, true);
                        }
                        else
                        {
                            G_GameWorld()->send_user_dungeon_inout_message(
                                user, 0x2aff, false);
                        }
                    }
                }
            }

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c1317-0x084c1322 无具名槽
                // （eax 直传 this）⇒ 链式单语句。
                G_CDataManager()->GetAutoMarketContitionsControl()
                    ->SendNotiSetDurability(user);
            }

            // [BigUserLogic] ORIG 无判空：getEffectNum 直调。
            CGlobalEffectManager* fx = GlobalData::s_globalEffectManager_;
            if (fx->getEffectNum() != 0)
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
                    // [BigUserLogic] ORIG 0x084c17a? 布局：IsSetServerMessage 结果
                    // `test %al; je <清零体在后>` ⇒ 源码为正向 if + then=取 msg。
                    if (sharedMsg->IsSetServerMessage())
                    {
                        EVENT_SERVER_MESSAGE* msg =
                            reinterpret_cast<EVENT_SERVER_MESSAGE*>(
                                sharedMsg->GetCurServerMessage());
                        if (msg != 0)
                        {
                            // ORIG:const_iterator 循环,7 个字段逐一经 operator*() 拷贝
                            // (de 站点 ×6/字节域 ×1;m_count 先整体拷贝)。
                            // [BigUserLogic] 原 dstMsg 命名局部产 1 条基址 spill 槽
                            // + 每写点多 1 条 reload；ORIG 4.1 每写点直发 sig 基址
                            // 变址。改逐点表达式。
                            reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(
                                &sig->m_eventMessageCount)->m_count = msg->m_count;
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
                                SIG_EVENT_SERVER_MESSAGE* dst =
                                    reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(
                                        &sig->m_eventMessageCount);
                                dst->m_msg[idx].m_lenMsg = (*smi).m_lenMsg;
                                dst->m_msg[idx].m_kind = (*smi).m_kind;
                                dst->m_msg[idx].m_endTime = (*smi).m_endTime;
                                // [BigUserLogic 修复] 原用 sizeof(=0x14) 多拷 1 字节；
                                // ORIG 0x084c18e0 push 立即数 0x13。
                                strncpy(dst->m_msg[idx].m_msg2, (*smi).m_msg2, 0x13);
                                dst->m_msg[idx].m_index = (*smi).m_index;
                                // [BigUserLogic 修复] 同上：ORIG 0x084c193d 为 0x31。
                                strncpy(dst->m_msg[idx].m_msg, (*smi).m_msg, 0x31);
                                dst->m_msg[idx].m_lenMsg2 = (*smi).m_lenMsg2;
                                idx = idx + 1;
                                smi++;
                            }
                            }
                        }
                    }
                    else
                    {
                        sig->m_eventMessageCount = 0;
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
                static_cast<CDataAlias*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(7)))->loadData(user, sig->m_mixedRegion + 0x55f);
            }

            {
                if (user->GetCurCharacExpertJobType() != 0 ||
                    user->GetCurCharacExpertJobExp() == -1)
                {
                    // [BigInterLoadEtc/P259] ORIG 无 stream 具名局部：Acquire 结果
                    // 直接作 CStreamGuard 实参（eax 直传 0x4(%esp)）。
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xed7), true);
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
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf20), true);
                guard.operator*()->operator<<(0x1a7);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }

            // ORIG:两个独立 if,各自重调 G_GameWorld()->IsIntegratedPvPBaseChannel()。
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf2a), true);
                guard.operator*()->operator<<(0x1d5);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((char)user->GetServerGroup());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                CStreamGuard guard2(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf36), true);
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
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf44), true);
                    guard.operator*()->operator<<(0x1e6);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
                else
                {
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf4d), true);
                    guard.operator*()->operator<<(0x1e8);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
            }

            user->send_RedeemInfo();
            // [BigUserLogic 修复] ORIG 0x084c222f 判的是 user(param_2)!=0，
            // 不是 manager；原写法把判空对象改错 ⇒ 语义差。
            if (user != 0)
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
                // [BigUserLogic] ORIG 无判空。
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
                // [BigUserLogic] ORIG 0x084c24??：实参右到左 = cnt(0xc9b7,movzbl%esi)
                // → total(0xc9b3,%ebx) → getCurSec() 调用最后；三命名局部形产 6 条
                // 槽往返，单行直传复现。
                user->setPcRoomPlayTimeData(
                    (unsigned int)GlobalData::s_systemTime_.getCurSec(),
                    sig->m_totalPcRoomPlayTime,
                    sig->m_pcRoomPlayTimeRewardCnt);
            }
            user->set_aura_avatar_option(*reinterpret_cast<stAuraAvatarOption*>(
                sig->m_mixedRegion + 0xd70));
            user->send_aura_avatar_option();
            // [BigUserLogic] ORIG 无判空。
            GlobalData::s_serverStateManager_->sendState(user);
            user->setCharacOption(*reinterpret_cast<CharacterOption*>(
                sig->m_mixedRegion + 0x2108));
            user->sendCharacOption();
            if (sig->m_fieldC != 0)
                user->SendTagCharacInfo();

            const_cast<_Charac_info*>(user->getCurCharacR())->m_field14BC = 0;   // _Charac_info pad 拆分后 +0x14bc 具名字段

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2169-0x084c2173 无具名槽。
                G_CGameManager()->GetCraneMinigameManager()
                    ->SendCraneItemNeedMaterial(user);
            }
            processAttendAnceEvent(&sig->m_attendanceEvent, *user);
            user->SendMaxEquipLevel();
            processGrowthWeaponEvent(*user);
            processGrowthCreatureEvent(*user);
            user->sendBingoAddData();
            user->requestCubePremiumInfoIfContracted();
            CerashopAddRestrict::Manager::LoadRequestBuyRestrictItem(user);

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2207-0x084c2213 无 ev 槽
                // （GetRepeatEvent 结果 eax 直传虚调用）。
                if (GlobalData::s_event_manager->GetRepeatEvent(0x9e)->IsEventing(0))
                    DB_LoadEventLevelupSupport::makeRequest((int)user->GetUID(),
                                                            user->getCurCharacNo());
            }
            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2226-0x084c2229 无具名槽。
                ARAD::Singleton<HeroMissionEvent>::Get()->loadEventData(user);
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

            return user->unlock4DataLoad() ? 0 : 0x1139;
        }
    }
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：Inter_LoadEtc 生命周期（ORIG objdump 转写，报告缺失标注）
// ---------------------------------------------------------------------------

// ORIG W 0x084e9cb6 sz 0x1c（_ZN13Inter_LoadEtcC1Ev，objdump 直接转写，
// 无独立报告）：call ISigDispatcher ctor + vptr=0x8c8fff0。
// ORIG W 0x084e9cd2 sz 0x30（D1：vptr=0x8c8fff0 → ISigDispatcher D1 → delete
// 条件体）。out-of-line 定义使本 TU 成为 vtable for Inter_LoadEtc 的发出点
// （key function=析构；此前无定义导致链接 undefined vtable）。
// 判据㉓ 符号绑定：ORIG _ZN13Inter_LoadEtcD1Ev/D2Ev 0x084e9cd2、D0Ev 0x084e9d02
// 全 W（头内定义形）；我方外联定义发 T ⇒ 本 TU 内定义加 weak（不改头）。
__attribute__((weak)) Inter_LoadEtc::~Inter_LoadEtc() {}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：CAradRyosikaEvent::DoEvent（ORIG T 0x081a107c sz 0x7d，
// 报告 docs/class_func_reports/CAradRyosikaEvent/DoEvent.md 全文已读）：
// user!=0 时 vptr+0x34（IsEventing(0)）为真且 ApplyDatePeriod(user) 为真
// → SendMail(user, param2, 1, param4)（param3 不透传，ORIG 汇编压栈序实证）。
void CAradRyosikaEvent::DoEvent(CUser* user, int param2, int param3, int param4)
{
    // [b28c-L18 代落] ORIG 0x081a107c body 尾有 `jmp <统一exit>` 中继岛 ⇒ P223
    // goto 尾空标签形（then 尾跳出）；虚调用 `call *%edx` 差记 N7。
    if (user == 0)
        goto done;
    if (IsEventing(0) && ApplyDatePeriod(user))
    {
        SendMail(user, param2, 1, param4);
    }
done:;
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
// 判据㉓ 符号绑定：ORIG `_ZNK27PvPSkillTreeParameterScript21getInitSkillTreeGradeEi`
// = W 0x084e913a sz 0x6a（头内定义形；同 TU 其它成员 getGivePvPSkillPoint 等为 T）
// ⇒ 本 TU 内定义加 weak（不改头）。
__attribute__((weak)) int PvPSkillTreeParameterScript::getInitSkillTreeGrade(int grade) const
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
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 200/200（fresh，0 差异块）：p167 CLEAR（33/33）。
    // 本轮已修：去 skillR/skillW 缓存（P0/P183）+ giveIdx 预清（P198①）+ **两处
    // givePoint 去具名局部改内联条件表达式**（P0：ORIG `call; test; setne; test; je` 无槽）
    // + 第二处 P272 正写（!= 0）使 then 直落、givePvPSkillTree 落尾。
    int usedSp = 0;     // calcUsedSP 出参1（ORIG -0x2c）
    int usedSfp = 0;    // calcUsedSP 出参2（ORIG -0x30，后续未读）
    // [b28d-solo-Inter_LoadEtc_impl] P0/P183：ORIG 无 skillR 缓存局部
    // （每处 getCurCharacSkillR 返回值寄存器直传实参槽）。
    reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
        ->calcUsedSP(skill, usedSp, usedSfp);

    // ORIG：grade/secondGrow/firstGrow/job 三个计算点各自重取（P48 去缓存）；
    // getCurCharacSkillR 亦两处调用（calcUsedSP 后 get_remain_sp_at_index 前重新 GetInBuffer）。
    int pvpPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
        user->get_charac_job(),
        (int)(char)user->getCurCharFirstGrowType(),
        (int)(char)user->getCurCharSecondGrowType(),
        user->get_pvp_grade(), false);

    int remainSp = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(kind);
    int avail = pvpPoint - (usedSp + remainSp);      // ORIG -0x20
    int giveIdx = 0;                                 // ORIG -0x1c（预清 `movl $0x0`）
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
        // [b28d-solo-Inter_LoadEtc_impl] P0：ORIG `call; test %eax,%eax;
        // setne %al; test %al,%al; je` 无栈槽往返 ⇒ 原码为内联条件表达式，
        // 非 `int givePoint` 具名局部（旧形 `mov %eax,-0x20; cmpl $0,-0x20`）。
        if (G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
                user->get_charac_job(),
                (int)(char)user->getCurCharFirstGrowType(),
                (int)(char)user->getCurCharSecondGrowType(),
                user->get_pvp_grade()) != 0)
        {
            reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                ->set_remain_sp_at_index(remainSp + avail, kind);
        }
        else
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
    }
    else if (avail < 0)
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->clear_all_skills(kind);
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->set_parent(user);
        // 同上（P0）：第二处亦为内联条件表达式。
        // [b28d-solo-Inter_LoadEtc_impl] P272/P222：ORIG `setne %al; test; je <then>`
        // ⇒ then 体（set_remain_sp）直落、givePvPSkillTree 落尾 ⇒ 条件取 != 0 正写。
        if (G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
                user->get_charac_job(),
                (int)(char)user->getCurCharFirstGrowType(),
                (int)(char)user->getCurCharSecondGrowType(),
                user->get_pvp_grade()) != 0)
        {
            reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                ->set_remain_sp_at_index(pvpPoint, kind);
        }
        else
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
    }
    user->givePvPSkillTree(0, true, giveIdx);
}

// ORIG T 0x08a5dc74 sz 0xed（_ZNK27PvPSkillTreeParameterScript20getGivePvPSkillPointEiiii，
// 报告 docs/class_func_reports/PvPSkillTreeParameterScript/getGivePvPSkillPoint.md 全文已读）：
// grade 被 m_field4c(+0x4c) 收敛上限；m_map18(+0x18 map<int, map<int,int>>) 按
// job find；内层按 job*5+firstGrow find；命中返回 second，否则 0。
int PvPSkillTreeParameterScript::getGivePvPSkillPoint(int job, int firstGrow,
                                                      int secondGrow,
                                                      int grade) const
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 75/75：clamp 写回形参（P77 扩展）+
    // 内层键 job*5+firstGrow（真语义 bug 修正）+ 具名 key2 复现槽序（P236 反向）。
    // [b28d-solo-Inter_LoadEtc_impl] P77 扩展：ORIG 0x08a5dc74 clamp 直接写回形参
    // （0x18(%ebp) 原地改写，无 key 局部）；内层键 = job*5 + firstGrow——三重证据：
    // ①汇编 `mov 0xc(%ebp);shl2;add;add 0x10(%ebp)`；②Ghidra `param_2*5+param_3`；
    // ③调用点符号 080fdf20=get_charac_job / 08110c94=getCurCharFirstGrowType /
    // 0822f23c=getCurCharSecondGrowType / 0819ee4a=get_pvp_grade。旧
    // firstGrow*5+secondGrow 为真语义 bug（secondGrow 在 ORIG 本函数内未被消费）。
    (void)secondGrow;
    if (m_field4c < grade)
    {
        grade = m_field4c;
    }
    std::map<int, std::map<int, int> >::const_iterator it = m_map18.find(grade);
    if (it != m_map18.end())
    {
        int key2 = job * 5 + firstGrow;
        std::map<int, int>::const_iterator it2 = it->second.find(key2);
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
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 133/133：去 inventory 局部
    // （P0：getCurCharacInvenR 返回值寄存器直传 GetInvenRef）⇒ 帧 0x30→0x20 归位。
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
    // [b28d-solo-Inter_LoadEtc_impl] P0：ORIG 0x084e6d70 无 inventory 局部
    // （getCurCharacInvenR 返回值寄存器直传 GetInvenRef 实参）。
    const Inven_Item* ref = user->getCurCharacInvenR()->GetInvenRef(1, m_param2);
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
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 31/31：size 具名局部改 **unsigned int**
    // （P211 档③/P274 同异型旋钮）⇒ 4.4 实参装载序翻为 dest-first 且成员偏移取址
    // 恢复单条 `lea 0x46(%eax),%edx`（原 `mov;add` 两段 + size-first 序为 int 型固有）。
    // 探针 /tmp/b28d_ile/probe2.cpp g1(SS*,const MS*,unsigned) 逐条同构。
    unsigned int size = 0x198;
    if (slotFlag == -1 || slotFlag == 0)
    {
        memcpy(skillSlot->m_skillSlot, skill, size);
    }
    else if (slotFlag == 1)
    {
        memcpy(skillSlot->m_skillSlot2, skill, size);
    }
}

// ORIG T _ZN27Inter_VillageAttackedReward17_send_reward_itemEP5CUsermii
// @0x084e0390 sz0x232（docs/class_func_reports/Inter_VillageAttackedReward/
// _send_reward_item.md 全文已读）。按 itemIdx 现做道具经系统邮件发放：
// -1 → 错误日志直返；1 → 耐久字段 m_fieldb 置 1；第 4 参 ORIG 未消费。
void Inter_VillageAttackedReward::_send_reward_item(CUser* user,
                                                    itemIndexOnlyServer_t itemIdx,
                                                    int count, int rank)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 151/151：content 声明提前于 title
    // （ORIG 先清 -0x173 后清 -0x36 ⇒ 声明序=清零序，P214③）。
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
    // [b28d-63] ORIG 无 memset 调用（p167 EXTRA ×2）⇒ 聚合零初始化（P214③）。
    char content[256] = {0};
    char title[30] = {0};   // [b28d-solo-Inter_LoadEtc_impl] ORIG 先清 content（-0x173）
                            // 后清 title（-0x36）⇒ content 声明在前（声明序=清零序）。
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_124", 0),
            0x1d);
    strncpy(content, g_scriptStringManager_.findString(4, "game_server_msg_125", 0),
            0xff);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, static_cast<unsigned int>(user->getCurCharacNo()), content,
        static_cast<int>(strlen(content)), 0xc, user->GetServerGroup(), false,
        false);
}
