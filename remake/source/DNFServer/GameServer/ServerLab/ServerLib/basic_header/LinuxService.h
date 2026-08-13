#ifndef NSL_LINUXSERVICE_H_
#define NSL_LINUXSERVICE_H_

#include "Thread.h"
#include "stdafx.h"

namespace nsl {

typedef const char* LPCTSTR;

class ServiceInfo
{
public:
    ServiceInfo()
    {
        m_szChannelName[0] = '\0';
    }

    void setRunAsService(bool bRunService)
    {
        m_bRunAsService = bRunService;
    }

    bool getRunAsService() const
    {
        return m_bRunAsService;
    }

    char* getPIDFileName()
    {
        return m_szPIDFileName;
    }

    void setPIDFileName(const char* p_szPIDFileName);

    const char* getConfigFileName() const
    {
        return m_szConfigFileName;
    }

    const char* getChannelName() const
    {
        return m_szChannelName;
    }

    int getChannelNo() const
    {
        return m_nChannelNo;
    }

    int getServiceID() const
    {
        return m_nServiceID;
    }

    const char* getServceIP() const
    {
        return m_szServerIP;
    }

    void setInfo(const char* p_szServiceName, const char* p_szDisplayName,
                 const char* p_szBinaryName, const char* p_szConfigFileName);

    char m_szServerIP[256];
    char m_szServiceName[256];
    char m_szDisplayName[256];
    char m_szBinaryName[256];
    char m_szConfigFileName[256];
    char m_szPIDFileName[256];
    char m_szChannelName[256];
    int m_nServiceID;
    int m_nChannelNo;
    bool m_bRunAsService;
};

class IPlatform
{
public:
    IPlatform()
    {
    }

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
    virtual ~IPlatform()
    {
    }
};

class LinuxService : public ServiceInfo, public IPlatform
{
public:
    enum ENUM_INFOTYPE
    {
        ENUM_OTP = 0,
    };

    LinuxService();
    virtual ~LinuxService();

    static LinuxService* getInstance()
    {
        return theInstance;
    }

    bool IsStopReceived()
    {
        return mbStopRecevied;
    }

    void SetStopReceived()
    {
        mbStopRecevied = true;
    }

    void processCommandLine(int argc, char** argv, char* ServiceName, bool isSignalRegist);

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
    virtual void SendInfoMessage(ENUM_INFOTYPE infoType);

    void setServiceState(DWORD p_State);
    void setStopped();
    void stopPending();
    void setRunning();

    virtual void readConfig(char*) = 0;
    virtual void prepareRun(char*) = 0;
    virtual void run() = 0;
    virtual void finishRun() = 0;
    virtual void stop() = 0;
    virtual void onStop() = 0;
    virtual void onPause() = 0;
    virtual void onContinue() = 0;

    static LinuxService* theInstance;

    bool isTerminated_;
    DWORD m_dwServiceState;
    bool m_bStop;
    char m_command[0x1e];
    bool mbStopRecevied;
};

extern char gServiceName[0x14];

bool Neof_registerSignalHandler(int sig, void (*func_name)(int));
void Neof_SignalHandler(int sig);
bool Neof_registerSignalHandlers();
void Neof_dumpCoreFile();
bool Neof_sendTerminateSignal(char* ServiceName);
void Neof_sendSuspendSignal();
void Neof_setCoreLimit();
bool delete_pid();
bool save_pid(char* pidName);
void make_dir(char* szdir_name);

} // namespace nsl

#endif // NSL_LINUXSERVICE_H_
