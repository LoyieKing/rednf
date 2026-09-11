// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 TaiwanCash.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN6Taiwan10TaiwanCashC2Ehs
//   ORIG TU     ：TaiwanCash.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "TaiwanCash.h"
#include <cstdio>
#include <cstring>
#include "CEnvironment.h"  // G_CEnvironment / STDBConnInfo / DB_HANDLE_E（InitBillingCash）
#include "LogManager.h"    // Initialize 失败路径 logFormat（ORIG IPGMgr.cpp 0x98 行文案）
#include "DNFFunctionLib.h"  // NumberToString(ull,int)（billing trace 直调）
#include "TaiwanQueries.h"

namespace Taiwan
{
#include "TaiwanCashOrig/TaiwanCash.inc"
}  // namespace Taiwan
