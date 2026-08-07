// Rebuilt from df_auction_r DWARF (ExpireTimeDictionary.cpp CU), 2026-08-08
#include <deque>

#include "ExpireTimeDictionary.h"

ExpireTimeDictionary::ExpireTimeDictionary()
    : mExpireTimeDicDataPool(0x20),
      mExpireTimeDicQueue(std::deque<ExpireTimeDictionaryData*>())
{
}

ExpireTimeDictionary::~ExpireTimeDictionary()
{
}

int ExpireTimeDictionary::PushRegistedItem(long expireTime, __int64 auctionId,
                                           ExpireTimeDictionaryData** pOutEtData)
{
    ExpireTimeDictionaryData* ptr_data = mExpireTimeDicDataPool.malloc();
    if (ptr_data == NULL)
    {
        return 9;
    }
    ptr_data->expire_time = expireTime;
    ptr_data->auction_id = auctionId;
    mExpireTimeDicQueue.push(ptr_data);
    *pOutEtData = ptr_data;
    return 0;
}

int ExpireTimeDictionary::PeekMostRecentExpireItem(ExpireTimeDictionaryData** pOutEtData)
{
    if (mExpireTimeDicQueue.empty())
    {
        return 0x2c;
    }
    *pOutEtData = mExpireTimeDicQueue.front();
    return 0;
}

int ExpireTimeDictionary::PopMostRecentExpireItem()
{
    if (mExpireTimeDicQueue.empty())
    {
        return 0x2c;
    }
    ExpireTimeDictionaryData* ptr_data = mExpireTimeDicQueue.front();
    mExpireTimeDicQueue.pop();
    mExpireTimeDicDataPool.free(ptr_data);
    return 0;
}
