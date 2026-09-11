// ============================================================================
// [SplitC 2026-09-11 · TU 迁移（ORIG GI 锚点）] 本文件由 CCeraShopAddRestrict.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN19CerashopAddRestrict8readTimeEPs
//   ORIG TU     ：RDARScriptCeraShopAddRestrictType.cpp
//   .inc 为原样搬移（零文本改动）；本壳只承载 TU 级外壳 + 依赖头。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>

#include "LogManager.h"

// ORIG T 0x08abf0b0（定义在 CCeraShopAddRestrict.cpp；本 TU 仅取声明）。
int ScanInt(bool* ok);

namespace CerashopAddRestrict
{

#include "RDARScriptCeraShopAddRestrictType/readTime.inc"

}  // namespace CerashopAddRestrict
