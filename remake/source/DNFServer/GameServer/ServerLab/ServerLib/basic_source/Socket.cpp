// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Socket/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "TCPSocket.h"
#include "TraceLog.h"
#include "Script.h"

namespace nsl {

int TCPSocket::msRecvBufSize = 0;
int TCPSocket::msSendBufSize = 0;
const int TCPSocket::MAX_SEND_RETRY = 0x65;

#include "Socket/TCPSocket.inc"
}

#include "Socket/dtor_TCPSocket.inc"
}

#include "Socket/open.inc"
}

#include "Socket/bind.inc"
}

#include "Socket/listen.inc"
}

#include "Socket/send.inc"
}

#include "Socket/recv.inc"
}

#include "Socket/shutdown.inc"
}

#include "Socket/close.inc"
}

#include "Socket/setOptNonBlock.inc"
}

#include "Socket/setOptReuseAdrs.inc"
}

#include "Socket/setOptLinger.inc"
}

#include "Socket/setOptNagle.inc"
}

#include "Socket/setOptResizeSendBuf.inc"
}

#include "Socket/setOptResizeRecvBuf.inc"
}

#include "Socket/connect.inc"
}

#include "Socket/connect_nonb.inc"
}

#include "Socket/accept.inc"
}

#include "Socket/getPeerAdrs.inc"
}

#include "Socket/getPeerPort.inc"
}

#include "Socket/getHandle.inc"
}

#include "Socket/pollReadEvent.inc"
}

#include "Socket/pollWriteEvent.inc"
}

#include "Socket/pollErrorEvent.inc"
}

#include "Socket/UDPSocket.inc"
}

#include "Socket/dtor_UDPSocket.inc"
}

#include "Socket/open_1.inc"
}

#include "Socket/bind_1.inc"
}

#include "Socket/bind_2.inc"
}

#include "Socket/setOptNonBlock_1.inc"
}

#include "Socket/send_1.inc"
}

#include "Socket/recv_1.inc"
}

#include "Socket/getHandle_1.inc"
}

#include "Socket/pollReadEvent_1.inc"
}

#include "Socket/pollWriteEvent_1.inc"
}

#include "Socket/pollErrorEvent_1.inc"
}

#include "Socket/close_1.inc"
}

#include "Socket/setOptResizeSendBuf_1.inc"
}

#include "Socket/setOptResizeRecvBuf_1.inc"
}

#include "Socket/SetRecvBufSize.inc"
}

#include "Socket/SetSendBufSize.inc"
}

} // namespace nsl
