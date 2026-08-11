// df_guild_r — DNFServerConfig (split from source/guild per ORIG layout)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PacketHeader.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFServerConfig.h"
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
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

ST_ServerInfo* CServerConfig::GetServerInfo()
{
    return m_info;
}

int CServerConfig::Parse_Table(char* line, int idx)
{
    if (*line == '#')
    {
        return 0;
    }
    char* tok[5];
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tok, 5) == 5)
    {
        if (idx <= 0xfe)
        {
            ST_ServerInfo* info = &m_info[idx];
            info->m_field0 = (char)atoi(tok[0]);
            info->m_field1 = (char)atoi(tok[1]);
            info->m_field2 = (char)atoi(tok[2]);
            info->m_string = tok[3];
            info->m_ushort = (unsigned short)atoi(tok[4]);
            return 1;
        }
    }
    return 0;
}

void CServerConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 0xff);
    if (0 < rc && rc < 0xff)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x39, "./log/TableError", "Server Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CServerConfig::Load_Setup_Table() Exception Break!");
}

CServerConfig::~CServerConfig()
{
}

ST_ServerInfo::ST_ServerInfo()
    : m_field0(0), m_field1(0), m_field2(0xff), m_ushort(0)
{
}

CServerConfig::CServerConfig()
{
    for (int i = 0; i < 255; i++)
    {
        new ((void*)&m_info[i]) ST_ServerInfo;
    }
}
