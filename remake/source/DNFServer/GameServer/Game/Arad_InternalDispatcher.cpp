// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_InternalDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::DISPATCHER 内部流派发器体系实现。
// 依据 docs/class_func_reports/ARAD__DISPATCHER__*/ 各报告 + ORIG nm/objdump。
#include "Arad_InternalDispatcher.h"
#include "CStreamGuard.h"
#include "PacketGuard.h"
#include "GameEnums.h"
#include <algorithm>
#include "GlobalData.h"
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get
#include "MySQL.h"
#include "AvatarCoin.h"
#include "DBMgr.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "Stream.h"
#include "MoreTypes.h"      // arad::Sig* 线格式结构
#include "StreamPool.h"    // GlobalData::s_stream_pool->Acquire（LoadRewardEventItem/LoadServerState）
#include "MsgQueueMgr.h"   // MsgQueueMgr::DISPATCH_Q / put（LoadRewardEventItem/LoadServerState）

// [A31-GI · P327] Arad_InternalDispatcher::C1 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN4ARAD10DISPATCHER23Arad_InternalDispatcherC2Ev`（ORIG 0x819a31a）。
// 零语义/机器码变化，仅发射序。
#include "Arad_InternalDispatcher/Arad_InternalDispatcher.inc"
}

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN4ARAD10DISPATCHER23Arad_DatabaseDispatcherC2Ev`
// （P327：GI 名 = TU 内第一个「强符号·外部链接·本 TU 定义」实体；全限定定义
//  可写在命名空间块之外，发射序因此提前）。**语义与机器码零变化，仅发射序。**
// ---------------------------------------------------------------------------
// ORIG ctor 0x08184c7c：map C1 后跟一次显式 map::clear。
#include "Arad_InternalDispatcher/Arad_DatabaseDispatcher.inc"
}

// ============================================================================
// arad::SigSaveRewardEventItem 生命周期（置于全局作用域；MoreTypes.h namespace arad 声明）（声明于 MoreTypes.h；ORIG 见其注释）
// ============================================================================
// ORIG 0x08195ce8：6 dword 清零（+0x00..+0x14）。
#include "Arad_InternalDispatcher/init.inc"
}

#include "Arad_InternalDispatcher/set.inc"
}


namespace ARAD
{

namespace DISPATCHER
{

// ============================================================================
// make_internal_stream_jpn —— ORIG 0x081977b6 T（ARAD__DISPATCHER/make_internal_stream_jpn.md）
// guard->0x2dc(魔数) → param → code；写入序按报告 Ghidra C 还原。
// ============================================================================
#include "Arad_InternalDispatcher/make_internal_stream_jpn.inc"
}

// ============================================================================
// packetheader_jpn 【objdump】ORIG make_cmd_packetheader_jpn 0x08197812 T /
// make_noti_packetheader_jpn 0x08197862 T（无独立 md 报告，直接反汇编转写）：
//   bool 归一化(0/1) 后 put_header(typeFlag, (int)code) + put_byte(normalized)；
//   cmd 流 typeFlag=1，noti 流 typeFlag=0。
// ============================================================================
#include "Arad_InternalDispatcher/make_cmd_packetheader_jpn.inc"
}

#include "Arad_InternalDispatcher/make_noti_packetheader_jpn.inc"
}

// ============================================================================
// Arad_IInterDispatcher —— ORIG ctor 0x0819a36a / D1 0x0819a382 / get_code 0x819a3c6
// ============================================================================
#include "Arad_InternalDispatcher/Arad_IInterDispatcher.inc"
}

#include "Arad_InternalDispatcher/dtor_Arad_IInterDispatcher.inc"
}

// ============================================================================
// Arad_IDatabaseDispatcher —— ORIG ctor 0x081867a4 / D1 0x081867bc / get_code 0x8186800
// ============================================================================
#include "Arad_InternalDispatcher/Arad_IDatabaseDispatcher.inc"
}

#include "Arad_InternalDispatcher/dtor_Arad_IDatabaseDispatcher.inc"
}

// ============================================================================
// Arad_DatabaseDispatcher —— 容器（ARAD__DISPATCHER__Arad_DatabaseDispatcher 各报告）
// ============================================================================

// ORIG ~D1 0x08184cbe：for_each(begin,end,DeleteDispatcher 删除各派发器) 后析构 map。
#include "Arad_InternalDispatcher/dtor_Arad_DatabaseDispatcher.inc"
}

// ORIG 0x081850e0：find(code)；!=(end) 返回 second，否则 NULL。
#include "Arad_InternalDispatcher/GetDispatcher.inc"
}

// ORIG 0x08185144：GetDispatcher 为空则 map[code]=d；返回"之前不存在"。
#include "Arad_InternalDispatcher/_registDispatcher.inc"
}


// ORIG 0x0818518a：不存在则直接返回；命中则 erase。
// ORIG 0x0818518a 命中路径：erase(find(code))。
#include "Arad_InternalDispatcher/_unregistDispatcher.inc"
}

// ORIG 0x0818680c W：delete pair.second。
#include "Arad_InternalDispatcher/operator.inc"
}

// ============================================================================
// Arad_InternalDispatcher —— 容器（与 Database 同构；ORIG 0x081984d4 起）
// （C1 定义已按 P327 前移至 TU 首 [A31-GI] 段）
// ============================================================================
#include "Arad_InternalDispatcher/dtor_Arad_InternalDispatcher.inc"
}

#include "Arad_InternalDispatcher/GetDispatcher_1.inc"
}

#include "Arad_InternalDispatcher/_registDispatcher_1.inc"
}

#include "Arad_InternalDispatcher/_unregistDispatcher_1.inc"
}

#include "Arad_InternalDispatcher/operator_1.inc"
}

// ============================================================================
// Init —— ORIG Arad_DatabaseDispatcher::Init 0x08184d40 T
//   （ARAD__DISPATCHER__Arad_DatabaseDispatcher/Init.md）：
//   顺序 new 9 个 DB 流派发器并注册 get_code()；任一步 new 失败或注册冲突即
//   return false（ORIG 未对失败对象做清理），全部成功返回 true。
// ============================================================================
#include "Arad_InternalDispatcher/Init.inc"
}

// ============================================================================
// Init —— ORIG Arad_InternalDispatcher::Init 0x08198598 T
//   （ARAD__DISPATCHER__Arad_InternalDispatcher/Init.md）：同构，10 个 INTER 流。
// ============================================================================
#include "Arad_InternalDispatcher/Init_1.inc"
}


// ============================================================================
// 具体 DB 流派发器 —— ctor 形态（ORIG 各 ctor 0x24 字节反汇编）：
//   调基类 Arad_IDatabaseDispatcher(enum 立即数) 后由编译器写自身 vptr；
//   析构隐式（ORIG W D1/D0，仅置基类 vptr + 调基类析构）。
//   ORIG 各派生 ctor 帧为 sub $0x18（例 kAradEventItemLog@0x08186836、
//   kAvatarRoulette@0x0819a3fc），call outgoing 仅占 0x8 ⇒ 需 0x10 dead pad 撑帧。
// ============================================================================
#define ARAD_DB_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IDatabaseDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) \
    { char pad[0x10]; (void)pad; }

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAradEventItemLog, 0x20)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kAradEventItemLog.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSetCharacCreateTime.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kLoadRewardEventItem.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSaveRewardEventItem.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kLoadServerState.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSaveServerState.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kAvatarHiddenOptionSave.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kAvatarEnduranceSave.inc"
#include "Arad_InternalDispatcher/dtor_Arad_DB_kResetLimitedCreateCharacPerMid.inc"
#include "Arad_InternalDispatcher/dispatch.inc"
}
#include "Arad_InternalDispatcher/dispatch_1.inc"
}
// ORIG 0x08185574：select event_id, charac_no ... limit 10 →
//   SigLoadRewardEventItem(count + eventId[10] + characNo[10]) →
//   make_internal_stream_jpn(0x26, uid) + put_binary(0x56) →
//   MsgQueueMgr::put(DISPATCH_Q)。返回值 = put 的返回值。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08186f7c，204/204 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
#include "Arad_InternalDispatcher/dispatch_2.inc"
}
#include "Arad_InternalDispatcher/dispatch_3.inc"
}
// ORIG 0x08185d20：select category, code, state where end_time >= now() →
//   SigServerState（m_flag/m_count + tagData[20]，逐行填 m_category/m_code/
//   get_binary(msg,0xc)）→ make_internal_stream_jpn(0x29, -1) + put_binary(0x195)
//   → DISPATCH_Q，成功返回 true。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08187024，224/224 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
#include "Arad_InternalDispatcher/dispatch_4.inc"
}
#include "Arad_InternalDispatcher/dispatch_5.inc"
}
#include "Arad_InternalDispatcher/dispatch_6.inc"
}
#include "Arad_InternalDispatcher/dispatch_7.inc"
}

#include "Arad_InternalDispatcher/dispatch_8.inc"
}

// ============================================================================
// 具体 INTER 流派发器
// ============================================================================
#define ARAD_INTER_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IInterDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) \
    { char pad[0x10]; (void)pad; }   // ORIG 派生 ctor 帧 0x18（见 DB 宏注释）

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarRoulette, 0x2c)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarRoulette.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kLoadRewardEventItem.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kSaveServerState.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kLoadServerState.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kDeleteServerState.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAradJumpingCharacter.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarHiddenOptionChange.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kUseAvatarRecharge.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kEmblemCompound.inc"
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarConvert.inc"
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get

// ORIG Dispatcher_AradAvatarConvert::checkWorkState 0x081a003e W：
//   Secu Check(user,0x2a) 非 0 → SendCmdErrorPacket(0x24f,err) 返回 false；
//   否则 Singleton<AvatarConvertServer>::Get()->CheckCondition(user,&sig)；
//   尾部 ORIG 0x081a00a8 形 `call; xor $0x1,%eax; test; je →mov$1 岛` = !boolcall()（技巧1）。
#include "Arad_InternalDispatcher/checkWorkState.inc"
}

// ORIG Dispatcher_AradEmblemCompound::checkWorkState 0x0819fcb4 W：
//   ServiceRestrictManager::isRestricted(user,Enum_1,0x15) → 错误 0xd1；
//   Secu Check(user,0x2a)；最后 EmblemCompoundServer::CheckCondition。
#include "Arad_InternalDispatcher/checkWorkState_1.inc"
}

// ORIG Dispatcher_AradUseAvatarRecharge::checkWorkState 0x0819f9d6 W。
#include "Arad_InternalDispatcher/checkWorkState_2.inc"
}

// ORIG Dispatcher_AvartarHiddenOptionChange::checkWorkState 0x0819f956 W。
#include "Arad_InternalDispatcher/checkWorkState_3.inc"
}

// ORIG Dispatcher_AvartarRoulette::checkWorkState 0x0819ee6c W：
//   Secu Check(user,0x45) → err 透传；AvatarCoin(+0x658 ABI 镜像) 不可用 →
//   错误 0x16；GetRemainCapacity(INVEN 2, ITEM_TYPE_0)<=0 → 错误 4。
#include "Arad_InternalDispatcher/checkWorkState_4.inc"
}

// ORIG Dispatcher_AradJumpingCharacter::checkWorkState 0x0819f44c W：
//   Secu Check(user,0x2a)；eventKind==2 时要求存在 m_level>0x3b 的角色，
//   否则错误 8；当前角色 level>0x11 → 错误 0x12；取 (INVEN 1,
//   sig.m_itemIndex 槽) 物品：空 → 错误 0x11；eventKind==1 须 item idx
//   ==0x28d468、eventKind==2 须 ==0x28d469，否则错误 0x11；其余 eventKind
//   → 错误 0x11。全部通过返回 true。
#include "Arad_InternalDispatcher/checkWorkState_5.inc"
}
}  // namespace DISPATCHER
}  // namespace ARAD

