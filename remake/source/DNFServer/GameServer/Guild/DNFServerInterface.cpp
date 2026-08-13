// df_guild_r — DNFServerInterface (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFServerInterface.h"
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
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CServerInterface::CServerInterface()
{
    m_info = 0;
    m_connected = 0;
    m_heartBeatCnt = 0;
    m_heartBeatOverCnt = 0;
    m_sock = 0;
}

CServerInterface::CServerInterface(stServerInfo* info)
{
    m_info = info;
    m_connected = 0;
    m_heartBeatCnt = 0;
    m_heartBeatOverCnt = 0;
    m_sock = 0;
}

CServerInterface::~CServerInterface()
{
}

bool CServerInterface::Initialize()
{
    if (m_sock == 0)
    {
        m_sock = (int)new CUdpHandler;
        ((CUdpHandler*)m_sock)->InitClientSocket();
        return true;
    }
    return false;
}

bool CServerInterface::Destroy()
{
    if (m_sock != 0)
    {
        ::operator delete((void*)m_sock);
    }
    return true;
}

int CServerInterface::SendToServer(char* buf, int len)
{
    if (m_sock != 0)
    {
        return ((CUdpHandler*)m_sock)->SendToServer(
            buf, len, m_info->m_port, (const char*)((unsigned int)m_info + 3));
    }
    return 0;
}

bool CServerInterface::IsValidServer()
{
    if (m_info->m_group == 0xff)
        return 0;
    return 1;
}

bool CServerInterface::IsConnected()
{
    return m_connected;
}

void CServerInterface::SetConnFlag(bool flag)
{
    m_connected = flag;
}

void CServerInterface::OnDisconnect()
{
    m_connected = 0;
    m_heartBeatCnt = 0x14;
    m_heartBeatOverCnt = 0;
}

int CServerInterface::IsHeartBeatTimeOver()
{
    if (--m_heartBeatCnt == 0)
    {
        if (0x14 < (unsigned char)++m_heartBeatOverCnt)
        {
            return 1;
        }
        m_heartBeatCnt = 0x14;
    }
    return 0;
}

void CServerInterface::ResetHeartBeat()
{
    m_heartBeatCnt = 0x14;
    m_heartBeatOverCnt = 0;
}

void CServerInterface::SetServerInfo(stServerInfo* info)
{
    m_info = info;
}

stServerInfo* CServerInterface::GetServerInfo()
{
    return m_info;
}

void* CServerInterface::GetUdpHandler()
{
    return (void*)m_sock;
}

unsigned char CServerInterface::GetChannelNo()
{
    return m_info->m_field1;
}

unsigned char CServerInterface::GetGroupNo()
{
    return *(unsigned char*)m_info;
}
