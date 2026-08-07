// Zone.cpp
#include "Zone.h"
#include "GlobalInstance.h"

Zone::Zone()
{
    GSArea* pArea = new GSArea(0);
    mArea[0] = pArea;
}

Zone::~Zone()
{
    if (mArea[0] != NULL)
    {
        delete mArea[0];
    }
}

Zone* G_Zone()
{
    return nsl::GlobalInstance<Zone>::inst_ptr();
}
