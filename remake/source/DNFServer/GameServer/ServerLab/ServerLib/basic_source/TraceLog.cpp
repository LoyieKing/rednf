// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TraceLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "Script.h"
#include "TraceLog.h"
#include "LogSendThread.h"

extern "C" int __xstat(int ver, const char* path, struct stat* buf);

extern "C" __attribute__((weak)) int stat(const char* __path, struct stat* __statbuf)
{
    return __xstat(3, __path, __statbuf);
}

namespace nsl {

bool bChangedDataForLog = false;

#include "TraceLog/TraceLog.inc"
}

#include "TraceLog/dtor_TraceLog.inc"
}

#include "TraceLog/init.inc"
}

#include "TraceLog/sysNSL_LOG_date_ch.inc"
}


#include "TraceLog/sysLog.inc"
}

#include "TraceLog/errorLog.inc"
}

#include "TraceLog/get_mask.inc"
}

#include "TraceLog/set_mask.inc"
}

#include "TraceLog/set_fd.inc"
}

#include "TraceLog/get_time.inc"
}

#include "TraceLog/getBaseName.inc"
}

#include "TraceLog/MakeDir.inc"
}

#include "TraceLog/checkLogServer.inc"
}

#include "TraceLog/VsnPrintf.inc"
}

#include "TraceLog/SnPrintf.inc"
}

#include "TraceLog/setMask.inc"
}

#include "TraceLog/setLogSendThread.inc"
}

#include "TraceLog/getServiceIdentity.inc"
}

} // namespace nsl
