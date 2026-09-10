// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DisPatcher_DebugCommand_cmds/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// P227 全表达式临时链：ORIG 在 cMyTrace ctor 前右起求 varargs 并以 callee-saved
// 保活（name→%esi、accStr→%ebx），具名 accStr 局部/trace 对象均致槽物化
// （b28d-S129 逐件 diff 实证：Off/ClearSkill/SetGrowType/LevelUpDown/AddItem）。
#define DBG_TRACE(pretty, line, fmt)                            \
    do {                                                        \
        cMyTrace((pretty), (line), 0)(                          \
            (fmt), NumberToString(user->get_acc_id(), 0),       \
            user->getCurCharacName());                          \
    } while (0)

// ===========================================================================
// 2. _debugCommandOn —— 0x0858e724（无 GM 门禁，分级在 dispatch_sig）
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：DBG_TRACE 全表达式链 + getCurCharacR
// `!=0` 正向包裹（ORIG setne 形；负极性 ==0 反产 sete）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandOn.inc"
                                              MSG_DEBUG_COMMAND& /*msg*/)
{
    DBG_TRACE(g_dbgCmdPrettyOn, 0x10, "%s:'%s' GM Mode ON.");   // +0x2b
    user->SetGameMasterMode(true);              // +0x7e SetGameMasterMode(1)

    // ORIG `test %eax,%eax; setne %al; test %al,%al; je <尾 mov $0>` ⇒ 非零才进主流程
    // （正向包裹，P222/P214①；负极性 `==0 return false` 反产 sete 形）。
    if (user->getCurCharacR() != 0)             // +0x83 getCurCharacR!=0
    {
        PacketGuard packet;                     // -0x24 local_28
        packet.put_header(0, 0xe);              // +0xbc put_header(0,0xe)
        packet.put_byte(0);                     // +0xcf put_byte(0)
        packet.put_short(3);                    // +0xe2 put_short(3)
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x10c slot 1
                static_cast<INVEN_TYPE>(1), 1, packet))
            return false;
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x147 slot 0
                static_cast<INVEN_TYPE>(1), 0, packet))
            return false;
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x17f slot 2
                static_cast<INVEN_TYPE>(1), 2, packet))
            return false;
        packet.finalize(true);                  // +0x1a0 finalize(true)
        user->Send(packet);                     // +0x1b2 Send
        return true;                            // +0x1b7 ret 1
    }
    return false;
}

// ===========================================================================
// 1. _debugCommandOff —— 0x0858e918
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 局部内联为 P168a① bool 裸形
// （ORIG `call isGMUser; xor $1; test %al; je`）+ DBG_TRACE 改 P227 全表达式临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandOff.inc"
                                               MSG_DEBUG_COMMAND& /*msg*/)
{
    // P168a①：ORIG `call isGMUser; xor $1; test %al; je <成功块>` ⇒ bool callee 裸形。
    if (!user->isGMUser())
        return false;
    DBG_TRACE(g_dbgCmdPrettyOff, 0x41, "%s:'%s' GM Mode OFF.");
    user->SetGameMasterMode(false);             // +0x94 SetGameMasterMode(0)
    return true;
}

// ===========================================================================
// 3. _debugCommandClearSkill —— 0x0858e9c4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：N93附「尾双 return 块物理序互换」
// 由 P223 goto 尾标签翻正——成功体尾 `goto ok;` + 块外 `return false;` +
// `ok: return true;` 复现 ORIG `je <尾 mov$0 块>` / `mov$1` 后置布局
// （P222 正向包裹的 goto 变体，探针 p3 实测）。P167 callsite CLEAR 10/10。
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearSkill.inc"
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    if (!user->isGMUser())                      // +0x08（P168a① 裸形）
        return false;
    DBG_TRACE(g_dbgCmdPrettyClearSkill, 0x8a, "%s:'%s' GM Mode Clear Skill.");
    if (user->ClearSkills())                    // +0x92 ClearSkills；==0 → ret 0
    {
        user->send_skill_info();                // +0xa1
        if (user->get_state() == 5)             // +0xb1 state==5
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // P223：成功体尾 jmp 岛 + 失败块前
    }
    return false;                               // +0xe0 失败块（mov$0）
ok:
    return true;                                // +0xe7 成功块（mov$1）
}

// ===========================================================================
// 4. _debugCommandSetSkill —— 0x0858eab8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：SetSkillLevel bool 化
// （P248 档①，Main 代落 CUser.h）+ P223 goto 尾标签翻尾块序（同 ClearSkill）；
// P167 callsite CLEAR 10/10。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetSkill.inc"
                                                    int level)
{
    if (!user->isGMUser())                      // +0x08
        return false;
    // ORIG `call SetSkillLevel; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹 + callee bool
    // （P248 档①：int callee 的一切调用点写法恒产 sete 四段）。
    if (user->SetSkillLevel(skillIdx, level))   // +0x38
    {
        cMyTrace(g_dbgCmdPrettySetSkill, 0xa2, 0)(
            "%s:'%s' GM Mode Set Skill(%d) Level(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(),
            skillIdx, level);
        user->send_skill_info();                // +0xbb
        if (user->get_state() == 5)             // +0xd1
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // P223：成功体尾 jmp 岛 + 失败块前
    }
    return false;
ok:
    return true;
}

// ===========================================================================
// 5. _debugCommandMasterNewSkill —— 0x0858ebcc
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 内联（P168a①）+ P227 临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandMasterNewSkill.inc"
                                                          int skillIdx,
                                                          int level)
{
    if (!user->isGMUser())                      // +0x08
        return false;
    stBuySkillInfo info;                        // -0x1b 栈帧 3 字节
    info.m_field0 = 0;                          // +0x34 movb $0
    info.m_skillIdx = (char)skillIdx;           // +0x28 mov al
    info.m_count = (char)level;                 // +0x31 mov al
    user->master_new_skill(info, false);        // +0x49 master_new_skill(info,0)
    cMyTrace(g_dbgCmdPrettyMasterNewSkill, 0xbd, 0)(
        "%s:'%s' GM Mode Set Skill(%d) Level(%d).",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacName(),
        skillIdx, level);
    return true;
}

// ===========================================================================
// 6. _debugCommandAddItem —— 0x0858ec92
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 4.4.6 后端 %edx（与 ORIG 同形）；4.4.7 下恒 %eax 才是
// NEAR（P230 补）。已修：slot>=0 正向包裹翻尾块序（P222）、outSpace 删预清（P171）、
// getCurSec 双调内联实参、`&&` 拆嵌套 if（P225 推广）。P167 CLEAR 16/16。
#include "DisPatcher_DebugCommand_cmds/_debugCommandAddItem.inc"
                                                   unsigned long itemIdxOnlyServer,
                                                   int count)
{
    if (!user->isGMUser())                      // +0x08
        return false;

    if ((int)itemIdxOnlyServer == -1)           // +0x24 cmpl $-1 → PC 房一天高级会员
    {
        // P227 内联实参：ORIG 两次 getCurSec 结果直入 %ebx/%eax，无具名局部。
        WongWork::CHandlePremium::handleSetUserPCRoom(
            user, GlobalData::s_systemTime_.getCurSec(),     // +0x43 再调一次（ORIG 如此）
            GlobalData::s_systemTime_.getCurSec() + 0x15180);  // end=当前秒+86400
    }
    if (count < 1)                              // +0x5b count<1 → 100
        count = 100;

    CItem* item = G_CDataManager()->find_item((int)itemIdxOnlyServer);  // +0x77
    if (item != 0)                              // +0x7f
    {
        // P225 推广：ORIG 两条件各自 setcc 物化（is_stackable 直判 + setl），
        // `&&` 恒折直跳 ⇒ 拆嵌套 if。
        if (item->is_stackable())                                // +0x85 is_stackable
        {
            if (static_cast<const CStackableItem*>(item)
                    ->getStackableLimit() < count)
            {
                // +0xaf 取上限
                count = static_cast<const CStackableItem*>(item)
                            ->getStackableLimit();
            }
        }
        if (item->IsAvatarItem())               // +0xbf vtbl+0x10 虚槽；avatar 类禁加
            return false;
    }

    ENUM_ITEMSPACE outSpace;                    // -0x24 local_28（ORIG 无预清）
    int slot = user->AddItem((int)itemIdxOnlyServer, count,
                             static_cast<eItemAddReason>(6), outSpace, 0);  // +0xff reason6
    // ORIG +0x10a `js <尾 mov$0 块>`：成功条件正向包裹，失败块后置（P222/P59）。
    if (slot >= 0)
    {
        cMyTrace(g_dbgCmdPrettyAddItem, 0xf4, 0)("%s:'%s' GM Mode Add Item(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), slot);
        user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                 outSpace, slot);   // +0x19f SendUpdateItemList(1,space,slot)
        return true;
    }
    return false;
}

// ===========================================================================
// 7. _debugCommandSetGrowType —— 0x0858ee54
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 内联 + ChangeGrowType_GM bool 化
// （Main 代落）+ get_state 判定改 switch（P251，if/else-if 链产 sete 反形）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetGrowType.inc"
                                                       int value)
{
    if (!user->isGMUser())
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

    // ORIG `call ChangeGrowType_GM; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹 + callee bool。
    if (user->ChangeGrowType_GM(firstGrow, secondGrow))  // +0xef
    {
        // ORIG `call get_state; cmp $3; je; cmp $5; je; jmp` 无槽直比链 ⇒ switch
        // （P251；if/else-if 链在 4.4 产 sete 物化）。
        switch (user->get_state())              // +0x117
        {
        case 3:
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                                 static_cast<ENUM_NOTIPACKET>(2), 0);
            break;
        case 5:                                 // +0x11f
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            break;
        default:
            break;
        }
        user->send_skill_info();                // +0x171
        return true;
    }
    return false;
}

// ===========================================================================
// 8. _debugCommandLevelUpDown —— 0x0858f3ba
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：ORIG 的 -0x14/-0x10 是 std::min
// 的两个**匿名临时槽**（表达式右值 + 常量 200 物化），不是具名局部——写具名
// candA/candB 会让 4.4 把取址标量排到深槽（-0x20/-0x24）、trace 占浅槽 -0x1c；
// 改 `std::min(200, get_charac_level()+delta)` 内联后槽位/寄存器全等（P0 右值内联）。
// SetCharacLevel bool 化（Main）+ get_state switch（P251）。P167 callsite CLEAR 29/29。
#include "DisPatcher_DebugCommand_cmds/_debugCommandLevelUpDown.inc"
{
    if (!user->isGMUser())
        return false;
    DBG_TRACE(g_dbgCmdPrettyLevelUpDown, 0x1ce, "%s:'%s' GM Mode Level Up.");

    if (delta * delta >= 2)                     // +0x93 imul/jle：仅 |delta|<=1 通过
        return false;

    bool changed = true;                        // -0x9 local_d
    // P0：ORIG 的 -0x14/-0x10 是 min 的**匿名临时槽**（表达式右值 + 常量物化），
    // 无具名局部（具名局部会占据取址槽并改变 4.4 分配序）。
    if (user->SetCharacLevel(std::min(200, user->get_charac_level() + delta)) != 1)
        changed = false;
    if (changed)
    {
        // ORIG `call get_state; cmp $3; je; cmp $5; je; jmp` 无槽直比链 ⇒ switch（P251）。
        switch (user->get_state())
        {
        case 3:                                 // +0x101 村庄态
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
            break;
        }
        case 5:                                 // +0x109 战场态
        {
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(0x25), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            PacketGuard questPacket;            // -0x3c
            user->getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&questPacket));
            user->Send(questPacket);
            break;
        }
        default:
            break;
        }
    }
    return changed ? 1 : 0;                     // +0x286 movzbl local_d
}

// ===========================================================================
// 9. _debugCommandGuildLevelUp —— 0x0858f64c
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] NEAR（N7 封顶，双后端一致）：
// callee-saved 角色互换——ORIG 主线 edi/esi/ebx（guildkey/characNo/UID）+
// STGuildProxy 段 ecx/edx；4.4.7 与 4.4.6 后端均 edi/ebx/esi + edx/eax
// （非路由可解；callee 侧 cast 旋钮双向试过零变化）。已修：链式 receiver 双处
// （P52）+ proxyInfo.m_agitFlag=0 补写 + GuildDbInfoView 具名偏移（消 add→disp）。
// P167 callsite CLEAR 15/15。
#include "DisPatcher_DebugCommand_cmds/_debugCommandGuildLevelUp.inc"
{
    if (!user->isGMUser())
        return false;

    if (user->get_charac_guildkey() == 0)       // +0x25 guildkey==0
    {
        // P227：ORIG 先求 getCurCharacNo（%ebx）再 ctor ⇒ 全表达式临时链。
        cMyTrace(g_dbgCmdPrettyGuildLevelUp, 0x20a, 0)(
            "GM is not Guild Member!( %d )", user->getCurCharacNo());
        return false;
    }

    // P52 链式 receiver：ORIG 实参先物化（%edi/%esi/%ebx）后 GetServerProxy，
    // 具名 proxy 局部反致落槽 + 实参寄存器角色漂移。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))                      // +0xc1 GetServerProxy(...,0)
        ->SendGuildLevelup(user->GetUID(),
                           user->getCurCharacNo(),
                           user->get_charac_guildkey(), 0);  // +0xdd

    char* dbInfo = reinterpret_cast<char*>(user->GetGuildDBInfo());  // +0xe2（raw DB blob，
                                                                     // 沿用仓库裸读先例）
    // §4.3 具名视图（本 TU 私有，零新符号）：ORIG 用 disp 形
    // `movzbl 0x95(%eax),%eax`/`movzbl 0x1b(%eax),%eax`，裸 char* 下标在 4.4
    // 产 add+解引用（N107 族）⇒ 视图成员访问折叠回 disp。
    struct GuildDbInfoView
    {
        char m_pad00[0x1b];             // +0x00..0x1a
        unsigned char m_level;          // +0x1b
        char m_pad1c[0x95 - 0x1c];      // +0x1c..0x94
        unsigned char m_powerSide;      // +0x95
    };
    STGuildProxy proxyInfo(dbInfo,                              // ctor(name,lev,powerSide)
                           (unsigned char)(((GuildDbInfoView*)dbInfo)->m_level + 1),  // 等级 +1
                           (unsigned char)((GuildDbInfoView*)dbInfo)->m_powerSide);
    proxyInfo.m_agitFlag = 0;   // ORIG 0x0858f78c `movb $0x0,-0x2d(%ebp)`（+0x19）
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))                      // +0x128 再取一次
        ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(),
                               proxyInfo);                      // +0x158 ModifyGuildInfoProxy
    return true;
}

// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 %edx 与 ORIG 同形（见 AddItem 判词）。已修：
// `slot != -1` 正向包裹翻失败出口块序（P222）、usablePeriod 内联（P227）、
// G_CEnvironment 具名字段。P167 callsite CLEAR 18/18。
#include "DisPatcher_DebugCommand_cmds/_debugCommandAddAvatar.inc"
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
                     G_CEnvironment()->m_serverEnvir.m_channelNo,  // +0x1b0（具名字段）
                     (p->tm_year + 0x76c) % 2000,               // +0xa3 (yr+1900)%2000
                     p->tm_mon + 1,
                     p->tm_mday,
                     (unsigned int)user->getCurCharacNo());     // %010d

    CItem* item = G_CDataManager()->find_item((int)msg.m_arg);  // +0x106 find_item(msg[0xe])
    // ORIG `mov $1; jmp; mov $0; test; je <ret0>` ⇒ 正条件包裹（P214①）。
    if (item != 0 && item->IsAvatarItem())                      // +0x114 vtbl+0x10 虚槽
    {
        int slot = 0;                                           // ORIG +0x13a movl $0,-0x1c
        // P227：ORIG 先求 usablePeriod(%ebx)/periodKind(%esi) 再 ctor。
        cMyTrace(g_dbgCmdPrettyAddAvatar, 0x231, 0)(
            "[DEBUG_COMMAND_ADD_AVATAR] Param.Sub.AddAvatar.period : %d, "
            "item->getUsablePeriod() : %d",
            (unsigned int)(unsigned char)msg.payload.avatar.periodKind,
            item->getUsablePeriod());                           // +0x145 thunk

        if (msg.payload.avatar.periodKind == 0)                 // +0x1a3 分支一
        {
            slot = user->getCurCharacInvenW()->AddAvatarItem(   // +0x221
                (int)msg.m_arg,
                item->getUsablePeriod(),                        // 分支一重新调用
                /*b*/ false,
                /*c*/ (char)msg.payload.avatar.slotByte,        // sbyte 槽位口径
                /*d*/ (char)-1,
                /*agency*/ dateKey,
                static_cast<eAvatarItemAddReason>(9),
                /*e*/ false,
                /*f*/ msg.payload.avatar.count);                // u16 数量
        }
        else                                                    // +0x22e 分支二
        {
            if (msg.payload.avatar.periodKind == 0x63)          // 哨兵 99 → 0
                msg.payload.avatar.periodKind = 0;
            slot = user->getCurCharacInvenW()->AddAvatarItem(   // +0x2b5
                (int)msg.m_arg,
                (int)msg.payload.avatar.periodKind,
                /*b*/ false,
                /*c*/ (char)msg.payload.avatar.slotByte,
                /*d*/ (char)-1,
                /*agency*/ dateKey,
                static_cast<eAvatarItemAddReason>(9),
                /*e*/ false,
                /*f*/ msg.payload.avatar.count);                // u16 数量
        }

        // ORIG +0x2c1 `je <尾 mov$0 块>`：成功条件正向包裹，失败块后置
        // （P222/P59；早返形 4.4 恒把 mov$0 放就地、成功块后置）。
        if (slot != -1)
        {
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                     static_cast<ENUM_ITEMSPACE>(1), slot);
            return true;
        }
        return false;
    }
    return false;
}

// ===========================================================================
// 11. _debugCommandPVPRankSet —— 0x0858faba
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPVPRankSet.inc"
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    // §4.3 具名视图（本 TU 私有，零新符号）：ORIG @0x0858faba 三处均
    // `movzbl 0xe(%eax)` ⇒ +0x0e 按 **字节** 访问（int m_arg 形产 4 字节 mov，
    // 实测异形）。共享头 MSG_DEBUG_COMMAND 的 int 口径由其它命令维持，
    // 此处仅按 ORIG 访存宽度建 pack(1) 字节视图，不动共享布局。
#pragma pack(push, 1)
    struct DbgCmdArgView
    {
        unsigned char m_head[0xe];   // +0x00..0x0d
        unsigned char m_argByte;     // +0x0e（ORIG movzbl 宽度实证）
    };
#pragma pack(pop)
    static_assert(offsetof(DbgCmdArgView, m_argByte) == 0x0e, "view drift");

    if (((DbgCmdArgView*)&msg)->m_argByte > 0x25)               // +0x22 字节比较
        return false;

    int nextPoint = GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(
        ((DbgCmdArgView*)&msg)->m_argByte);
    user->getPVPResultW()->m_expPoint = nextPoint;              // +0x5a 写 +8（getPVPResultW 直用）
    user->getPVPResultW()->m_pvpGrade =
        ((DbgCmdArgView*)&msg)->m_argByte;                      // +0x72 写 +0x14 二次独立读取
    G_GameWorld()->send_AllBasicInfo(user);                     // +0x87
    return true;
}

// ===========================================================================
// 12. _debugCommandSetEnchant —— 0x0858fb4e
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] N104/N93附封顶（EH 帧/寄存器策略）：
// 删段实验证明——去掉 PacketGuard 段后 4.4 与 ORIG 完全同形（push %ebx/sub 0x24、
// invenType -0x10、slotInt→%ebx）；带 PacketGuard EH landing 时 4.4 恒多用
// %esi/%edi 并把标量槽加深 0x10（帧 0x3c vs ORIG 0x30）。已修：getCurCharacR
// `!=0` 正向包裹（P222/P214①）。P167 callsite CLEAR 16/16。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetEnchant.inc"
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;
    // ORIG `test %eax,%eax; setne %al; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹
    // （P222/P214①；负极性 `==0 return false` 反产 sete）。
    if (user->getCurCharacR() != 0)             // +0x2e getCurCharacR!=0 才续
    {
        int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_arg);   // +0x42 itemspace→type
        Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(
            invenType, msg.payload.ench.slotInt);               // +0x6c GetInvenRef(space,slot)
        if (ref != 0)                                           // +0x74
        {
            ref->m_fieldd = msg.payload.ench.enchantVal;        // +0x86 写 +0xd 整体 int

            PacketGuard packet;                                 // -0x1c
            packet.put_header(0, 0xe);                          // +0xad header(0,0xe)
            packet.put_byte(msg.m_arg);                         // +0xc2 put_byte(itemspace)
            packet.put_short(1);                                // +0xdf put_short(1)
            // [b28e] ORIG 0x0858fc4e 调用点为 `call MakeItemPacket; xor $0x1,%eax;
            // test %al,%al; je 0858fc61`（否定式直推导数，P292/P248 档①）且失败
            // 分支是 `mov $0x0,%ebx; jmp 0858fca8`（早返 false 经 ebx 汇聚到
            // 析构入口）⇒ 原码是 **`if (!MakeItemPacket(…)) return false;` 早返
            // 形**；写 `if (MakeItemPacket(…)) {…; return true;}` 会反产
            // `test; je <失败>` 并把返回值物化到第二个 callee-saved（%esi/%edi，
            // 帧 0x30→0x3c、多 3 条）。
            if (!user->getCurCharacInvenR()->MakeItemPacket(    // +0x100 MakeItemPacket
                    static_cast<INVEN_TYPE>(invenType),
                    msg.payload.ench.slotInt, packet))
            {
                return false;
            }
            packet.finalize(true);                              // +0x133 finalize
            user->Send(packet);                                 // +0x14d Send
            return true;
        }
    }
    return false;                                               // 失败析构后返 0
}

// ===========================================================================
// 13. _debugCommandInvisible —— 0x0859058a
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandInvisible.inc"
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
    // P227 全表达式临时链：ORIG @0x0859065x 在 cMyTrace ctor 前右起求 varargs
    // 并以 %ebx 跨 ctor 保活；具名 name 局部（旧 register 缓存形）+ 具名 trace
    // 对象反使 packet 槽平移（b28c-lane12）。
    cMyTrace(g_dbgCmdPrettyInvisible, 0x5c, 0)(
        "GM(%s) is invisible \n", user->getCurCharacName());
    return true;
}

// ===========================================================================
// 14. _debugCommandVisible —— 0x085906ac
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：area/vill/name 全部内联实参
// （ORIG 无槽，P0/P183）+ P227 临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandVisible.inc"
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
    // ORIG area→%ebx、vill→%esi 均无槽 ⇒ 内联实参（P0/P183）；name 同（%ebx）。
    G_GameWorld()->send_to_area((int)(char)user->getCurCharacVill(),
                                user->get_area(false), packet); // +0x18e send_to_area
    cMyTrace(g_dbgCmdPrettyVisible, 0x83, 0)("GM(%s) is visible \n",
        const_cast<char*>(user->getCurCharacName()));
    return true;
}

// ===========================================================================
// 15. _debugCommandSetBlackCount —— 0x0858fcc4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：DbgCmdArgWordView
// 缺 pack(1) 使 m_argU 自然对齐到 +0x10（读到 msg+0x10 而非 ORIG `mov 0xe(%eax),%edx`
// 的 +0x0e）⇒ 补 pack(1)+static_assert，读偏移归位。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetBlackCount.inc"
                                                         MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    // §4.3 本 TU 私有 pack(1) 视图（零新符号、不动共享头）：ORIG @0x0859058a
    // 段为 `mov 0xe(%eax),%edx` 单段直落传 setCurrCharacBlackCount(unsigned int)
    // ⇒ 该处按 **unsigned int** 访问（P211 同型化；共享头 int 口径由
    // DisPatcherCore.cpp:13237/13252 的 `m_arg < 1`/`0 < m_arg` 有符号判定锁定，
    // 换头会把 jl/jle 翻成 jb/jbe，故在消费点侧建视图）。
    // pack(1) 必需：非 pack 时 m_argU 被自然对齐到 +0x10（实测读到 +0x10 而非
    // ORIG `mov 0xe(%eax),%edx` 的 +0x0e —— 真语义 bug）。
#pragma pack(push, 1)
    struct DbgCmdArgWordView { unsigned char m_head[0xe]; unsigned int m_argU; };
#pragma pack(pop)
    static_assert(offsetof(DbgCmdArgWordView, m_argU) == 0x0e, "view drift");
    user->setCurrCharacBlackCount(                              // +0x34 setBlackCount(msg[0xe])
        ((DbgCmdArgWordView*)&msg)->m_argU);
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
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerwarPointUp.inc"
                                                           MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    Packet_Guild_Change_Power_War_Point pkt;                    // ctor(0x6a4,0x2d)
    pkt.m_winPoint = (unsigned char)msg.m_arg;                  // pkt+0x2b ← msg[0xe] 低字节
    pkt.m_losePoint = 0;                                        // pkt+0x2c = 0
    pkt.m_powerSide = user->getPowerSide();                     // pkt+0x0a ← getPowerSide
    pkt.m_characNo[0] = user->get_charac_no(-1);                // pkt+0x0b dword ← charac_no(-1)
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendPowerWarPoint(pkt);  // +0x7a
    return true;
}

// ===========================================================================
// 17. _debugCommandUserPowerwarPointUp —— 0x0858fee8
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandUserPowerwarPointUp.inc"
    CUser* user, MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
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
#include "DisPatcher_DebugCommand_cmds/_debugCommandInitUserPowerwarPoint.inc"
    CUser* user, MSG_DEBUG_COMMAND& /*msg*/)
{
    if (!user->isGMUser())
        return false;

    user->DecreasePowerWarPoint(user->GetUserPowerWarPoint());  // +0x27/+0x3c 直扩实参
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
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearInven.inc"
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    // 主背包槽 9..0x137 全清零（equip 动态槽数组 +0x650 = m_pEquipSlot）
    for (int i = 9; i <= 0x137; ++i)
    {
        user->getCurCharacInvenW()->m_pEquipSlot[i].reset();    // Inven_Item::reset stride 0x3d
    }
    if (!user->send_itemspace(0))                               // +0x43 send_itemspace(INVENTORY=0)
    {
        LogManager::logFormat(
            1, "DisPatcher_DebugCommand_First.h",
            g_dbgCmdPrettyClearInven, 0x3af,
            "DisPatcher_DebugCommand::_debugCommandClearInven, pUser(%s)->"
            "send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
        return false;
    }
    return true;
}

// ===========================================================================
// 20. _debugCommandClearAvtInven —— 0x08590254
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearAvtInven.inc"
                                                         MSG_DEBUG_COMMAND& /*msg*/)
{
    // [b23S3C] GmCmds_B CClearAvatarInvenCmd::execute(0x084aafb0) 同族：
    // 尾检循环（ORIG 头仅 -0xc i 预清+jmp tail，`i<=0x68` setle 形）；
    // 两读位各独立 getCurCharacInvenR、delete/event 各独立 W（P48）；
    // delete_item 直测 xor$1 形（bool 返）；NumberToString 直落实参槽；
    // send_itemspace 循环外尾段（xor$1 形——Main 已代落 CUser.h bool）。
    int avatarUid;   // [b23S3C] 试：先声明使 4.4 槽序对齐 ORIG（i=-0xc/此=-0x10）
    for (int i = 0; i <= 0x68; ++i)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo != 0)
        {
            avatarUid =
                user->getCurCharacInvenR()->m_pAvatarSlot[i].m_addInfo2;
            if (!user->getCurCharacInvenW()->delete_item(
                    static_cast<INVEN_TYPE>(2), i, 1,
                    static_cast<eItemDelReason>(3), true))
            {
                LogManager::logFormat(
                    1, "DisPatcher_DebugCommand_First.h",
                    g_dbgCmdPrettyClearAvtInven, 0x40c,
                    "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
                    "getCurCharacInvenW()->delete_item return FAILED",
                    NumberToString(user->get_acc_id(), 0));
                return false;
            }
            user->getCurCharacInvenW()->SendAvatarEvent(0x36, avatarUid, 1,
                                                        i + 0xa, 0);
        }
    }
    if (user->send_itemspace(1) != 1)
    {
        LogManager::logFormat(
            1, "DisPatcher_DebugCommand_First.h",
            g_dbgCmdPrettyClearAvtInven, 0x417,
            "DisPatcher_DebugCommand::_debugCommandClearAvtInven, pUser(%s)->"
            "send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
        return false;
    }
    return true;
}

// ===========================================================================
// 21. _debugCommandCargoCapacity —— 0x0858fdc8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：派生类
// `static const unsigned int packetSize=0x14` 遮蔽 PacketHeader::packetSize
// 成员（+0x2，ctor 写 0x1e）⇒ 发送长度少 0xa 字节。改 `pkt.PacketHeader::packetSize`
// 后复现 ORIG `movzwl -0x24(%ebp),%eax`；receiver 改链式（P52）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandCargoCapacity.inc"
                                                         MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    Packet_Guild_Guild_Cargo_Upgrade pkt;                       // ctor(0x713,0x1e)
    pkt.m_guildKey = user->get_charac_guildkey();               // pkt+0xa
    pkt.m_charNo = user->getCurCharacNo();                      // pkt+0xe
    pkt.m_curCapacity = user->GetGuildCargoCapacity();          // pkt+0x12 当前容量
    pkt.m_newCapacity = msg.m_arg;                              // pkt+0x16 新容量=msg[0xe]
    // P52 链式 receiver；长度取 PacketHeader 基类 ushort 成员（ORIG 0x0858fe3x
    // `movzwl -0x24(%ebp),%eax` 读 pkt+2=0x1e）；派生类同名 static const 遮蔽会
    // 折叠成立即数 0x14 —— 真语义 bug（少发 0xa 字节）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendTcpPacket(
            reinterpret_cast<char*>(&pkt),
            pkt.PacketHeader::packetSize);                      // size = *(u16*)(pkt+2)
    return true;
}

// ===========================================================================
// 22. _debugCommandRequestMid —— 0x08590506
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：len 具名局部落槽（ORIG -0x1c）、
// name 内联实参（ORIG %edi 无槽）+ 链式 receiver（P52）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandRequestMid.inc"
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (msg.m_arg <= 0x1d)                                      // +0x12 len ≤ 29
    {
        // ORIG：len 落槽 -0x1c（具名局部），name 落 %edi（内联实参无槽），
        // receiver 链式后置（P52）。
        int len = msg.m_arg;
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())                             // +0x57 GetServerProxy(group)
            ->SendRequestUserMid(user->get_acc_id(), user->GetUID(),
                                 msg.payload.nameBuf, len);      // +0x72
    }
    return true;
}

// ===========================================================================
// 23. _debugCommandUserRepel —— 0x0859048e
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：arg4/arg3 物化位置互换的钥匙 =
// `&msg.payload.nameBuf[0]`（取首元素地址 vs 数组退化改变 4.4 表达式树）——ORIG
// `mov 0xe(%eax),%edi`（m_arg→%edi）+ nameBuf 地址落槽 -0x1c；直接写 nameBuf 反致
// 两者互换（探针 r8/r9 实测）。P167 callsite CLEAR 5/5。
#include "DisPatcher_DebugCommand_cmds/_debugCommandUserRepel.inc"
                                                     MSG_DEBUG_COMMAND& msg)
{
    // ORIG：msg.m_arg 先求落 %edi，name 作内联实参由 4.1 落槽 -0x1c
    // （callee-saved 已被 %edi/%esi/%ebx 占满）；receiver 链式后置（P52）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())                                 // +0x4a GetServerProxy(group)
        ->SendUserRepel(user->get_acc_id(), user->GetUID(),
                        &msg.payload.nameBuf[0], msg.m_arg);    // +0x65 SendUserRepel
    return true;
}

// ===========================================================================
// 24. _debugCommandPowerUpOn —— 0x0858ffee
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerUpOn.inc"
                                                     MSG_DEBUG_COMMAND& /*msg*/)
{
    // P168a①：callee 返 bool ⇒ 一律裸形直产 `call; xor$1; testb; je`；
    // 具名 bool 局部反产槽物化两段（mov %al,-9 + movzbl 回读，b28c-lane12 实证）。
    if (!user->isGMUser())
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
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarStart.inc"
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPowerWarStart(user->get_acc_id());
    return true;
}

// ===========================================================================
// 26. _debugCommandPowerWarEnd —— 0x0859044e
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarEnd.inc"
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPowerWarEnd(user->get_acc_id());
    return true;
}

// ===========================================================================
// 27. _debugCommandSetUserDefine —— 0x085908ba：ORIG je → 返回 1（user==0 → 1；非空 → 0）。
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetUserDefine.inc"
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

#include "DisPatcher_DebugCommand_cmds/DoUserDefineCommand.inc"
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
    // ORIG 尾 ret 无 eax 写（返回值未定义，调用方不使用）——不写 return。
}

// ===========================================================================
// 29. _Z28SendVillageAttackedGMCommandP5CUseri @0x0820bb68 (0x76) —— objdump 逐指令转写。
//     Packet_VillageAttackedGMCommand 栈包（0x1a）；gcc 将包内字段寻址为栈局部：
//     -0x18=+0x0a uid、-0x14=+0x0e characNo、-0xc=+0x16 state；
//     GetServerProxy(user->GetServerGroup())->SendTcpPacket(&pkt, 0x1a)。
// ===========================================================================
#include "Packet_VillageAttackedGMCommand.h"

#include "DisPatcher_DebugCommand_cmds/SendVillageAttackedGMCommand.inc"
{
    Packet_VillageAttackedGMCommand pkt;                         // ORIG 0x820bb75
    pkt.m_uid = user->GetUID();                                  // ORIG 0x820bb80（-0x18）
    pkt.m_characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));      // ORIG 0x820bb96（get_charac_no(0xffffffff)）
    pkt.m_field16 = static_cast<unsigned int>(state);            // ORIG 0x820bba1（-0xc）
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->SendTcpPacket(                  // ORIG 0x820bbbf（&pkt 跨调用物化 ebx）
        reinterpret_cast<char*>(&pkt), 0x1a);                    // ORIG 0x820bbd3
}
