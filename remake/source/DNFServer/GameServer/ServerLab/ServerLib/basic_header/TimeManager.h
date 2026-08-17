#ifndef NSL_TIMEMANAGER_H_
#define NSL_TIMEMANAGER_H_

#include "RBTree.h"

namespace nsl {

class ITimeEntity;
class InternalMsg;

typedef unsigned long long __int64;

class TimeManager
{
public:
    TimeManager();
    ~TimeManager();
    void onTime();
    // ORIG：头内 inline 空实现（point 无 out-of-line 符号）
    void preReservedMap()
    {
    }
    void insert2PeriodQueue(ITimeEntity* ent);
    void delete2PeriodQueue(ITimeEntity* ent);

    RBTree<int, ITimeEntity*, 1024>::RBIterator<changes_meaning::RBNode<int, ITimeEntity*, 1024>, ITimeEntity*> iter;
    RBTree<int, ITimeEntity*, 1024> timePeriodMap;
    int mtotal_entity;
    __int64 last_tick;
    __int64 cur_tick;
    __int64 elapsedTick;
};

} // namespace nsl

#endif // NSL_TIMEMANAGER_H_
