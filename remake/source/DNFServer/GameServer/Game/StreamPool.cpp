// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StreamPool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CStreamGuard.h"   // StreamPool::Free(CStreamGuard&) 取流用
// df_game_r 还原 —— StreamPool / SmallStreamPool / BigStreamPool（S1 组）
#include "CEnvironment.h"
#include "StreamPool.h"

#include "StreamPool/StreamPool.inc"
}

#include "StreamPool/dtor_StreamPool.inc"
}

#include "StreamPool/Acquire.inc"
}

#include "StreamPool/Free.inc"
}

// ORIG _ZN10StreamPool4FreeER12CStreamGuard W 0x08101080（弱内联，无独立报告）：
// objdump 0x8101080——取 guard 持有的 Stream* 后复用 Free(Stream*)。
#include "StreamPool/Free_1.inc"
}

#include "StreamPool/Size.inc"
}

#include "StreamPool/Remain.inc"
}

#include "StreamPool/SmallStreamPool.inc"
}

#include "StreamPool/dtor_SmallStreamPool.inc"
}

#include "StreamPool/Acquire_1.inc"
}

#include "StreamPool/Free_2.inc"
}

#include "StreamPool/Size_1.inc"
}

#include "StreamPool/Remain_1.inc"
}

#include "StreamPool/BigStreamPool.inc"
}

#include "StreamPool/dtor_BigStreamPool.inc"
}

#include "StreamPool/Acquire_2.inc"
}

#include "StreamPool/Free_3.inc"
}

#include "StreamPool/Size_2.inc"
}

#include "StreamPool/Remain_2.inc"
}
