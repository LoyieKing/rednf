#ifndef DNF_APPLICATION_H_
#define DNF_APPLICATION_H_

#include <deque>
#include <queue>
#include <string>

#include "Thread.h"
#include "DNFTickHandler.h"
#include "HWSpecResearcher.h"
#include "FrameLagCollector.h"
#include "UdpCharacteristic.h"
#include "Statistics.h"
#include "DNFTableBase.h"

class CAppConfig;
class CServerConfig;
class CServerHandler;
class CAppInit;
class CInnerMsgHandler;
class CUdpHandler;
class CSignalTranslator;
class CPacketDecoder;
class CPacketTranslater;
class CUdpRecvBuffer;

// ---- CAppBase：vptr@0 ----
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
// CHWSpecResearcher@0x18(0x6C) / FrameLagCollector@0x84(0x1E8) /
// UdpCharacteristic@0x26c(0x2C) / CServerHandler*@0x298 /
// CFrameCountHandler@0x29c(0x30) / CInnerMsgHandler*@0x2cc / queue@0x2d0(0x28) /
// CUdpHandler*@0x2f8 / CMutex@0x2fc(0x18) / CMutex@0x314(0x18) /
// StatisticManager@0x32c
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
    void App_Stop();
    void SendTestPacket_1();
    void SendTestPacket_2();
    void TranslateSignal();
    void AttachAppInitor(char** argv);
    unsigned char Get_ServerGroup();
    std::queue<CUdpRecvBuffer*>* Get_QPacket();
    CMutex* Get_QLock();
    CMutex* Get_BLock();
    CUdpHandler* Get_UdpHandler();
    CServerHandler* Get_ServerHandler();
    StatisticManager* Get_StatisticManager();
    FrameLagCollector* Get_FrameLagCollector();
    CHWSpecResearcher* Get_HWspecResearch();
    UdpCharacteristic* Get_UdpCharacteristic();

    bool m_loaded;                    // +4
    int m_reserved1;                  // +8
    CAppInit* m_appInit;              // +0xc
    CTableBase* m_appConfig;          // +0x10
    CTableBase* m_serverConfig;       // +0x14
    CHWSpecResearcher m_hwSpec;       // +0x18
    FrameLagCollector m_frameLag;     // +0x84
    UdpCharacteristic m_udpChar;      // +0x26c
    CServerHandler* m_serverHandler;  // +0x298
    CFrameCountHandler m_frame;       // +0x29c
    CInnerMsgHandler* m_innerMsg;     // +0x2cc
    std::queue<CUdpRecvBuffer*> m_queue;  // +0x2d0
    CUdpHandler* m_udpHandler;        // +0x2f8
    CMutex m_lock1;                   // +0x2fc
    CMutex m_lock2;                   // +0x314
    StatisticManager m_statistic;     // +0x32c
};

CApplication* CApplicationInstance();
void ShowLogo();

#endif // DNF_APPLICATION_H_
