#ifndef GAME_CHADESSERVERPROXY_H_
#define GAME_CHADESSERVERPROXY_H_

// ============================================================================
// df_game_r 还原 —— CHadesServerProxy（Hades 反外挂服务器 TCP+UDP 代理）。
// 参照 docs/class_func_reports/CHadesServerProxy.md + docs/hades_protocol.md。
// 布局：BaseServerProxy（0x30e0c）+
//       +0x30e0c m_udpPort | +0x30e10 m_udpHandler | +0x30e18 m_connSec
//       （sizeof = 0x30e1c）
// vtable：[0]=OnDisconnect [1]=Disconnect（继承） [2]=Parsing
// 全局单例 GlobalData::s_hades_proxy；ORIG 无析构符号（new 后不 delete）。
// ============================================================================

#include "BaseServerProxy.h"
#include "CUdpHandler.h"

class CHadesServerProxy : public BaseServerProxy
{
public:
    CHadesServerProxy(char* ip, int tcpPort, int udpPort);  // 0x08470510

    bool HadesSvrInit();            // 0x0847059a
    bool ConnTcpHadesSvr();         // 0x084705be
    bool InitUdp();                 // 0x08470628
    bool IsConnect();               // 0x084705fe

    virtual void OnDisconnect();               // vtable[0] 0x084707b6
    virtual bool Parsing(int recvLen);         // vtable[2] 0x08470794

    void RunConnTimer();                       // 0x084707ca
    int SendHadesTcpPacket(char* packet);      // 0x08470752
    void SendHadesUdpPacket(char* packet);     // 0x084706be
    void SendHeartBeatPacket(unsigned short udpPort, int channelNo);  // 0x08470682

    int GetHadesSocketConnSec();               // 0x084b9ab2（inline，弱符号）

    // ---- 对象布局（ORIG 直访成员，public）----
    int m_udpPort;             // +0x30e0c
    CUdpHandler m_udpHandler;  // +0x30e10
    int m_connSec;             // +0x30e18
};

inline int CHadesServerProxy::GetHadesSocketConnSec() { return m_connSec; }

#endif  // GAME_CHADESSERVERPROXY_H_
