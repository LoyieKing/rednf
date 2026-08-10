// df_guild_r — ServerLoadChecker (split from source/guild per ORIG layout)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PacketHeader.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "ServerLoadChecker.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CAppLoadChecker::AddLoad(int n)
{
    m_udpRecvLevel = (char)n;
}

void CAppLoadChecker::AddLoadTotal(int n)
{
    m_tcpSendLevel = (char)n;
}

int CAppLoadChecker::IsLoadComplete()
{
    return m_tcpRecvLast != 0;
}

CAppLoadChecker* CAppLoadCheckerInstance()
{
    static CAppLoadChecker instance;
    return &instance;
}

CAppLoadChecker::CAppLoadChecker()
{
    m_tcpRecvLast = 0;
    m_udpRecvLast = 0;
    m_tcpSendLast = 0;
    m_tcpRecvLevel = 0;
    m_udpRecvLevel = 0;
    m_tcpSendLevel = 0;
}

bool CAppLoadChecker::checkTcpRecvLoad(int n)
{
    if ((char)m_tcpRecvLevel < 1 && 0x32 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 1;
        return 1;
    }
    if ((char)m_tcpRecvLevel < 2 && 100 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 2;
        return 1;
    }
    if ((char)m_tcpRecvLevel < 3 && 200 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 3;
        return 1;
    }
    if ((char)m_tcpRecvLevel < 4 && 500 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 4;
        return 1;
    }
    if ((char)m_tcpRecvLevel < 5 && 1000 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 5;
        return 1;
    }
    if ((char)m_tcpRecvLevel <= 5 && 5000 < n - m_tcpRecvLast)
    {
        m_tcpRecvLevel = 6;
        return 1;
    }
    if ((char)m_tcpRecvLevel == 6 && 5000 < n - m_tcpRecvLast)
    {
        return 1;
    }
    if ((char)m_tcpRecvLevel >= 0 && 0x32 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xff;
        return 1;
    }
    if ((char)m_tcpRecvLevel >= (char)0xff && 100 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xfe;
        return 1;
    }
    if ((char)m_tcpRecvLevel >= (char)0xfe && 200 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xfd;
        return 1;
    }
    if ((char)m_tcpRecvLevel >= (char)0xfd && 500 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xfc;
        return 1;
    }
    if ((char)m_tcpRecvLevel >= (char)0xfc && 1000 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xfb;
        return 1;
    }
    if ((char)m_tcpRecvLevel >= (char)0xfb && 5000 < m_tcpRecvLast - n)
    {
        m_tcpRecvLevel = (char)0xfa;
        return 1;
    }
    if ((char)m_tcpRecvLevel == (char)0xfa && 5000 < m_tcpRecvLast - n)
    {
        return 1;
    }
    return 0;
}

bool CAppLoadChecker::checkUdpRecvLoad(int n)
{
    if ((char)m_udpRecvLevel < 1 && 0x32 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 1;
        return 1;
    }
    if ((char)m_udpRecvLevel < 2 && 100 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 2;
        return 1;
    }
    if ((char)m_udpRecvLevel < 3 && 200 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 3;
        return 1;
    }
    if ((char)m_udpRecvLevel < 4 && 500 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 4;
        return 1;
    }
    if ((char)m_udpRecvLevel < 5 && 1000 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 5;
        return 1;
    }
    if ((char)m_udpRecvLevel <= 5 && 5000 < n - m_udpRecvLast)
    {
        m_udpRecvLevel = 6;
        return 1;
    }
    if ((char)m_udpRecvLevel == 6 && 5000 < n - m_udpRecvLast)
    {
        return 1;
    }
    if ((char)m_udpRecvLevel >= 0 && 0x32 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xff;
        return 1;
    }
    if ((char)m_udpRecvLevel >= (char)0xff && 100 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xfe;
        return 1;
    }
    if ((char)m_udpRecvLevel >= (char)0xfe && 200 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xfd;
        return 1;
    }
    if ((char)m_udpRecvLevel >= (char)0xfd && 500 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xfc;
        return 1;
    }
    if ((char)m_udpRecvLevel >= (char)0xfc && 1000 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xfb;
        return 1;
    }
    if ((char)m_udpRecvLevel >= (char)0xfb && 5000 < m_udpRecvLast - n)
    {
        m_udpRecvLevel = (char)0xfa;
        return 1;
    }
    if ((char)m_udpRecvLevel == (char)0xfa && 5000 < m_udpRecvLast - n)
    {
        return 1;
    }
    return 0;
}

bool CAppLoadChecker::checkTcpSendLoad(int n)
{
    if ((char)m_tcpSendLevel < 1 && 0x32 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 1;
        return 1;
    }
    if ((char)m_tcpSendLevel < 2 && 100 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 2;
        return 1;
    }
    if ((char)m_tcpSendLevel < 3 && 200 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 3;
        return 1;
    }
    if ((char)m_tcpSendLevel < 4 && 500 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 4;
        return 1;
    }
    if ((char)m_tcpSendLevel < 5 && 1000 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 5;
        return 1;
    }
    if ((char)m_tcpSendLevel <= 5 && 5000 < n - m_tcpSendLast)
    {
        m_tcpSendLevel = 6;
        return 1;
    }
    if ((char)m_tcpSendLevel == 6 && 5000 < n - m_tcpSendLast)
    {
        return 1;
    }
    if ((char)m_tcpSendLevel >= 0 && 0x32 < m_tcpSendLast - n)
    {
        m_udpRecvLevel = (char)0xff;  // ORIG 此处写 +0xd（原版固有）
        return 1;
    }
    if ((char)m_tcpSendLevel >= (char)0xff && 100 < m_tcpSendLast - n)
    {
        m_tcpSendLevel = (char)0xfe;
        return 1;
    }
    if ((char)m_tcpSendLevel >= (char)0xfe && 200 < m_tcpSendLast - n)
    {
        m_tcpSendLevel = (char)0xfd;
        return 1;
    }
    if ((char)m_tcpSendLevel >= (char)0xfd && 500 < m_tcpSendLast - n)
    {
        m_tcpSendLevel = (char)0xfc;
        return 1;
    }
    if ((char)m_tcpSendLevel >= (char)0xfc && 1000 < m_tcpSendLast - n)
    {
        m_tcpSendLevel = (char)0xfb;
        return 1;
    }
    if ((char)m_tcpSendLevel >= (char)0xfb && 5000 < m_tcpSendLast - n)
    {
        m_tcpSendLevel = (char)0xfa;
        return 1;
    }
    if ((char)m_tcpSendLevel == (char)0xfa && 5000 < m_tcpSendLast - n)
    {
        return 1;
    }
    return 0;
}

int CAppLoadChecker::CheckTcpRecvQ(int n)
{
    if (checkTcpRecvLoad(n))
    {
        setTcpRecvQueue(n);
        return 1;
    }
    return 0;
}

int CAppLoadChecker::CheckUdpRecvQ(int n)
{
    if (checkUdpRecvLoad(n))
    {
        setUdpRecvQueue(n);
        return 1;
    }
    return 0;
}

int CAppLoadChecker::CheckTcpSendQ(int n)
{
    if (checkTcpSendLoad(n))
    {
        setTcpSendQueue(n);
        return 1;
    }
    return 0;
}

void CAppLoadChecker::RequestDB(CServerHandler* handler, int a, int b)
{
    Packet_Server_Queue_Load_Statistic pkt;
    pkt.m_flag = (char)0xcb;
    pkt.m_param = (char)a;
    pkt.m_value = (unsigned short)b;
    handler->SendToDB((PacketHeader*)&pkt);
}

void CAppLoadChecker::setTcpRecvQueue(int n)
{
    m_tcpRecvLast = n;
}

void CAppLoadChecker::setUdpRecvQueue(int n)
{
    m_udpRecvLast = n;
}

void CAppLoadChecker::setTcpSendQueue(int n)
{
    m_tcpSendLast = n;
}

