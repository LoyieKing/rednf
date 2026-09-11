// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I__ZN18CharacManageScript9instance_E
//   源 host：CtorProducers_2.cpp；新 TU：RDARScriptCharacManage.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "CharacManageScript.h"
CharacManageScript* CharacManageScript::instance_ = 0;    // _ZN18CharacManageScript9instance_E B 0x094f6b98
