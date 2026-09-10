// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_InternalDispatcher_INTER/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// [M3S4 2026-09-11 · §6.5 串盲区 · 父泳道 M3FreeNext15B 批准] 文件作用域（全局命名空间）typedef：
//   ORIG prett `_ZZN…sendHelperBox…__PRETTY_FUNCTION__` @0x08ba2e40 size 0x5b =
//   "bool ARAD::DISPATCHER::Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser*, byte, byte)"。
// GCC `__PRETTY_FUNCTION__` 取**定义处**拼写并保留 typedef 名（探针 /tmp/m3s4_probe_prett.cpp 实证），
//   但置于 namespace 内会打印限定名 `ARAD::DISPATCHER::byte`（本 TU 实测）⇒ 必须全局作用域。
//   底层类型/mangled/sizeof/偏移/布局全不变（同 WarField.cpp:307 先例）。
typedef unsigned char byte;

#include "Arad_InternalDispatcher.h"

#include <time.h>
#include <string.h>
#include <string>

#include "GlobalData.h"
#include "CUser.h"              // CUser::eSendTarget / UserQuest / quest getter
#include "CUserCharacInfo.h"
#include "CInventory.h"         // CInventory / OS_API::snprintf
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h"    // AvatarRouletteServer / LevelUpSkill
#include "CGuildServerProxy.h"  // CEventBase / CEventManager::GetRepeatEvent
#include "CSystemTime.h"
#include "CEnvironment.h"
#include "Quest.h"
#include "QuestClear.h"                 // WongWork::CQuestClear
#include "CDataManager.h"               // G_CDataManager / find_quest / m_aradJumping
#include "Arad_DataManager.h"           // ARAD::Singleton<T> / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "AradServerStateMessage.h"
#include "Arad_ServerStateManager.h"    // insertState/deleteState/ENUM_GLOBAL_EFFECT_ARAD
#include "AvatarFixedHiddenOptionServer.h"
#include "AvatarRechargeServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarConvertServer.h"
#include "MoreTypes.h"       // arad::SigAradEmblemCompound 权威定义（MoreTypes.h:257）

namespace
{

// ORIG INTER 包体字节视图（包字段布局归 OnlyOrig_ARAD 批的 Sig 结构化，
// 此处仅为指令级位移寻址形态保持：packet+offset 直读）。
#pragma pack(push, 1)
struct PktSaveServerStateView
{
    char m_padE[0xe];        // +0x00..0x0d
    int m_effect;            // +0x0e ENUM_GLOBAL_EFFECT_ARAD
    unsigned int m_endSec;   // +0x12
};
#pragma pack(pop)

}  // namespace

// ============================================================================
// AvatarRouletteServer::PickedAvatarInfo ctor —— ORIG W 0x0819a336
// （嵌套弱符号；dispatch_sig 栈对象需要，登记于本 TU）
// ============================================================================
// 类在全局命名空间（DataManagerInitDepends.h），无须 ns 限定。
#include "Arad_InternalDispatcher_INTER/PickedAvatarInfo.inc"
{
    m_pad0 = 0;              // +0x00
    m_itemId = 0;            // +0x04
    m_optionOrSkillIdx = 0;  // +0x08
    m_pad9 = 0;              // +0x09
    m_zoneType = 0;          // +0x0a
    m_gradeBand = 0;         // +0x0b
}

namespace ARAD
{

namespace DISPATCHER
{

// ============================================================================
// Arad_INTER_kLoadServerState —— ORIG 0x081992fe
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig.inc"
{
    // param_3 即 (arad::SigServerState*) 包指针复用位。
    // 【b26 NL10】局部即目标指针形（cast 收敛到声明处）：ORIG 0x081992fe
    // insertState 调用为先载 receiver(全局)→eax、后载槽→edx；cast 延到调用点会
    // 使 4.4 反转求值序（探针 p_s2e probe4 实证）。
    arad::SigServerState* packet = (arad::SigServerState*)body;
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x533, "SigServerState is null.");
        return 0x534;
    }
    GlobalData::s_serverStateManager_->insertState(packet);
    return 0;
}

// ============================================================================
// Arad_INTER_kSaveServerState —— ORIG 0x08199278
//   栈上构造 AradServerStateMessage{m_active=1, m_endSec=*(pkt+0x12)}，
//   insertState(*(ENUM_GLOBAL_EFFECT_ARAD*)(pkt+0xe), msg)。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_1.inc"
{
    const char* packet = body;   // ORIG 用 body(char*@+0x10)；我们误 cast len(int@+0x14)
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x521, "Packet_Arad_ApplyEffect is null.");
        return 0x522;
    }

    AradServerStateMessage msg;                      // ORIG W ctor：全零
    msg.m_active = 1;                                // short @+0x00
    msg.m_endSec = ((const PktSaveServerStateView*)packet)->m_endSec;

    GlobalData::s_serverStateManager_->insertState(
        (ENUM_GLOBAL_EFFECT_ARAD)((const PktSaveServerStateView*)packet)->m_effect, msg);
    return 0;
}

// ============================================================================
// Arad_INTER_kDeleteServerState —— ORIG 0x0819935e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_2.inc"
{
    const char* packet = body;   // ORIG 用 body(char*@+0x10)；我们误 cast len(int@+0x14)
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x540, "Packet_Arad_DeleteEffect is null.");
        return 0x541;
    }
    GlobalData::s_serverStateManager_->deleteState(
        (ENUM_GLOBAL_EFFECT_ARAD)((const PktSaveServerStateView*)packet)->m_effect);
    return 0;
}

// ============================================================================
// Arad_INTER_kLoadRewardEventItem —— ORIG 0x08198fd0
//   user/CurCharac 空 → 0x4d3；sig 空 → log 0x4d8 返 0x4d9；
//   addUserRewardData → eventReward<FnRewardLethe>(0x8f) →
//   GetRepeatEvent(0x9b)->IsEventing(NULL) 命中再 eventReward<FnRewardMomoji>(0x9b)。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_3.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE 闭合（ORIG 0x08198fd0）：190=190 条、帧/槽全等。
    // 真语义 bug 已修：sig 原取自 len（0x14(%ebp) int），ORIG 0x08199007 读
    // 0x10(%ebp)=body(char*) ⇒ 原码解引用的是包体指针而非长度。
    if (user == 0 || user->getCurCharacR() == 0)
        return 0x4d3;

    arad::SigLoadRewardEventItem* sig = (arad::SigLoadRewardEventItem*)body;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x4d8, "SigLoadRewardEventItem is null.");
        return 0x4d9;
    }

    // [b28d-solo] P52/P134 链式单语句：ORIG 0x08199073/0x08199082 findGameScript 结果
    // 直接经 %eax 传入 receiver 调用（无具名 epdm 槽）；eventReward 的 curSec 在
    // string 构造前求值并 %ebx 保活（0x081990ee）。具名局部形落 4 槽、帧 0x60 vs 0x40。
    ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
        std::string("Arad_EventPeriodDataManager")))->addUserRewardData(user, *sig);

    ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
        std::string("Arad_EventPeriodDataManager")))->eventReward<FnRewardLethe>(
            (ENUM_REPEAT_EVENT_CODE)0x8f, GlobalData::s_systemTime_.getCurSec(), user);

    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 NULL。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9b)->IsEventing(0))
    {
        ((Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
            std::string("Arad_EventPeriodDataManager")))->eventReward<FnRewardMomoji>(
                (ENUM_REPEAT_EVENT_CODE)0x9b, GlobalData::s_systemTime_.getCurSec(), user);
    }
    return 0;
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::clearEpicQuest —— ORIG 0x081993c4
//   第一遍扫角色 20 槽 in-progress 任务（等级达标即结清+发奖）；第二遍遍历
//   QuestList::getQuestNPCList(GRADE 0) 的 multimap value 对应任务，排除转职
//   0xe04..0xe08 且职业匹配、未清者结清+发奖。返回是否有任一结清。
// ============================================================================
#include "Arad_InternalDispatcher_INTER/clearEpicQuest.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER] P157/P183 逐字段重读 + P171 照抄：ORIG 0x081993c4 全链式
    // （getCurCharacQuestW/R 结果直接下标，无具名 questW/questR 槽）；
    // 返回类型 bool（尾 movzbl -0x29(%ebp),%eax 双出口）；r0 原码未初始化
    // （仅 3 条 movl $0 落 -0x34/-0x38/-0x3c）；getQuestNPCList 无 npcQuestList
    // 判空（ORIG 直接 mgr+0x18 取表调用）。
    bool anyCleared = false;                                // -0x29

    // [b28d-solo] ORIG 调 const 版 begin/end（_ZNKSt8multimap…3endEv/5beginEv，p167
    // MISSING）⇒ 指针本身 const。
    const std::multimap<int, int>* npcMap =
        G_CDataManager()->m_questList->getQuestNPCList((ENUM_QUEST_GRADE)0);  // -0x28
    if (npcMap == 0)
        return anyCleared;

    int clearedCount = 0;                                   // -0x24
    int level = user->get_charac_level();                   // -0x20

    for (int i = 0; i < 0x14; ++i)                          // -0x1c
    {
        if (user->getCurCharacQuestW()->m_quest[i] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(
            user->getCurCharacQuestR()->m_quest[i]);        // -0x18
        if (quest == 0)
            continue;
        if (quest->m_type != 0)
            continue;
        int r3 = 0, r2 = 0, r1 = 0, r0;                     // -0x34/-0x38/-0x3c/-0x40
        int levelMin = quest->m_levelMin;                   // -0x14
        if (levelMin < level)
        {
            user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                (unsigned int)user->getCurCharacQuestR()->m_quest[i]);
            user->getCurCharacQuestW()->m_quest[i] = 0;
            user->getCurCharacQuestW()->m_trigger[i] = 0;
            user->quest_basic_reward(quest, r3, r2, r1, r0, true);
            anyCleared = true;
            ++clearedCount;
        }
    }

    // 第二遍：NPC list 对应任务族（P74 迭代器双旋钮：end 先于 begin 求值）。
    // [b28d-solo] P96/P270：ORIG 调 const_iterator 族（p167 EXTRA 非 const 5 符号）⇒
    // 显式 const_iterator（Ghidra C 亦为 _Rb_tree_const_iterator）。
    std::multimap<int, int>::const_iterator itEnd = npcMap->end();       // -0x30
    for (std::multimap<int, int>::const_iterator it = npcMap->begin();   // -0x44
         it != itEnd; ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);   // -0x10
        if (quest == 0)
            continue;
        if (quest->m_index == 0xe04)
            continue;
        if (quest->m_index == 0xe05)
            continue;
        if (quest->m_index == 0xe06)
            continue;
        if (quest->m_index == 0xe07)
            continue;
        if (quest->m_index == 0xe08)
            continue;
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;
        int r3 = 0, r2 = 0, r1 = 0, r0;                     // -0x48/-0x4c/-0x50/-0x54
        int levelMin = quest->m_levelMin;                   // -0xc
        if (levelMin < level)
        {
            if (!user->getCurCharacQuestW()->m_cleared.isClearedQuest(
                    (unsigned int)quest->m_index))
            {
                user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                    (unsigned int)quest->m_index);
                user->quest_basic_reward(quest, r3, r2, r1, r0, true);
                anyCleared = true;
                ++clearedCount;
            }
        }
    }
    return anyCleared;
}

// ----------------------------------------------------------------------------
// sendHelperBox —— ORIG 0x0819973e：按 ticket 类型取标题/正文脚本串，经
// AradJumping_Script 取奖励物品后 create_item 并 ReqDBSendNewSystemMail 投递。
// ----------------------------------------------------------------------------
#include "Arad_InternalDispatcher_INTER/sendHelperBox.inc"
                                                     byte ticketType,
                                                     byte growValue)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] NEAR→封 N7/P78（ORIG 0x0819973e）：146=146 条、帧 0x1b8 全等、
    // 槽基 item@-0x6e/body@-0x16e/title@-0x31/rewardIndex@-0x1c 全等；唯一残差 =
    // callee-saved 角色互换（ORIG job→%esi、ticketType→%edi；4.4 恒 job→%edi、
    // ticketType→%esi，压参 0x20/0x1c 随之）。实测三旋钮均不翻：①内联实参
    // ②裸 register 两声明序 ③去 rewardIndex 冗余 cast ⇒ P248 条件②/N109 封顶。
    // 真语义 bug 已修：getCurRewardItem 第三参原传独立局部 itemId，ORIG 传
    // &item.m_addInfo（Inven_Item+0x02 物品 idx 出参）⇒ 原码 create_item 造 0 号物品。
    Inven_Item item;                                        // -0x6e
    char body[256];                                         // -0x16e
    char title[21];                                         // -0x31
    int rewardIndex;                                        // -0x1c

    if (ticketType == 1)
    {
        rewardIndex = (int)growValue;
        strncpy(title, g_scriptStringManager_.findString(
                           4, "jumping_character_title_01", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(
                          4, "jumping_character_mail_01", 0), 0xff);
    }
    else
    {
        rewardIndex = (int)growValue + 0x10;
        strncpy(title, g_scriptStringManager_.findString(
                           4, "jumping_character_title_02", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(
                          4, "jumping_character_mail_02", 0), 0xff);
    }

    // [b28d-solo] 真 bug 修复：ORIG 0x0819989a 第三参 = `lea -0x6e(%ebp); add $0x2`
    // 即 &item.m_addInfo（Inven_Item+0x02 物品 idx 出参）。原用独立局部 unsigned long
    // itemId ⇒ getCurRewardItem 结果写栈槽、item.m_addInfo 恒 0，create_item 造 0 号物品。
    // [b28d-solo] ORIG 0x081998b7 `xor $1; test %al; je <success>`：bool callee 裸形
    // `if (!call())` 直产（P168a①，勿经 bool 局部——4.4 会落槽 mov%al/cmpb）。
    // 负极性直落 error、success 为跳转目标（P222 正向包裹/P194 块序）。
    if (!G_CDataManager()->m_aradJumping.getCurRewardItem(
            user->get_charac_job(), rewardIndex,
            *reinterpret_cast<unsigned long*>(&item.m_addInfo)))
    {
        // [b28d-solo] ORIG 0x081998c2..0x08199909 右起求值：get_charac_job→%esi、
        // ticketType→%edi、get_charac_no→%ebx、get_acc_id→%eax 内联实参保活。
        // [M3S4 实测-3形 · 146=146 NEAR 封顶] 残差仅 callee-saved 角色互换（ORIG
        // job→%esi/ticketType→%edi，我方恒 job→%edi/ticketType→%esi）：①单 register
        // 变量（ticketType 内联实参）同互换；②两 register 声明序反转（ticket 先）
        // 反而 NEAR→DIFF（%esi 落到 ticketType 载入、job 变 %edi 且槽序错位）；
        // ③连同旧判词的内联实参/裸 register 两序/去 cast 共 6 形 ⇒ 4.4 callee-saved
        // 分配序差，无源码形可翻。
        // [M3S4 §6.5 串盲区 · 真契约差，待 .h 全局 typedef 落地] ORIG prett
        // _ZZN…sendHelperBox…__PRETTY_FUNCTION__ @0x08ba2e40 size 0x5b =
        // "bool ARAD::DISPATCHER::Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser*, byte, byte)"；
        // 我方发射 "(CUser*, unsigned char, unsigned char)"（+22 字节）。探针实证
        // （/tmp/m3s4_probe_prett.cpp）：GCC 取**定义处**拼写且保留 typedef 名（声明写
        // unsigned char、定义写 byte ⇒ 打印 byte）；但 namespace 内 typedef 会打成
        // 限定名 `ARAD::DISPATCHER::byte`（本 TU 实测）⇒ 必须在**全局**作用域提供
        // `typedef unsigned char byte;`（.h 待办；定义侧两行形参改 byte 即复现，
        // mangled/sizeof/布局不变，1 次复编）。
        register unsigned int jobArg = (unsigned int)user->get_charac_job();
        register unsigned int ticketArg = (unsigned int)ticketType;
        LogManager::logFormat
            (1, "localjapan/Arad_InterDispatcher.cpp",
             __PRETTY_FUNCTION__,
             0x5d5,
             "JUMPING CHARACTER : REWARD ERROR - m_id:%u, charac_no:%u, tickettype:%d, job:%d, growValue:%d",
             (unsigned int)user->get_acc_id(),
             (unsigned int)user->get_charac_no(-1),
             ticketArg,
             jobArg,
             rewardIndex);
    }
    else
    {
        // ORIG 直读 mgr+0xc（getItemList 虽头内 inline，4.4 -O0 仍折成 call）。
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
        item.m_addInfo2 = 1;                                // ORIG movl $1,-0x67(%ebp) 直写
        // [b28d-solo] ORIG 0x08199977..0x081999dc 右起求值：GetServerGroup→%ebx、
        // strlen→%esi、getCurCharacNo→%eax 全部内联实参（具名局部会落 3 槽 + 帧 +0x14）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 100000, (unsigned long)user->getCurCharacNo(), body,
            (int)strlen(body), 0, (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
        return true;
    }
    return false;
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::dispatch_sig —— ORIG 0x081999f2
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_4.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER] 真 bug 修复：sig 原取自 len（0x14(%ebp) int），ORIG 0x08199a28 读
    // 0x10(%ebp)=body(char*)；字段访问改 arad::SigAradJumpingCharacter 具名成员
    // （ORIG movzwl 0xb(%eax)/movzbl 0x9(%eax)/0xa(%eax) disp 折叠，裸偏移 sig+N
    // 恒产 add $N,%eax + movzwl (%eax) 两步，§4.3）。
    if (user == 0)
        return 0x5e9;
    if (user->getCurCharacR() == 0)
        return 0x5ee;

    arad::SigAradJumpingCharacter* sig = (arad::SigAradJumpingCharacter*)body;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x5f3, "JUMPING_CHARACTER INTER_DSP(SigAradJumpingCharacter) null.");
        return 0x5f4;
    }

    // 跳级券：delete_item(INVEN 1, slot=sig->m_itemIndex, 1, reason 3, true)
    // [b28d-solo] P157 逐字段重读：ORIG 两次独立 movzwl 0xb(%eax)（delete_item 前
    // %ebx 保活、SendUpdateItemList 处重读），禁具名 slot/inventory 缓存。
    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, sig->m_itemIndex, 1,
                                                (eItemDelReason)3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x13);
        return 0;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, sig->m_itemIndex);

    // 升到 19→20 区间（<=0x13 循环自增）。
    while (user->get_charac_level() <= 0x13)
        user->SetCharacLevel(user->get_charac_level() + 1);

    // 一次成长（growValue=m_growType），二段成长（m_eventKind==2 时两次跳段）。
    int firstGrowTarget = (int)sig->m_growType;             // -0x14
    int secondGrowFlag = 0;                                 // -0x10
    user->ChangeGrowType_GM(firstGrowTarget, secondGrowFlag);
    if (sig->m_eventKind == 2)
    {
        while (user->get_charac_level() <= 0x31)
            user->SetCharacLevel(user->get_charac_level() + 1);
        secondGrowFlag = 1;
        // ORIG 0x08199bab `call getCurCharFirstGrowType; movsbl %al,%eax` 直传，
        // 无具名 curFirstGrow 槽（具名形使 sig 起全帧槽基深 4）。
        user->ChangeGrowType_GM((int)user->getCurCharFirstGrowType(), secondGrowFlag);
        while (user->get_charac_level() <= 0x36)
            user->SetCharacLevel(user->get_charac_level() + 1);
    }

    user->send_skill_info();
    clearEpicQuest(user);

    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info((char*)&packet);
    user->Send(packet);
    user->send_clear_quest_list();
    user->sendCharacQp();

    int exp = G_CDataManager()->get_level_exp(user->get_charac_level());
    user->setCurCharacExp(exp);

    sendHelperBox(user, (unsigned char)sig->m_eventKind, sig->m_growType);

    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
    user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    return 0;
}

// ============================================================================
// Arad_INTER_kAvatarHiddenOptionChange —— ORIG 0x08199d6e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_5.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE（ORIG 0x08199d6e）：112=112、
    // 帧 0x30/槽 -0xc/-0x18 全等；SigHV 视图改真实字段、sete 直测形见下方注释。
    // ORIG ebx 出口寄存：各失败路径直接设值返回（检查顺序 user→sig→getCurCharacR）。
    if (user == 0)
        return 0x664;

    arad::SigAvatarHiddenOptionChange* sig = (arad::SigAvatarHiddenOptionChange*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x669, "AVATAR HIDDENOPTION CHANGE INTER_DSP(SigAvatarHiddenOptionChange) null.");
        return 0x66a;
    }
    // [b28d-solo-Arad_InternalDispatcher_INTER] ORIG 0x08199dd3：call; test %eax,%eax; sete %al; test %al,%al; je
    // ＝ 直接 `if (x == 0)` 的 4.4 sete 物化直测形（int callee 非 bool）。
    // 反例：`bool z=(x==0); if(z)` 落 `mov %al,-0x9; cmpb $0,-0x9` 撑帧；
    // `register bool z` 免槽但落 %bl（P78 汇聚变量寄存器差）⇒ 均劣于直接 if。
    if (user->getCurCharacR() == 0)
        return 0x670;

    // P48：ORIG 每次使用均重调 Singleton<...>::Get()，不许提升到局部。
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<AvatarFixedHiddenOptionServer>::Get()->ChangeHiddenOption(user, sig))
        return 0;

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24c);
    packet.put_byte(1);
    // ORIG movzwl 0x2(%eax)+cwtl / movzwl 0x4(%eax)+movzwl %ax disp 折叠
    // ＝ 真实字段 short m_avatarSlot / unsigned short m_hiddenOption。
    packet.put_short((int)sig->m_avatarSlot);
    packet.put_short((unsigned int)sig->m_hiddenOption);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

// ============================================================================
// Arad_INTER_kUseAvatarRecharge —— ORIG 0x08199f0e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_6.inc"
{
    // [b28d-59 P0 清绑] ORIG 0x08199fxx：各失败路径 `mov $imm,%ebx; jmp end`＝
    // 出口寄存器跨 call 存活；原 `register int ret asm("ebx")` 绑定已按 P0/N96
    // 清为纯 register 存储类提示（P231：裸 register 合法且仍参与分配决策）。
    register int ret;
    if (user == 0)
    {
        ret = 0x68e;
        goto out;
    }
    if (user->getCurCharacR() == 0)
    {
        ret = 0x691;
        goto out;
    }

    {
        arad::SigAradUseAvatarRecharge* sig = (arad::SigAradUseAvatarRecharge*)body;   // ORIG 读 0x10(%ebp)=body
        if (sig == 0)
        {
            LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                                  0x696, "AVATAR USE AVATAR RECHARGE INTER_DSP(SigAradUseAvatarRecharge) null.");
            ret = 0x697;
            goto out;
        }

        // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Recharge 每次各自 Get()。
        if (!Singleton<AvatarRechargeServer>::Get()->CheckConditionUseRecharge(user, sig))
        {
            ret = 0;
            goto out;
        }
        if (!Singleton<AvatarRechargeServer>::Get()->Recharge(user, sig))
        {
            ret = 0;
            goto out;
        }
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x24d);
            packet.put_byte(1);
            packet.put_int((int)sig->enduranceAfter);       // +0x0c（操作后耐久）
            packet.put_int(sig->addEndurance);              // +0x04（本次增加耐久）
            packet.put_short((unsigned int)sig->avatarSlot);  // +0x00（avatar 槽）
            packet.finalize(true);
            user->Send(packet);
            ret = 0;
            goto out;
        }
    }
out:
    return ret;
}

// ============================================================================
// Arad_INTER_kEmblemCompound —— ORIG 0x0819a0c0
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_7.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE（ORIG 0x0819a0c0）：90=90、
    // push edi/esi/ebx 组与槽位全等。解法 = P227 全表达式临时链（见函数尾注释）。
    if (user == 0)
        return 0x6b6;
    if (user->getCurCharacR() == 0)
        return 0x6b9;

    arad::SigAradEmblemCompound* sig = (arad::SigAradEmblemCompound*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6be, "EMBLEM COMPOUND INTER_DSP(kEmblemCompound) null.");
        return 0x6bf;
    }

    // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Process 每次各自 Get()。
    if (!Singleton<EmblemCompoundServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<EmblemCompoundServer>::Get()->Process(user, sig))
        return 0;

    // [b28d-solo-Arad_InternalDispatcher_INTER] P227 全表达式临时链：ORIG 0x0819a199..0x0819a1d5 在 cMyTrace
    // ctor 前右起求 varargs（getCurCharacNo→%ebx、m_resultIdx→槽、m_slot1→%edi、
    // m_slot0→%esi）并跨 ctor 保活；具名局部形 4.4 全落栈槽（前测 4 槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x6cc, 0)(
        "EMBLEM COMPOUND : success [slot1:%d] [slot2:%d] [selectNo:%d] [resultIdx:%d] [charac_no:%d]",
        (unsigned int)sig->m_slot0, (unsigned int)sig->m_slot1,
        (unsigned int)sig->m_resultIdx, (unsigned int)user->getCurCharacNo());
    return 0;
}

// ============================================================================
// Arad_INTER_kAvatarConvert —— ORIG 0x0819a20e
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_8.inc"
{
    if (user == 0)
        return 0x6d8;
    if (user->getCurCharacR() == 0)
        return 0x6db;

    arad::SigAradAvatarConvert* sig = (arad::SigAradAvatarConvert*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6e0, "AVATAR CONVERT INTER_DSP(kAvatarConvert) null.");
        return 0x6e1;
    }

    // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Convert 每次各自 Get()。
    if (!Singleton<AvatarConvertServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<AvatarConvertServer>::Get()->Convert(user, sig))
        return 0;
    return 0;
}


// ============================================================================
// Arad_INTER_kAvatarRoulette —— ORIG 0x08198a9a
// ============================================================================
#include "Arad_InternalDispatcher_INTER/dispatch_sig_9.inc"
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] 封 N82/P211/P77（ORIG 0x08198a9a）：320 vs 322，残差 2 条 +
    // 全帧槽基 +4（属 N82/N95 槽复用/槽基差，指令一一对应）：
    // ①levelParam unsigned short 局部：4.4 恒 16 位 store+movzwl 回读 vs ORIG
    //   32 位槽直存（P77/P10 跨版本局部宽度；int/unsigned int 承载、去 cast 各试均劣）；
    // ②`picked.m_itemId`（unsigned int 字段→int 形参）4.4 多 1 条 regmove
    //   （ORIG 单条 mov -0x50(%ebp),%ebx；无 cast 形同，P211/N18a）。
    // ③AvatarCoin::Use 已由 Main 代落 bool（原 unsigned int ⇒ 调用点 setne 多 1 条，
    //   P292；323→322）。p167 47/47 CLEAR。
    // 真 bug 修复：①getCurCharacInvenRefW() 返回 CInventory*（巨类），裸 `+0x658`
    // 按元素缩放成 +0x30f740；ORIG 0x08198ca2 是字节偏移 add $0x658 ⇒ (char*) 化。
    // ②G_CEnvironment()+0x1b0 裸偏移改具名 m_serverEnvir.m_channelNo（ORIG 单条
    // mov 0x1b0(%eax),%eax）。③sig 改 arad::SigAvatarRoulette 具名成员。
    if (user == 0)
        return 0x3c8;

    arad::SigAvatarRoulette* sig = (arad::SigAvatarRoulette*)body;  // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3cd, "AVATAR ROULETTE INTER_DSP(SigAvatarRoulette) null.");
        return 0x3ce;
    }
    if (user->getCurCharacR() == 0)
        return 0x3d4;

    char dateKey[24];                                       // -0x44
    memset(dateKey, 0, sizeof(dateKey));                    // ORIG memset(buf,0,0x18)

    time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();   // -0x48
    tm* broken = localtime(&now);                           // -0x28

    OS_API::snprintf(dateKey, 0x14, "M%02d%02d%02d%02d%10d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (broken->tm_year + 1900) % 2000, broken->tm_mon + 1,
                     broken->tm_mday, (unsigned int)user->getCurCharacNo());

    AvatarRouletteServer::PickedAvatarInfo picked;
    // ORIG 0x08198bf3 直接 `mov <A>,%eax` 传 receiver（无 roulette 具名槽）。
    if (!((AvatarRouletteServer*)GlobalData::s_pAvatarRoulette)->Pickup(
            picked, sig->m_type, sig->m_slotIdx))
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3e7, "[kAvatarRoulette] Avatar Roulette Pickup failed. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x236, 1);
        return 0;
    }

    // gradeBand>1 -> 归约成档位 3；否则 -1（ORIG cmpb $0x1,%al; jle = signed char）。
    signed char decBand = (signed char)-1;                  // -0x21
    if (picked.m_gradeBand > 1)
        decBand = (signed char)3;

    // AvatarCoin 可用性判定(refW+0x658)，不可用则拒抽。
    // P48：ORIG 每次使用均重调 getCurCharacInvenRefW()+0x658，不许提升到局部。
    // [b28d-solo] 真 bug 修复：getCurCharacInvenRefW() 返回 CInventory*（巨类），
    // 裸 `+0x658` 按元素缩放成 +0x30f740；ORIG 0x08198ca2 为字节偏移 add $0x658。
    if (!((AvatarCoin*)((char*)user->getCurCharacInvenRefW() + 0x658))->IsUsable())
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3f9, "[kAvatarRoulette] need avatarCoin. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return 0;
    }

    int avatarUid = 0;                                      // -0x20
    if ((unsigned char)picked.m_zoneType == 3)
        avatarUid = 0;                                      // ORIG 重写同值，保留指令序
    // [M3S4 落地 · DIFF→AE，320=320] ORIG 0x08198d3d-0x08198d46 `movzbl -0x4b; cbtw;
    // movzwl %ax,%eax; mov %eax,-0x6c` = 32 位槽直存、无 16 位中继槽。旧形
    // `unsigned short levelParam` 在 4.4 下多一个 -0x1e 16 位中继槽（`mov %ax,-0x1e;
    // movzwl -0x1e,%eax; mov %eax,-0x6c`）⇒ 全帧槽基 +4（buf/sig/now/broken/picked/
    // packet 全体下移 4，decBand/avatarUid -0x21/-0x20→-0x25/-0x24）。
    // 实测：①`unsigned int` 32 位承载（`(unsigned short)` 截断）⇒ 323（-0x20 中继槽）；
    // ②删具名变量、把 `(unsigned short)(signed char)picked.m_pad9` **内联进实参位**
    // ⇒ 320，槽集合与 ORIG 全等（-0x1c/-0x20/-0x21/-0x28/-0x2c/-0x44/-0x48..-0x4c/
    // -0x50/-0x54/-0x60/-0x6c/-0x70），TU 四数 diff 2→1、ae 20→21。
    // ORIG 0x08198d57 先装实参到 %edi/%esi/%ebx 再 call getCurCharacInvenW（无槽）。
    int slot = user->getCurCharacInvenW()->AddAvatarItem(
        picked.m_itemId, avatarUid, false,
        (char)picked.m_optionOrSkillIdx, (char)decBand,
        dateKey, (eAvatarItemAddReason)0x15, false,
        (unsigned short)(signed char)picked.m_pad9);

    if (slot == -1)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x407, "[kAvatarRoulette] no space in inventory. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 4);
        return 0;
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1, slot);

    if (!((AvatarCoin*)((char*)user->getCurCharacInvenRefW() + 0x658))->Use())
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x410, "[kAvatarRoulette] need avatarCoin. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return 0;
    }

    AvatarCoin::HistoryLog::SubLog(user, 1);
    AvatarCoin::SendSyncPacket(user);

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24a);
    packet.put_byte(1);
    packet.put_short((int)(short)slot);
    packet.put_byte((int)(signed char)picked.m_optionOrSkillIdx);
    packet.put_short((int)(signed char)picked.m_pad9);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

}  // namespace DISPATCHER

}  // namespace ARAD
