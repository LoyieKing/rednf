// df_monitor_r — GMAccounts（从 MonitorTypes/App/Table 拆分）
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

#include "GMAccounts.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

namespace WongWork
{
CGMAccounts::CGMAccounts() {}

CGMAccounts::~CGMAccounts() {}

bool CGMAccounts::isGM(unsigned int dbid)
{
    for (std::list<stGMInfo_t>::iterator it = m_list.begin(); it != m_list.end(); ++it)
    {
        if (it->m_dbid == dbid)
        {
            return true;
        }
    }
    return false;
}

CGMAccounts::stGMInfo_t CGMAccounts::getGMInfo(unsigned int dbid) const
{
    stGMInfo_t out;
    out.m_dbid = 0;
    out.m_field4 = 3;
    for (std::list<stGMInfo_t>::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
    {
        if (it->m_dbid == dbid)
        {
            out = *it;
            break;
        }
    }
    return out;
}

void CGMAccounts::clearGmList()
{
    m_list.clear();
}

void CGMAccounts::AppendGM_Sys(unsigned int dbid, char level)
{
    for (std::list<stGMInfo_t>::iterator it = m_list.begin(); it != m_list.end(); ++it)
    {
        if (it->m_dbid == dbid)
        {
            return;
        }
    }
    stGMInfo_t info;
    info.m_dbid = dbid;
    info.m_field4 = (unsigned int)level;
    m_list.push_back(info);
}

char CGMAccounts::loadGMAccounts(const char* path) { return 1; }

char CGMAccounts::appendGM(unsigned int a, unsigned int b)
{
    stGMInfo_t info;
    return 0;
}

char CGMAccounts::removeGM(unsigned int a, unsigned int b)
{
    stGMInfo_t info[3];
    return 0;
}

void CGMAccounts::LoadGmList(unsigned int dbid, int level)
{
    stGMInfo_t info;
    info.m_dbid = dbid;
    info.m_field4 = (unsigned int)level;
    m_list.push_back(info);
}

bool CGMAccounts::stGMInfo_t::operator==(const stGMInfo_t& other) const
{
    return m_dbid == other.m_dbid;
}
}
