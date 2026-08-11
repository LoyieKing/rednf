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
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_sock = 0;
}

CServerInterface::CServerInterface(stServerInfo* info)
{
    m_info = info;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_sock = 0;
}

CServerInterface::~CServerInterface()
{
}

bool CServerInterface::Initialize()
{
    int old = m_sock;
    if (old == 0)
    {
        m_sock = (int)new CUdpHandler;
        ((CUdpHandler*)m_sock)->InitClientSocket();
    }
    return old == 0;
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
        return ((CUdpHandler*)m_sock)->SendToServer(buf, len, m_info->m_port, m_info->m_name);
    }
    return 0;
}

bool CServerInterface::IsValidServer()
{
    return m_info != 0 && m_info->m_group != 0xff;
}

bool CServerInterface::IsConnected()
{
    return m_field8;
}

void CServerInterface::SetConnFlag(bool flag)
{
    m_field8 = flag;
}

void CServerInterface::OnDisconnect()
{
    m_field8 = 0;
    m_field9 = 0x14;
    m_fielda = 0;
}

int CServerInterface::IsHeartBeatTimeOver()
{
    m_field9 = (char)(m_field9 - 1);
    if (m_field9 == 0)
    {
        m_fielda = (char)(m_fielda + 1);
        if (0x14 < (unsigned char)m_fielda)
        {
            return 1;
        }
        m_field9 = 0x14;
    }
    return 0;
}

void CServerInterface::ResetHeartBeat()
{
    m_field9 = 0x14;
    m_fielda = 0;
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
    return m_info ? *(unsigned char*)((char*)m_info + 1) : 0;
}

unsigned char CServerInterface::GetGroupNo()
{
    return *(unsigned char*)m_info;
}
