// Zone.cpp
#include "Zone.h"
#include "GlobalInstance.h"

int GSArea::GetSpareKey()
{
    return ++mSpareKey;
}

void GSArea::SetServiceRunning(bool bRun)
{
    mbServiceRunning = bRun;
}

Zone::Zone()
{
    GSArea* pArea = new GSArea(0);
    mArea[0] = pArea;
}

Zone::~Zone()
{
    delete mArea[0];
}

Zone* G_Zone()
{
    return nsl::GlobalInstance<Zone>::inst_ptr();
}
