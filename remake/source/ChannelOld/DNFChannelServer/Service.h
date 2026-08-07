#ifndef SERVICE_H_
#define SERVICE_H_

#include "stdafx.h"

typedef void (*handler)(int);

class ServiceInfo
{
public:
    ServiceInfo();
    void setInfo(const char* szServiceName, const char* szDisplayName, const char* szBinaryName, const char* szConfigFileName);
    void setPIDFileName(const char* szPIDFileName);
    void setRunAsService(bool bRunService);
    const char* getPIDFileName() const;
    const char* getConfigFileName() const;
    const char* getChannelName() const;

protected:
    char m_szServerIP[0x100];
    char m_szServiceName[0x100];
    char m_szDisplayName[0x100];
    char m_szBinaryName[0x100];
    char m_szConfigFileName[0x100];
    char m_szPIDFileName[0x100];
    char m_szChannelName[0x100];
    int m_nServiceID;
    int m_nChannelNo;
    bool m_bRunAsService;
    bool isTerminated_;
};

class IService
{
public:
    IService();
    virtual bool checkConfigFile() = 0;
    virtual bool checkPIDFile() = 0;
    virtual void prepareStart() = 0;
    virtual void install() = 0;
    virtual void remove() = 0;
    virtual void start() = 0;
    virtual void main(char* service_identify) = 0;
    virtual void sendBroadCastMessage(const char* szMsg) = 0;
    virtual void controlStop() = 0;
    virtual void controlPause() = 0;
    virtual void controlContinue() = 0;
};

class LinuxService : public ServiceInfo, public IService
{
public:
    LinuxService();
    ~LinuxService();

    static LinuxService* getInstance();

    void processCommandLine(int argc, char** argv);
    void setServiceState(DWORD p_State);
    void setRunning();
    void stopPending();
    void setStopped();

    virtual bool checkConfigFile();
    virtual bool checkPIDFile();
    virtual void prepareStart();
    virtual void install();
    virtual void remove();
    virtual void start();
    virtual void main(char* service_identify);
    virtual void sendBroadCastMessage(const char* szMsg);
    virtual void controlStop();
    virtual void controlPause();
    virtual void controlContinue();

    virtual void readConfig() = 0;
    virtual void prepareRun(char* szCmdLine) = 0;
    virtual void run() = 0;
    virtual void finishRun() = 0;
    virtual void stop() = 0;
    virtual void onStop() = 0;
    virtual void onPause() = 0;
    virtual void onContinue() = 0;

private:
    DWORD m_dwServiceState;
    bool m_bStop;
    char m_command[0x1e];

    static LinuxService* theInstance;
};

bool Neof_registerSignalHandler(int sig, handler func_name);
void Neof_SignalHandler(int sig);
bool Neof_registerSignalHandlers();
void Neof_dumpCoreFile();
void Neof_setCoreLimit();
bool Neof_sendTerminateSignal();
void Neof_sendSuspendSignal();
bool save_pid();
bool delete_pid();
void make_dir(char* szdir_name);

#endif // SERVICE_H_
