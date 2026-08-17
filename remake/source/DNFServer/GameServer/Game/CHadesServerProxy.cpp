// df_game_r 还原 —— CHadesServerProxy（E1 批次）。
// 参照 docs/class_func_reports/CHadesServerProxy.md + docs/hades_protocol.md。
#include "CHadesServerProxy.h"

#include "GlobalData.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "PacketHeader.h"
#include "Packet_Hades_Udp_HeartBeat.h"

CHadesServerProxy::CHadesServerProxy(char* ip, int tcpPort, int udpPort)
    : BaseServerProxy(ip, tcpPort, "HadesSvr"), m_udpPort(udpPort), m_connSec(0)
{
    HadesSvrInit();
}

bool CHadesServerProxy::HadesSvrInit()
{
    ConnTcpHadesSvr();
    InitUdp();
    return 1;
}

bool CHadesServerProxy::ConnTcpHadesSvr()
{
    BaseServerProxy::Init();
    BaseServerProxy::SetSocketState(ENUM_SOCKET_STATE_CONNECTED);
    m_connSec = GlobalData::s_systemTime_.getCurSec();
    return 1;
}

bool CHadesServerProxy::InitUdp()
{
    if (m_udpHandler.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_HadesServerProxy.cpp",
                              "bool CHadesServerProxy::InitUdp()", 0xd5,
                              "[CHadesServerProxy::Init] Can't init udp client socket");
        return false;
    }
    return true;
}

bool CHadesServerProxy::IsConnect()
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        return 1;
    }
    return 0;
}

void CHadesServerProxy::OnDisconnect()
{
    RunConnTimer();
}

bool CHadesServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_ServerPacket(recvLen);
}

void CHadesServerProxy::RunConnTimer()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DF_HadesServerProxy.cpp", 0x119),
                       true);
    (**guard) << 0x23a;
    (**guard) << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

int CHadesServerProxy::SendHadesTcpPacket(char* packet)
{
    if (BaseServerProxy::GetSocketState() == ENUM_SOCKET_STATE_CONNECTED)
    {
        BaseServerProxy::SendTcpPacket(packet, ((PacketHeader*)packet)->packetSize);
    }
    return 1;
}

void CHadesServerProxy::SendHadesUdpPacket(char* packet)
{
    std::string serverIP = GetServerIP();
    m_udpHandler.SendToServer(packet, ((PacketHeader*)packet)->packetSize,
                              (unsigned short)m_udpPort, serverIP.c_str());
}

void CHadesServerProxy::SendHeartBeatPacket(unsigned short udpPort, int channelNo)
{
    Packet_Hades_Udp_HeartBeat packet;
    packet.m_channelNo = (unsigned short)channelNo;
    packet.m_udpPort = udpPort;
    SendHadesUdpPacket((char*)&packet);
}
