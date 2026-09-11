// ============================================================================
// [SplitC 2026-09-11 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_Dispatcher.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD23Arad_Dispatcher_ManagerC2Ev
//   ORIG TU     ：Arad_Dispatch_Manager.cpp
//   .inc 为原样搬移（零文本改动）；本壳只承载 TU 级外壳 + 依赖头。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；其 __ioinit 为
//   weak，不参与 GI 命名，故键取本 TU 首个强外联定义 = Manager ctor（C2 形）。
// ============================================================================
#include <iostream>

#include "Arad_Dispatcher.h"

namespace ARAD
{

#include "Arad_Dispatch_Manager/Arad_Dispatcher_Manager.inc"

}  // namespace ARAD
