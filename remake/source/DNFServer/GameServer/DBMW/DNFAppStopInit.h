#ifndef DBMW_DNFAPPSTOPINIT_H_
#define DBMW_DNFAPPSTOPINIT_H_

#include "DBMWCommon.h"


class CAppStopInit : public CAppInit
{
public:
    CAppStopInit();
    virtual ~CAppStopInit();
    virtual void Init(CApplication* app, int argc, char** argv);
};

#endif  // DBMW_DNFAPPSTOPINIT_H_
