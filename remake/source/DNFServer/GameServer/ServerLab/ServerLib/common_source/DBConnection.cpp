// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DBConnection/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "DBConnection.h"

#include <errmsg.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "TraceLog.h"

namespace nsl
{

#include "DBConnection/DBConnection.inc"
}

#include "DBConnection/dtor_DBConnection.inc"
}

#include "DBConnection/init_db_handle.inc"
}

#include "DBConnection/init.inc"
}

#include "DBConnection/close.inc"
}

#include "DBConnection/open.inc"
}

#include "DBConnection/set_query.inc"
}

#include "DBConnection/exec.inc"
}

#include "DBConnection/clear_result_set.inc"
}

#include "DBConnection/fetch.inc"
}

#include "DBConnection/ping.inc"
}

#include "DBConnection/set_compress_option.inc"
}

#include "DBConnection/set_read_default_grp_option.inc"
}

#include "DBConnection/set_charset_name_option.inc"
}

#include "DBConnection/set_reconnect_option.inc"
}

#include "DBConnection/escape_string.inc"
}

#include "DBConnection/blob_to_str.inc"
}

#include "DBConnection/getAffectedRowCount.inc"
}

#include "DBConnection/exec_query.inc"
}


} // namespace nsl
