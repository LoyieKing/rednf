#ifndef DNFAPPSTARTINIT_H_
#define DNFAPPSTARTINIT_H_

#include <string>

#include "PacketHeader.h"
#include "DNFAppStartInit.h"

class CAppInit;
class CAppStartInit;
class CApplication;

// from GuildInit.h
class CAppInit
{
public:
    CAppInit();
    virtual ~CAppInit();
    virtual void Init(CApplication* app, int argc, char** argv) = 0;
    void Load(CApplication* app, int argc, char** argv);
    void Free(CApplication* app);
};

// from GuildInit.h
class CAppStartInit : public CAppInit
{
public:
    CAppStartInit();
    virtual ~CAppStartInit();
    virtual void Init(CApplication* app, int argc, char** argv);
    int Init_Daemon(int argc, char** argv);
    bool Save_pid(const std::string& name);
};

#endif
