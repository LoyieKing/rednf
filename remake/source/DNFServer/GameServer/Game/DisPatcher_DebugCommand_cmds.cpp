// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DisPatcher_DebugCommand_cmds/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
//
// ============================================================================
// b28f-SweepGroup17 判词汇总（四数 21/71/1/2 保持，零回退）：
//   · `_debugCommandGuildLevelUp`（ORIG T 0x0858f64c，NEAR 99=99，2 块）：唯一差为
//     三个 callee-saved 结果的角色互换（ORIG `mov %eax,%esi`→第2个槽 /
//     `mov %eax,%ebx`→第3个槽，4.4 相反）+ 对应 vararg 压栈序 `mov %esi,0x8(%esp)`
//     ↔ `mov %ebx,0x8(%esp)`；助记符/条数/槽集全等 ⇒ P75/N82附 跨版本寄存器调度，
//     禁绑定形（P0/N96）⇒ 封顶。
//   · `_debugCommandSetEnchant`（ORIG T 0x0858fb4e，DIFF 109/112，8 SEM）：同族
//     寄存器角色（`movzbl %al,%ecx`↔`%edx`、装载序）+ 槽分区 ⇒ 封顶。
//   · p167 两件调用点差集全 CLEAN。
//   ⇒ 本 TU 剩余真缺口 = 0。
// ============================================================================
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
}

// ===========================================================================
// 1. _debugCommandOff —— 0x0858e918
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 局部内联为 P168a① bool 裸形
// （ORIG `call isGMUser; xor $1; test %al; je`）+ DBG_TRACE 改 P227 全表达式临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandOff.inc"
}

// ===========================================================================
// 3. _debugCommandClearSkill —— 0x0858e9c4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：N93附「尾双 return 块物理序互换」
// 由 P223 goto 尾标签翻正——成功体尾 `goto ok;` + 块外 `return false;` +
// `ok: return true;` 复现 ORIG `je <尾 mov$0 块>` / `mov$1` 后置布局
// （P222 正向包裹的 goto 变体，探针 p3 实测）。P167 callsite CLEAR 10/10。
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearSkill.inc"
}

// ===========================================================================
// 4. _debugCommandSetSkill —— 0x0858eab8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：SetSkillLevel bool 化
// （P248 档①，Main 代落 CUser.h）+ P223 goto 尾标签翻尾块序（同 ClearSkill）；
// P167 callsite CLEAR 10/10。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetSkill.inc"
}

// ===========================================================================
// 5. _debugCommandMasterNewSkill —— 0x0858ebcc
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 内联（P168a①）+ P227 临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandMasterNewSkill.inc"
}

// ===========================================================================
// 6. _debugCommandAddItem —— 0x0858ec92
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 4.4.6 后端 %edx（与 ORIG 同形）；4.4.7 下恒 %eax 才是
// NEAR（P230 补）。已修：slot>=0 正向包裹翻尾块序（P222）、outSpace 删预清（P171）、
// getCurSec 双调内联实参、`&&` 拆嵌套 if（P225 推广）。P167 CLEAR 16/16。
#include "DisPatcher_DebugCommand_cmds/_debugCommandAddItem.inc"
}

// ===========================================================================
// 7. _debugCommandSetGrowType —— 0x0858ee54
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 内联 + ChangeGrowType_GM bool 化
// （Main 代落）+ get_state 判定改 switch（P251，if/else-if 链产 sete 反形）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetGrowType.inc"
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
}

// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 %edx 与 ORIG 同形（见 AddItem 判词）。已修：
// `slot != -1` 正向包裹翻失败出口块序（P222）、usablePeriod 内联（P227）、
// G_CEnvironment 具名字段。P167 callsite CLEAR 18/18。
#include "DisPatcher_DebugCommand_cmds/_debugCommandAddAvatar.inc"
}

// ===========================================================================
// 11. _debugCommandPVPRankSet —— 0x0858faba
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPVPRankSet.inc"
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
}

// ===========================================================================
// 13. _debugCommandInvisible —— 0x0859058a
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandInvisible.inc"
}

// ===========================================================================
// 14. _debugCommandVisible —— 0x085906ac
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：area/vill/name 全部内联实参
// （ORIG 无槽，P0/P183）+ P227 临时链。
#include "DisPatcher_DebugCommand_cmds/_debugCommandVisible.inc"
}

// ===========================================================================
// 15. _debugCommandSetBlackCount —— 0x0858fcc4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：DbgCmdArgWordView
// 缺 pack(1) 使 m_argU 自然对齐到 +0x10（读到 msg+0x10 而非 ORIG `mov 0xe(%eax),%edx`
// 的 +0x0e）⇒ 补 pack(1)+static_assert，读偏移归位。
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetBlackCount.inc"
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
}

// ===========================================================================
// 17. _debugCommandUserPowerwarPointUp —— 0x0858fee8
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandUserPowerwarPointUp.inc"
}

// ===========================================================================
// 18. _debugCommandInitUserPowerwarPoint —— 0x08590092（清零个人 Power War 点）
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandInitUserPowerwarPoint.inc"
}

// ===========================================================================
// 19. _debugCommandClearInven —— 0x0859019e
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearInven.inc"
}

// ===========================================================================
// 20. _debugCommandClearAvtInven —— 0x08590254
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandClearAvtInven.inc"
}

// ===========================================================================
// 21. _debugCommandCargoCapacity —— 0x0858fdc8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：派生类
// `static const unsigned int packetSize=0x14` 遮蔽 PacketHeader::packetSize
// 成员（+0x2，ctor 写 0x1e）⇒ 发送长度少 0xa 字节。改 `pkt.PacketHeader::packetSize`
// 后复现 ORIG `movzwl -0x24(%ebp),%eax`；receiver 改链式（P52）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandCargoCapacity.inc"
}

// ===========================================================================
// 22. _debugCommandRequestMid —— 0x08590506
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：len 具名局部落槽（ORIG -0x1c）、
// name 内联实参（ORIG %edi 无槽）+ 链式 receiver（P52）。
#include "DisPatcher_DebugCommand_cmds/_debugCommandRequestMid.inc"
}

// ===========================================================================
// 23. _debugCommandUserRepel —— 0x0859048e
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：arg4/arg3 物化位置互换的钥匙 =
// `&msg.payload.nameBuf[0]`（取首元素地址 vs 数组退化改变 4.4 表达式树）——ORIG
// `mov 0xe(%eax),%edi`（m_arg→%edi）+ nameBuf 地址落槽 -0x1c；直接写 nameBuf 反致
// 两者互换（探针 r8/r9 实测）。P167 callsite CLEAR 5/5。
#include "DisPatcher_DebugCommand_cmds/_debugCommandUserRepel.inc"
}

// ===========================================================================
// 24. _debugCommandPowerUpOn —— 0x0858ffee
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerUpOn.inc"
}

// ===========================================================================
// 25. _debugCommandPowerWarStart —— 0x0859040e
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarStart.inc"
}

// ===========================================================================
// 26. _debugCommandPowerWarEnd —— 0x0859044e
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandPowerWarEnd.inc"
}

// ===========================================================================
// 27. _debugCommandSetUserDefine —— 0x085908ba：ORIG je → 返回 1（user==0 → 1；非空 → 0）。
// ===========================================================================
#include "DisPatcher_DebugCommand_cmds/_debugCommandSetUserDefine.inc"
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
}

// ===========================================================================
// 29. _Z28SendVillageAttackedGMCommandP5CUseri @0x0820bb68 (0x76) —— objdump 逐指令转写。
//     Packet_VillageAttackedGMCommand 栈包（0x1a）；gcc 将包内字段寻址为栈局部：
//     -0x18=+0x0a uid、-0x14=+0x0e characNo、-0xc=+0x16 state；
//     GetServerProxy(user->GetServerGroup())->SendTcpPacket(&pkt, 0x1a)。
// ===========================================================================
#include "Packet_VillageAttackedGMCommand.h"

#include "DisPatcher_DebugCommand_cmds/SendVillageAttackedGMCommand.inc"
}
