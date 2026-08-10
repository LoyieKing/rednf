#ifndef SERVICE_H_
#define SERVICE_H_

// ServiceInfo：+0x100 起每字段 0x100 字节
class ServiceInfo
{
public:
    ServiceInfo();
    void setInfo(const char* name, const char* display, const char* binary, const char* config);
    void setPIDFileName(const char* fn);
    void setRunAsService(bool b)
    {
        m_bRunAsService = b;
    }
    const char* getConfigFileName() const
    {
        return m_configFileName;
    }
    const char* getPIDFileName() const
    {
        return m_pidFileName;
    }
    const char* getChannelName() const
    {
        return m_channelName;
    }

    char m_serverIP[0x100];
    char m_serviceName[0x100];
    char m_displayName[0x100];
    char m_binaryName[0x100];
    char m_configFileName[0x100];
    char m_pidFileName[0x100];
    char m_channelName[0x100];
    int m_serviceID;
    int m_channelNo;
    bool m_bRunAsService;
    bool m_terminated;
};

// 多态接口基类（primary base，vptr@0）
class IService
{
public:
    IService();
    virtual bool checkConfigFile() = 0;
    virtual bool checkPIDFile() = 0;
    virtual bool prepareStart() = 0;
    virtual void install() = 0;
    virtual void remove() = 0;
    virtual void start() = 0;
    virtual void main() = 0;
    virtual void sendBroadCastMessage(const char*) = 0;
    virtual void controlStop() = 0;
    virtual void controlPause() = 0;
    virtual void controlContinue() = 0;
};

#endif // SERVICE_H_
