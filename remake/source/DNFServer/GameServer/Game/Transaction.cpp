// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I__ZN5nexon4cash18TransactionManager6tx_id_E
//   源 host：nexon_cash.cpp；新 TU：Transaction.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "nexon_cash.h"
namespace nexon
{
namespace cash
{
int TransactionManager::tx_id_ = 0;             // B 0x09407d90
}  // namespace cash
}  // namespace nexon
