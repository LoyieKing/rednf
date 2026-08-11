// df_relay_r — RelayService::getTickLog（round-8：独立 TU）
// ORIG 该函数栈帧为 sub $0x4（4 字节对齐），而同一 RelayService.cpp TU 的
// setTick 为 sub $0x8（8 字节对齐）。GCC 4.4.6 无法在同一 TU 内对单函数
// 切换 -mpreferred-stack-boundary，故将 getTickLog 移入本 TU 并以
// -mpreferred-stack-boundary=2 编译（CMake SRC_OPTS），精确复现 ORIG 帧形态。
#include "RelayService.h"

namespace RelayServiceApp
{

long long RelayService::getTickLog()
{
    if (m_tickLog == 0)
    {
        m_tickLog = getTick();
    }
    return m_tickLog;
}

} // namespace RelayServiceApp
