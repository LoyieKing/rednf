// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUdpHandler/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CUdpHandler（UDP 句柄，ORIG 0x8483518 族，8 函数）。
// 参照 docs/class_func_reports/CUdpHandler.md；getErrno 同 TU（ORIG 0x8483e75）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CUdpHandlerC2Ev`（ORIG 0x8483e59）；键名符号需前移（首定义现为 getErrno）。
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CUdpHandler.h"
#include "GameTypes.h"

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x8483e59 `_ZN11CUdpHandlerC2Ev`。
#include "CUdpHandler/CUdpHandler.inc"
}

#include "CUdpHandler/getErrno.inc"
}

#include "CUdpHandler/InetAddr.inc"
}

#include "CUdpHandler/InitClientSocket.inc"
}

#include "CUdpHandler/InitServerSocket.inc"
}

#include "CUdpHandler/RecvFromClient.inc"
}

#include "CUdpHandler/RecvFromServer.inc"
}

#include "CUdpHandler/SendToClient.inc"
}

#include "CUdpHandler/SendToServer.inc"
}
