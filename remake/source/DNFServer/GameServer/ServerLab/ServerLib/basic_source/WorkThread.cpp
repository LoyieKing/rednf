// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WorkThread/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <pthread.h>

#include "WorkThread.h"
#include "Message.h"
#include "TCPUser.h"
#include "TraceLog.h"
#include "Script.h"
#include "ServiceFactory.h"
#include "Dispatchers.h"
#include "Threads.h"
#include "TimerThread.h"
#include "TCPDispatcher.h"
#include "InterDispatcher.h"
#include "CommonDataPool.h"
#include "ActiveNetClose.h"
#include "TMsgCell.h"
#include "GetMessageBuffer.h"

namespace nsl {

__thread unsigned int tlsThreadId;

#include "WorkThread/WorkThread.inc"
}

#include "WorkThread/dtor_WorkThread.inc"
}

#include "WorkThread/SetArea.inc"
}

#include "WorkThread/createOrderPool.inc"
}

#include "WorkThread/destroyOrderPool.inc"
}

#include "WorkThread/PushTransaction.inc"
}

#include "WorkThread/PopTransaction.inc"
}

#include "WorkThread/GetThreadId.inc"
}

#include "WorkThread/GetQueueSizeNoLock.inc"
}

#include "WorkThread/GetTransactionCntPerSec.inc"
}

#include "WorkThread/InitTransactionCntPerSec.inc"
}

#include "WorkThread/loop.inc"
}

} // namespace nsl
