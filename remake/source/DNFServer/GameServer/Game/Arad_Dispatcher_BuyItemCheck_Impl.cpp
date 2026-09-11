// ============================================================================
// [SplitC 2026-09-11 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_Dispatcher.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD33Arad_Dispatcher_BuyItemCheck_ImplC2Ev
//   ORIG TU     ：Arad_Dispatcher_BuyItemCheck_Impl.cpp
//   .inc 为原样搬移（零文本改动）；本壳只承载 TU 级外壳 + 依赖头。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
//   ⚠ 原 TU 内「基类 ctor 定义可见（P210 帧继承）」条件随拆分消失 = 本批 P210 探针件。
// ============================================================================
#include <iostream>

#include "Arad_Dispatcher.h"

namespace ARAD
{

#include "Arad_Dispatcher_BuyItemCheck_Impl/Arad_Dispatcher_BuyItemCheck_Impl.inc"

}  // namespace ARAD
