// df_monitor_r — BuddyRegisterManager（从 MonitorTypes/App/Table 拆分）
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

#include "BuddyRegisterManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFBuddy.h"

CBuddyRegisterManager::CBuddyRegisterManager()
{
    m_map.clear();
}

CBuddyRegisterManager::~CBuddyRegisterManager()
{
    m_map.clear();
}

void CBuddyRegisterManager::addBuddyRegister(unsigned int key, unsigned int value)
{
    m_map.insert(std::make_pair(key, value));
}

int CBuddyRegisterManager::delBuddyRegister(unsigned int key, unsigned int value)
{
    std::multimap<unsigned int, unsigned int>::iterator it = m_map.lower_bound(key);
    for (; it != m_map.upper_bound(key); ++it)
    {
        if (it->second == value)
        {
            m_map.erase(it);
            return 1;
        }
    }
    return 0;
}

void CBuddyRegisterManager::delBuddyRegister(unsigned int key)
{
    std::multimap<unsigned int, unsigned int>::iterator it = m_map.lower_bound(key);
    while (it != m_map.upper_bound(key))
    {
        m_map.erase(it++);
    }
}

void CBuddyRegisterManager::findBuddyRegister(unsigned int key, std::vector<unsigned int>& out)
{
    std::multimap<unsigned int, unsigned int>::iterator it = m_map.lower_bound(key);
    std::multimap<unsigned int, unsigned int>::iterator end = m_map.upper_bound(key);
    for (; it != end; ++it)
    {
        out.push_back(it->second);
    }
}

void CBuddyRegisterManager::printBuddyRegister(char* name, unsigned int flag)
{
    for (std::multimap<unsigned int, unsigned int>::iterator it = m_map.lower_bound(flag);
         it != m_map.upper_bound(flag); ++it)
    {
        DNF_LOG_SCOPE_LINE(0x68, "./log/buddyRegister", "[%s] rcharNo(%d)", name, it->second);
    }
}
