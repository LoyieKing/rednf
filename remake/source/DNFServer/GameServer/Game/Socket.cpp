// ============================================================================
// [SplitC 2026-09-11 batch5 · 数据类 TU 迁移（ORIG GI 锚点）] 数据 TU：_GLOBAL__I__ZN5nexon4cash9TCPSocket14MAX_SEND_RETRYE
//   源 host：nexon_cash.cpp；新 TU：Socket.cpp（配方见 /tmp/lane_data26/report.md §4）
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "nexon_cash.h"
namespace nexon
{
namespace cash
{
const unsigned int TCPSocket::MAX_SEND_RETRY;  // R 0x08bb49e0（类内 =0x64 的类外定义，发射 R 符号）
}  // namespace cash
}  // namespace nexon
