// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMCAPManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CMCAPManager.h"
#include <algorithm>

#include "GameTypes.h"   // cMyTrace

namespace WongWork
{
// ORIG W 0x8694fa0：+0x00=1、+0x04=1000、+0x08=+0x00（当前倍率拷贝）、
// +0x0c=1、+0x10=0、+0x14=0（逐 dword/byte 直写，无 memset）。
#include "CMCAPManager/reset.inc"
}

// ORIG T 0x0822eea4：max<unsigned>(min<unsigned>(10000, m_point), 0)
// ORIG W 0x08695038：m_field10(+0x10) += 1。
#include "CMCAPManager/incExposedCount.inc"
}

// ORIG 仅非 const getPoint（W 0x0822eea4，读 this+0x00）；const 版为发明，已删。

// ORIG T 0x0822eeea：写 +0x14=0
#include "CMCAPManager/disableCleanPadPoint.inc"
}

// ORIG T 0x080f8be0：trace 后把 0xf3c 个 int 拷贝到 pointPerHackType_。
#include "CMCAPManager/setPointPerHackType.inc"
}
}  // namespace WongWork
