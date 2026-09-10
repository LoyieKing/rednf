// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BaseServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r 还原 —— BaseServerProxy（E1 批次）。
// 参照 docs/class_func_reports/BaseServerProxy.md。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15BaseServerProxyC2EPciS0_`（ORIG 0x82fdd4f）；ctor 已是首强符号。
#include <iostream>

#include "BaseServerProxy.h"

#include <errno.h>
#include <string.h>

#include "GameBase.h"

// ============================================================================
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   InitTcp 0x082fd47c  DIFF(171/167) P167 CLEAR(25/25)；本 TU 经
//     dnf_game_cxx.sh 路由 C6446R（4.4.6 后端，N7 敏感族翻正）；残差 =
//     errnoVal/ret 寄存器互换（路由表注释已记，P230 N7 边界）⇒ N 封顶。
// ============================================================================

#include "BaseServerProxy/BaseServerProxy.inc"
}

#include "BaseServerProxy/dtor_BaseServerProxy.inc"
}

#include "BaseServerProxy/OnDisconnect.inc"
}

#include "BaseServerProxy/Init.inc"
}

#include "BaseServerProxy/InitTcp.inc"
}

#include "BaseServerProxy/Disconnect.inc"
}

#include "BaseServerProxy/SendTcpPacket.inc"
}

#include "BaseServerProxy/OnTcpRecv.inc"
}

#include "BaseServerProxy/OnTcpSend.inc"
}

#include "BaseServerProxy/OnTcpError.inc"
}
