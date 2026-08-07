#ifndef AUCTION_ZONE_H_
#define AUCTION_ZONE_H_

#include "IArea.h"

class GSArea : public nsl::IArea
{
public:
    GSArea(int mapIndex)
        : nsl::IArea(mapIndex)
    {
        mBackServerKey = 0;
        mSpareKey = 0;
        mbServiceRunning = false;
    }
    bool IsServiceRunning()
    {
        return mbServiceRunning;
    }

    int mBackServerKey;
    int mSpareKey;
    bool mbServiceRunning;
};

class Zone
{
public:
    Zone();
    ~Zone();

    nsl::IArea* mArea[5];
};

Zone* G_Zone();

#endif // AUCTION_ZONE_H_
