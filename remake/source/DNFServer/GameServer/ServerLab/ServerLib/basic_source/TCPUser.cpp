// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TCPUser/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <stdio.h>

#include "TCPUser.h"
#include "TCPSocket.h"
#include "ISession.h"
#include "WorkThread.h"
#include "RecvBuffer.h"
#include "TraceLog.h"
#include "Script.h"
#include "ServiceFactory.h"
#include "DataPools.h"
#include "DataPool.h"

namespace nsl {

static unsigned int user_count = 0;
int ddebug = 0;

#include "TCPUser/TCPUser.inc"
}

#include "TCPUser/dtor_TCPUser.inc"
}

#include "TCPUser/initialize.inc"
}

#include "TCPUser/shutdown.inc"
}

#include "TCPUser/startupAfterSetSocket.inc"
}

#include "TCPUser/setSession.inc"
}

#include "TCPUser/getSession.inc"
}

#include "TCPUser/setSendDataType.inc"
}

#include "TCPUser/getSendDataType.inc"
}

#include "TCPUser/setRecvDataType.inc"
}

#include "TCPUser/getRecvDataType.inc"
}

#include "TCPUser/setSocket.inc"
}

#include "TCPUser/getSocket.inc"
}

#include "TCPUser/getHandle.inc"
}

#include "TCPUser/setNeedReconnect.inc"
}

#include "TCPUser/getNeedReconnect.inc"
}

#include "TCPUser/IncPendingWorkNum.inc"
}

#include "TCPUser/DecPendingWorkNum.inc"
}

#include "TCPUser/IncPendingSendNum.inc"
}

#include "TCPUser/DecPendingSendNum.inc"
}

#include "TCPUser/setBindedSession.inc"
}

#include "TCPUser/SetSending.inc"
}

#include "TCPUser/SetWorking.inc"
}

#include "TCPUser/postDisconnected.inc"
}

#include "TCPUser/setLastAccessTime.inc"
}

#include "TCPUser/getLastAccessTime.inc"
}

#include "TCPUser/isAboutToDisconnect.inc"
}

#include "TCPUser/isDisconnected.inc"
}

#include "TCPUser/IsSending.inc"
}

#include "TCPUser/IsWorking.inc"
}

#include "TCPUser/isPassiveCloseSyncByWorker.inc"
}

#include "TCPUser/setPassiveSyncByWorker.inc"
}

#include "TCPUser/isActiveCloseSyncByWorker.inc"
}

#include "TCPUser/setActiveSyncByWorker.inc"
}

#include "TCPUser/onPassiveClose.inc"
}

#include "TCPUser/onActiveClose.inc"
}

#include "TCPUser/getWorkThread.inc"
}

#include "TCPUser/onRead.inc"
}

#include "TCPUser/onWriteByCMsg.inc"
}

#include "TCPUser/PopSendMessage.inc"
}

#include "TCPUser/ClearRecvMsgs.inc"
}

#include "TCPUser/onError.inc"
}

} // namespace nsl
