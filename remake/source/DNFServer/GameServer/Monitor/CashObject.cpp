// df_monitor_r — CashObject（从 MonitorTypes/App/Table 拆分）
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

#include "CashObject.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "BlackUser.h"
#include "DNFBuddy.h"
#include "DNFMember.h"

void* CCashObject::operator new(unsigned int size) { return ::operator new(size); }

void CCashObject::operator delete(void* p) { ::operator delete(p); }

void CCashObject::operator delete(void* p, unsigned int size) { ::operator delete(p); }

CCashObject::CCashObject()
    : m_lifeTime(5), m_characNo(0), m_memberObject(0)
{
    ClearBuddys();
    ClearMapBlackUsers();
}

CCashObject::~CCashObject()
{
    m_lifeTime = -1;
    m_memberObject = 0;
    m_characNo = 0;
    for (int i = 0; i <= 0x1f; i++)
    {
        m_buddys[i] = 0;
    }
    m_blackUsers.clear();
}

unsigned int CCashObject::GetCharacNo() { return m_characNo; }

CMember* CCashObject::GetMemberObject() { return m_memberObject; }

void CCashObject::SetCharacNo(unsigned int charNo) { m_characNo = charNo; }

void CCashObject::SetMemberObject(CMember* member) { m_memberObject = member; }

void CCashObject::SetBuddysObject(CBuddy** buddies, int count)
{
    if (0x20 < count)
    {
        count = 0x20;
    }
    for (int i = 0; i < count; i++)
    {
        m_buddys[i] = buddies[i];
    }
}

void CCashObject::SetBlackUsersObject(std::map<unsigned int, CBlackUser*>& map)
{
    if (!map.empty())
    {
        m_blackUsers.clear();
        for (std::map<unsigned int, CBlackUser*>::iterator it = map.begin();
             it != map.end(); ++it)
        {
            m_blackUsers.insert(std::make_pair(it->first, it->second));
        }
    }
}

void CCashObject::SetLifeTime(unsigned int lifeTime) { m_lifeTime = lifeTime; }

void CCashObject::ClearMemberObject() { m_memberObject = 0; }

void CCashObject::DeleteMemberObject()
{
    if (m_memberObject != 0)
    {
        delete m_memberObject;
    }
    m_memberObject = 0;
}

int CCashObject::GetBuddysObject(CBuddy** buddies)
{
    int i = 0;
    for (;;)
    {
        buddies[i] = m_buddys[i];
        if (buddies[i] != 0)
        {
            i++;
            if (i <= 0x1f)
            {
                continue;
            }
            i = 0x20;
        }
        break;
    }
    ClearBuddys();
    return i;
}

void CCashObject::DeleteBuddys()
{
    for (int i = 0; i <= 0x1f; i++)
    {
        if (m_buddys[i] != 0)
        {
            delete m_buddys[i];
            m_buddys[i] = 0;
        }
    }
}

void CCashObject::DeleteBlackUsers()
{
    if (!m_blackUsers.empty())
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackUsers.begin();
             it != m_blackUsers.end(); ++it)
        {
            if (it->second != 0)
            {
                CBlackUser::operator delete(it->second);
            }
        }
        m_blackUsers.clear();
    }
}

void CCashObject::ClearBuddys()
{
    for (int i = 0; i <= 0x1f; i++)
    {
        if (m_buddys[i] != 0)
        {
            m_buddys[i] = 0;
        }
    }
}

char CCashObject::IsLifeTimeOut()
{
    if (m_lifeTime == -1)
    {
        return 0;
    }
    m_lifeTime = m_lifeTime - 1;
    return (char)(m_lifeTime == 0);
}

std::map<unsigned int, CBlackUser*>* CCashObject::GetBlackUsersObject() { return &m_blackUsers; }

void CCashObject::ClearMapBlackUsers() { m_blackUsers.clear(); }
