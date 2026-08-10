// df_monitor_r — DNFKillUserConfig（从 MonitorTypes/App/Table 拆分）
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

#include "DNFKillUserConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CKillUSRConfig::CKillUSRConfig() {}

CKillUSRConfig::~CKillUSRConfig() { Clear_Table(); }

void CKillUSRConfig::Load_Table(const std::string& path) {}

int CKillUSRConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tokens[4];
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 4) == 4)
    {
        ST_KillUSRConfig* cfg = new (std::nothrow) ST_KillUSRConfig;
        if (cfg != 0)
        {
            cfg->m_type = atoi(tokens[0]);
            cfg->m_val = atoi(tokens[1]);
            cfg->m_b = atoi(tokens[2]);
            cfg->m_c = atoi(tokens[3]);
            m_vec.push_back(cfg);
            return 1;
        }
    }
    return 0;
}

void CKillUSRConfig::Clear_Table()
{
    if (!m_vec.empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = m_vec.begin();
             it != m_vec.end(); ++it)
        {
            if (*it != 0)
            {
                ::operator delete(*it);
                *it = 0;
            }
        }
        m_vec.clear();
    }
}

std::vector<ST_KillUSRConfig*>* CKillUSRConfig::GetInfo() const
{
    return const_cast<std::vector<ST_KillUSRConfig*>*>(&m_vec);
}

ST_KillUSRConfig::ST_KillUSRConfig()
{
    m_type = 0;
    m_val = 0;
    m_b = 0;
    m_c = 0;
}

