// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_RequestCharacSkillInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_RequestCharacSkillInfo 实现（唯一 owner）
// ORIG 汇编转写：docs/class_func_reports/DB_RequestCharacSkillInfo/（4 符号）
//   _GetSkillTreeIndex T @0x08447162 / _GetSkillInfo T @0x08447312 /
//   dispatch T @0x084474f2 / sendResult T @0x084475a0。
// 返回类型：PRETTY_FUNCTION 实证为 bool（logFormat func 实参字符串）。
// ============================================================================

#include "DB_RequestCharacSkillInfo.h"

#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr
#include "StreamPool.h"     // StreamPool::Acquire
#include "Stream.h"         // Stream::GetOutBuffer
#include "CStreamGuard.h"   // CStreamGuard(Stream*,bool)/operator<</operator->/GetInBuffer
#include "MsgQueueMgr.h"    // MsgQueueMgr::put + QUEUE_IDX
#include "DBMgr.h"          // DBMgr::GetDBHandle（完整类型）
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP
#include "LogManager.h"
#include "SigTypes_D.h"     // SIG_REQUEST_CHARAC_SKILL_INFO

#include <string.h>         // memcpy

// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；保持 ORIG 全局
// mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z19get_skill_slot_namec @0x083fbd93（report 已读：-1/0/其它→"skill_slot"，
//   1→"skill_slot_2nd"）。
const char* get_skill_slot_name(char slot)
{
    if (slot == (char)-1 || slot == 0)
        return "skill_slot";
    if (slot == 1)
        return "skill_slot_2nd";
    return "skill_slot";
}
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79（定义在 DB_Login.cpp）。
#include "DB_RequestCharacSkillInfo/get_compressed_blob_data.inc"
#include "DB_RequestCharacSkillInfo/_GetSkillTreeIndex.inc"
}

// ORIG 0x08447312：seLect %s from skill（DB_HANDLE_3，列名=get_skill_slot_name(slot)）；
// exec/fetch 失败或 count==0 → logFormat 后返回 0；get_compressed_blob_data(0, 0x198, 2)
// 解压已学技能表。
// [b28d-19] N：P75 —— 实参装载时刻（ORIG 在 i++ 前先载 len→%ecx，4.4 末位装载；cast/逗号/具名三形同产）；ORIG 0x08447312。
#include "DB_RequestCharacSkillInfo/_GetSkillInfo.inc"
}

// ORIG 0x084474f2：从 stream 的 out buffer 取 SIG，先取技能树索引，再取技能表，
// 成功才 sendResult(slot, sig)；任一失败返回 0。
#include "DB_RequestCharacSkillInfo/dispatch.inc"
}

// ORIG 0x084475a0：Acquire 流 → CStreamGuard → <<0x2fd/<<userIdx →
// GetInBuffer<SIG> memcpy 0x1a0 → MsgQueueMgr::put(DISPATCH_Q)。
#include "DB_RequestCharacSkillInfo/sendResult.inc"
}
