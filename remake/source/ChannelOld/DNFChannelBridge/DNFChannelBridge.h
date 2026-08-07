#ifndef DNFCHANNELBRIDGE_H_
#define DNFCHANNELBRIDGE_H_

#include "LinuxService.h"
#include "ChannelServiceApp.h"

extern ChannelServiceApp::ChannelService* g_pService;

class App : public LinuxService
{
public:
    App();
    virtual void readConfig();
    virtual void prepareRun(char* service_identify);
    virtual void run();
    virtual void finishRun();
    virtual void stop();
    virtual void onStop();
    virtual void onPause();
    virtual void onContinue();
    bool load_script();
};

#endif // DNFCHANNELBRIDGE_H_
