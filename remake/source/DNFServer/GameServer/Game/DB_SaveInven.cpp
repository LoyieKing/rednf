// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_SaveInven/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_SaveInven 实现（ORIG 9 符号）
// 逐符号转写自 docs/class_func_reports/DB_SaveInven/（汇编+反编译 C 全文）。
// SQL 字符串、压缩区偏移（equipslot +0x19 0x2dc / inventory +0x557 0x3b55 /
// creature +0x710b 0x184e / cargo +0x895d 0x2438 / jewel +0x40ac 0xf03）、
// 异常码（0x4427/0x4429/0x4430，catch(unsigned int) 后 cMyTrace 记日志返 0）
// 与 ORIG 指令一一对应；SIG_SAVE_INVENTORY 字段语义见 SigTypes_E.h
// （pack(1)，sizeof 0xad99，2026-08-23 字段化）。
// 唯一 owner：本 TU。ORIG 地址核对（nm --defined-only df_game_r）：
//   C1/C2 0x08416a42 | D1/D2 0x08416a5e | D0 0x08416a8e | dispatch 0x08416aac
//   _saveCharacInvenExpand 0x08416b38 | SaveInven 0x08416cfa | SendResult 0x08417204
// ============================================================================

#include "DB_SaveInven.h"
#include "ARAD_Database.h"          // ARAD::DATABASE::source*Size（各快照长度）

#include "GlobalData.h"             // s_db_mgr
#include "DBMgr.h"                  // DBMgr::GetDBHandle
#include "MySQL.h"                  // MySQL::set_query/exec/blob_to_str
#include "GameTypes.h"              // cMyTrace
#include "AdvanceAltarZip.h"        // compress_zip（唯一实现归 APSystemOnlyOrig.cpp）
#include "SigTypes.h"               // SIG_SAVE_INVENTORY
#include "Stream.h"                 // Stream::GetOutBuffer
#include "CEnvironment.h"           // DB_HANDLE_3 / E_SERVER_GROUP_NONE

#include <string.h>                 // memset

// NumberToString 权威声明在 CInventory.h（0x0810904b u32 版），本 TU 仅前向
// 声明同名全局函数，与权威签名一致（W8LT_DBTask.cpp 同款做法）。
char* NumberToString(unsigned int value, int radix);

// ===========================================================================
// C1/C2 — DB_SaveInven.md @0x08416a42：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08416a5e/@0x08416a8e：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
#include "DB_SaveInven/DB_SaveInven.inc"
}

#include "DB_SaveInven/dtor_DB_SaveInven.inc"
}

// ===========================================================================
// SendResult — SendResult.md @0x08417204：恒返 1（0x0a 字节）。
// ===========================================================================
#include "DB_SaveInven/SendResult.inc"
}

// ===========================================================================
// dispatch — dispatch.md @0x08416aac
// GetOutBuffer 判空返 0；SaveInven 失败返 0；m_saveFlag==0/2 时
// _saveCharacInvenExpand 失败返 0；否则 1。
// ===========================================================================
#include "DB_SaveInven/dispatch.inc"
}

// ===========================================================================
// _saveCharacInvenExpand — 报告 @0x08416b38（const）
// cargo(+0x895d,0x2438)/jewel(+0x40ac,0xf03) 压缩失败返 0；
// upDate charac_inven_expand 后直接返回 exec 结果。
// ===========================================================================
#include "DB_SaveInven/_saveCharacInvenExpand.inc"
}

// ===========================================================================
// SaveInven — SaveInven.md @0x08416cfa（函数主体）
// m_saveFlag==0/2 时压缩 equipslot/inventory/creature 三区（失败抛 unsigned int
// 异常码并 catch 记日志返 0）；按 flag 分支 set_query；exec 成功后再
// inSert member_avatar_coin（m_id 用 NumberToString 十进制）。
// ===========================================================================
#include "DB_SaveInven/SaveInven.inc"
}