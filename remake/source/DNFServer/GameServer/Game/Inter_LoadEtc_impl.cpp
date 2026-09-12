#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_LoadEtc_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
        &m_sig->m_backupSkill,
        ENUM_SKILL_TREE_KIND(0));
    m_user->SaveBackupSkillInfo(
        &m_sig->m_backupSkill2ND,
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
            // [PoolF 2026-09-11 负结论] Main「指针局部→整型承载地址」旋钮在本件
            // **无效**：`CSkill* skill`→`unsigned int skillAddr` + `((CSkill*)skillAddr)->`
            // 实测读数逐值不变（536/540、ndiff 18）⇒ 本件槽映射差不在 skill 指针分区，
            // 已回退原形（保 p167 同调用集）。
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
                const char*, unsigned int);        // ORIG T 0x084bfb84（producer 归 Item/Event 域）
#include "Inter_LoadEtc_impl/CheckDailyScheduleTime.inc"
#include "Inter_LoadEtc_impl/ComputeDaysScheduleTime.inc"

#include "Inter_LoadEtc_impl/Taiwan_GarenaCyberCafe.inc"
#include "Inter_LoadEtc_impl/Taiwan_GarenaAuthPCRoom.inc"

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

#include "Inter_LoadEtc_impl/SetRankRecord.inc"
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

#include "Inter_LoadEtc_impl/_fnEnumQuestClear.inc"


#include "Inter_LoadEtc_impl/SetFatigue.inc"


#include "Inter_LoadEtc_impl/verifyCharacVillage.inc"


#include "Inter_LoadEtc_impl/RequestDBEventArad.inc"


#include "Inter_LoadEtc_impl/SetPvpRecord.inc"


#include "Inter_LoadEtc_impl/SendResult.inc"


#include "Inter_LoadEtc_impl/giveBreakAwayReward.inc"


#include "Inter_LoadEtc_impl/processGrowthWeaponEvent.inc"


#include "Inter_LoadEtc_impl/processGrowthCreatureEvent.inc"


#include "Inter_LoadEtc_impl/processSeriaRoomAniDecoEvent.inc"


#include "Inter_LoadEtc_impl/processCheckInvalidExpPerLevel.inc"


#include "Inter_LoadEtc_impl/_checkUserDisadvantage.inc"


#include "Inter_LoadEtc_impl/ProcessOneDayLethe.inc"


#include "Inter_LoadEtc_impl/ProcessOneDayLethe2ND.inc"


#include "Inter_LoadEtc_impl/processAttendAnceEvent.inc"


#include "Inter_LoadEtc_impl/dispatch_sig.inc"


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

#include "Inter_LoadEtc_impl/DoEvent.inc"


#include "Inter_LoadEtc_impl/ApplyDatePeriod.inc"


#include "Inter_LoadEtc_impl/SendMail.inc"


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

#include "Inter_LoadEtc_impl/SetPvPSkillTree_1.inc"


#include "Inter_LoadEtc_impl/getGivePvPSkillPoint.inc"


// ====================================================================
// LinkE2 批：Inter_NpcLimitBuyItemInfo / Inter_RequestCharacSkillInfo /
//            Inter_VillageAttackedReward（逐指令转写自对应报告）
// ====================================================================

#include "Inter_LoadEtc_impl/sendPacket.inc"


#include "Inter_LoadEtc_impl/_PutSkillInfoToCharac.inc"


#include "Inter_LoadEtc_impl/_send_reward_item.inc"

