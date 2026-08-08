#ifndef SERVERLIBRARY2_0_H_
#define SERVERLIBRARY2_0_H_

#include "LinuxService.h"

class HandlerFor_GA_;
class HandlerFor_GP_;
class HandlerFor_TE_;
class HandlerFor_DB_;
class InterHandler;
class GameDataPool;

namespace nsl {
class DBConnection;
}

class App : public nsl::LinuxService
{
public:
    App();
    virtual ~App();

    bool load_script(char* service_identify);
    virtual void readConfig(char* service_identify);
    virtual void prepareRun(char* Service_identify);
    virtual void run();
    virtual void finishRun();
    virtual void stop();
    virtual void onStop();
    virtual void onPause();
    virtual void onContinue();
};

extern HandlerFor_GA_* handlerFor_GA_;
extern HandlerFor_GP_* handlerFor_GP_;
extern HandlerFor_TE_* handlerFor_TE_;
extern InterHandler* interHandler;
extern HandlerFor_DB_* handlerFor_DB_;
extern GameDataPool* gameDataPool_[5];
extern nsl::DBConnection* pGameDbConnection;
extern nsl::DBConnection* pAuctionDbConnection;

bool initAuctionString(const char* file);

#endif // SERVERLIBRARY2_0_H_
