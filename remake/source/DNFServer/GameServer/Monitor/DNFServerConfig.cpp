// df_monitor_r — DNFServerConfig（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "DNFServerConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CServerConfig::CServerConfig() {}

CServerConfig::~CServerConfig() {}

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

bool CServerConfig::Parse_Table(char* line, int idx)
{
    if (*(char*)line == '#')
    {
        return 0;
    }
    char* pad;
    char* tokens[5];
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 5) == 5)
    {
        if (idx <= 0xfe)
        {
            ST_ServerInfo* entry = &m_table[idx];
            entry->m_field0 = (char)atoi(tokens[0]);
            entry->m_field1 = (char)atoi(tokens[1]);
            entry->m_field2 = (char)atoi(tokens[2]);
            entry->m_str = tokens[3];
            entry->m_field8 = (unsigned short)atoi(tokens[4]);
            return 1;
        }
    }
    return 0;
}
