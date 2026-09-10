// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_AccountCargoSync/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// DB 同步 stub 类实现（batch-5 迁移）。
// 6 个 makeRequest 依据 class_func_reports 对应 md + 同族 DB packet 模式实现：
// 包构建约定一致——StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard
// （写 src/字段/header opcode）→ MsgQueueMgr::put(MsgQueueMgr::DB_Q(==2), guard)。
// 各函数 ORIG 地址见注释；字段顺序/长度直接来自反编译 C 与汇编。

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "DB_AccountCargoSync.h"
#include "MySQL.h"
#include "CInventory.h"       // NumberToString(unsigned int, int)
#include "CEnvironment.h"     // DB_HANDLE_2 / E_SERVER_GROUP_NONE
#include <cstring>
#include "GameBasicsScripts.h"   // DB_UpdateAuraAvatarOption
#include "DBMgr.h"           // DBMgr::GetDBHandle
// ============================================================================
// DB_UpgradeAccountCargo::dispatch —— ORIG T 0x0843af76。
// StreamPool::Acquire("DBThread.cpp",0x8bfb) → CStreamGuard → <<userIdx <<slot →
// GetInBuffer<SIG_UPGRADE_ACCOUNT_CARGO_RESULT>（memset 0x8）→ >>u32 a >>u32 b →
// GetDBHandle(DB_HANDLE_2,0) → UPDATE account_cargo；exec 失败返 0；
// affected==0 → flag=0x15 发包(Q1)；否则 flag=0、capacity=b 发包(Q1)。
// ============================================================================
#include "DB_AccountCargoSync/dispatch.inc"
}

// ============================================================================
// DB_CreateAccountCargo::makeRequest ORIG @ 0x0843ae86
//   opcode 0x1ee | param_1(int uid) | param_2(uint accId) | param_3(uint level)
// ============================================================================
#include "DB_AccountCargoSync/makeRequest.inc"
}

// ============================================================================
// DB_DeleteAccountCargo::makeRequest ORIG @ 0x0843b2e0
//   opcode 0x1f0 | param_1(int uid) | param_2(uint accId)
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_1.inc"
}

// ============================================================================
// DB_UpgradeAccountCargo::makeRequest ORIG @ 0x0843b15e
//   opcode 0x1ef | param_1(int uid) | param_2(uint accId) | param_3(uint level)
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_2.inc"
}

// ============================================================================
// DB_GuildExpBookDelete::makeRequest ORIG @ 0x0843c408
//   opcode 0x209 | -1 | param_1(uint) | param_2(int) | param_3(ENUM_SERVER_GROUP)
//   注意 ORIG 在三个参数前固定写 -1（int）。（分段推断：-1 为 ORIG 常量。）
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_3.inc"
}

// ============================================================================
// DB_InsertRandomOptionItemInform::makeRequest ORIG @ 0x08442d70
//   opcode 0x299 | param_1(int uid) | param_2(uint accId) | param_3(uchar 零扩展 uint)
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_4.inc"
}

// ============================================================================
// DB_PassPadUpdateFailCnt::makeRequest ORIG @ 0x08423eb4
//   opcode 0xa4 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(failCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。ORIG 由 dispatch @0x08423bc4 如此消费
//   （+0 m_id、+4 fail_cnt、+8 ip_info）。
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_5.inc"
}

// ============================================================================
// DB_PassPadUpdateCancelCnt::makeRequest ORIG @ 0x0842403a
//   opcode 0xa5 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(cancelCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_6.inc"
}

// ============================================================================
// DB_SecurityCardUpdateCancelCnt::makeRequest ORIG @ 0x0842c112
//   opcode 0x102 | -1，缓冲 +0x00 param_1(accId)、+0x04 param_2(cancelCnt)。
// ============================================================================
#include "DB_AccountCargoSync/makeRequest_7.inc"
}

// DB_UpdateAuraAvatarOption::makeRequest 权威实现见 W8LT_DBTask.cpp（FixCall_D 统一）。

namespace APSystem
{
#include "DB_AccountCargoSync/makeRequest_8.inc"
}  // TODO(G2)
}