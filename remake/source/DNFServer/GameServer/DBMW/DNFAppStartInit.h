#ifndef DBMW_DNFAPPSTARTINIT_H_
#define DBMW_DNFAPPSTARTINIT_H_

#include "DBMWCommon.h"


class CAppInit
{
public:
    CAppInit();
    virtual ~CAppInit();
    // ORIG _ZTV8CAppInit slot[2] = __cxa_pure_virtual（且 ORIG 无 _ZN8CAppInit4Init… 符号）
    // ⇒ 本类是纯接口，实现在 CAppStartInit / CAppStopInit；原空壳 {} 违反「禁止空壳充数」。
    virtual void Init(CApplication* app, int argc, char** argv) = 0;
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
