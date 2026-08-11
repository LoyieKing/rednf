#ifndef DBMW_DNFAPPSTARTINIT_H_
#define DBMW_DNFAPPSTARTINIT_H_

#include "DBMWCommon.h"


class CAppInit
{
public:
    CAppInit();
    virtual ~CAppInit();
    virtual void Init(CApplication* app, int argc, char** argv) {}
};

class CAppStartInit : public CAppInit
{
public:
    CAppStartInit();
    virtual ~CAppStartInit();
    virtual void Init(CApplication* app, int argc, char** argv);
    int Init_Daemon(int argc, char** argv);
    bool Save_pid(const std::string& path);
};

#endif  // DBMW_DNFAPPSTARTINIT_H_
