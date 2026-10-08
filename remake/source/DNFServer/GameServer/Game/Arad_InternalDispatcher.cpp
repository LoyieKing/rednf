// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_InternalDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#if !remake_verify
// [DNF-MOD 2026-10-08 GM-skip lv60 check] WongWork::CGMAccounts 完整定义（isGM，供
//   checkWorkState_5.inc 的 GM 跳过分支）；仅现代构建包含，verify 构建 include 集不变。
#include "../Statics/GMAccounts.h"
#endif

#include "Arad_InternalDispatcher/Arad_InternalDispatcher.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] Arad_DatabaseDispatcher 已迁出至 Arad_DatabaseDispatcher.cpp（ORIG GI 键对齐）。



#include "Arad_InternalDispatcher/init.inc"
{
    // ORIG：6 dword 清零通过 set(0,0,0,0,0,0) 栈传参（0x08195ce8）。
    set(0, 0, 0, 0, 0, 0);
}

#include "Arad_InternalDispatcher/set.inc"
                                       unsigned long d, short e, unsigned int f)
{
    // ORIG 0x08195d18 栈槽：0xc→+0x00、0x10→+0x04、0x14→+0x08、
    //   0x18(d 低32位)→+0x10、0x1c(e 低16位)→+0x14、0x20(f)→+0x0c。
    m_field00 = a;
    m_field04 = b;
    m_field08 = c;
    m_field10 = (unsigned int)d;
    m_field14 = (short)e;
    m_field0c = f;
}


namespace ARAD
{

namespace DISPATCHER
{

#include "Arad_InternalDispatcher/make_internal_stream_jpn.inc"


#include "Arad_InternalDispatcher/make_cmd_packetheader_jpn.inc"


#include "Arad_InternalDispatcher/make_noti_packetheader_jpn.inc"


#include "Arad_InternalDispatcher/Arad_IInterDispatcher.inc"


#include "Arad_InternalDispatcher/dtor_Arad_IInterDispatcher.inc"


#include "Arad_InternalDispatcher/Arad_IDatabaseDispatcher.inc"


#include "Arad_InternalDispatcher/dtor_Arad_IDatabaseDispatcher.inc"


// ============================================================================
// Arad_DatabaseDispatcher —— 容器（ARAD__DISPATCHER__Arad_DatabaseDispatcher 各报告）
// ============================================================================

#include "Arad_InternalDispatcher/dtor_Arad_DatabaseDispatcher.inc"


#include "Arad_InternalDispatcher/GetDispatcher.inc"


#include "Arad_InternalDispatcher/_registDispatcher.inc"



#include "Arad_InternalDispatcher/_unregistDispatcher.inc"


#include "Arad_InternalDispatcher/operator.inc"


#include "Arad_InternalDispatcher/dtor_Arad_InternalDispatcher.inc"


#include "Arad_InternalDispatcher/GetDispatcher_1.inc"


#include "Arad_InternalDispatcher/_registDispatcher_1.inc"


#include "Arad_InternalDispatcher/_unregistDispatcher_1.inc"


#include "Arad_InternalDispatcher/operator_1.inc"


#include "Arad_InternalDispatcher/Init.inc"


#include "Arad_InternalDispatcher/Init_1.inc"



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

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSetCharacCreateTime, 0x24)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSetCharacCreateTime.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kLoadRewardEventItem, 0x26)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kLoadRewardEventItem.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSaveRewardEventItem, 0x27)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSaveRewardEventItem.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kLoadServerState, 0x29)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kLoadServerState.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kSaveServerState, 0x2a)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kSaveServerState.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAvatarHiddenOptionSave, 0x2f)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kAvatarHiddenOptionSave.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kAvatarEnduranceSave, 0x31)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kAvatarEnduranceSave.inc"

ARAD_DB_DISPATCHER_CTOR(Arad_DB_kResetLimitedCreateCharacPerMid, 0x37)
#include "Arad_InternalDispatcher/dtor_Arad_DB_kResetLimitedCreateCharacPerMid.inc"

#include "Arad_InternalDispatcher/dispatch.inc"

#include "Arad_InternalDispatcher/dispatch_1.inc"

#include "Arad_InternalDispatcher/dispatch_2.inc"

#include "Arad_InternalDispatcher/dispatch_3.inc"

#include "Arad_InternalDispatcher/dispatch_4.inc"

#include "Arad_InternalDispatcher/dispatch_5.inc"

#include "Arad_InternalDispatcher/dispatch_6.inc"

#include "Arad_InternalDispatcher/dispatch_7.inc"


#include "Arad_InternalDispatcher/dispatch_8.inc"


// ============================================================================
// 具体 INTER 流派发器
// ============================================================================
#define ARAD_INTER_DISPATCHER_CTOR(IMPL, CODE) \
    IMPL::IMPL() : Arad_IInterDispatcher((ENUM_INTERNALPACKET_JPN)(CODE)) \
    { char pad[0x10]; (void)pad; }   // ORIG 派生 ctor 帧 0x18（见 DB 宏注释）

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarRoulette, 0x2c)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarRoulette.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kLoadRewardEventItem, 0x26)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kLoadRewardEventItem.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kSaveServerState, 0x2a)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kSaveServerState.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kLoadServerState, 0x29)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kLoadServerState.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kDeleteServerState, 0x2b)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kDeleteServerState.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAradJumpingCharacter, 0x2d)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAradJumpingCharacter.inc"
// dispatch_sig/clearEpicQuest/sendHelperBox 真实现见 Arad_InternalDispatcher_INTER.cpp。

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarHiddenOptionChange, 0x2e)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarHiddenOptionChange.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kUseAvatarRecharge, 0x30)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kUseAvatarRecharge.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kEmblemCompound, 0x32)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kEmblemCompound.inc"

ARAD_INTER_DISPATCHER_CTOR(Arad_INTER_kAvatarConvert, 0x33)
#include "Arad_InternalDispatcher/dtor_Arad_INTER_kAvatarConvert.inc"

// ============================================================================
// cmd 流业务派发器 checkWorkState —— ORIG 各 W 函数逐条转写。
//   依赖：GlobalData::s_pSecuProtectionField、ARAD::Singleton<T>::Get、
//         各 Server::CheckCondition / AvatarRechargeServer::
//         CheckConditionUseRecharge（定义见各 Server 权威 TU）。
// ============================================================================
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get

#include "Arad_InternalDispatcher/checkWorkState.inc"


#include "Arad_InternalDispatcher/checkWorkState_1.inc"


#include "Arad_InternalDispatcher/checkWorkState_2.inc"


#include "Arad_InternalDispatcher/checkWorkState_3.inc"


// [SplitC 2026-09-11 batch3 · TU 迁移] checkWorkState_4 已迁出至 Arad_PacketDispatcher.cpp（ORIG GI 键对齐）。



#include "Arad_InternalDispatcher/checkWorkState_5.inc"

}  // namespace DISPATCHER
}  // namespace ARAD

