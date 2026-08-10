#ifndef LINUXSERVICE_H_
#define LINUXSERVICE_H_

#include "Service.h"

class LinuxService : public ServiceInfo, public IService
{
public:
    LinuxService();
    ~LinuxService();

    virtual bool checkConfigFile();
    virtual bool checkPIDFile();
    virtual bool prepareStart();
    virtual void install();
    virtual void remove();
    virtual void start();
    virtual void main();
    virtual void sendBroadCastMessage(const char*);
    virtual void controlStop();
    virtual void controlPause();
    virtual void controlContinue();

    static LinuxService* getInstance();
    void processCommandLine(int argc, char** argv);
    void setServiceState(unsigned long);
    void stopPending();
    void setRunning();
    void setStopped();

    virtual void readConfig() = 0;
    virtual void prepareRun() = 0;
    virtual void run() = 0;
    virtual void finishRun() = 0;
    virtual void stop() = 0;
    virtual void onStop() = 0;
    virtual void onPause() = 0;
    virtual void onContinue() = 0;

    int m_dwServiceState;
    char m_command[0x1e];

private:
    static LinuxService* theInstance;
};

#endif // LINUXSERVICE_H_
