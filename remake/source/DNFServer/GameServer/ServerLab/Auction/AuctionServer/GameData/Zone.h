#ifndef AUCTION_ZONE_H_
#define AUCTION_ZONE_H_

#include "IArea.h"

class GSArea : public nsl::IArea
{
public:
    GSArea(int mapIndex)
        : nsl::IArea(mapIndex)
    {
        mSpareKey = 0x61a80;
        mbServiceRunning = false;
    }
    virtual ~GSArea()
    {
    }
    virtual void setBackServerKey(int key)
    {
        mBackServerKey = key;
    }
    virtual int getBackServerKey()
    {
        return mBackServerKey;
    }
    int GetSpareKey();
    void SetServiceRunning(bool bRun);
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
