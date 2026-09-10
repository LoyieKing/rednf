// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DbmwClient/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// DbmwClient.cpp —— 按报告 docs/class_func_reports/DbmwClient/*.md 忠实转写。
#include "DbmwClient.h"

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10DbmwClientC2Ev 的前提，P338）
#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"

// ===================== 生命周期 =====================

// ORIG @0x08120454：+0=+1=+2=0、-4=0、+8=-1、new(0x200)、
// +0x1038=0、memset(+0x34,0,0x1001)。
#include "DbmwClient/DbmwClient.inc"
}

// ORIG T _ZN10DbmwClientD1Ev/D2Ev @0x081204be（0x3c）：
// m_connected(+0x02)=0 → close(m_sock(+0x08)) → m_initialized(+0x01)=0 → m_sock=-1
// → operator delete(m_recvBuf(+0x2c))。【Main 补定义：ORIG 4.1 -O0 无条件发射，历轮误删】
#include "DbmwClient/dtor_DbmwClient.inc"
}

// ORIG T _ZN10DbmwClient9set_queryEPKcz @0x081204fa（0xec）：fmt 空返 false；
// memset(m_query,0,0x1001) → vsprintf → m_queryLen=strlen；越界（>0xfff 或 <=7）
// printf("QueryLength Size ERROR QUERY_SIZE-%d MAX",len) 返 false；否则
// strncasecmp(m_query,"select",6)==0 → m_isSelect=1/0，返 true。
#include "DbmwClient/set_query.inc"
}

#include "DbmwClient/Init.inc"
}

#include "DbmwClient/SetServerAddress.inc"
}
#include "DbmwClient/SendHeader.inc"
}

#include "DbmwClient/RegistToMiddleware.inc"
}

#include "DbmwClient/ReceiveRegistAck.inc"
}

#include "DbmwClient/Regist.inc"
}

#include "DbmwClient/Connect.inc"
}

#include "DbmwClient/Close.inc"
}

#include "DbmwClient/ReadData.inc"
}

#include "DbmwClient/DoQuery.inc"
}

#include "DbmwClient/DoReceive.inc"
}

#include "DbmwClient/Reconnect.inc"
}

#include "DbmwClient/exec.inc"
}

#include "DbmwClient/getAffectedRowCount.inc"
}

// IsInitialized/IsConnected 头内联（W）——cpp 不再重复定义。

#include "DbmwClient/Error.inc"
}
