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
    // ORIG：无命名局部（new 结果寄存器直存 mArea[0]）。
    mArea[0] = new GSArea(0);
}

Zone::~Zone()
{
    delete mArea[0];
}

Zone* G_Zone()
{
    return nsl::GlobalInstance<Zone>::inst_ptr();
}
