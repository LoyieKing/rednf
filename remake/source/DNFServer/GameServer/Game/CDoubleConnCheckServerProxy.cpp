// df_game_r 还原 —— CDoubleConnCheckServerProxy（双开检测 UDP 代理，G2-4 批次）。
// 逐函数对照 docs/class_func_reports/CDoubleConnCheckServerProxy.md 与
// ORIG 反汇编实现。布局：+0x00 std::string m_ip | +0x04 int m_port |
// +0x08 CUdpHandler m_udpHandler（sizeof 0x10）。
// 与 BaseServerProxy/CHadesServerProxy 不同：本类不继承 BaseServerProxy，
// 直接内嵌 string + port + CUdpHandler（ORIG ctor 0x846d300 逐成员推导）。
#include <string>

#include "CUdpHandler.h"
#include "LogManager.h"

// ---- UDP 双开检测包（布局来自 shared/packet/include，ctor 为独立 TU
//      0x846d602/0x846d58a/0x846d5c6；本地声明避免与共享头内联 ctor 冲突） ----
class Packet_Double_Check_Server_UDP_HeartBeat
{
public:
    Packet_Double_Check_Server_UDP_HeartBeat();   // 0x0846d602

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned char m_channel;      // +0x0a
    unsigned char m_group;        // +0x0b
} __attribute__((packed));

class Packet_DoubleCheck_UDP_Login
{
public:
    Packet_DoubleCheck_UDP_Login();   // 0x0846d58a

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned int m_userID;        // +0x0a
    unsigned char m_channel;      // +0x0e
    unsigned char m_group;        // +0x0f
} __attribute__((packed));

class Packet_DoubleCheck_UDP_Logout
{
public:
    Packet_DoubleCheck_UDP_Logout();   // 0x0846d5c6

    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    unsigned int m_accId;         // +0x0a
    unsigned char m_channel;      // +0x0e
    unsigned char m_serverGroup;  // +0x0f
} __attribute__((packed));

// ---------------------------------------------------------------------------
// CDoubleConnCheckServerProxy
// ---------------------------------------------------------------------------
class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);  // 0x0846d300 T

    bool Init();                                 // 0x0846d356 T
    void SendHeartBeat(int channel, int group);  // 0x0846d3ae T
    void SendLogin(unsigned int userID, int channel, int group);  // 0x0846d412 T
    void SendLogout(unsigned int userID, int channel, int group); // 0x0846d47c T
    void SendPacket(char* packet, int size);     // 0x0846d4ee T

    std::string m_ip;          // +0x00
    int m_port;                // +0x04
    CUdpHandler m_udpHandler;  // +0x08
};

// ============================================================================
// 实现
// ============================================================================

CDoubleConnCheckServerProxy::CDoubleConnCheckServerProxy(std::string ip, int port)
    : m_ip(ip), m_port(port), m_udpHandler()
{
}

bool CDoubleConnCheckServerProxy::Init()
{
    if (m_udpHandler.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_DoubleConnCheckServerProxy.cpp",
                              "Init", 0x78,
                              "[CDoubleConnCheckServerProxy::Init] Can\'t init udp client socket\n");
        return false;
    }
    return true;
}

void CDoubleConnCheckServerProxy::SendHeartBeat(int channel, int group)
{
    Packet_Double_Check_Server_UDP_HeartBeat packet;
    packet.m_connNo = 1000;
    packet.m_channel = (unsigned char)channel;
    packet.m_group = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0xc,
                              (unsigned short)m_port, m_ip.c_str());
}

void CDoubleConnCheckServerProxy::SendLogin(unsigned int userID, int channel,
                                            int group)
{
    Packet_DoubleCheck_UDP_Login packet;
    packet.m_connNo = userID;
    packet.m_userID = userID;
    packet.m_channel = (unsigned char)channel;
    packet.m_group = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0x10,
                              (unsigned short)m_port, m_ip.c_str());
}

void CDoubleConnCheckServerProxy::SendLogout(unsigned int userID, int channel,
                                             int group)
{
    if (userID == 0)
    {
        return;
    }
    Packet_DoubleCheck_UDP_Logout packet;
    packet.m_connNo = userID;
    packet.m_accId = userID;
    packet.m_channel = (unsigned char)channel;
    packet.m_serverGroup = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0x10,
                              (unsigned short)m_port, m_ip.c_str());
}

void CDoubleConnCheckServerProxy::SendPacket(char* packet, int size)
{
    m_udpHandler.SendToServer(packet, size,
                              (unsigned short)m_port, m_ip.c_str());
}
