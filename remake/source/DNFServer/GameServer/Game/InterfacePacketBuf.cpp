// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InterfacePacketBuf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "InterfacePacketBuf.h"

#include "InterfacePacketBuf/InterfacePacketBuf.inc"
}

#include "InterfacePacketBuf/dtor_InterfacePacketBuf.inc"
}

// throw() 与头声明同步（见 InterfacePacketBuf.h 的 ORIG EH 形证据注释）
#include "InterfacePacketBuf/AcquirePacketBuf.inc"
}

#include "InterfacePacketBuf/bind_packet.inc"
}

#include "InterfacePacketBuf/clear.inc"
}

#include "InterfacePacketBuf/finalize.inc"
}

#include "InterfacePacketBuf/get.inc"
}

#include "InterfacePacketBuf/getLastError.inc"
}

#include "InterfacePacketBuf/get_binary.inc"
}

#include "InterfacePacketBuf/get_index.inc"
}

#include "InterfacePacketBuf/get_int.inc"
}

#include "InterfacePacketBuf/get_len.inc"
}

#include "InterfacePacketBuf/get_packet.inc"
}

#include "InterfacePacketBuf/get_short.inc"
}

#include "InterfacePacketBuf/is_finallized.inc"
}

#include "InterfacePacketBuf/put_binary.inc"
}

#include "InterfacePacketBuf/put_binary_1.inc"
}

#include "InterfacePacketBuf/put_byte.inc"
}

#include "InterfacePacketBuf/put_byte_1.inc"
}

#include "InterfacePacketBuf/put_header.inc"
}

#include "InterfacePacketBuf/put_int.inc"
}

#include "InterfacePacketBuf/put_int_1.inc"
}

#include "InterfacePacketBuf/put_item_idx.inc"
}

#include "InterfacePacketBuf/put_packet.inc"
}

#include "InterfacePacketBuf/put_short.inc"
}

#include "InterfacePacketBuf/put_short_1.inc"
}

#include "InterfacePacketBuf/put_str.inc"
}

#include "InterfacePacketBuf/put_str_1.inc"
}

#include "InterfacePacketBuf/set_index.inc"
}
