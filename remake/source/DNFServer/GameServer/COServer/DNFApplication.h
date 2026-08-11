#ifndef DNF_APPLICATION_H_
#define DNF_APPLICATION_H_

#include <deque>
#include <queue>
#include <string>

#include "DNFAppThread.h"
#include "DNFUser.h"
#include "PacketHeader.h"
#include "Thread.h"

class CFrameCountHandler;
class CUserManager;
class CTableBase;
class CAppConfig;
class CServerConfig;
class CServerHandler;
class CAppInit;
class CInnerMsgHandler;
class CAppThread;
class CUdpHandler;
class CNetworkThread;
class CSignalTranslator;
class CPacketDecoder;
class CPacketTranslater;
class CKillUSRConfig;
class CGameServer;
class CUdpRecvBuffer;

// ---- CAppBase：vptr@0 ----
// vtable：dtor / dtor / Process / Init / Load / Free
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

// ---- CApplication ----
// vptr@0 / loaded@4 / @8 / CAppInit*@0xc / CAppConfig*@0x10 / CServerConfig*@0x14 /
// CKillUSRConfig*@0x18 / CServerHandler*@0x1c / CFrameCountHandler@0x20(0x30) /
// CInnerMsgHandler*@0x50 / queue[10]@0x54(0x28) / CMutex[10]@0x1e4(0x18) /
// CUdpHandler*[101]@0x2d4 / CNetworkThread*[101]@0x468 / CAppThread*[10]@0x5f4 /
// CMutex@0x624 / CUserManager@0x63c（内嵌，0xf4）
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
    bool Send_Term_Signal(const std::string& name);
    void Send_Suspend_Signal(const std::string& name);
    CGameServer* FindGameServer(int group, int channel);
    void App_Stop();
    void SendTestPacket_1();
    void SendTestPacket_2();
    void TranslateSignal();
    void AttachAppInitor(char** argv);
    std::queue<CUdpRecvBuffer*>* Get_QPacket();
    CMutex* Get_QLock();
    CMutex* Get_BLock();
    CUdpHandler* Get_UdpHandler(int idx);
    CUserManager* Get_UserManager();

    bool m_loaded;                    // +4
    int m_reserved1;                  // +8
    CAppInit* m_appInit;              // +0xc
    CTableBase* m_appConfig;          // +0x10
    CTableBase* m_serverConfig;       // +0x14
    CKillUSRConfig* m_killUsrConfig;  // +0x18
    CServerHandler* m_serverHandler;  // +0x1c
    CFrameCountHandler m_frame;       // +0x20
    CInnerMsgHandler* m_innerMsg;     // +0x50
    std::queue<CUdpRecvBuffer*> m_udpQueues[10];  // +0x54
    CMutex m_udpLocks[10];            // +0x1e4
    CUdpHandler* m_udpHandlers[101];  // +0x2d4
    CThreadInterface* m_netThreads[101]; // +0x468（ORIG 为基类指针：new 赋值带基类转换，
                                        //   i 先存 esi + lea 形态）
    CThreadInterface* m_appThreads[10];  // +0x5fc（同上）
    CMutex m_lock1;                   // +0x624
    CUserManager m_userManager;       // +0x63c（内嵌）
};

// ---- 事件包（coserver 原版大小：Start 0x12 / End 0xa）----
class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();
    unsigned int m_field1;    // +0xa
    unsigned short m_field2;  // +0xe
    unsigned short m_field3;  // +0x10
} __attribute__((packed));

class Packet_Monitor_Event_End : public PacketHeader
{
public:
    Packet_Monitor_Event_End();
};

// ---- CInnerMsgHandler：空类（1 字节）----
class CInnerMsgHandler
{
public:
    CInnerMsgHandler() throw();
    ~CInnerMsgHandler() throw();
};

// 全局单例
CApplication* CApplicationInstance();
void ShowLogo();
CSignalTranslator* CSignalTranslatorInstance();
CPacketDecoder* CPacketDecoderInstance();

#endif // DNF_APPLICATION_H_
