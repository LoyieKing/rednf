// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ServiceFactory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <stdio.h>
#include <string.h>
#include <string>
#include <new>

#include "ServiceFactory.h"
#include "Threads.h"
#include "TCPSocket.h"
#include "TraceLog.h"
#include "Script.h"
#include "DataPool.h"
#include "WorkThread.h"
#include "System.h"
#include "TimerThread.h"
#include "TCPThread.h"
#include "TCPSendThread.h"
#include "NSLDBThread.h"
#include "TCPDispatcher.h"
#include "InterDispatcher.h"
#include "DBDispatcher.h"
#include "TActiveConnect.h"
#include "EncyptTools.h"
#include "Message.h"
#include "DBConnection.h"

namespace nsl {

char configpath[256];
ServiceFactory* pApp = 0;
bool is_config_changed = false;
bool is_config_reloaded = false;

#include "ServiceFactory/Threads.inc"
}

#include "ServiceFactory/getScopedLittleWorkIdx.inc"
}

#include "ServiceFactory/setMostLittleQueue.inc"
}

#include "ServiceFactory/EncyptTools.inc"
}

#include "ServiceFactory/getNetWorkHandler.inc"
}

#include "ServiceFactory/getInterHandler.inc"
}

#include "ServiceFactory/getDBHandler.inc"
}

#include "ServiceFactory/ServiceFactory.inc"
}

#include "ServiceFactory/dtor_ServiceFactory.inc"
}

#include "ServiceFactory/setTick.inc"
}

#include "ServiceFactory/getTick.inc"
}

#include "ServiceFactory/startup.inc"
}

#include "ServiceFactory/shutdown.inc"
}

} // namespace nsl
