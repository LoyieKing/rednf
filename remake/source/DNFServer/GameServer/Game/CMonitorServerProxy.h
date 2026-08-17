#ifndef GAME_CMONITORSERVERPROXY_H_
#define GAME_CMONITORSERVERPROXY_H_

// ============================================================================
// df_game_r 还原 —— CMonitorServerProxy（监视服务器客户端代理，D4/E2 批次）。
// 参照 docs/class_func_reports/CMonitorServerProxy.md + df_game_r 反汇编。
// 布局（sizeof 0x30e1c，CServerProxyMgr<CMonitorServerProxy> 内嵌于 +0x10）：
//   +0x00 std::string m_ip
//   +0x04 int m_udpPort
//   +0x08 int m_tcpPort
//   +0x0c CNetwork<100000,100000> m_tcp
//   +0x30e04 CUdpHandler m_udp
// 内联函数（默认 ctor / Init(string,int,int) / dtor / GetTcpHandler）为 ORIG
// 弱符号（0x82a69cc / 0x82a6a4a / 0x82b312c / 0x847173c），与本头定义逐字对应。
// 依赖的 Gm_List / CEnvironment 等声明见 CGuildServerProxy.h（共享外部类型）。
// ============================================================================

#include <string>

#include "CGuildServerProxy.h"

class CMonitorServerProxy
{
public:
    CMonitorServerProxy();                                      // 0x082a69cc W
    CMonitorServerProxy(std::string ip, int udpPort, int tcpPort);// 0x0847097c T
    ~CMonitorServerProxy();                                     // 0x082b312c W
    void Init(std::string ip, int udpPort, int tcpPort);        // 0x082a6a4a W

    bool Init();                        // 0x08470a00
    bool InitUdp();                     // 0x08470a7c
    bool InitTcp();                     // 0x08470ad6
    void Disconnect();                  // 0x08470bee
    void SendCharLevelGrowType(unsigned int, unsigned int, short, short);// 0x08470c04
    void SendHeartBeat(int);            // 0x08470c5e
    void SendLogin(int, unsigned int, int, char*, unsigned char, char*);// 0x08470c94
    void SendLogout(unsigned int, int, char*, unsigned int, unsigned int,
                    unsigned int, unsigned char, unsigned char);// 0x08470d68
    void SendPacket(char*, int);        // 0x08470df4
    void SendTcpPacket(char*, int);     // 0x08470e44
    void SendPacketCallGuildMembers(unsigned int, unsigned int);// 0x08470e88
    void SendGuildLevelup(int, unsigned int, unsigned int);     // 0x08470ec4
    void SendCallGuildInfo(unsigned int, unsigned int, unsigned char,
                           unsigned int*);                      // 0x08470f06
    void SendMemberPayTax(bool, unsigned int, int, int);        // 0x08470fb4
    void SendUserRepel(unsigned int, unsigned int, char*, int); // 0x0847101a
    void SendNotifyNewMail(unsigned int);  // 0x08471072
    void SendGuildWarEnd(unsigned char);   // 0x084710a8
    void SendGuildWarStart(unsigned char); // 0x084710e6
    void SendExchangeServerInfo(int, int, short);                // 0x08471124
    void SendNoticeCharLive(unsigned int);  // 0x08471170
    void SendRequestUserMid(unsigned int, unsigned int, char*, int);// 0x084711a6
    void SendRequestOnTimeEventReward(unsigned int, unsigned int,
                                      unsigned int);            // 0x084711fe
    void updateTowerRank(unsigned int, unsigned int, unsigned int,
                         unsigned int);                         // 0x08471240
    void SendChangeCharName(unsigned int, unsigned int, unsigned int,
                            char*);                             // 0x08471288
    int OnTcpRecv();                    // 0x084712e0
    int OnTcpSend();                    // 0x0847139c
    bool OnTcpError();                  // 0x08471444
    bool Parsing(int);                  // 0x084714ce
    void SendGameServerInfo(char*, unsigned short, unsigned char,
                            unsigned char);                     // 0x084714ec

    CNetwork<100000, 100000>* GetTcpHandler()  // 0x0847173c W
    {
        return &m_tcp;
    }

private:
    std::string m_ip;          // +0x00
    int m_udpPort;             // +0x04
    int m_tcpPort;             // +0x08
    CNetwork<100000, 100000> m_tcp;  // +0x0c
    CUdpHandler m_udp;         // +0x30e04
};

// ---- 内联弱符号（ORIG 0x82a69cc / 0x82b312c / 0x82a6a4a）----
inline CMonitorServerProxy::CMonitorServerProxy()
{
    m_udpPort = 0;
    m_tcpPort = 0;
}

inline CMonitorServerProxy::CMonitorServerProxy(std::string ip, int udpPort,
                                                int tcpPort)
    : m_ip(ip), m_udpPort(udpPort), m_tcpPort(tcpPort), m_tcp(), m_udp()
{
}

inline CMonitorServerProxy::~CMonitorServerProxy()
{
}

inline void CMonitorServerProxy::Init(std::string ip, int udpPort, int tcpPort)
{
    m_ip = ip;
    m_udpPort = udpPort;
    m_tcpPort = tcpPort;
    Init();
}

#endif  // GAME_CMONITORSERVERPROXY_H_
