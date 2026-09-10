// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Inter_LoadEtc_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "Inter_LoadEtc_impl/CSkillTreeParam.inc"
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
// [round5-A20 复核] fresh .o 539/536：①ORIG 0x084ef4ae/0x084ef4f3 两处
//   `lea 0x29fd/0x2b97(%eax),%edx`（数组退化取址）vs 4.4 `add $K,%eax; mov %eax,%edx`
//   = N107（P214① 数组名算术形亦试，4.4 恒 add）；②第 145 行处多 1 条 `jmp`（N121
//   epilogue 尾岛）⇒ 双封顶。全部调用点 p167 CLEAR(87/87)。
// [round5-A20 D08 试前闸门] 岛差同样 = 1（< 2）⇒ 归 4.4 relay 族，不试合并。
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


#include "Inter_LoadEtc_impl/SetPvPSkillTree.inc"
#include "Inter_LoadEtc_impl/RewardItem.inc"
#include "Inter_LoadEtc_impl/CheckDailyScheduleTime.inc"
#include "Inter_LoadEtc_impl/ComputeDaysScheduleTime.inc"
#include "Inter_LoadEtc_impl/Taiwan_GarenaCyberCafe.inc"
#include "Inter_LoadEtc_impl/Taiwan_GarenaAuthPCRoom.inc"
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

#include "Inter_LoadEtc_impl/SetRankRecord.inc"
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

// [round5-A20 复核·N107/P23] fresh .o 40/42。残余=ORIG 0x084c2b0e/0x084c2b1a
//   两处 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)`（2 条/处）vs 4.4 `addl $0x4,0xc(%ebp)`
//   （1 条/处）= N107 lea-vs-add 族（P319 整型加法+回 cast 形已试，4.4 恒 add）
//   + put_short 实参装载序 ORIG 先入 %edx（P23/P75 跨版本）。语义等价，封顶。
#include "Inter_LoadEtc_impl/_fnEnumQuestClear.inc"
}

// [round5-A20 复核] fresh .o 170/169。残余=①curCharging 槽 -0x28 vs ORIG -0x50
//   （4.1 trace 前求值缓存落深区、4.4 落浅区，N82 分区）；②vararg %esi/%edi 角色互换
//   （ORIG `mov %eax,%esi`/`mov 0x7b(%eax),%edi` vs 4.4 %edi/%esi，N7）
//   ⇒ N82+N7 双封顶，语义完成。
#include "Inter_LoadEtc_impl/SetFatigue.inc"
}

// [round5-A20 复核·N121] fresh .o 102/101：判据⑪ 直方图逐值全等 {−0x9,−0x10,−0x14}；
//   唯一残差=我方 switch 尾多 1 条 `jmp`（.o 内 1272: nop; 1273: jmp 1276; 1275: nop
//   —— default 岛），ORIG 单 nop 直落 epilogue ⇒ 4.4 自身 epilogue-threading 尾岛
//   （N121，四形实测不可消）。
// [round5-A20 D08 试前闸门] **我方岛数 − ORIG 岛数 = 1**（< 闸门阈值 2）⇒ 按
//   C05③ 闸门归「4.4 rel8 跳板/epilogue-threading」族（同 A28/A06 反例），
//   不做多守卫合并试验（无 ≥2 岛差即无「守卫相邻同目标」可赚）。
#include "Inter_LoadEtc_impl/verifyCharacVillage.inc"
}

#include "Inter_LoadEtc_impl/RequestDBEventArad.inc"
}

#include "Inter_LoadEtc_impl/SetPvpRecord.inc"
}

// [round5-A20 复核·N95] fresh .o 402/397：PacketGuard 槽 -0x3c vs ORIG -0x44（整帧局部
//   槽基差 8）——4.1 大局部可顶到 ebp、4.4 恒从 ebp-8 起 ⇒ N95「局部槽基 gap 差」
//   （double framePad 反致下移，P190/N95 实证）。语义完成。
#include "Inter_LoadEtc_impl/SendResult.inc"
}

#include "Inter_LoadEtc_impl/giveBreakAwayReward.inc"
}

#include "Inter_LoadEtc_impl/processGrowthWeaponEvent.inc"
}

#include "Inter_LoadEtc_impl/processGrowthCreatureEvent.inc"
}

#include "Inter_LoadEtc_impl/processSeriaRoomAniDecoEvent.inc"
}

#include "Inter_LoadEtc_impl/processCheckInvalidExpPerLevel.inc"
}

#include "Inter_LoadEtc_impl/_checkUserDisadvantage.inc"
}

// [round5-A20 复核] fresh .o 251/247（gap +4）：3~4 处「字节读比较」双段形
//   ORIG `movzbl 0x2b95(%eax),%edx; movzbl <A>,%eax; cmp %al,%dl`
//   vs 我方 `movzbl 0x2b95(%eax),%eax; mov %eax,%edx; …`（多 eax→edx 中转）
//   = N18a/N7（4.4 对 uchar 比较恒插中转）。**本轮新试 P266 形
//   `*reinterpret_cast<unsigned char*>(sig->m_backupSkill + 0x198)`：实测劣化
//   251→256（add $0x29fd + add $0x198 + movzbl 三段），已回退** ⇒ 封顶。
#include "Inter_LoadEtc_impl/ProcessOneDayLethe.inc"
}

// [round5-A20 复核] 同 1 号（ORIG 0x084e8e1e 同构）：fresh .o 251/247；残余同为
//   3~4 处 uchar 比较 eax→edx 中转（N18a/N7，P266 形实测劣化已回退）+ 槽深 N95。
#include "Inter_LoadEtc_impl/ProcessOneDayLethe2ND.inc"
}

#include "Inter_LoadEtc_impl/processAttendAnceEvent.inc"
}

// [round5-A20 复核·N82/P98] fresh .o 2324/2296（gap +28）、判据⑪ frame ORIG 0x15c vs
//   我方 0x1bc（+0x60 = ~24 个 4B 槽）：ORIG 把 `data` 形参副本按需惰性落槽
//   （`mov 0x10(%ebp),%eax; mov %eax,-0x60(%ebp)`），我方前移至 -0xc4 序言；
//   另有 1 处 ORIG 无的 `lea -0xe4; mov (%esp); call` ⇒ 4.1/4.4 槽分区与重载选型差
//   （N82 附/N98），语义完成、p167 CLEAR。
#include "Inter_LoadEtc_impl/dispatch_sig.inc"
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
#include "Inter_LoadEtc_impl/DoEvent.inc"
}

// ORIG T 0x081a0fda sz 0xa（ApplyDatePeriod.md 全文已读）：恒返 true。
#include "Inter_LoadEtc_impl/ApplyDatePeriod.inc"
}

// ORIG T 0x081a0fe4 sz 0x97（SendMail.md 全文已读）：
// m_field10 < 0 → send_mail(user,p2,p3,p4,m_field0c)；
// 否则 p4==7 → p3=5；m_field10 == p4 → send_mail(user,p2,p3,p4,m_field0c)。
// 依赖 GlobalInstance<Arad_GiftItem_Set>::inst()（ORIG W 0x081a1155）。
#include "Inter_LoadEtc_impl/SendMail.inc"
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
#include "Inter_LoadEtc_impl/SetPvPSkillTree_1.inc"
}

// ORIG T 0x08a5dc74 sz 0xed（_ZNK27PvPSkillTreeParameterScript20getGivePvPSkillPointEiiii，
// 报告 docs/class_func_reports/PvPSkillTreeParameterScript/getGivePvPSkillPoint.md 全文已读）：
// grade 被 m_field4c(+0x4c) 收敛上限；m_map18(+0x18 map<int, map<int,int>>) 按
// job find；内层按 job*5+firstGrow find；命中返回 second，否则 0。
#include "Inter_LoadEtc_impl/getGivePvPSkillPoint.inc"
}

// ====================================================================
// LinkE2 批：Inter_NpcLimitBuyItemInfo / Inter_RequestCharacSkillInfo /
//            Inter_VillageAttackedReward（逐指令转写自对应报告）
// ====================================================================

// ORIG T _ZN25Inter_NpcLimitBuyItemInfo10sendPacketEP5CUser @0x084e6c42 sz0x1d7
// （docs/class_func_reports/Inter_NpcLimitBuyItemInfo/sendPacket.md 全文已读）。
// 限制购买 NPC 物品信息回执包（0x1d1）：m_data1c==0 时附带 m_param1 与
// m_invenItem1 全字段（含 m_amp 强化选项、put_packet 原始物品体）。
#include "Inter_LoadEtc_impl/sendPacket.inc"
}

// ORIG T _ZN28Inter_RequestCharacSkillInfo21_PutSkillInfoToCharacEcPK15_Mastered_skillP9SkillSlot
// @0x084e7820 sz0x64（docs/class_func_reports/Inter_RequestCharacSkillInfo/
// _PutSkillInfoToCharac.md 全文已读）。slotFlag 0/-1 → m_skillSlot[0]，
// 1 → m_skillSlot2[0]；单条 _Mastered_skill（0x198 字节）memcpy 拷贝。
#include "Inter_LoadEtc_impl/_PutSkillInfoToCharac.inc"
}

// ORIG T _ZN27Inter_VillageAttackedReward17_send_reward_itemEP5CUsermii
// @0x084e0390 sz0x232（docs/class_func_reports/Inter_VillageAttackedReward/
// _send_reward_item.md 全文已读）。按 itemIdx 现做道具经系统邮件发放：
// -1 → 错误日志直返；1 → 耐久字段 m_fieldb 置 1；第 4 参 ORIG 未消费。
#include "Inter_LoadEtc_impl/_send_reward_item.inc"
}
