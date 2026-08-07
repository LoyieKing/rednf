// Rebuilt from df_auction_r DWARF (ExpireTimeDictionary.cpp CU), 2026-08-08
#ifndef EXPIRETIMEDICTIONARY_H_
#define EXPIRETIMEDICTIONARY_H_

#include <queue>

#include "ObjectPoolByBoostPool.h"

typedef unsigned long long __int64;

class ExpireTimeDictionary
{
public:
    struct ExpireTimeDictionaryData
    {
        long expire_time;      // @0
        __int64 auction_id;    // @4
    };

    ExpireTimeDictionary();
    virtual ~ExpireTimeDictionary();
    int PushRegistedItem(long expireTime, __int64 auctionId,
                         ExpireTimeDictionaryData** pOutEtData);
    int PeekMostRecentExpireItem(ExpireTimeDictionaryData** pOutEtData);
    int PopMostRecentExpireItem();

private:
    nsl::object_pool_by_boost_pool<ExpireTimeDictionaryData, int, int, int>
        mExpireTimeDicDataPool;                                   // @4
    std::queue<ExpireTimeDictionaryData*> mExpireTimeDicQueue;    // @36
};

#endif // EXPIRETIMEDICTIONARY_H_
