// df_monitor_r — LoginLogoutStatistics（从 MonitorTypes/App/Table 拆分）
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

#include "LoginLogoutStatistics.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFServerHandler.h"
#include "DNFApplication.h"
#include "DNFPacketTranslater.h"

CLoginLogoutStatistics::CLoginLogoutStatistics(CApplication& app)
    : m_app(&app)
{
    m_fieldac = 0;
    m_fieldb0 = 0;
}

CLoginLogoutStatistics::~CLoginLogoutStatistics()
{
    for (int i = 0; i < 7; i++)
    {
        m_maps[i].clear();
    }
}

void CLoginLogoutStatistics::ProcessByMinute()
{
    Packet_DBMW_Statistic_Login_Logout pkt;
    int v0, v1, v2, v3;
    int cnt = 0;
    v0 = m_fieldac;
    v1 = m_fieldb0;
    v2 = m_fieldb4;
    v3 = m_fieldb8;
    m_fieldb4 = 0;
    m_fieldb8 = 0;
    m_app->Get_ServerHandler()->GetDBServer()->SendToServer((char*)&pkt, 0x618);
}

void CLoginLogoutStatistics::LoginLogout(ENUM_LOGIN_LOGOUT type, unsigned char channel)
{
    std::map<unsigned char, stLoginLogout>::iterator it = m_maps[(int)type].find(channel);
    if (it != m_maps[(int)type].end())
    {
        it->second.m_count = it->second.m_count + 1;
    }
    else
    {
        for (int i = 0; i < 7; i++)
        {
            stLoginLogout st;
            st.m_field0 = 0;
            st.m_count = 0;
            st.m_field8 = 0;
            m_maps[i].insert(std::pair<const unsigned char, stLoginLogout>(channel, st));
        }
        LoginLogout(type, channel);
    }
}

void CLoginLogoutStatistics::CountNumOfLoginout(ENUM_LOGIN_LOGOUT type)
{
    if ((int)type == 0)
    {
        m_fieldb4 = m_fieldb4 + 1;
    }
    else if ((int)type == 6)
    {
        m_fieldb8 = m_fieldb8 + 1;
    }
}

void CLoginLogoutStatistics::CountNumOfOccupations(ENUM_LOGIN_LOGOUT type, int value)
{
    if ((int)type == 0)
    {
        m_fieldac = value;
    }
    else if ((int)type == 4)
    {
        m_fieldb0 = value;
    }
}

stLoginLogout::stLoginLogout() { m_field0 = 0; }
