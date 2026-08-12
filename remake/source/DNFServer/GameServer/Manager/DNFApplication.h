#ifndef DNF_APPLICATION_H_
#define DNF_APPLICATION_H_

#include <queue>
#include <string>
#include <vector>

#include "DNFPacketBuffer.h"
#include "DNFTickHandler.h"
#include "TcpNetSystem.h"
#include "Thread.h"
#include "UserManager.h"

class CAppConfig;
class CAppInit;
class CKillUSRConfig;
class CMonitorServer;
class CServerConfig;
class CServerHandler;
class CTableBase;
class CTcpServer;
class CUdpNetworkThread;

// ---- CVersionMgr ----
class CVersionMgr
{
public:
    CVersionMgr(int a, int b, int c, int d);
    int m_versions[4];  // +0
};

// ---- CSourceVersionMgr ----
class CSourceVersionMgr
{
public:
    class SourceVersion
    {
    public:
        SourceVersion(char* name, int version);
        SourceVersion(const SourceVersion& other);
        SourceVersion& operator=(const SourceVersion& other);
        ~SourceVersion();
        std::string m_name;  // +0
        int m_version;       // +4
    };
    CSourceVersionMgr();
    ~CSourceVersionMgr();
    void InsertSourceVersion(char* name, int version);
    std::vector<SourceVersion> m_versions;  // +0
};

// ---- CAppBase ----
class CAppBase
{
public:
    CAppBase();
    virtual ~CAppBase();
    virtual void Process();
    virtual void Init(int argc, char** argv) = 0;
    virtual void Load(int argc, char** argv) = 0;
    virtual void Free() = 0;
    void Create(int argc, char** argv);
    void Clear();
};

void ShowLogo();

// ---- CApplication（manager 布局见 ctor 反编译：0x25c）----
class CApplication : public CAppBase
{
public:
    CApplication();
    virtual ~CApplication();
    virtual void Process();
    virtual void Init(int argc, char** argv);
    virtual void Load(int argc, char** argv);
    virtual void Free();
    void CheckArgv(int argc, char** argv);
    void App_Stop();
    void SendTestPacket_1();
    void SendTestPacket_2();
    void TranslateSignal();
    void SwitchQueueTCP();
    void SwitchQueueUDP();
    // ORIG 返回类型 bool：CAppStopInit::Init 中 !Send_Term_Signal(...)
    // 编译为 xor $1（int 返回会 sete 物化）。函数为 std::string 参数，
    // 属 STL 豁免，不计入 identical 统计。
    bool Send_Suspend_Signal(const std::string& msg);
    bool Send_Term_Signal(const std::string& msg);
    void AttachAppInitor(char** argv);
    CUserManager* Get_UserManager();
    CServerHandler* Get_ServerHandler();
    void* Get_UdpHandler();
    CUserManager* GetUserManager() { return &m_userManager; }
    CTcpNetSystem* Get_TcpNetSystem();
    CMutex* Get_UdpQLock();
    CMutex* Get_UdpBLock();
    UdpRecvQueue* Get_UdpPacketRecvQ();
    UdpRecvQueue* Get_UdpPacketParseQ();
    CTcpServer* FindTcpServer(unsigned int idx);
    CMonitorServer* FindMonitorServer(int idx);

    bool m_loaded;                      // +4
    char m_pad5[3];                     // +5
    int m_field8;                       // +8
    CAppInit* m_appInitor;              // +0xc
    // R14: ORIG 为基类指针 CTableBase*（coserver 同款布局；CAppStartInit::Init
    // 中 new 赋值为 upcast，产生 `mov %ebx,%eax; mov %eax,%edx` 物化形态）。
    CTableBase* m_appConfig;            // +0x10
    CTableBase* m_serverConfig;         // +0x14
    CServerHandler* m_serverHandler;    // +0x18
    CFrameCountHandler m_frameCount;    // +0x1c
    CUserManager m_userManager;         // +0x4c
    CKillUSRConfig* m_killUsrConfig;    // +0x68
    CSwapQueue<UdpRecvQueue, 2> m_udpSwapQueue;  // +0x6c
    void* m_udpHandler;                 // +0xc4
    CThreadInterface* m_udpNetworkThread;   // +0xc8（ORIG 赋值 upcast 形态：ebx→eax→edx）
    CTcpNetSystem m_tcpNetSystem;       // +0xcc
    CMutex m_mutex22c;                  // +0x22c
    CMutex m_mutex244;                  // +0x244
};

#endif  // DNF_APPLICATION_H_
