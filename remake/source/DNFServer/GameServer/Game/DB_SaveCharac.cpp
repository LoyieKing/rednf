// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_SaveCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_SaveCharac 实现（ORIG 10 符号）
// 逐符号转写自 docs/class_func_reports/DB_SaveCharac/（汇编+反编译 C 全文）。
// SQL 字符串、执行顺序、错误码（MySQL::getDBError()==0x426 → 转 upDate 重试）
// 与 ORIG 指令一一对应；SIG_SAVE_CHARAC 字段语义见 SigTypes_D.h（pack(1)，
// sizeof 0x10d，本批字段化）。
// 唯一 owner：本 TU。ORIG 地址核对（nm --defined-only df_game_r）：
//   C1/C2 0x08415c2e | D1/D2 0x08415c4a | D0 0x08415c7a | dispatch 0x08415c98
//   _SaveContractOfCubePremiumInfo 0x08415dbe | _SaveCharacInfo 0x08415eda
//   _SaveCharacStat 0x084163f2 | SendResult 0x08416904 | _SaveCharacManageInfo 0x0841690e
// ============================================================================

#include "DB_SaveCharac.h"

#include "GlobalData.h"             // s_db_mgr / s_psimpleSSO
#include "DBMgr.h"                  // DBMgr::GetDBHandle
#include "MySQL.h"                  // MySQL::set_query/exec/getDBError/getAffectedRowCount/blob_to_str
#include "LogManager.h"             // LogManager::logFormat
#include "CSimpleSSO.h"             // CSimpleSSO::updateSSOExpireTime / stUpdateSSOExpireTime_t
#include "CEnvironment.h"           // DB_HANDLE_2 / E_SERVER_GROUP_NONE
#include "SigTypes.h"               // SIG_SAVE_CHARAC
#include "Stream.h"                 // Stream::GetOutBuffer

// ===========================================================================
// C1/C2 — DB_SaveCharac.md @0x08415c2e：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08415c4a/@0x08415c7a：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
#include "DB_SaveCharac/DB_SaveCharac.inc"
}

#include "DB_SaveCharac/dtor_DB_SaveCharac.inc"
}

// ===========================================================================
// SendResult — SendResult.md @0x08416904：恒返 1（0x0a 字节）。
// ===========================================================================
#include "DB_SaveCharac/SendResult.inc"
}

// ===========================================================================
// dispatch — dispatch.md @0x08415c98
// 脏块标志驱动四保存步骤；ManageInfo 失败仅 logFormat 不中断。
// ===========================================================================
#include "DB_SaveCharac/dispatch.inc"
}

// ===========================================================================
// _SaveContractOfCubePremiumInfo — 报告 @0x08415dbe
// 0x10a 标志未置直接成功；exec 失败返 0；受影响行数==0 时转 inSert。
// ===========================================================================
#include "DB_SaveCharac/_SaveContractOfCubePremiumInfo.inc"
}

// ===========================================================================
// _SaveCharacManageInfo — 报告 @0x0841690e
// 先 inSert；重复键（getDBError()==0x426）转 upDate 重试；exec 失败返 0。
// ===========================================================================
#include "DB_SaveCharac/_SaveCharacManageInfo.inc"
}

// ===========================================================================
// _SaveCharacInfo — 报告 @0x08415eda
// 标志组合三分支：(4&&6) 全 24 列；(4&&!6) 主 22 列（无 premium/guild/event）；
// (!4&&6) 仅 premium/guild/event 三列。exec 成功判定即返回值。
// ===========================================================================
#include "DB_SaveCharac/_SaveCharacInfo.inc"
}

// ===========================================================================
// _SaveCharacStat — 报告 @0x084163f2
// 45 列 upDate charac_stat（格式串 1017 字节，自 ORIG .rodata @0xc010b4 提取）；
// exec 成功后回写 SSO 到期时间（m_ssoId + 五字段 → updateSSOExpireTime）。
// ===========================================================================
#include "DB_SaveCharac/_SaveCharacStat.inc"
}
