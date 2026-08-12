// df_monitor_r — DNFAppConfig（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFAppConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CAppConfig::CAppConfig() : m_str1(""), m_str2(""), m_ushort28(0), m_ushort30(0)
{
}

CAppConfig::~CAppConfig()
{
    clearServerInfoMap();
}

void CAppConfig::clearServerInfoMap()
{
    for (std::multimap<unsigned int, stServerInfo*>::iterator it = m_serverInfo.begin();
         it != m_serverInfo.end(); it++)
    {
        if (it->second != 0)
        {
            ::operator delete(it->second);
        }
        it->second = 0;
    }
    m_serverInfo.clear();
}

void CAppConfig::Load_Table(const std::string& path)
{
    std::string cfg = "./cfg/" + path + ".cfg";
    int rc = Load_Txt_Table_Data(cfg.c_str(), 0x32);
    if (0 < rc && rc < 0x33)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0xc4, "./log/TableError", "App Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CAppConfig::Load_Setup_Table() Exception Break!");
}

bool CAppConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    if (idx < 8)
    {
        char pad_if[4];
        char* tokens[2];
        if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 2) == 2)
        {
            switch (idx)
            {
            case 0:
                m_frameCountValue = (unsigned char)atoi(tokens[1]);
                break;
            case 1:
                m_serverUdpPort = (unsigned short)atoi(tokens[1]);
                break;
            case 2:
                m_serverGroup = (unsigned char)atoi(tokens[1]);
                break;
            case 3:
                m_serverTcpPort = (unsigned short)atoi(tokens[1]);
                break;
            case 4:
                m_str1 = std::string(tokens[1]);
                break;
            case 5:
                m_ushort28 = (unsigned short)atoi(tokens[1]);
                break;
            case 6:
                m_str2 = std::string(tokens[1]);
                break;
            case 7:
                m_ushort30 = (unsigned short)atoi(tokens[1]);
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
            si->m_field0 = (unsigned char)atoi(tokens[2]);
            si->m_field1 = (unsigned char)atoi(tokens[3]);
            strncpy((char*)si + 3, tokens[4], 0x10);
            si->m_port = (unsigned short)atoi(tokens[5]);
            m_serverInfo.insert(std::pair<const unsigned int, stServerInfo*>(si->m_field2, si));
            return 1;
        }
    }
    return 0;
}

void CAppConfig::Check_FileName(const std::string& filename)
{
    std::string cfg = "./cfg/" + filename + ".cfg";
    std::string pid = "./pid/" + filename + ".pid";
    if (access(cfg.c_str(), 0) != 0)
    {
        DNF_LOG_SCOPE_LINE(0xdb, "./log/Init", "Check_FileName - %s,%s\n", cfg.c_str(), pid.c_str());
        throw CDNFException(
            "CAppConfig::Set_FileName() There is no cfg file Exception Break! Check ./cfg/*.cfg files");
    }
    if (access(pid.c_str(), 0) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xe5, "./log/Init", "Check_FileName - %s,%s\n", cfg.c_str(), pid.c_str());
        throw CDNFException(
            "CAppConfig::Set_FileName() There is already pid file! Exception Break! Check ./pid/*.pid files");
    }
}

std::multimap<unsigned int, stServerInfo*>* CAppConfig::GetServerInfoMap()
{
    return &m_serverInfo;
}

unsigned char CAppConfig::Get_ServerGroup()
{
    return m_serverGroup;
}

unsigned short CAppConfig::Get_FrameCountValue()
{
    return m_frameCountValue;
}

unsigned short CAppConfig::Get_ServerUdpPort()
{
    return m_serverUdpPort;
}

const char* CAppConfig::Get_DBMWTcpIP()
{
    return m_str1.c_str();
}

unsigned short CAppConfig::Get_DBMWTcpPort()
{
    return m_ushort28;
}

const char* CAppConfig::Get_ManagerTcpIP()
{
    return m_str2.c_str();
}

unsigned short CAppConfig::Get_ManagerTcpPort()
{
    return m_ushort30;
}

unsigned short CAppConfig::Get_ServerTcpPort()
{
    return m_serverTcpPort;
}
