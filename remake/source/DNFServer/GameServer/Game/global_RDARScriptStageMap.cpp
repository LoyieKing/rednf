// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I_g_stageMapScriptBaseDirectory
//   源 host：InitScriptProducers_5.cpp；新 TU：global_RDARScriptStageMap.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
char g_stageMapScriptBaseDirectory[0x100];           // D 0x093f4900 (initStageMapScript)
