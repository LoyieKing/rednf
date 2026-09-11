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

// [P139 本波] 13 个函数的 pretty 串改用 `__PRETTY_FUNCTION__`（在函数内展开，发射
// `_ZZN23DisPatcher_DebugCommand…E19__PRETTY_FUNCTION__` 局部静态符号，与 ORIG 同名
// 同 size，见各报告 `movl $&_ZZ…__PRETTY_FUNCTION__,0x4(%esp)`）。删除原手写数组。
// [P8S2 2026-09-11 收口] 原「唯一例外 _debugCommandAddItem」已解除：P7 把
//   `typedef unsigned long itemIndexOnlyServer_t;` 落在 GameTypes.h:52（本 TU 已 include），
//   故定义处参数拼写改用 typedef 名 + 使用点改 __PRETTY_FUNCTION__ ⇒ 发射
//   `_ZZN23DisPatcher_DebugCommand20_debugCommandAddItemEP5CUsermiE19__PRETTY_FUNCTION__`
//   size=0x57，与 ORIG 0x08cb8280 串逐字节一致（dumpstr 实证）。原手写数组
//   `g_dbgCmdPrettyAddItem` 已无任何引用，而 4.4 -O0 仍把它发射到 .rodata ⇒ OUR-only 符号；
//   [Main 授权 2026-09-11] 已删除该 TU 级 static（ORIG 全 ELF 无此名，0 命中）。

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

#include "DisPatcher_DebugCommand_cmds/_debugCommandOn.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandOff.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandClearSkill.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandSetSkill.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandMasterNewSkill.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandAddItem.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandSetGrowType.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandLevelUpDown.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandGuildLevelUp.inc"


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
        cMyTrace(__PRETTY_FUNCTION__, 0x231, 0)(
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

#include "DisPatcher_DebugCommand_cmds/_debugCommandSetEnchant.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandInvisible.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandVisible.inc"


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

#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerwarPointUp.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandUserPowerwarPointUp.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandInitUserPowerwarPoint.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandClearInven.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandClearAvtInven.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandCargoCapacity.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandRequestMid.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandUserRepel.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerUpOn.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarStart.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarEnd.inc"


#include "DisPatcher_DebugCommand_cmds/_debugCommandSetUserDefine.inc"


// ===========================================================================
// LinkFinal2 核销（/tmp/undef_r26.txt）：
// 28. _Z19DoUserDefineCommandP5CUseriPKc @0x0820ba90 (0xd8) —— objdump 逐指令转写。
//     CParser 栈对象；m_user=param；sprintf(m_buf,"%d %s",cmd,arg)（0x8bc2c30）；
//     parse()（ORIG 直接 call 非虚派发）；GetCommand(m_id) → SetParser → HandleRequest。
//     ORIG 返回值未定义（尾 ret 无 eax 写）；调用方不使用返回值，此处返 0。
// ===========================================================================
#include "GameMasterCmd.h"      // game_master::{CParser,CCommand,CGameMasterMgr}

#include "DisPatcher_DebugCommand_cmds/DoUserDefineCommand.inc"


// ===========================================================================
// 29. _Z28SendVillageAttackedGMCommandP5CUseri @0x0820bb68 (0x76) —— objdump 逐指令转写。
//     Packet_VillageAttackedGMCommand 栈包（0x1a）；gcc 将包内字段寻址为栈局部：
//     -0x18=+0x0a uid、-0x14=+0x0e characNo、-0xc=+0x16 state；
//     GetServerProxy(user->GetServerGroup())->SendTcpPacket(&pkt, 0x1a)。
// ===========================================================================
#include "Packet_VillageAttackedGMCommand.h"

#include "DisPatcher_DebugCommand_cmds/SendVillageAttackedGMCommand.inc"

