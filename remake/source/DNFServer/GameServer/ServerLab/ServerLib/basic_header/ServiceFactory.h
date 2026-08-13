#ifndef NSL_SERVICEFACTORY_H_
#define NSL_SERVICEFACTORY_H_

#include <string.h>

#include "Threads.h"
#include "Dispatchers.h"
#include "DataPools.h"
#include "Reactor.h"
#include "IHandlers.h"
#include "DBConnections.h"
#include "EncyptTools.h"
#include "IActiveConManager.h"
#include "PortInfo.h"

namespace nsl {

typedef unsigned long long __int64;

class ServiceFactory
{
public:
    ServiceFactory(char* Service_identify, char* ConfigFileName);
    ~ServiceFactory();
    int startup();
    int shutdown();
    void setTick();
    __int64 getTick() const;

    PortInfo super_PortInfo;
    Dispatchers super_Dispatchers;
    Threads super_Threads;
    DataPools super_DataPools;
    Reactor super_Reactor;
    EncyptTools super_EncyptTools;
    IActiveConManager super_IActiveConManager;
    IHandlers super_IHandlers;
    DBConnections super_DBConnections;
    __int64 tick_;
    char ServiceFactoryName[21];
    char ServiceConfigName[81];
};

extern ServiceFactory* pApp;

} // namespace nsl

#endif // NSL_SERVICEFACTORY_H_
