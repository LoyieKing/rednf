// df_guild_r — DNFAppConfig (split from source/guild per ORIG layout)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PacketHeader.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFAppConfig.h"
#include "BlackUser.h"
#include "CashObject.h"
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
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CAppConfig::~CAppConfig()
{
    clearServerInfoMap();
}

int CAppConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    if (idx < 6)
    {
        char pad_if[4];
        char* tokens[2];
        if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 2) == 2)
        {
            switch (idx)
            {
            case 0:
                m_frameCount = (unsigned char)atoi(tokens[1]);
                break;
            case 1:
                m_udpPort = (unsigned short)atoi(tokens[1]);
                break;
            case 2:
                m_group = (char)atoi(tokens[1]);
                break;
            case 3:
                m_tcpPort = (unsigned short)atoi(tokens[1]);
                break;
            case 4:
                m_name = std::string(tokens[1]);
                break;
            case 5:
                m_dbmwTcpPort = (unsigned short)atoi(tokens[1]);
                break;
            default:
                return 0;
            }
            return 1;
        }
    }
    else
    {
        char pad_else[4];
        char* tokens[6];
        if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 6) == 6)
        {
            stServerInfo* si = (stServerInfo*)operator new(0x16);
            si->m_field2 = (unsigned char)atoi(tokens[1]);
            si->m_group = (unsigned char)atoi(tokens[2]);
            si->m_field1 = (unsigned char)atoi(tokens[3]);
            strncpy((char*)((int)si + 3), tokens[4], 0x10);
            si->m_port = (unsigned short)atoi(tokens[5]);
            m_serverInfo.insert(std::pair<const unsigned int, stServerInfo*>(si->m_field2, si));
            return 1;
        }
    }
    return 0;
}

void CAppConfig::Load_Table(const std::string& path)
{
    std::string full = "./cfg/" + path;
    int rc = Load_Txt_Table_Data(full.c_str(), 0x32);
    if (0 < rc && rc < 0x33)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0xb4, "./log/TableError", "App Config Table - ReturnCode = %d\n", rc);
    throw CDNFException(
        "CAppConfig::Load_Table() App Config Table File Load Failed");
}

void CAppConfig::Check_FileName(const std::string& filename)
{
    std::string cfg = "./cfg/" + filename + ".cfg";
    std::string pid = "./pid/" + filename + ".pid";
    if (access(cfg.c_str(), 0) != 0)
    {
        DNF_LOG_SCOPE_LINE(0x6a, "./log/Init", "Check_FileName - %s,%s\n", cfg.c_str(), pid.c_str());
        throw CDNFException(
            "CAppConfig::Set_FileName() There is no cfg file Exception Break! Check ./cfg/*.cfg files");
    }
    if (access(pid.c_str(), 0) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x74, "./log/Init", "Check_FileName - %s,%s\n", cfg.c_str(), pid.c_str());
        throw CDNFException(
            "CAppConfig::Set_FileName() There is already pid file! Exception Break! Check ./pid/*.pid files");
    }
}

std::multimap<unsigned int, stServerInfo*>* CAppConfig::GetServerInfoMap()
{
    return &m_serverInfo;
}

void CAppConfig::clearServerInfoMap()
{
    for (std::multimap<unsigned int, stServerInfo*>::iterator it = m_serverInfo.begin();
         it != m_serverInfo.end(); it++)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
        it->second = 0;
    }
    m_serverInfo.clear();
}

unsigned int CAppConfig::Get_FrameCountValue()
{
    return m_frameCount;
}

unsigned short CAppConfig::Get_ServerUdpPort()
{
    return m_udpPort;
}

unsigned char CAppConfig::Get_ServerGroup()
{
    return m_group;
}

unsigned short CAppConfig::Get_ServerTcpPort()
{
    return m_tcpPort;
}

const char* CAppConfig::Get_DBMWTcpIP()
{
    return m_name.c_str();
}

unsigned short CAppConfig::Get_DBMWTcpPort()
{
    return m_dbmwTcpPort;
}

CAppConfig::CAppConfig()
    : m_name(""), m_dbmwTcpPort(0)
{
}
