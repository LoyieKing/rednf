// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinuxService/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-07
#include "LinuxService.h"

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

// The original toolchain's <cstring> provided std::strstr(char*, const char*)
// (weak, out-of-line at -O0).  The c6root glibc defines
// __CORRECT_ISO_CPP_STRING_H_PROTO so this overload is absent; reproduce the
// original _ZSt6strstrPcPKc symbol.
#include "Script.h"
#include "TraceLog.h"

namespace std
{
char* strstr(char* __s1, const char* __s2) throw()
{
    return ::strstr(__s1, __s2);
}
}

namespace nsl {

char gServiceName[0x14];
LinuxService* LinuxService::theInstance = NULL;

static void _evlog(LPCTSTR sz)
{
    puts(sz);
}

#include "LinuxService/LinuxService.inc"
}

#include "LinuxService/dtor_LinuxService.inc"
}

#include "LinuxService/checkConfigFile.inc"
}

#include "LinuxService/checkPIDFile.inc"
}

#include "LinuxService/prepareStart.inc"
}

#include "LinuxService/install.inc"
}

#include "LinuxService/remove.inc"
}

#include "LinuxService/start.inc"
}

#include "LinuxService/main.inc"
}

#include "LinuxService/sendBroadCastMessage.inc"
}

#include "LinuxService/SendInfoMessage.inc"
}

#include "LinuxService/controlStop.inc"
}

#include "LinuxService/controlPause.inc"
}

#include "LinuxService/controlContinue.inc"
}

#include "LinuxService/setStopped.inc"
}

#include "LinuxService/stopPending.inc"
}

#include "LinuxService/setRunning.inc"
}

#include "LinuxService/setServiceState.inc"
}

#include "LinuxService/processCommandLine.inc"
}

#include "LinuxService/setPIDFileName.inc"
}

#include "LinuxService/setInfo.inc"
}

#include "LinuxService/Neof_registerSignalHandler.inc"
}

#include "LinuxService/Neof_SignalHandler.inc"
}

#include "LinuxService/Neof_registerSignalHandlers.inc"
}

#include "LinuxService/Neof_dumpCoreFile.inc"
}

#include "LinuxService/Neof_setCoreLimit.inc"
}

#include "LinuxService/Neof_sendTerminateSignal.inc"
}

#include "LinuxService/Neof_sendSuspendSignal.inc"
}

#include "LinuxService/delete_pid.inc"
}

#include "LinuxService/save_pid.inc"
}

#include "LinuxService/make_dir.inc"
}

} // namespace nsl
