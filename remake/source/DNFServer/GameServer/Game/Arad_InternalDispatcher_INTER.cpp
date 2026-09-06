// ============================================================================
// df_game_r Game/ ARAD::DISPATCHER INTER 流具体派发器（dispatch_sig 家族）。
//
// 依据 docs/class_func_reports/ARAD__DISPATCHER__Arad_INTER_*/ 各报告逐指令转写：
//   Arad_INTER_kAvatarRoulette            0x08198a9a
//   Arad_INTER_kLoadRewardEventItem       0x08198fd0
//   Arad_INTER_kSaveServerState           0x08199278
//   Arad_INTER_kLoadServerState           0x081992fe
//   Arad_INTER_kDeleteServerState         0x0819935e
//   Arad_INTER_kAradJumpingCharacter      0x081999f2
//     + clearEpicQuest                    0x081993c4
//     + sendHelperBox                     0x0819973e
//   Arad_INTER_kAvatarHiddenOptionChange  0x08199d6e
//   Arad_INTER_kUseAvatarRecharge         0x08199f0e
//   Arad_INTER_kEmblemCompound            0x0819a0c0
//   Arad_INTER_kAvatarConvert             0x0819a20e
//
// 本 TU 为唯一实现点（Arad_InternalDispatcher.cpp 已删对应空定义）；
// ctor/dtor 空体仍由 Arad_InternalDispatcher.cpp 承载。
// 包体字段访问：ORIG 偏移直读（MoreTypes.h Sig 布局归 OnlyOrig_ARAD 批）。
// ============================================================================
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
AvatarRouletteServer::PickedAvatarInfo::PickedAvatarInfo()
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
int Arad_INTER_kLoadServerState::dispatch_sig(CUser* user, char* body, int len)
{
    // param_3 即 (arad::SigServerState*) 包指针复用位。
    char* packet = body;   // ORIG 用 body(char*@+0x10)；我们误 cast len(int@+0x14)
    if (packet == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x533, "SigServerState is null.");
        return 0x534;
    }
    GlobalData::s_serverStateManager_->insertState((arad::SigServerState*)packet);
    return 0;
}

// ============================================================================
// Arad_INTER_kSaveServerState —— ORIG 0x08199278
//   栈上构造 AradServerStateMessage{m_active=1, m_endSec=*(pkt+0x12)}，
//   insertState(*(ENUM_GLOBAL_EFFECT_ARAD*)(pkt+0xe), msg)。
// ============================================================================
int Arad_INTER_kSaveServerState::dispatch_sig(CUser* /*user*/, char* body, int len)
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
int Arad_INTER_kDeleteServerState::dispatch_sig(CUser* /*user*/, char* body, int len)
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
int Arad_INTER_kLoadRewardEventItem::dispatch_sig(CUser* user, char* /*body*/, int len)
{
    if (user == 0 || user->getCurCharacR() == 0)
        return 0x4d3;

    arad::SigLoadRewardEventItem* sig = (arad::SigLoadRewardEventItem*)len;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x4d8, "SigLoadRewardEventItem is null.");
        return 0x4d9;
    }

    Arad_EventPeriodDataManager* epdm =
        (Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
            std::string("Arad_EventPeriodDataManager"));
    epdm->addUserRewardData(user, *sig);

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    Arad_EventPeriodDataManager* epdm2 =
        (Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
            std::string("Arad_EventPeriodDataManager"));
    epdm2->eventReward<FnRewardLethe>((ENUM_REPEAT_EVENT_CODE)0x8f, curSec, user);

    CEventBase* repeat = GlobalData::s_event_manager->GetRepeatEvent(0x9b);
    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 NULL。
    if (repeat->IsEventing(0))
    {
        unsigned int curSec2 = GlobalData::s_systemTime_.getCurSec();
        Arad_EventPeriodDataManager* epdm3 =
            (Arad_EventPeriodDataManager*)Singleton<Arad_DataManager>::Get()->findGameScript(
                std::string("Arad_EventPeriodDataManager"));
        epdm3->eventReward<FnRewardMomoji>((ENUM_REPEAT_EVENT_CODE)0x9b, curSec2, user);
    }
    return 0;
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::clearEpicQuest —— ORIG 0x081993c4
//   第一遍扫角色 20 槽 in-progress 任务（等级达标即结清+发奖）；第二遍遍历
//   QuestList::getQuestNPCList(GRADE 0) 的 multimap value 对应任务，排除转职
//   0xe04..0xe08 且职业匹配、未清者结清+发奖。返回是否有任一结清。
// ============================================================================
void Arad_INTER_kAradJumpingCharacter::clearEpicQuest(CUser* user)
{
    bool anyCleared = false;                                // -0x29

    QuestList* npcQuestList = G_CDataManager()->m_questList;  // mgr+0x18
    std::multimap<int, int>* npcMap =
        npcQuestList ? npcQuestList->getQuestNPCList((ENUM_QUEST_GRADE)0) : 0;
    if (npcMap == 0)
        return;

    int level = user->get_charac_level();    // -0x20

    for (int i = 0; i < 0x14; ++i)                          // -0x1c
    {
        UserQuest* questW = user->getCurCharacQuestW();
        if (questW->m_quest[i] == 0)                        // base+8+(i+0x1d4c)*4
            continue;
        UserQuest* questR = user->getCurCharacQuestR();
        Quest* quest = G_CDataManager()->find_quest(questR->m_quest[i]);
        if (quest != 0 && quest->m_type == 0)
        {
            int r0 = 0, r1 = 0, r2 = 0, r3 = 0;             // -0x34/-0x38/-0x3c/-0x40
            if (quest->m_levelMin < level)                  // +0x20
            {
                WongWork::CQuestClear* clearR =
                    &user->getCurCharacQuestR()->m_cleared;
                clearR->setClearedQuest((unsigned int)questR->m_quest[i]);
                UserQuest* questW2 = user->getCurCharacQuestW();
                questW2->m_quest[i] = 0;                    // 进行区清零
                UserQuest* questW3 = user->getCurCharacQuestW();
                questW3->m_trigger[i] = 0;                  // trigger 区清零(+0x1d60 段)
                user->quest_basic_reward(quest, r3, r2, r1, r0, true);
                anyCleared = true;
            }
        }
    }

    // 第二遍：NPC list 对应任务族（Ghidra clearedCount 与 anyCleared 同步自增）。
    for (std::multimap<int, int>::iterator it = npcMap->begin(); it != npcMap->end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        if (quest == 0)
            continue;
        if (quest->m_index == 0xe04 || quest->m_index == 0xe05 ||
            quest->m_index == 0xe06 || quest->m_index == 0xe07 ||
            quest->m_index == 0xe08)
            continue;
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;
        {
            int r0 = 0, r1 = 0, r2 = 0, r3 = 0;             // -0x48/-0x4c/-0x50/-0x54
            if (quest->m_levelMin < level)
            {
                UserQuest* questW = user->getCurCharacQuestW();
                WongWork::CQuestClear* clear = &questW->m_cleared;
                if (!clear->isClearedQuest((unsigned int)quest->m_index))
                {
                    clear->setClearedQuest((unsigned int)quest->m_index);
                    user->quest_basic_reward(quest, r3, r2, r1, r0, true);
                    anyCleared = true;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// sendHelperBox —— ORIG 0x0819973e：按 ticket 类型取标题/正文脚本串，经
// AradJumping_Script 取奖励物品后 create_item 并 ReqDBSendNewSystemMail 投递。
// ----------------------------------------------------------------------------
bool Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser* user,
                                                     unsigned char ticketType,
                                                     unsigned char growValue)
{
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

    int job = user->get_charac_job();
    unsigned long itemId = 0;
    bool got = G_CDataManager()->m_aradJumping.getCurRewardItem(job, rewardIndex, itemId);
    if (got)
    {
        CItemList* itemList = G_CDataManager()->getItemList();
        itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
        item.set_add_info(1);                               // item+0x07 = 1
        ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
        unsigned long bodyLen = strlen(body);
        unsigned long characNo = (unsigned long)user->getCurCharacNo();
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 100000, characNo, body, (int)bodyLen, 0, group, false, false);
    }
    else
    {
        unsigned int jobU = (unsigned int)user->get_charac_job();
        unsigned int characNo = (unsigned int)user->get_charac_no(-1);
        unsigned int accId = (unsigned int)user->get_acc_id();
        LogManager::logFormat
            (1, "localjapan/Arad_InterDispatcher.cpp",
             __PRETTY_FUNCTION__,
             0x5d5,
             "JUMPING CHARACTER : REWARD ERROR - m_id:%u, charac_no:%u, tickettype:%d, job:%d, growValue:%d",
             accId, characNo, (unsigned int)ticketType, jobU, (unsigned int)rewardIndex);
    }
    return got ? true : false;
}

// ============================================================================
// Arad_INTER_kAradJumpingCharacter::dispatch_sig —— ORIG 0x081999f2
// ============================================================================
int Arad_INTER_kAradJumpingCharacter::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x5e9;
    if (user->getCurCharacR() == 0)
        return 0x5ee;

    unsigned char* sig = (unsigned char*)len;               // SigAradJumpingCharacter*
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x5f3, "JUMPING_CHARACTER INTER_DSP(SigAradJumpingCharacter) null.");
        return 0x5f4;
    }

    // 跳级券：delete_item(INVEN 1, slot=*(ushort*)(sig+0xb), 1, reason 3, true)
    unsigned short slot = *(unsigned short*)(sig + 0xb);
    CInventory* inventory = user->getCurCharacInvenW();
    bool deleted = (inventory->delete_item((INVEN_TYPE)1, slot, 1,
                                           (eItemDelReason)3, true) == 1);
    if (!deleted)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x13);
        return 0;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);

    // 升到 19→20 区间（<=0x13 循环自增）。
    while (user->get_charac_level() <= 0x13)
        user->SetCharacLevel(user->get_charac_level() + 1);

    // 一次成长（growValue=sig[10]），二段成长（sig[9]==2 时两次跳段）。
    int firstGrowTarget = (int)(unsigned char)sig[0xa];     // -0x14
    int secondGrowFlag = 0;                                 // -0x10
    user->ChangeGrowType_GM(firstGrowTarget, secondGrowFlag);
    if ((char)sig[9] == 2)
    {
        while (user->get_charac_level() <= 0x31)
            user->SetCharacLevel(user->get_charac_level() + 1);
        secondGrowFlag = 1;
        int curFirstGrow = (int)user->getCurCharFirstGrowType();
        user->ChangeGrowType_GM(curFirstGrow, secondGrowFlag);
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

    sendHelperBox(user, sig[9], sig[0xa]);

    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
    user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    return 0;
}

// ============================================================================
// Arad_INTER_kAvatarHiddenOptionChange —— ORIG 0x08199d6e
// ============================================================================
int Arad_INTER_kAvatarHiddenOptionChange::dispatch_sig(CUser* user, char* body, int len)
{
    int result = 0x664;                                       // ORIG ebx 出口寄存
    do {
        if (user == 0)
            break;

        arad::SigAvatarHiddenOptionChange* sig = (arad::SigAvatarHiddenOptionChange*)len;
        if (sig == 0)
        {
            LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                                  0x669, "AVATAR HIDDENOPTION CHANGE INTER_DSP(SigAvatarHiddenOptionChange) null.");
            result = 0x66a;
            break;
        }
        if (user->getCurCharacR() == 0)
        {
            result = 0x670;
            break;
        }

        AvatarFixedHiddenOptionServer* server =
            Singleton<AvatarFixedHiddenOptionServer>::Get();
        if (!server->CheckCondition(user, sig))
            break;
        if (!server->ChangeHiddenOption(user, sig))
            break;
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x24c);
        packet.put_byte(1);
        packet.put_short((int)*(short*)((char*)sig + 2));
        packet.put_short((unsigned int)*(unsigned short*)((char*)sig + 4));
        packet.finalize(true);
        user->Send(packet);
    } while (0);
    return result;
}

// ============================================================================
// Arad_INTER_kUseAvatarRecharge —— ORIG 0x08199f0e
// ============================================================================
int Arad_INTER_kUseAvatarRecharge::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x68e;
    if (user->getCurCharacR() == 0)
        return 0x691;

    arad::SigAradUseAvatarRecharge* sig = (arad::SigAradUseAvatarRecharge*)len;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x696, "AVATAR USE AVATAR RECHARGE INTER_DSP(SigAradUseAvatarRecharge) null.");
        return 0x697;
    }

    AvatarRechargeServer* server = Singleton<AvatarRechargeServer>::Get();
    if (server->CheckConditionUseRecharge(user, sig))
    {
        if (server->Recharge(user, sig))
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x24d);
            packet.put_byte(1);
            packet.put_int(*(int*)((char*)sig + 0xc));
            packet.put_int(*(int*)((char*)sig + 4));
            packet.put_short((unsigned int)*(unsigned short*)sig);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    return 0;
}

// ============================================================================
// Arad_INTER_kEmblemCompound —— ORIG 0x0819a0c0
// ============================================================================
int Arad_INTER_kEmblemCompound::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x6b6;
    if (user->getCurCharacR() == 0)
        return 0x6b9;

    arad::SigAradEmblemCompound* sig = (arad::SigAradEmblemCompound*)len;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6be, "EMBLEM COMPOUND INTER_DSP(kEmblemCompound) null.");
        return 0x6bf;
    }

    EmblemCompoundServer* server = Singleton<EmblemCompoundServer>::Get();
    if (server->CheckCondition(user, sig))
    {
        if (server->Process(user, sig))
        {
            unsigned int resultIdx = (unsigned int)sig->m_resultIdx;
            unsigned int selectNo = (unsigned int)sig->m_slot1;
            unsigned int slot1 = (unsigned int)sig->m_slot0;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x6cc, 0);
            trace("EMBLEM COMPOUND : success [slot1:%d] [slot2:%d] [selectNo:%d] [resultIdx:%d] [charac_no:%d]",
                  slot1, selectNo, resultIdx,
                  (unsigned int)user->getCurCharacNo());
        }
    }
    return 0;
}

// ============================================================================
// Arad_INTER_kAvatarConvert —— ORIG 0x0819a20e
// ============================================================================
int Arad_INTER_kAvatarConvert::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x6d8;
    if (user->getCurCharacR() == 0)
        return 0x6db;

    arad::SigAradAvatarConvert* sig = (arad::SigAradAvatarConvert*)len;
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6e0, "AVATAR CONVERT INTER_DSP(kAvatarConvert) null.");
        return 0x6e1;
    }

    AvatarConvertServer* server = Singleton<AvatarConvertServer>::Get();
    if (server->CheckCondition(user, sig))
    {
        server->Convert(user, sig);          // ORIG Check 条件后 Convert，两路均返 0
    }
    return 0;
}


// ============================================================================
// Arad_INTER_kAvatarRoulette —— ORIG 0x08198a9a
// ============================================================================
int Arad_INTER_kAvatarRoulette::dispatch_sig(CUser* user, char* body, int len)
{
    if (user == 0)
        return 0x3c8;

    unsigned char* sig = (unsigned char*)len;               // SigAvatarRoulette*
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

    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    OS_API::snprintf(dateKey, 0x14, "M%02d%02d%02d%02d%10d",
                     *(const int*)((const char*)G_CEnvironment() + 0x1b0),
                     (broken->tm_year + 1900) % 2000, broken->tm_mon + 1,
                     broken->tm_mday, characNo);

    AvatarRouletteServer* roulette =
        (AvatarRouletteServer*)GlobalData::s_pAvatarRoulette;
    AvatarRouletteServer::PickedAvatarInfo picked;
    if (!roulette->Pickup(picked, sig[0], sig[1]))
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3e7, "[kAvatarRoulette] Avatar Roulette Pickup failed. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x236, 1);
        return 0;
    }

    // gradeBand>1 -> 归约成档位 3；否则 -1。
    signed char decBand = (signed char)-1;                  // -0x21
    if ((unsigned char)picked.m_gradeBand > (unsigned char)1)
        decBand = (signed char)3;

    // AvatarCoin 可用性判定(refW+0x658)，不可用则拒抽。
    AvatarCoin* coin =
        (AvatarCoin*)(user->getCurCharacInvenRefW() + 0x658);
    if (!coin->IsUsable())
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
    unsigned short levelParam =                             // -0x6c
        (unsigned short)(int)(signed char)picked.m_pad9;

    CInventory* inventory = user->getCurCharacInvenW();
    int slot = inventory->AddAvatarItem(
        (int)picked.m_itemId, avatarUid, false,
        (char)picked.m_optionOrSkillIdx, (char)decBand,
        dateKey, (eAvatarItemAddReason)0x15, false, levelParam);

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

    if (!coin->Use())
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
