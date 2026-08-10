#ifndef DNF_APP_STOP_INIT_H_
#define DNF_APP_STOP_INIT_H_

#include "DNFAppStartInit.h"

class CApplication;

// ---- CAppStopInit ----
class CAppStopInit : public CAppInit
{
public:
    CAppStopInit();
    virtual ~CAppStopInit();
    virtual void Init(CApplication* app, int argc, char** argv);
};

#endif  // DNF_APP_STOP_INIT_H_
