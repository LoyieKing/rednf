// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MySQL/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff）。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— MySQL（D4 批次）
// 参照 docs/class_func_reports/MySQL.md；源文件 DBMgr.cpp（ORIG 日志文件名）。
#include "MySQL.h"

#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "CQueryCounter.h"
#include "CUnixTimer.h"
#include "GameBase.h"

// ---- cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）----
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};
// [T1-GI · P327] MySQL ctor 定义前移为本 TU 首个「强符号 · 外部链接 · 本 TU 定义」
// 实体 ⇒ GI 键名 `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff；4.4 外联 ctor 发 C1+C2，
// C2 先入 assemble 队列）。改前键名 = `_GLOBAL__I__ZN9CDnFTimer11SetLastTimeEv`。
#include "MySQL/MySQL.inc"
}

// ---- CDnFTimer / CDnFTimerFactory（G7 计时族，ORIG 弱符号；算法正确即可）----
#include "MySQL/SetLastTime.inc"
#include "MySQL/GetTimeInterval.inc"
#include "MySQL/CreateDnFTimer.inc"
#include "MySQL/DestroyDnFTimer.inc"


#include "MySQL/dtor_MySQL.inc"
}

#include "MySQL/set_compress_option.inc"
}

#include "MySQL/set_read_default_grp_option.inc"
}

#include "MySQL/set_charset_name_option.inc"
}

#include "MySQL/set_reconnect_option.inc"
}

#include "MySQL/init_db_handle.inc"
}

#include "MySQL/init.inc"
}

#include "MySQL/close.inc"
}

#include "MySQL/open.inc"
}

#include "MySQL/open_1.inc"
}

#include "MySQL/set_query.inc"
}

#include "MySQL/getAffectedRowCount.inc"
}

#include "MySQL/exec.inc"
}

#include "MySQL/fetch.inc"
}

#include "MySQL/ping.inc"
}

#include "MySQL/blob_to_str.inc"
}

#include "MySQL/getServerVersion.inc"
}

#include "MySQL/getClientVersion.inc"
}

#include "MySQL/select.inc"
}

#include "MySQL/select_1.inc"
}

#include "MySQL/insert.inc"
}

#include "MySQL/update.inc"
}

#include "MySQL/set_query_1.inc"
}

#include "MySQL/escape_string.inc"
}

#include "MySQL/clear_result_set.inc"
}

#include "MySQL/exec_query.inc"
}

// ============================================================================
// GetIdentityFromDB（ORIG 0x83fbc66 T）
// 包装 WongWork::DBCommon::GetIdentity（0x83f9ad4）：查询 @@identity（上次
// INSERT 的自增 ID），供 CQueryCounter::insertCounter 取 q_id 使用。
// ORIG：call WongWork::DBCommon::GetIdentity → leave/ret。
// ============================================================================
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

}  // namespace WongWork

#include "MySQL/GetIdentityFromDB.inc"
}
