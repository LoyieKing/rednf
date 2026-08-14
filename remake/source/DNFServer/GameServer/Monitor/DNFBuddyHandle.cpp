// df_monitor_r — DNFBuddyHandle（从 MonitorTypes/App/Table 拆分）
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

#include "DNFBuddyHandle.h"
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
#include "DNFMember.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFUser.h"

CBuddyHandle::CBuddyHandle()
{
    m_prUser = 0;
    m_buddyDBFlag = 0;
}

unsigned short CBuddyHandle::GetBuddyDBFlag() { return m_buddyDBFlag; }

void CBuddyHandle::SetBuddyDBFlag(unsigned short flag) { m_buddyDBFlag |= flag; }

CBuddyHandle::~CBuddyHandle() { reset(0, 0); }

int CBuddyHandle::addDB(CServerHandler* handler, char* name)
{
    bool invalid = true;
    if (m_prUser != 0 && m_prUser->GetUniqCharNo() != 0)
    {
        invalid = false;
    }
    if (invalid)
    {
        DNF_LOG_SCOPE_LINE(0x5a, "./log/buddy", "Buddy::addDB m_prUser is NULL");
        return 1;
    }
    if (name == 0)
    {
        return 1;
    }
    if (m_buddies.size() < 0x20)
    {
        if (m_buddies.find(name) == m_buddies.end())
        {
            if (strcmp(name, m_prUser->GetCharName()) == 0)
            {
                return 1;
            }
            if (strlen(name) < 0x1e)
            {
                Packet_DBMW_Add_Buddy pkt;
                pkt.m_uniqCharNo = m_prUser->GetUniqCharNo();
                memcpy(pkt.m_charName, name, 0x1d);
                handler->SendToDB(&pkt);
                return 0;
            }
            return -1;
        }
        return 0x12;
    }
    return 4;
}

int CBuddyHandle::delDB(CServerHandler* handler, char* name)
{
    bool invalid = true;
    if (m_prUser != 0 && m_prUser->GetUniqCharNo() != 0)
    {
        invalid = false;
    }
    if (invalid)
    {
        DNF_LOG_SCOPE_LINE(0xb7, "./log/buddy", "Buddy::addDB m_prUser is NULL");
        return 1;
    }
    if (name == 0)
    {
        return 1;
    }
    if (strlen(name) < 0x1e)
    {
        std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
        if (it != m_buddies.end())
        {
            Packet_DBMW_Del_Buddy pkt;
            pkt.m_uniqCharNo = m_prUser->GetUniqCharNo();
            pkt.m_buddyCharNo =
                ((STBuddyDBInfo*)it->second->getBuddyDBInfo())->m_characNo;
            memcpy(pkt.m_charName, name, 0x1d);
            handler->SendToDB(&pkt);
            return 0;
        }
        return 0x12;
    }
    return -1;
}

void CBuddyHandle::setBuddyCharName(int charNo, const std::string& newName)
{
    for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
         it != m_buddies.end(); ++it)
    {
        if (it->second != 0 &&
            charNo == ((STBuddyDBInfo*)it->second->getBuddyDBInfo())->m_characNo)
        {
            CBuddy* buddy = it->second;
            m_buddies.erase(it);
            m_buddies.insert(std::pair<const std::string, CBuddy*>(newName, buddy));
            break;
        }
    }
}

int CBuddyHandle::getBuddysCharNo(unsigned int* out)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    int count = 0;
    std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
    std::map<std::string, CBuddy*>::iterator end = m_buddies.end();
    for (; it != end; ++it)
    {
        CBuddy* b = it->second;
        if (b != 0)
        {
            out[count] = ((STBuddyDBInfo*)b->getBuddyDBInfo())->m_characNo;
        }
        count++;
        if (0x20 < count)
        {
            DNF_LOG_SCOPE_LINE(0x135,"./log/buddy", "CBuddyHandle::GetBuddysCharNo iCnt(%d) > MAX_BUDDY_COUNT(%d)",
                count, 0x20);
            return 0x20;
        }
    }
    return count;
}

int CBuddyHandle::getBuddys(CBuddy** out)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    int count = 0;
    std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
    std::map<std::string, CBuddy*>::iterator end = m_buddies.end();
    for (; it != end; ++it)
    {
        out[count] = it->second;
        count++;
        if (0x20 < count)
        {
            DNF_LOG_SCOPE_LINE(0x153,"./log/buddy", "CBuddyHandle::GetBuddysCharNo iCnt(%d) > MAX_BUDDY_COUNT(%d)",
                count, 0x20);
            return 0x20;
        }
    }
    return count;
}

int CBuddyHandle::del(std::string name)
{
    std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
    if (it == m_buddies.end())
    {
        return 1;
    }
    if (it->second != 0)
    {
        delete it->second;
    }
    m_buddies.erase(it);
    return 0;
}

int CBuddyHandle::addFromCash(CBuddy* buddy)
{
    if (m_buddies.size() > 0x1f)
    {
        return 0;
    }
    return m_buddies.insert(
               std::make_pair(((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_name,
                              buddy))
        .second;
}

void CBuddyHandle::insert(CBuddy* buddy)
{
    if (m_buddies.size() > 0x1f)
    {
        return;
    }
    m_buddies.insert(
        std::make_pair(((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_name, buddy));
}

CBuddy* CBuddyHandle::findBuddy(std::string name)
{
    if (m_buddies.empty())
    {
        return 0;
    }
    std::map<std::string, CBuddy*>::iterator it = m_buddies.find(name);
    if (it != m_buddies.end())
    {
        return it->second;
    }
    return 0;
}

CBuddy* CBuddyHandle::findBuddyByCharNo(unsigned int charNo)
{
    std::map<std::string, CBuddy*>::iterator it;
    std::map<std::string, CBuddy*>::iterator it2 = m_buddies.begin();
    for (; it2 != m_buddies.end(); ++it2)
    {
        CBuddy* buddy = it2->second;
        if (buddy != 0 &&
            ((STBuddyDBInfo*)buddy->getBuddyDBInfo())->m_characNo == charNo)
        {
            return it2->second;
        }
    }
    return 0;
}

void CBuddyHandle::printBuddys(char* out)
{
    if (m_prUser != 0 && !m_buddies.empty())
    {
        for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
             it != m_buddies.end(); ++it)
        {
            CBuddy* buddy = it->second;
            char* info = (char*)buddy->getBuddyDBInfo();
            DNF_LOG_SCOPE_LINE(0x16e,"./log/buddy",
                "[%s] name(%s) fname(%s) flevel(%d) fjob(%d) fgrowtype(%d) fcharNo(%d) "
                "fsex(%d)",
                out, m_prUser->GetCharName(), info,
                (int)((STBuddyDBInfo*)info)->m_lev,
                (int)((STBuddyDBInfo*)info)->m_job,
                (int)((STBuddyDBInfo*)info)->m_growType,
                ((STBuddyDBInfo*)info)->m_characNo,
                (int)((STBuddyDBInfo*)info)->m_sex);
        }
    }
}

int CBuddyHandle::add(std::string name, STBuddyDBInfo& info)
{
    if (m_buddies.size() < 0x20)
    {
        CBuddy* buddy = new CBuddy(info);
        std::pair<std::map<std::string, CBuddy*>::iterator, bool> r =
            m_buddies.insert(std::pair<const std::string, CBuddy*>(name, buddy));
        if (!r.second)
        {
            delete buddy;
            return 0;
        }
        return 1;
    }
    return 0;
}

void CBuddyHandle::reset(CUser* user, bool flag)
{
    m_prUser = user;
    m_buddyDBFlag = 0;
    clear(flag);
}

void CBuddyHandle::clear(bool flag)
{
    if (m_buddies.empty())
    {
    }
    else
    {
        if (flag)
        {
            for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
                 it != m_buddies.end(); ++it)
            {
                if (it->second != 0)
                {
                    delete it->second;
                }
            }
        }
        m_buddies.clear();
    }
    return;
}
