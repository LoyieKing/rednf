#ifndef DNFAPPSTOPINIT_H_
#define DNFAPPSTOPINIT_H_

#include <string>

#include "PacketHeader.h"
#include "DNFAppStartInit.h"

class CAppStopInit;
class CApplication;

// from GuildInit.h
class CAppStopInit : public CAppInit
{
public:
    CAppStopInit();
    virtual ~CAppStopInit();
    virtual void Init(CApplication* app, int argc, char** argv);
};

#endif
