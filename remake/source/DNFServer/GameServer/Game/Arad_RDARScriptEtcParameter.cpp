// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_Script.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD6SCRIPT8readTimeEPs
//   ORIG TU     ：Arad_RDARScriptEtcParameter.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "Arad_Script.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "CDataManager.h"               // ARAD::SCRIPT::AradJumping*（m_aradJumping 权威段）
#include <ctime>
#include "DNFLexWrapper.h"               // loadRDARScriptFile / ScanType / ScanInt(bool*)
#include "LogManager.h"                  // LogManager::logFormat(int, file, func, line, fmt, ...)
#include "Arad_EventPeriodDataManager.h" // SEventPeriodData / SRewardPeriodData / readTime 声明

namespace ARAD
{
namespace SCRIPT
{
#include "Arad_RDARScriptEtcParameter/readTime.inc"
}  // namespace SCRIPT
}  // namespace ARAD
