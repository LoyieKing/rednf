// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I_g_independentDropScriptBaseDirectory
//   源 host：InitScriptProducers_5.cpp；新 TU：RDARScriptIndependentDrop.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
char g_independentDropScriptBaseDirectory[0x100];    // B 0x094f7840 (initIndependentDropScript)
