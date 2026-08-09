// nsl::IArea.cpp
#include "IArea.h"
#include "ISession.h"
#include "TraceLog.h"

namespace nsl {

IArea::IArea(int mapIndex)
{
    mMapIndex = mapIndex;
}

IArea::~IArea()
{
}

int IArea::getMemberNum()
{
    return (int)mMapObj.size();
}

bool IArea::regist(unsigned int id, ISession* obj)
{
    std::pair<MAP_OBJECTS_ITER, bool> ret;
    ret = mMapObj.insert(std::make_pair(id, obj));
    if (ret.second != false)
    {
        return true;
    }
    G_TraceLog()->sysLog(7, "IArea Insert Fail-%d, %x\n", id, this);
    return false;
}

void IArea::unregist(unsigned int id)
{
    mMapObj.erase(id);
}

void IArea::notifyAllAreaMember()
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.begin();
    while (mapIter != mMapObj.end())
    {
        ++mapIter;
    }
}

bool IArea::isIterEnd(MAP_OBJECTS_ITER iter)
{
    // ORIG：test+je RET0(al==0 → false); mov $1; jmp EPI; RET0: mov $0。
    // 注意 je 在 al==0 时跳，故语义为 equal→true（与直接 return iter==end 相同）。
    if (iter == mMapObj.end())
    {
        return true;
    }
    return false;
}

MAP_OBJECTS_ITER IArea::getBeginIter()
{
    return mMapObj.begin();
}

ISession* IArea::getValueFromIter(MAP_OBJECTS_ITER iter)
{
    return iter->second;
}

ISession* IArea::getMemberAt(unsigned int id)
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.find(id);
    if (mapIter != mMapObj.end())
    {
        return mapIter->second;
    }
    return NULL;
}

ISession* IArea::getMemberFirst()
{
    MAP_OBJECTS_ITER mapIter;
    if (mMapObj.begin() == mMapObj.end())
    {
        return NULL;
    }
    return mMapObj.begin()->second;
}

} // namespace nsl
