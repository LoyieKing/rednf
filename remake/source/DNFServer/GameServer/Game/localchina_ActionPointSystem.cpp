// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I__ZN8APSystem19CActionPointManager7_IsGMOnE
//   源 host：APSystemOnlyOrig.cpp；新 TU：localchina_ActionPointSystem.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "CUser.h"
namespace APSystem
{
bool CActionPointManager::_IsGMOn;                                // ORIG B 0x093ff07c
}  // namespace APSystem
