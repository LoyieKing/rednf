#include "CMCAPManager.h"
#include <algorithm>

#include "GameTypes.h"   // cMyTrace

namespace WongWork
{
// ORIG W 0x8694fa0：+0x00=1、+0x04=1000、+0x08=+0x00（当前倍率拷贝）、
// +0x0c=1、+0x10=0、+0x14=0（逐 dword/byte 直写，无 memset）。
void CMCAPManager::reset()
{
    m_point = 1;
    m_rate = 0x3e8;
    m_field08 = static_cast<int>(m_point);
    m_field0c = 1;
    m_field10 = 0;
    m_saveCleanPadPoint = false;
}

// ORIG T 0x0822eea4：max<unsigned>(min<unsigned>(10000, m_point), 0)
// ORIG W 0x08695038：m_field10(+0x10) += 1。
void CMCAPManager::incExposedCount()
{
    m_field10 = m_field10 + 1;
}

// ORIG 仅非 const getPoint（W 0x0822eea4，读 this+0x00）；const 版为发明，已删。

// ORIG T 0x0822eeea：写 +0x14=0
void CMCAPManager::disableCleanPadPoint()
{
    m_saveCleanPadPoint = false;
}

// ORIG T 0x080f8be0：trace 后把 0xf3c 个 int 拷贝到 pointPerHackType_。
void CMCAPManager::setPointPerHackType(int* data)
{
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x1d2, 0);
    trace("Load cleanpad point per hack type");
    for (int i = 0; i <= 0xf3b; ++i)
        pointPerHackType_[i] = data[i];
}
}  // namespace WongWork
