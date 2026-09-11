// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I__ZN8WongWork13CSkillChanger16g_nStatUpItemIdxE
//   源 host：SkillSlot.cpp；新 TU：SkillChanger.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "SkillSlot.h"
namespace WongWork
{
int CSkillChanger::g_nStatUpItemIdx[9] = {8, 0x40f, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416};
}  // namespace WongWork
