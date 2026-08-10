#ifndef DNFRELAYSERVER_H_
#define DNFRELAYSERVER_H_

#include "LinuxService.h"

class App : public LinuxService
{
public:
    App();
    ~App();
    bool load_script();
    virtual void readConfig();
    virtual void prepareRun();
    virtual void run();
    virtual void finishRun();
    virtual void stop();
    virtual void onStop();
    virtual void onPause();
    virtual void onContinue();
};

#endif // DNFRELAYSERVER_H_
