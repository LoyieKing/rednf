// ============================================================================
// df_game_r 还原 —— W6_TailB：DisPatcher_DebugCommand::_debugCommand* 家族
//
// 26 个方法（SetLevel 已由 DispDebugJoint4.cpp 提供）转写自
// docs/class_func_reports/DisPatcher_DebugCommand/<fn>.md（## 汇编 + ## 反编译 C）。
// 声明在 DisPatcherCore.h class DisPatcher_DebugCommand 内（纯追加）。
//
// ORIG 地址与 mangled：
//   _debugCommandOn              T 0x0858e724 _ZN23DisPatcher_DebugCommand15_debugCommandOnEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandOff             T 0x0858e918 _ZN23DisPatcher_DebugCommand16_debugCommandOffEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandAddItem         T 0x0858ec9c _ZN23DisPatcher_DebugCommand20_debugCommandAddItemEP5CUsermi
//   _debugCommandVisible         T 0x085906ac _ZN23DisPatcher_DebugCommand20_debugCommandVisibleEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandSetSkill        T 0x0858eab8 _ZN23DisPatcher_DebugCommand21_debugCommandSetSkillEP5CUserii
//   _debugCommandAddAvatar       T 0x0858f7b6 _ZN23DisPatcher_DebugCommand22_debugCommandAddAvatarEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandInvisible       T 0x0859058a _ZN23DisPatcher_DebugCommand22_debugCommandInvisibleEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandPowerUpOn       T 0x0858ffee _ZN23DisPatcher_DebugCommand22_debugCommandPowerUpOnEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandUserRepel       T 0x0859048e _ZN23DisPatcher_DebugCommand22_debugCommandUserRepelEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandClearInven      T 0x0859019e _ZN23DisPatcher_DebugCommand23_debugCommandClearInvenEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandClearSkill      T 0x0858e9c4 _ZN23DisPatcher_DebugCommand23_debugCommandClearSkillEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandPVPRankSet      T 0x0858faba _ZN23DisPatcher_DebugCommand23_debugCommandPVPRankSetEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandRequestMid      T 0x08590506 _ZN23DisPatcher_DebugCommand23_debugCommandRequestMidEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandSetEnchant      T 0x0858fb4e _ZN23DisPatcher_DebugCommand23_debugCommandSetEnchantEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandLevelUpDown     T 0x0858f3ba _ZN23DisPatcher_DebugCommand24_debugCommandLevelUpDownEP5CUseri
//   _debugCommandPowerWarEnd     T 0x0859044e _ZN23DisPatcher_DebugCommand24_debugCommandPowerWarEndEP5CUser
//   _debugCommandSetGrowType     T 0x0858ee54 _ZN23DisPatcher_DebugCommand24_debugCommandSetGrowTypeEP5CUserii
//   _debugCommandGuildLevelUp    T 0x0858f64c _ZN23DisPatcher_DebugCommand25_debugCommandGuildLevelUpEP5CUser
//   _debugCommandCargoCapacity   T 0x0858fdc8 _ZN23DisPatcher_DebugCommand26_debugCommandCargoCapacityEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandClearAvtInven   T 0x08590254 _ZN23DisPatcher_DebugCommand26_debugCommandClearAvtInvenEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandPowerWarStart   T 0x0859040e _ZN23DisPatcher_DebugCommand26_debugCommandPowerWarStartEP5CUser
//   _debugCommandSetBlackCount   T 0x0858fcc4 _ZN23DisPatcher_DebugCommand26_debugCommandSetBlackCountEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandSetUserDefine   T 0x085908ba _ZN23DisPatcher_DebugCommand26_debugCommandSetUserDefineEP5CUserR9PacketBuf
//   _debugCommandMasterNewSkill  T 0x0858ebcc _ZN23DisPatcher_DebugCommand27_debugCommandMasterNewSkillEP5CUserii
//   _debugCommandPowerwarPointUp T 0x0858fe62 _ZN23DisPatcher_DebugCommand28_debugCommandPowerwarPointUpEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandUserPowerwarPointUp T 0x0858fee8 _ZN...32_debugCommandUserPowerwarPointUpEP5CUserR17MSG_DEBUG_COMMAND
//   _debugCommandInitUserPowerwarPoint T 0x08590092 _ZN...34_debugCommandInitUserPowerwarPointEP5CUserR17MSG_DEBUG_COMMAND
// ============================================================================

#include <time.h>
#include <string.h>
#include <vector>

#include "DisPatcherCore.h"     // DisPatcher_DebugCommand（含 MSG_BASE/PacketBuf）
#include "CUser.h"              // CUser/CUserCharacInfo（PvpResultType/get_*）
#include "CDataManager.h"       // G_CDataManager/find_item/is_stackable
#include "CInventory.h"         // CInventory/GetInvenRef/m_pEquipSlot/OS_API::snprintf/NumberToString
#include "Inven_Item.h"         // Inven_Item（m_addInfo/m_addInfo2/reset）
#include "CItem.h"              // CItem::IsAvatarItem(虚槽 vtbl+0x10)/getUsablePeriod
#include "CStackableItem.h"     // CStackableItem::getStackableLimit（ORIG W 0x0822c9fc；is_stackable 守卫后下行转型真调用）
#include "GameWorld.h"          // G_GameWorld/send_all/send_to_area/SendPowerUpList
#include "PacketGuard.h"
#include "LogManager.h"         // LogManager::logFormat
#include "GameTypes.h"          // cMyTrace
#include "GlobalData.h"         // s_systemTime_/s_guild_proxy_mgr/s_monitor_proxy_mgr/s_ref_pvp_grade
#include "RefPvpGrade.h"        // RefPvpGrade::GetPvpNextRankPoint
#include "CHandlePremium.h"     // WongWork::CHandlePremium::handleSetUserPCRoom
#include "CEnvironment.h"       // G_CEnvironment()->m_serverEnvir.m_channelNo(+0x1b0)
#include "CGuildServerProxy.h"  // STGuildProxy/SendGuildLevelup/SendPowerWar{Start,End,Point}/ModifyGuildInfoProxy/SendTcpPacket
#include "CMonitorServerProxy.h" // SendRequestUserMid/SendUserRepel
#include "CServerProxyMgr.h"    // CServerProxyMgr<T>::GetServerProxy

namespace OS_API { int snprintf(char* buf, unsigned int n, const char* fmt, ...); }

#include "Packet_Guild_Change_Power_War_Point.h"
#include "Packet_Guild_Guild_Cargo_Upgrade.h"

// ---------------------------------------------------------------------------
// MSG_DEBUG_COMMAND —— 全局作用域 POD（ORIG mangled R17MSG_DEBUG_COMMAND；
// ---------------------------------------------------------------------------
// MSG_DEBUG_COMMAND —— 权威定义已提升至 DisPatcherCore.h（本 TU 直接使用头
// 内唯一共享定义，布局口径不变，消除跨 TU 重复定义风险）。
// ---------------------------------------------------------------------------

// stBuySkillInfo 完整定义（与 CUser.cpp / CEventCharacterHandler.cpp 同布局；
// CUser.h 仅前向声明）。MasterNewSkill 需构造三字节入参。
struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};

// ---------------------------------------------------------------------------
// b22 桥清零（P201 成员调用=栈参 shim 同形定理）：本 TU 原 extern"C" mangled
// 直连四桥中三处已转真成员调用——
//   SetCharacLevel   → user->SetCharacLevel(int)（CUser.h 权威 int 返回）
//   SetSkillLevel    → user->SetSkillLevel(int,int)（返回型 void→int 经 Main
//                      代落声明 + W7_UserOnlyC.cpp:457 定义同步，ORIG 尾
//                      mov eax,0x1 实证）
//   getStackableLimit→ static_cast<const CStackableItem*>(item)
//                      ->getStackableLimit()（is_stackable 虚函数守卫保证下行
//                      转型安全；ORIG 即以此 this 直调 CStackableItem 非虚成员）
//   ChangeGrowType_GM  → user->ChangeGrowType_GM(int,int)（Main 代落声明
//                      CUser.h:3330 int，定义 CUser.cpp:6572 同步；ORIG mangled
//                      _ZN5CUser17ChangeGrowType_GMEii 中 E 为嵌套名终结符、
//                      成员真名 ChangeGrowType_GM(17)）
// 本 TU 四桥全清。
// ---------------------------------------------------------------------------

// __PRETTY_FUNCTION__ / 日志字面串（照抄各报告原文）。
static const char g_dbgCmdPrettyOn[] =
    "bool DisPatcher_DebugCommand::_debugCommandOn(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyOff[] =
    "bool DisPatcher_DebugCommand::_debugCommandOff(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyClearSkill[] =
    "bool DisPatcher_DebugCommand::_debugCommandClearSkill(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettySetSkill[] =
    "bool DisPatcher_DebugCommand::_debugCommandSetSkill(CUser*, int, int)";
static const char g_dbgCmdPrettyMasterNewSkill[] =
    "bool DisPatcher_DebugCommand::_debugCommandMasterNewSkill(CUser*, int, int)";
static const char g_dbgCmdPrettyAddItem[] =
    "bool DisPatcher_DebugCommand::_debugCommandAddItem(CUser*, itemIndexOnlyServer_t, int)";
static const char g_dbgCmdPrettySetGrowType[] =
    "bool DisPatcher_DebugCommand::_debugCommandSetGrowType(CUser*, int, int)";
static const char g_dbgCmdPrettyLevelUpDown[] =
    "bool DisPatcher_DebugCommand::_debugCommandLevelUpDown(CUser*, int)";
static const char g_dbgCmdPrettyGuildLevelUp[] =
    "bool DisPatcher_DebugCommand::_debugCommandGuildLevelUp(CUser*)";
static const char g_dbgCmdPrettyAddAvatar[] =
    "bool DisPatcher_DebugCommand::_debugCommandAddAvatar(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyInvisible[] =
    "bool DisPatcher_DebugCommand::_debugCommandInvisible(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyVisible[] =
    "bool DisPatcher_DebugCommand::_debugCommandVisible(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyClearInven[] =
    "bool DisPatcher_DebugCommand::_debugCommandClearInven(CUser*, MSG_DEBUG_COMMAND&)";
static const char g_dbgCmdPrettyClearAvtInven[] =
    "bool DisPatcher_DebugCommand::_debugCommandClearAvtInven(CUser*, MSG_DEBUG_COMMAND&)";

// GM trace 通用形态：NumberToString(accId) + 角色名 两个固定前置实参。
#define DBG_TRACE(pretty, line, fmt)                            \
    do {                                                        \
        char* accStr = NumberToString(user->get_acc_id(), 0);   \
        cMyTrace trace((pretty), (line), 0);                    \
        trace((fmt), accStr, user->getCurCharacName());         \
    } while (0)

// ===========================================================================
// 1. _debugCommandOff —— 0x0858e918
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandOff(CUser* user,
                                               MSG_DEBUG_COMMAND& /*msg*/)
{
    bool isGm = user->isGMUser();               // +0x08 call isGMUser
    if (isGm == 1)
    {
        DBG_TRACE(g_dbgCmdPrettyOff, 0x41, "%s:'%s' GM Mode OFF.");
        user->SetGameMasterMode(false);         // +0x94 SetGameMasterMode(0)
    }
    return isGm == 1;
}

// ===========================================================================
// 2. _debugCommandOn —— 0x0858e724（无 GM 门禁，分级在 dispatch_sig）
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandOn(CUser* user,
                                              MSG_DEBUG_COMMAND& /*msg*/)
{
    DBG_TRACE(g_dbgCmdPrettyOn, 0x10, "%s:'%s' GM Mode ON.");   // +0x2b
    user->SetGameMasterMode(true);              // +0x7e SetGameMasterMode(1)

    if (user->getCurCharacR() == 0)             // +0x83 getCurCharacR==0 → ret 0
        return false;

    PacketGuard packet;                         // -0x24 local_28
    packet.put_header(0, 0xe);                  // +0xbc put_header(0,0xe)
    packet.put_byte(0);                         // +0xcf put_byte(0)
    packet.put_short(3);                        // +0xe2 put_short(3)
    if (!user->getCurCharacInvenR()->MakeItemPacket(            // +0x10c slot 1
            static_cast<INVEN_TYPE>(1), 1, packet))
        return false;
    if (!user->getCurCharacInvenR()->MakeItemPacket(            // +0x147 slot 0
            static_cast<INVEN_TYPE>(1), 0, packet))
        return false;
    if (!user->getCurCharacInvenR()->MakeItemPacket(            // +0x17f slot 2
            static_cast<INVEN_TYPE>(1), 2, packet))
        return false;
    packet.finalize(true);                      // +0x1a0 finalize(true)
    user->Send(packet);                         // +0x1b2 Send
    return true;                                // +0x1b7 ret 1
}

// ===========================================================================
// 3. _debugCommandClearSkill —— 0x0858e9c4
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandClearSkill(CUser* user,
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    bool isGm = user->isGMUser();               // +0x08
    if (isGm == 1)
    {
        DBG_TRACE(g_dbgCmdPrettyClearSkill, 0x8a, "%s:'%s' GM Mode Clear Skill.");
        if (user->ClearSkills())                // +0x92 ClearSkills；==0 → ret 0
        {
            user->send_skill_info();            // +0xa1
            if (user->get_state() == 5)         // +0xb1 state==5
                user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                     static_cast<ENUM_NOTIPACKET>(2), 1);
            return true;                        // +0xe7 ret 1
        }
        return false;
    }
    return false;
}

// ===========================================================================
// 4. _debugCommandSetSkill —— 0x0858eab8
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandSetSkill(CUser* user, int skillIdx,
                                                    int level)
{
    bool isGm = user->isGMUser();               // +0x08
    if (isGm != 1)
        return false;
    if (user->SetSkillLevel(skillIdx, level) == 0)   // +0x38；ORIG test al
        return false;

    char* accStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace trace(g_dbgCmdPrettySetSkill, 0xa2, 0);
    trace("%s:'%s' GM Mode Set Skill(%d) Level(%d).",
          accStr, user->getCurCharacName(), skillIdx, level);
    user->send_skill_info();                    // +0xbb
    if (user->get_state() == 5)                 // +0xd1
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
    return true;
}

// ===========================================================================
// 5. _debugCommandMasterNewSkill —— 0x0858ebcc
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandMasterNewSkill(CUser* user,
                                                          int skillIdx,
                                                          int level)
{
    bool isGm = user->isGMUser();               // +0x08
    if (isGm == 1)
    {
        stBuySkillInfo info;                    // -0x1b 栈帧 3 字节
        info.m_field0 = 0;                      // +0x34 movb $0
        info.m_skillIdx = (char)skillIdx;       // +0x28 mov al
        info.m_count = (char)level;             // +0x31 mov al
        user->master_new_skill(info, false);    // +0x49 master_new_skill(info,0)
        char* accStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(g_dbgCmdPrettyMasterNewSkill, 0xbd, 0);
        trace("%s:'%s' GM Mode Set Skill(%d) Level(%d).",
              accStr, user->getCurCharacName(), skillIdx, level);
    }
    return isGm == 1;
}

// ===========================================================================
// 6. _debugCommandAddItem —— 0x0858ec9c
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandAddItem(CUser* user,
                                                   unsigned long itemIdxOnlyServer,
                                                   int count)
{
    bool isGm = user->isGMUser();               // +0x08
    if (isGm != 1)
        return false;

    if ((int)itemIdxOnlyServer == -1)           // +0x24 cmpl $-1 → PC 房一天高级会员
    {
        long secA = GlobalData::s_systemTime_.getCurSec();   // +0x36 getCurSec
        long secB = GlobalData::s_systemTime_.getCurSec();   // +0x43 再调一次（ORIG 如此）
        WongWork::CHandlePremium::handleSetUserPCRoom(
            user, secB, secA + 0x15180);                     // end=当前秒+86400
    }
    if (count < 1)                              // +0x5b count<1 → 100
        count = 100;

    CItem* item = G_CDataManager()->find_item((int)itemIdxOnlyServer);  // +0x77
    if (item != 0)                              // +0x7f
    {
        if (item->is_stackable() &&                              // +0x85 is_stackable
            static_cast<const CStackableItem*>(item)
                    ->getStackableLimit() < count)
        {
            // +0xaf 取上限
            count = static_cast<const CStackableItem*>(item)
                        ->getStackableLimit();
        }
        if (item->IsAvatarItem())               // +0xbf vtbl+0x10 虚槽；avatar 类禁加
            return false;
    }

    ENUM_ITEMSPACE outSpace = static_cast<ENUM_ITEMSPACE>(0);   // -0x24 local_28
    int slot = user->AddItem((int)itemIdxOnlyServer, count,
                             static_cast<eItemAddReason>(6), outSpace, 0);  // +0xff reason6
    if (slot < 0)                               // +0x10a js
        return false;

    char* accStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace trace(g_dbgCmdPrettyAddItem, 0xf4, 0);
    trace("%s:'%s' GM Mode Add Item(%d).", accStr, user->getCurCharacName(), slot);
    user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                             outSpace, slot);   // +0x19f SendUpdateItemList(1,space,slot)
    return true;
}

// ===========================================================================
// 7. _debugCommandSetGrowType —— 0x0858ee54
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandSetGrowType(CUser* user, int kind,
                                                       int value)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;
    DBG_TRACE(g_dbgCmdPrettySetGrowType, 0x133, "%s:'%s' GM Mode Grow Type.");

    int firstGrow = 0;                          // -0x10
    int secondGrow = 0;                         // -0xc
    if (kind == 1)
    {
        firstGrow = value;
    }
    else if (kind == 2)
    {
        if ((char)user->getCurCharFirstGrowType() == 0)   // +0xae getCurCharFirstGrowType
            return false;
        firstGrow = (int)(char)user->getCurCharFirstGrowType();  // +0xd2 再调一次（ORIG 如此）
        secondGrow = value;
    }
    else
    {
        return false;                           // +0xe5 其余 kind 直接 ret 0
    }

    if (user->ChangeGrowType_GM(firstGrow, secondGrow) == 0)  // +0xef test al
        return false;

    if (user->get_state() == 3)                 // +0x117
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                             static_cast<ENUM_NOTIPACKET>(2), 0);
    else if (user->get_state() == 5)            // +0x11f
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
    user->send_skill_info();                    // +0x171
    return true;
}

// ===========================================================================
// 8. _debugCommandLevelUpDown —— 0x0858f3ba
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandLevelUpDown(CUser* user, int delta)
{
    if (user->isGMUser() != 1)
        return false;
    DBG_TRACE(g_dbgCmdPrettyLevelUpDown, 0x1ce, "%s:'%s' GM Mode Level Up.");

    if (delta * delta >= 2)                     // +0x93 imul/jle：仅 |delta|<=1 通过
        return false;

    bool changed = true;                        // -0x9 local_d
    int candA = user->get_charac_level() + delta;   // +0xb1 lv+delta
    int candB = 200;                            // +0xbe $200
    // ORIG min<int>(&lv+delta, &$200) 取较小者 → SetCharacLevel
    int newLevel = std::min(candB, candA);
    if (user->SetCharacLevel(newLevel) != 1)   // +0xdc xor $1/test al 判成功
        changed = false;
    if (changed)
    {
        int state = user->get_state();
        if (state == 3)                         // +0x101 村庄态
        {
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(0x25), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                                 static_cast<ENUM_NOTIPACKET>(2), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            PacketGuard questPacket;            // -0x30
            user->getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&questPacket));
            user->Send(questPacket);
        }
        else if (state == 5)                    // +0x109 战场态
        {
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(0x25), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            PacketGuard questPacket;            // -0x3c
            user->getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&questPacket));
            user->Send(questPacket);
        }
    }
    return changed ? 1 : 0;                     // +0x286 movzbl local_d
}

// ===========================================================================
// 9. _debugCommandGuildLevelUp —— 0x0858f64c
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandGuildLevelUp(CUser* user)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    if (user->get_charac_guildkey() == 0)       // +0x25 guildkey==0
    {
        cMyTrace trace(g_dbgCmdPrettyGuildLevelUp, 0x20a, 0);
        trace("GM is not Guild Member!( %d )", user->getCurCharacNo());
        return false;
    }

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));                 // +0xc1 GetServerProxy(...,0)
    proxy->SendGuildLevelup((int)user->GetUID(),
                            (unsigned int)user->getCurCharacNo(),
                            (unsigned int)user->get_charac_guildkey(), 0);  // +0xdd

    char* dbInfo = reinterpret_cast<char*>(user->GetGuildDBInfo());  // +0xe2（raw DB blob，
                                                                     // 沿用仓库裸读先例）
    STGuildProxy proxyInfo(dbInfo,                              // ctor(name,lev,powerSide)
                           (unsigned char)(dbInfo[0x1b] + 1),   // 等级 +1
                           (unsigned char)dbInfo[0x95]);
    proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0));                     // +0x128 再取一次
    proxy->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(),
                                proxyInfo);                     // +0x158 ModifyGuildInfoProxy
    return true;
}

bool DisPatcher_DebugCommand::_debugCommandAddAvatar(CUser* user,
                                                     MSG_DEBUG_COMMAND& msg)
{
    if (user->isGMUser() != 1)
        return false;

    char dateKey[0x18];                                         // -0x4c
    memset(dateKey, 0, sizeof(dateKey));                        // +0x28 memset 0x18
    time_t now = time(0);                                       // +0x43 time(NULL)
    struct tm broken;                                           // -0x80
    struct tm* p = localtime_r(&now, &broken);                  // +0x55 localtime_r
    // env+0x1b0 = Server_Envir(+0x198).m_channelNo(+0x18)
    OS_API::snprintf(dateKey, 0x14, "D%02d%02d%02d%02d%010d",
                     *(reinterpret_cast<const int*>(
                         reinterpret_cast<const char*>(G_CEnvironment()) + 0x1b0)),
                     (p->tm_year + 0x76c) % 2000,               // +0xa3 (yr+1900)%2000
                     p->tm_mon + 1,
                     p->tm_mday,
                     (unsigned int)user->getCurCharacNo());     // %010d

    CItem* item = G_CDataManager()->find_item((int)msg.m_arg);  // +0x106 find_item(msg[0xe])
    if (!(item != 0 && item->IsAvatarItem()))                   // +0x114 vtbl+0x10 虚槽
        return false;

    int usablePeriod = item->getUsablePeriod();                 // +0x145 thunk
    cMyTrace trace(g_dbgCmdPrettyAddAvatar, 0x231, 0);
    trace("[DEBUG_COMMAND_ADD_AVATAR] Param.Sub.AddAvatar.period : %d, "
          "item->getUsablePeriod() : %d",
          (unsigned int)(unsigned char)msg.payload.avatar.periodKind,
          usablePeriod);

    int slot;
    if (msg.payload.avatar.periodKind == 0)                     // +0x1a3 分支一
    {
        slot = user->getCurCharacInvenW()->AddAvatarItem(       // +0x221
            (int)msg.m_arg,
            item->getUsablePeriod(),                            // 分支一重新调用
            /*b*/ false,
            /*c*/ (char)msg.payload.avatar.slotByte,            // sbyte 槽位口径
            /*d*/ (char)-1,
            /*agency*/ dateKey,
            static_cast<eAvatarItemAddReason>(9),
            /*e*/ false,
            /*f*/ msg.payload.avatar.count);                    // u16 数量
    }
    else                                                        // +0x22e 分支二
    {
        if (msg.payload.avatar.periodKind == 0x63)              // 哨兵 99 → 0
            msg.payload.avatar.periodKind = 0;
        slot = user->getCurCharacInvenW()->AddAvatarItem(       // +0x2b5
            (int)msg.m_arg,
            (int)msg.payload.avatar.periodKind,
            /*b*/ false,
            /*c*/ (char)msg.payload.avatar.slotByte,
            /*d*/ (char)-1,
            /*agency*/ dateKey,
            static_cast<eAvatarItemAddReason>(9),
            /*e*/ false,
            /*f*/ msg.payload.avatar.count);                    // u16 数量
    }

    if (slot == -1)                                             // +0x2bd
        return false;
    user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_ITEMSPACE>(1), slot);
    return true;
}

// ===========================================================================
// 11. _debugCommandPVPRankSet —— 0x0858faba
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPVPRankSet(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    unsigned int rank = (unsigned int)(unsigned char)msg.m_arg; // +0x22 movzbl [0xe]
    if (rank > 0x25)                                            // jbe → ≤0x25 通过
        return false;

    int nextPoint = GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint((int)rank);
    PvpResultType* result = user->getPVPResultW();
    result->m_expPoint = nextPoint;                             // +0x5a 写 +8
    result = user->getPVPResultW();
    result->m_pvpGrade = rank;                                  // +0x72 写 +0x14
    G_GameWorld()->send_AllBasicInfo(user);                     // +0x87
    return true;
}

// ===========================================================================
// 12. _debugCommandSetEnchant —— 0x0858fb4e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandSetEnchant(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;
    if (user->getCurCharacR() == 0)             // +0x2e getCurCharacR!=0 才续
        return false;

    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_arg);       // +0x42 itemspace→type
    Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(
        invenType, msg.payload.ench.slotInt);                   // +0x6c GetInvenRef(space,slot)
    if (ref == 0)                                               // +0x74
        return false;
    ref->m_fieldd = msg.payload.ench.enchantVal;                // +0x86 写 +0xd 整体 int

    PacketGuard packet;                                         // -0x1c
    packet.put_header(0, 0xe);                                  // +0xad header(0,0xe)
    packet.put_byte(msg.m_arg);                                 // +0xc2 put_byte(itemspace)
    packet.put_short(1);                                        // +0xdf put_short(1)
    if (user->getCurCharacInvenR()->MakeItemPacket(             // +0x100 MakeItemPacket
            static_cast<INVEN_TYPE>(invenType),
            msg.payload.ench.slotInt, packet))
    {
        packet.finalize(true);                                  // +0x133 finalize
        user->Send(packet);                                     // +0x14d Send
        return true;
    }
    return false;                                               // 失败析构后返 0
}

// ===========================================================================
// 13. _debugCommandInvisible —— 0x0859058a
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandInvisible(CUser* user,
                                                     MSG_DEBUG_COMMAND& /*msg*/)
{
    user->SetInvisible();                                       // +0x0e SetInvisible()
    PacketGuard packet;                                         // -0x24
    packet.put_header(0, 6);                                    // +0x34 header(0,6)
    packet.put_short(user->get_unique_id() & 0xffff);           // +0x51 put_short(uid&0xffff)
    packet.finalize(true);                                      // +0x64 finalize
    G_GameWorld()->send_all_excepted_user(packet, user);        // +0x7f 广播排除自己
    user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),    // +0xa2 SendNotiPacket(1,2,0)
                         static_cast<ENUM_NOTIPACKET>(2), 0);
    char* name = const_cast<char*>(user->getCurCharacName());
    cMyTrace trace(g_dbgCmdPrettyInvisible, 0x5c, 0);
    trace("GM(%s) is invisible \n", name);
    return true;
}

// ===========================================================================
// 14. _debugCommandVisible —— 0x085906ac
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandVisible(CUser* user,
                                                   MSG_DEBUG_COMMAND& /*msg*/)
{
    user->SetVisible();                                         // +0x0e SetVisible()
    if (user->get_state() == 3)                                 // +0x21 村庄态才发提示
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                             static_cast<ENUM_NOTIPACKET>(2), 0);

    PacketGuard packet;                                         // -0x24
    packet.put_header(0, 0x17);                                 // +0x6c header(0,0x17)
    packet.put_short(user->get_unique_id() & 0xffff);           // +0x89 uid&0xffff
    packet.put_byte(user->getCurCharacVill());                  // +0xa6 vill
    packet.put_byte(user->get_area(false));                     // +0xc8 get_area(false)
    packet.put_short(user->get_posX() & 0xffff);                // +0xf5 posX
    packet.put_short(user->get_posY() & 0xffff);                // +0xf8 posY
    packet.put_byte(user->get_direction());                     // +0x115 direction
    packet.put_byte(user->get_charac_visible_values());         // +0x139 visible values
    packet.finalize(true);                                      // +0x14f finalize
    int area = user->get_area(false);
    int vill = (int)(char)user->getCurCharacVill();
    G_GameWorld()->send_to_area(vill, area, packet);            // +0x18e send_to_area
    char* name = const_cast<char*>(user->getCurCharacName());
    cMyTrace trace(g_dbgCmdPrettyVisible, 0x83, 0);
    trace("GM(%s) is visible \n", name);
    return true;
}

// ===========================================================================
// 15. _debugCommandSetBlackCount —— 0x0858fcc4
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandSetBlackCount(CUser* user,
                                                         MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    user->setCurrCharacBlackCount((unsigned int)msg.m_arg);     // +0x34 setBlackCount(msg[0xe])
    PacketGuard packet;                                         // -0x14
    packet.put_header(0, 2);                                    // +0x5a header(0,2)
    packet.put_byte(0);                                         // +0x6d put_byte(0)
    packet.put_short(1);                                        // +0x80 put_short(1)
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');  // +0x9a make_basic_info(buf,0)
    packet.finalize(true);                                      // +0xad finalize
    G_GameWorld()->send_all(packet, user);                      // +0xc8 send_all(packet,user)
    return true;
}

// ===========================================================================
// 16. _debugCommandPowerwarPointUp —— 0x0858fe62
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerwarPointUp(CUser* user,
                                                           MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    Packet_Guild_Change_Power_War_Point pkt;                    // ctor(0x6a4,0x2d)
    pkt.m_winPoint = (unsigned char)msg.m_arg;                  // pkt+0x2b ← msg[0xe] 低字节
    pkt.m_losePoint = 0;                                        // pkt+0x2c = 0
    pkt.m_powerSide = user->getPowerSide();                     // pkt+0x0a ← getPowerSide
    pkt.m_characNo[0] = user->get_charac_no(-1);                // pkt+0x0b dword ← charac_no(-1)
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPowerWarPoint(pkt);                              // +0x7a SendPowerWarPoint
    return true;
}

// ===========================================================================
// 17. _debugCommandUserPowerwarPointUp —— 0x0858fee8
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandUserPowerwarPointUp(
    CUser* user, MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    user->IncreasePowerWarPoint((short)msg.m_arg);              // +0x37 Increase(short(msg[0xe]))
    PacketGuard packet;                                         // -0x14
    packet.put_header(0, 2);                                    // +0x5d header(0,2)
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');  // +0x9d make_basic_info(buf,0)
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);                      // +0xcb send_all(packet,user)
    return true;
}

// ===========================================================================
// 18. _debugCommandInitUserPowerwarPoint —— 0x08590092（清零个人 Power War 点）
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandInitUserPowerwarPoint(
    CUser* user, MSG_DEBUG_COMMAND& /*msg*/)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    short cur = user->GetUserPowerWarPoint();                   // +0x27 GetUserPowerWarPoint
    user->DecreasePowerWarPoint(cur);                           // +0x3c Decrease(cur)
    PacketGuard packet;                                         // -0x14
    packet.put_header(0, 2);                                    // +0x5c header(0,2)
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);                      // +0xd0 send_all(packet,user)
    return true;
}

// ===========================================================================
// 19. _debugCommandClearInven —— 0x0859019e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandClearInven(CUser* user,
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    // 主背包槽 9..0x137 全清零（equip 动态槽数组 +0x650 = m_pEquipSlot）
    for (int i = 9; i <= 0x137; ++i)
    {
        CInventory* invW = user->getCurCharacInvenW();
        invW->m_pEquipSlot[i].reset();                          // Inven_Item::reset stride 0x3d
    }
    bool ok = user->send_itemspace(0) == 1 ? true : false;      // +0x43 send_itemspace(INVENTORY=0)
    if (!ok)
    {
        LogManager::logFormat(
            1, "DisPatcher_DebugCommand_First.h",
            g_dbgCmdPrettyClearInven, 0x3af,
            "DisPatcher_DebugCommand::_debugCommandClearInven, pUser(%s)->"
            "send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
    }
    return ok;
}

// ===========================================================================
// 20. _debugCommandClearAvtInven —— 0x08590254
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandClearAvtInven(CUser* user,
                                                         MSG_DEBUG_COMMAND& /*msg*/)
{
    int i = 0;                                                  // -0xc
    do {
        if (0x68 < i)                                           // 槽 0..0x68 结束
        {
            bool ok = user->send_itemspace(1) == 1;             // +0x150 AVATAR=1
            if (!ok)
            {
                LogManager::logFormat(
                    1, "DisPatcher_DebugCommand_First.h",
                    g_dbgCmdPrettyClearAvtInven, 0x417,
                    "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
                    "send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
                    NumberToString(user->get_acc_id(), 0));
            }
            return ok;
        }
        CInventory* invR = user->getCurCharacInvenW();
        if (invR->m_pAvatarSlot[i].m_addInfo != 0)              // +0x32 读 item+2
        {
            int avatarUid = invR->m_pAvatarSlot[i].m_addInfo2;  // +0x5a 读 item+7
            CInventory* invW = user->getCurCharacInvenW();
            bool ok = invW->delete_item(                        // +0x92 delete(2,i,1,reason3,true)
                           static_cast<INVEN_TYPE>(2), i, 1,
                           static_cast<eItemDelReason>(3), true) == 1;
            if (!ok)
            {
                LogManager::logFormat(
                    1, "DisPatcher_DebugCommand_First.h",
                    g_dbgCmdPrettyClearAvtInven, 0x40c,
                    "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
                    "getCurCharacInvenW()->delete_item return FAILED",
                    NumberToString(user->get_acc_id(), 0));
                return false;
            }
            invW->SendAvatarEvent(0x36, avatarUid, 1, i + 0xa, 0);  // +0x12a
        }
        ++i;
    } while (true);
}

// ===========================================================================
// 21. _debugCommandCargoCapacity —— 0x0858fdc8
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandCargoCapacity(CUser* user,
                                                         MSG_DEBUG_COMMAND& msg)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    Packet_Guild_Guild_Cargo_Upgrade pkt;                       // ctor(0x713,0x1e)
    pkt.m_guildKey = user->get_charac_guildkey();               // pkt+0xa
    pkt.m_charNo = user->getCurCharacNo();                      // pkt+0xe
    pkt.m_curCapacity = user->GetGuildCargoCapacity();          // pkt+0x12 当前容量
    pkt.m_newCapacity = msg.m_arg;                              // pkt+0x16 新容量=msg[0xe]
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         pkt.packetSize);                       // size = *(u16*)(pkt+2)
    return true;
}

// ===========================================================================
// 22. _debugCommandRequestMid —— 0x08590506
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandRequestMid(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (msg.m_arg <= 0x1d)                                      // +0x12 len ≤ 29
    {
        char* name = msg.payload.nameBuf;                       // buf 起点 +0x12
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());                        // +0x57 GetServerProxy(group)
        proxy->SendRequestUserMid(user->get_acc_id(), user->GetUID(),
                                  name, msg.m_arg);             // +0x72
    }
    return true;
}

// ===========================================================================
// 23. _debugCommandUserRepel —— 0x0859048e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandUserRepel(CUser* user,
                                                     MSG_DEBUG_COMMAND& msg)
{
    char* name = msg.payload.nameBuf;                           // +0x15 buf 起点 +0x12
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup());                            // +0x4a GetServerProxy(group)
    proxy->SendUserRepel(user->get_acc_id(), user->GetUID(),
                         name, msg.m_arg);                      // +0x65 SendUserRepel
    return true;
}

// ===========================================================================
// 24. _debugCommandPowerUpOn —— 0x0858ffee
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerUpOn(CUser* user,
                                                     MSG_DEBUG_COMMAND& /*msg*/)
{
    bool isGm = user->isGMUser();
    if (isGm != 1)
        return false;

    user->SetPowerUp(true);                                     // +0x2f SetPowerUp(1)
    std::vector<CUser*> targets;                                // -0x14 局部 vector
    targets.push_back(user);                                    // +0x4c push_back(user)
    G_GameWorld()->SendPowerUpList((ENUM_POWER_UP_ACTIVATE_TYPE)1, targets);
    return true;
}

// ===========================================================================
// 25. _debugCommandPowerWarStart —— 0x0859040e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerWarStart(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPowerWarStart(accId);
    return true;
}

// ===========================================================================
// 26. _debugCommandPowerWarEnd —— 0x0859044e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerWarEnd(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPowerWarEnd(accId);
    return true;
}

// ===========================================================================
// 27. _debugCommandSetUserDefine —— 0x085908ba：ORIG je → 返回 1（user==0 → 1；非空 → 0）。
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandSetUserDefine(CUser* user,
                                                         PacketBuf& packet)
{
    // P190：ORIG sub $0x140 撑帧（无其它局部）；保留等价栈预留。
    char framePad[0x140];   // P190 帧尺寸撑到 0x140
    (void)framePad;
    // ORIG 0x085908d0：成员函数 ABI 下 `cmpl $0,0xc(%ebp)` 即 user 参数；
    // `je <mov$1>` → user==0 返 1（P56 双分支 mov$0/mov$1 形态）。
    if (user != 0)
        return false;
    return true;
}

// ===========================================================================
// LinkFinal2 核销（/tmp/undef_r26.txt）：
// 28. _Z19DoUserDefineCommandP5CUseriPKc @0x0820ba90 (0xd8) —— objdump 逐指令转写。
//     CParser 栈对象；m_user=param；sprintf(m_buf,"%d %s",cmd,arg)（0x8bc2c30）；
//     parse()（ORIG 直接 call 非虚派发）；GetCommand(m_id) → SetParser → HandleRequest。
//     ORIG 返回值未定义（尾 ret 无 eax 写）；调用方不使用返回值，此处返 0。
// ===========================================================================
#include "GameMasterCmd.h"      // game_master::{CParser,CCommand,CGameMasterMgr}

int DoUserDefineCommand(CUser* user, int cmd, const char* arg)
{
    game_master::CParser parser;
    parser.m_user = user;                                        // ORIG -0x130 = parser+0x04
    sprintf(parser.m_buf, "%d %s", cmd, arg);                    // ORIG 0x820bad4
    parser.parse();                                              // ORIG 0x820bae2 直接 call
    game_master::CCommand* command =
        GlobalData::s_GameMasterMgr->GetCommand(parser.m_id);    // ORIG 0x820baf9（m_id=parser+0x18）
    if (command != 0)
    {
        command->SetParser(&parser);                             // ORIG 0x820bb17
        GlobalData::s_GameMasterMgr->HandleRequest(command);     // ORIG 0x820bb2b
    }
    return 0;
}

// ===========================================================================
// 29. _Z28SendVillageAttackedGMCommandP5CUseri @0x0820bb68 (0x76) —— objdump 逐指令转写。
//     Packet_VillageAttackedGMCommand 栈包（0x1a）；gcc 将包内字段寻址为栈局部：
//     -0x18=+0x0a uid、-0x14=+0x0e characNo、-0xc=+0x16 state；
//     GetServerProxy(user->GetServerGroup())->SendTcpPacket(&pkt, 0x1a)。
// ===========================================================================
#include "Packet_VillageAttackedGMCommand.h"

void SendVillageAttackedGMCommand(CUser* user, int state)
{
    Packet_VillageAttackedGMCommand pkt;                         // ORIG 0x820bb75
    pkt.m_uid = user->GetUID();                                  // ORIG 0x820bb80（-0x18）
    pkt.m_characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));      // ORIG 0x820bb96（get_charac_no(0xffffffff)）
    pkt.m_field16 = static_cast<unsigned int>(state);            // ORIG 0x820bba1（-0xc）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup());                             // ORIG 0x820bbbf（mgr 0x940be28）
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x1a);   // ORIG 0x820bbd3
}
