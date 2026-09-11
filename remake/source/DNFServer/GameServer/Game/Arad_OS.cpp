// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_Dispatcher.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD16GetQuarterOfYearEPc
//   ORIG TU     ：Arad_OS.cpp
//   .inc 为原样搬移（含原 TU 中的函数体，零语义改动）；本壳只承载 TU 级外壳。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "Arad_Dispatcher.h"

namespace ARAD
{

#include "Arad_OS/GetQuarterOfYear.inc"

}  // namespace ARAD
