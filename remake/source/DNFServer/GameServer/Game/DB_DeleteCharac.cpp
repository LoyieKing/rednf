// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_DeleteCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_DeleteCharac.cpp
// 目标：清零 /tmp/only_orig/DB_DeleteCharac.txt 全部 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_DeleteCharac/<fn>.md（汇编为准）转写。
// SIG_DELETE_CHARAC 权威布局见 SigTypes_B.h（pack(1)，size 0x14）：
//   +0x00 m_accId | +0x04 m_characNo | +0x08 m_errNo | +0x0c m_guildId
//   | +0x10 m_eventFlag
// ============================================================================

#include "DB_DeleteCharac.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "CEnvironment.h"       // G_CEnvironment / Server_Envir::m_serverGroup
#include "DB_LoadInventory.h"   // DB_LoadInventory::GetItemLock（static）
#include "CTitleBook.h"         // stItemLockInfo（物品锁 DB 值类型）

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ============================================================================
// DB_DeleteCharac::DB_DeleteCharac @0x08402bce（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x08402bea / D0 0x08402c1a deleting）由类定义产出。
// ============================================================================
#include "DB_DeleteCharac/DB_DeleteCharac.inc"
}

// ============================================================================
// ~DB_DeleteCharac @0x08402bea（D1）/ 0x08402c1a（D0 deleting）
// ============================================================================
#include "DB_DeleteCharac/dtor_DB_DeleteCharac.inc"
}

// ============================================================================
// DB_DeleteCharac::dispatch @0x08402c38
// 基类 dispatch 通过 → GetOutBuffer<SIG_DELETE_CHARAC>（空 → 0）→
// +0x8 清零 → getCurSec → stItemLockInfo[100] 自动构造 → GetItemLock
// 成功则扫描 100 项：state==1 或 (state==2 且 unlockSec+0x3f480>curSec)
// → +0x8=3 + SendResult 返 1。DeleteGuildInfo 失败 → cMyTrace 0x12a7 +
// SendResult；成功 → +0x10=0，CheckEventCharacter40 为真则
// DeleteEventCharacFlag + +0x10=1；DeleteCharac 失败 +0x8=1；
// DeleteCharacOption；SendResult 失败 → 0；成功 cMyTrace 0x12ce 返 1。
// ============================================================================
#include "DB_DeleteCharac/dispatch.inc"
}

// ============================================================================
// DB_DeleteCharac::DeleteGuildInfo @0x08402f4c
// guildId==0 → charac_info 查 guild_id 入 +0xc（失败/空 +0x8=1 返 0；
// 查到仍 0 → logFormat 0x1312 返 1）。guild_info 查 master_no：
// 失败 +0x8=1；characNo==master → +0x8=2 返 0；否则 guild_member 退会
// （member_flag=2/secede），count(*) 更新 guild_info.member_count。
// ============================================================================
#include "DB_DeleteCharac/DeleteGuildInfo.inc"
}

// ============================================================================
// DB_DeleteCharac::DeleteEventCharacFlag @0x084032aa
// GetDBHandle(6) → NumberToString(m_accId) → member_login 清
// event_charac_flag。exec 失败返 0。
// ============================================================================
#include "DB_DeleteCharac/DeleteEventCharacFlag.inc"
}

// ============================================================================
// DB_DeleteCharac::CheckEventCharacter40 @0x08403324
// charac_info 查 event_charac_level（m_id=accId 串、delete_flag=0）；
// exec/fetch/get_int 失败 → 0；level==0 → 0；否则 1。
// ============================================================================
#include "DB_DeleteCharac/CheckEventCharacter40.inc"
}

// ============================================================================
// DB_DeleteCharac::DeleteCharac @0x0840340c
// charac_info 置 delete_time=now()/delete_flag=1（charac_no=%u and m_id=%s）；
// exec 失败返 0。成功 → GetDBHandle(1) 从 dnf_master_charac 删除
// （m_id/servier_id=Server_Envir.m_serverGroup/charac_no）后返 1。
// ============================================================================
#include "DB_DeleteCharac/DeleteCharac.inc"
}

// ============================================================================
// DB_DeleteCharac::SendResult @0x084034fe
// StreamPool::Acquire("DBThread.cpp",0x13b0) → guard<<userIdx<<slot →
// put_binary(sig,0x14) → MsgQueueMgr::put(QUEUE_IDX 1)。恒返 1。
// ============================================================================
#include "DB_DeleteCharac/SendResult.inc"
}

// ============================================================================
// DB_DeleteCharac::DeleteCharacOption @0x084035e0
// charac_option 按 charac_no 删除。exec 失败返 0。
// ============================================================================
#include "DB_DeleteCharac/DeleteCharacOption.inc"
}
