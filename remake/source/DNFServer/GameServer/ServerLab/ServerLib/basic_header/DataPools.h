#ifndef NSL_DATAPOOLS_H_
#define NSL_DATAPOOLS_H_

#include <stdio.h>

namespace nsl {

class DataPool;
class CommonDataPool;

class DataPools
{
public:
    DataPools()
    {
        puts("Start SessionPools~");
    }
    DataPool* getDataPool()
    {
        return dataPool_;
    }
    void setDataPool(DataPool* pDataPool)
    {
        dataPool_ = pDataPool;
    }
    CommonDataPool* getCommonDataPool(int idx)
    {
        return commonDataPool_[idx];
    }
    void setCommonDataPool(int idx, CommonDataPool* pPool)
    {
        commonDataPool_[idx] = pPool;
    }

    DataPool* dataPool_;
    CommonDataPool* commonDataPool_[5];
};

} // namespace nsl

#endif // NSL_DATAPOOLS_H_
