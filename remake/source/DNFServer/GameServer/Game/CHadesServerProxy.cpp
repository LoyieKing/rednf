// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHadesServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN17CHadesServerProxyC2EPcii`（ORIG 0x84708c7）；ctor 已是首强符号。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— CHadesServerProxy（E1 批次）。
// 参照 docs/class_func_reports/CHadesServerProxy.md + docs/hades_protocol.md。
#include "CHadesServerProxy.h"

#include "GlobalData.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "PacketHeader.h"
#include "Packet_Hades_Udp_HeartBeat.h"

#include "CHadesServerProxy/CHadesServerProxy.inc"
}

#include "CHadesServerProxy/HadesSvrInit.inc"
}

#include "CHadesServerProxy/ConnTcpHadesSvr.inc"
}

#include "CHadesServerProxy/InitUdp.inc"
}

#include "CHadesServerProxy/IsConnect.inc"
}

#include "CHadesServerProxy/OnDisconnect.inc"
}

#include "CHadesServerProxy/Parsing.inc"
}

#include "CHadesServerProxy/RunConnTimer.inc"
}

#include "CHadesServerProxy/SendHadesTcpPacket.inc"
}

#include "CHadesServerProxy/SendHadesUdpPacket.inc"
}

#include "CHadesServerProxy/SendHeartBeatPacket.inc"
}
