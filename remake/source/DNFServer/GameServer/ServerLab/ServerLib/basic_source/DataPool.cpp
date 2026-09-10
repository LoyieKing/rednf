// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DataPool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <time.h>

#include "DataPool.h"
#include "Message.h"
#include "SendBuffer.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "ISession.h"
#include "WorkThread.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Reactor.h"

namespace nsl {

__int64 Message::ident = 0;

#include "DataPool/DataPool.inc"
}

#include "DataPool/getLogMessage.inc"
}

#include "DataPool/destroyLogMessage.inc"
}

#include "DataPool/createTCPSocket.inc"
}

#include "DataPool/destroyTCPSocket.inc"
}

#include "DataPool/createTCPUser.inc"
}

#include "DataPool/destroyTCPUser.inc"
}

#include "DataPool/GetTcpUserCount.inc"
}

} // namespace nsl
