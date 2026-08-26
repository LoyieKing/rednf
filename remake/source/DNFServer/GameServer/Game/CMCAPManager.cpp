#include "CMCAPManager.h"
#include <algorithm>

#include "GameTypes.h"   // cMyTrace

namespace WongWork
{
// ORIG 符号：_ZN7WongWork12CMCAPManager5resetEv (弱, 0x8694fa0)
void CMCAPManager::reset() {}  // TODO(G2)

// ORIG T 0x0822eea4：max<unsigned>(min<unsigned>(10000, m_point), 0)
// ORIG W 0x08695038：m_point += 1。
void CMCAPManager::incExposedCount()
{
    m_point = m_point + 1;
}

unsigned int CMCAPManager::getPoint() const
{
    unsigned int cap = 0x2710;
    unsigned int floor = 0;
    unsigned int capped = std::min(cap, m_point);
    return std::max(floor, capped);
}

// ORIG T 0x0822eede：读 +0x14
bool CMCAPManager::isSaveCleanPadPoint() const
{
    return m_saveCleanPadPoint;
}

// ORIG T 0x0822eeea：写 +0x14=0
void CMCAPManager::disableCleanPadPoint()
{
    m_saveCleanPadPoint = false;
}

// ORIG T 0x080f8be0：trace 后把 0xf3c 个 int 拷贝到 pointPerHackType_。
void CMCAPManager::setPointPerHackType(int* data)
{
    cMyTrace trace(
        "static void WongWork::CMCAPManager::setPointPerHackType(int*)",
        0x1d2, 0);
    trace("Load cleanpad point per hack type");
    for (int i = 0; i <= 0xf3b; ++i)
        pointPerHackType_[i] = data[i];
}
}  // namespace WongWork
