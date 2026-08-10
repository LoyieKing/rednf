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

CBuddyHandle::CBuddyHandle() {}

unsigned short CBuddyHandle::GetBuddyDBFlag() { return m_field1c; }

void CBuddyHandle::SetBuddyDBFlag(unsigned short flag) { m_field1c |= flag; }

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
                *(unsigned int*)((char*)it->second->getBuddyDBInfo() + 0x22);
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
            charNo == *(int*)((char*)it->second->getBuddyDBInfo() + 0x22))
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
    for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
         it != m_buddies.end(); ++it)
    {
        CBuddy* b = it->second;
        if (b != 0)
        {
            out[count] = *(unsigned int*)((char*)b->getBuddyDBInfo() + 0x22);
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
    for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
         it != m_buddies.end(); ++it)
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
    m_buddies.insert(std::make_pair(std::string((char*)buddy->getBuddyDBInfo()), buddy));
    return 1;
}

int CBuddyHandle::insert(CBuddy* buddy)
{
    if (m_buddies.size() > 0x1f)
    {
        return 0;
    }
    m_buddies.insert(std::make_pair(std::string((char*)buddy->getBuddyDBInfo()), buddy));
    return 1;
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
    for (std::map<std::string, CBuddy*>::iterator it = m_buddies.begin();
         it != m_buddies.end(); ++it)
    {
        CBuddy* buddy = it->second;
        if (buddy != 0 &&
            *(unsigned int*)((char*)buddy->getBuddyDBInfo() + 0x22) == charNo)
        {
            return buddy;
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
                out, m_prUser->GetCharName(), info, (int)*(short*)(info + 0x1e),
                (int)*(char*)(info + 0x20), (int)*(char*)(info + 0x21),
                *(int*)(info + 0x22), (int)*(char*)(info + 0x26));
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
    m_field1c = 0;
    clear(flag);
}

void CBuddyHandle::clear(bool flag)
{
    if (!m_buddies.empty())
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
}

