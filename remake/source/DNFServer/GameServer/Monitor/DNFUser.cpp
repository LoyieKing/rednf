// df_monitor_r — DNFUser（从 MonitorTypes/App/Table 拆分）
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

#include "DNFUser.h"
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
#include "DNFChannelWaitingUser.h"
#include "DNFGameServer.h"
#include "DNFMember.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"

void CUser::operator delete(void* p, unsigned int size) { ::operator delete(p); }

CUser::CUser()
{
    *(unsigned int*)((char*)this + 0x0) = 0;
    *(unsigned int*)((char*)this + 0x4) = 0;
    *(unsigned int*)((char*)this + 0x8) = 0;
    *(unsigned int*)((char*)this + 0xc) = 0;
    *(char*)((char*)this + 0x10) = 0;
    *(unsigned int*)((char*)this + 0x14) = 0;
    *(unsigned short*)((char*)this + 0x18) = 0;
    *(char*)((char*)this + 0x1a) = 0;
    *(unsigned int*)((char*)this + 0x1c) = 0;
    *(unsigned int*)((char*)this + 0x20) = 0xffffffff;
    *(char*)((char*)this + 0x42) = 0xff;
    *(char*)((char*)this + 0x43) = 0xff;
    *(unsigned short*)((char*)this + 0x44) = 0xffff;
    *(char*)((char*)this + 0x46) = 1;
    m_field68 = 0;
    *(char*)((char*)this + 0xb0) = 0;
    *(unsigned int*)((char*)this + 0xb4) = 0;
    memset((char*)this + 0x24, 0, 0x1e);
    memset((char*)this + 0x47, 0, 7);
    m_channelCount = 0;
    m_channelInfoMap.clear();
}

CUser::~CUser()
{
    *(unsigned int*)((char*)this + 0x8) = 0;
    *(unsigned int*)((char*)this + 0xc) = 0;
    *(unsigned int*)((char*)this + 0x0) = 0;
    *(unsigned int*)((char*)this + 0x4) = 0;
    *(unsigned int*)((char*)this + 0x20) = 0xffffffff;
    *(char*)((char*)this + 0x42) = 0xff;
    *(char*)((char*)this + 0x43) = 0xff;
    *(unsigned short*)((char*)this + 0x44) = 0xffff;
    memset((char*)this + 0x24, 0, 0x1e);
    ResetMemberInfo();
    *(char*)((char*)this + 0x10) = 0;
    m_field68 = 0;
    *(char*)((char*)this + 0x46) = 1;
    m_channelCount = 0;
    m_channelInfoMap.clear();
}

void* CUser::operator new(unsigned int size) { return ::operator new(size); }

unsigned int CUser::GetUniqCharNo() { return *(unsigned int*)((char*)this + 4); }

void CUser::AttachMember(CMember* member) {}

void CUser::operator delete(void* p) { ::operator delete(p); }

void* CUser::GetGameServer() { return *(void**)((char*)this + 8); }

void* CUser::GetTcpGameServer() { return 0; }

unsigned int CUser::GetDBID() { return *(unsigned int*)((char*)this + 0); }

short CUser::GetLevel() { return *(short*)((char*)this + 0x44); }

char CUser::GetJob() { return *(char*)((char*)this + 0x42); }

char CUser::GetGrowthType() { return *(char*)((char*)this + 0x43); }

unsigned int CUser::GetIdByChannel() { return *(unsigned int*)((char*)this + 0x20); }

char* CUser::GetCharName() { return (char*)this + 0x24; }

char CUser::IsBlackUser(unsigned int key)
{
    if (m_blackList.empty())
    {
        return 0;
    }
    std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(key);
    if (it != m_blackList.end())
    {
        return 1;
    }
    return 0;
}

unsigned char CUser::GetUpperMemberExpLevel()
{
    if (*(int*)((char*)this + 0x14) != 0)
    {
        CMember* member = (CMember*)*(int*)((char*)this + 0x14);
        if (member->GetMemberKey() != 0 && (GetMemberDBFlag() & 4) != 0)
        {
            return (unsigned char)member->GetUpperMemberExpLevel();
        }
    }
    return 0;
}

void CUser::SendTcpGameserver(PacketHeader* pkt)
{
    if (*(int*)((char*)this + 0xc) != 0)
    {
        CTcpGameServer* tcp = (CTcpGameServer*)*(int*)((char*)this + 0xc);
        char* buf = tcp->makePacketHeader(*(unsigned short*)pkt,
                                          *(unsigned short*)((char*)pkt + 2));
        if (buf != 0)
        {
            memcpy(buf + 10, (char*)pkt + 10, *(unsigned short*)((char*)pkt + 2) - 10);
            tcp->SendToGameServer(buf);
        }
    }
}

void CUser::SendToGameserver(char* buf, int len)
{
    if (m_gameServer)
    {
        m_gameServer->SendToServer(buf, len);
    }
}

void CUser::ResetChannelUserCount(int count)
{
    m_channelCount = count;
    m_channelInfoMap.clear();
}

void CUser::SetChannelUserCount(int a, int b, int c, int d)
{
    m_channelInfoMap.insert(std::make_pair(a, ChannelInfo(b, c, d)));
}

char CUser::IsCompleteChannelUserCount()
{
    if (m_channelCount != 0 && (int)m_channelInfoMap.size() == m_channelCount)
    {
        return 1;
    }
    return 0;
}

void CUser::GetChannelUserCount(STPvPChannelInfo* out, unsigned char& count)
{
    int idx = 0;
    for (std::map<int, ChannelInfo>::iterator it = m_channelInfoMap.begin();
         it != m_channelInfoMap.end(); ++it)
    {
        if ((int)(unsigned char)count <= idx)
        {
            break;
        }
        out[idx].m_channel = it->first;
        out[idx].m_countA = it->second.m_a;
        out[idx].m_countB = it->second.m_b;
        out[idx].m_countC = it->second.m_c;
        idx++;
    }
    count = (unsigned char)idx;
}

void CUser::MemberEnterProcess()
{
    if (*(int*)((char*)this + 0x1c) != 0)
    {
        *(char*)((char*)this + 0x1a) = *(char*)((char*)this + 0x1a) - 1;
        if ((signed char)*(char*)((char*)this + 0x1a) <= 0)
        {
            *(int*)((char*)this + 0x1c) = 0;
            *(char*)((char*)this + 0x1a) = 0;
        }
    }
}

void CUser::SetBuddyCharName(int dbid, const std::string& name)
{
    m_buddyHandle.setBuddyCharName(dbid, name);
}

int CUser::ChangeCharNameToBlackList(unsigned int dbid, char* name)
{
    if (!m_blackList.empty())
    {
        std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(dbid);
        if (it != m_blackList.end())
        {
            it->second->ChangeCharName(name);
            return 1;
        }
    }
    return 0;
}

void CUser::AddBuddyFromCash(CBuddy* buddy)
{
    m_buddyHandle.addFromCash(buddy);
}

void CUser::SetBuddyDBFlag(unsigned short flag)
{
    m_buddyHandle.SetBuddyDBFlag(flag);
}

int CUser::AddBuddyDB(CServerHandler* handler, char* name)
{
    return ((CBuddyHandle*)((char*)this + 0x6c))->addDB(handler, name);
}

int CUser::DelBuddyDB(CServerHandler* handler, char* name)
{
    return ((CBuddyHandle*)((char*)this + 0x6c))->delDB(handler, name);
}

int CUser::AddBuddy(STBuddyDBInfo& info)
{
    std::string name((char*)&info);
    return ((CBuddyHandle*)((char*)this + 0x6c))->add(name, info);
}

char CUser::DelBuddy(char* name)
{
    std::string s(name);
    return (char)((CBuddyHandle*)((char*)this + 0x6c))->del(s);
}

void CUser::RegisterToCashBlackList(std::map<unsigned int, CBlackUser*>& map)
{
    if (!map.empty())
    {
        m_blackList.clear();
        for (std::map<unsigned int, CBlackUser*>::iterator it = map.begin();
             it != map.end(); ++it)
        {
            m_blackList.insert(std::make_pair(it->first, it->second));
        }
    }
}

void CUser::SetBlackListDBFlag(unsigned short flag) { m_field68 |= flag; }

void CUser::SetDBID(unsigned int dbid) {}

void CUser::SetUniqCharNo(unsigned int charNo) {}

void CUser::SetIdByChannel(int channel) {}

void CUser::SetGameServer(CGameServer* server) { m_gameServer = server; }

void CUser::PrintBuddys(char* out)
{
    m_buddyHandle.printBuddys(out);
}

void CUser::SetUserPosState(unsigned char state) {}

void CUser::SetUserChangableInfo(short level, char flag)
{
    *(short*)((char*)this + 0x44) = level;
    *(char*)((char*)this + 0x43) = flag;
}

void CUser::SetUserInfo_CharNo(char a, char b, short level, unsigned int charNo, char* name)
{
    *(char*)((char*)this + 0x42) = a;
    *(char*)((char*)this + 0x43) = b;
    *(short*)((char*)this + 0x44) = level;
    *(unsigned int*)((char*)this + 4) = charNo;
    memcpy((char*)this + 0x24, name, 0x1d);
    ((CBuddyHandle*)((char*)this + 0x6c))->reset(this, true);
}

void CUser::SetSex(unsigned char sex)
{
    *(char*)((char*)this + 0x46) = (char)sex;
}

void CUser::SetSsn(char* ssn)
{
    memcpy((char*)this + 0x47, ssn, 6);
}

void CUser::SetEvent_idx(unsigned int idx)
{
    *(unsigned int*)((char*)this + 0xb4) = idx;
}

void CUser::Event_idx_modify_state()
{
    m_data2[8] = 1;
}

void CUser::SetTcpGameServer(CTcpGameServer* server)
{
    *(CTcpGameServer**)((char*)this + 0xc) = server;
}

void CUser::QueryBuddyInfo(CServerHandler* handler)
{
    Packet_DBMW_Query_Buddy_Info pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = GetUniqCharNo();
    handler->SendToDB(&pkt);
}

void CUser::ResetMemberInfo()
{
    *(unsigned int*)((char*)this + 0x14) = 0;
    *(unsigned short*)((char*)this + 0x18) = 0;
    *(char*)((char*)this + 0x1a) = 0;
    *(unsigned int*)((char*)this + 0x1c) = 0;
}

void CUser::ResetCharInfo(bool flag)
{
    *(unsigned int*)((char*)this + 4) = 0;
    *(char*)((char*)this + 0x42) = 0xff;
    *(char*)((char*)this + 0x43) = 0xff;
    *(unsigned short*)((char*)this + 0x44) = 0xffff;
    memset((char*)this + 0x24, 0, 0x1e);
    ResetMemberInfo();
    ((CBuddyHandle*)((char*)this + 0x6c))->reset(0, flag);
}

void CUser::ResetBlackList(int flag)
{
    if (!m_blackList.empty())
    {
        if (flag != 0)
        {
            for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
                 it != m_blackList.end(); ++it)
            {
                if (it->second != 0)
                {
                    delete it->second;
                }
            }
        }
        m_blackList.clear();
    }
}

void CUser::ResetBuddyList(bool flag)
{
    m_buddyHandle.reset(0, flag);
}

int CUser::GetBuddysCharNo(unsigned int* out)
{
    return ((CBuddyHandle*)((char*)this + 0x6c))->getBuddysCharNo(out);
}

void CUser::GetBuddiesCharNo(std::vector<unsigned int>& out)
{
    unsigned int buf[32];
    memset(buf, 0, 0x80);
    int count = ((CBuddyHandle*)((char*)this + 0x6c))->getBuddysCharNo(buf);
    for (int i = 0; i < count; i++)
    {
        out.push_back(buf[i]);
    }
}

int CUser::GetBuddys(CBuddy** out)
{
    return ((CBuddyHandle*)((char*)this + 0x6c))->getBuddys(out);
}

void CUser::SendNoticeBuddyInOut(unsigned char channel, unsigned int charNo, char* name,
                                 unsigned char flag1, unsigned char flag2, char flag3)
{
    if (GetGameServer() != 0)
    {
        Packet_Monitor_Notice_Buddy_In_Out pkt;
        pkt.m_charNo = charNo;
        pkt.m_idByChannel = GetIdByChannel();
        pkt.m_channel = channel;
        pkt.m_field13 = flag1;
        pkt.m_field14 = flag2;
        memcpy(pkt.m_name, name, 0x1d);
        pkt.m_field33 = (unsigned char)flag3;
        ((CServerInterface*)GetGameServer())->SendToServer(
            (char*)&pkt, *(unsigned short*)((char*)&pkt + 2));
    }
}

unsigned short CUser::GetBuddyDBFlag()
{
    return m_buddyHandle.GetBuddyDBFlag();
}

unsigned short CUser::GetBlackListDBFlag()
{
    return *(unsigned short*)((char*)this + 0x68);
}

std::map<unsigned int, CBlackUser*>* CUser::GetMapBlackList()
{
    return &m_blackList;
}

void CUser::GetBlackList(unsigned char& count, STBlackUserDBType* out)
{
    count = 0;
    if (!m_blackList.empty())
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end(); ++it)
        {
            memcpy((char*)out + (unsigned int)count * 0x28 + 4, it->second->GetName(), 0x1d);
            *(unsigned int*)((char*)out + (unsigned int)count * 0x28 + 0x24) =
                it->second->GetOccurTime();
            *(unsigned int*)((char*)out + (unsigned int)count * 0x28) = it->first;
            count++;
            if (9 < count)
            {
                return;
            }
        }
    }
}

void CUser::GetBlackList(unsigned char& count, unsigned int* out)
{
    count = 0;
    if (!m_blackList.empty())
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end(); ++it)
        {
            out[(unsigned int)count] = it->first;
            count++;
            if (9 < count)
            {
                return;
            }
        }
    }
}

unsigned int CUser::GetBlackListSize()
{
    return (unsigned int)m_blackList.size();
}

char CUser::RegisterToBlackList(unsigned int charNo, char* name)
{
    if (name == 0 || charNo == 0)
    {
        DNF_LOG_SCOPE_LINE(0x16e, "./log/BlackList", "Register Err(%d)(%s)", charNo, name);
        return 0;
    }
    CBlackUser* user = new CBlackUser;
    user->SetBlackUser(name, (unsigned int)time(0));
    std::pair<std::map<unsigned int, CBlackUser*>::iterator, bool> r =
        m_blackList.insert(std::pair<const unsigned int, CBlackUser*>(charNo, user));
    return r.second ? 1 : 0;
}

char CUser::RegisterToBlackList(unsigned int charNo, char* name, unsigned int time)
{
    CBlackUser* user = new CBlackUser;
    user->SetBlackUser(name, time);
    std::pair<std::map<unsigned int, CBlackUser*>::iterator, bool> r =
        m_blackList.insert(std::pair<const unsigned int, CBlackUser*>(charNo, user));
    return r.second ? 1 : 0;
}

int CUser::DeleteToBlackList(unsigned int charNo)
{
    if (!m_blackList.empty())
    {
        std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(charNo);
        if (it != m_blackList.end())
        {
            delete it->second;
            m_blackList.erase(it);
            return 1;
        }
    }
    return 0;
}

void CUser::SendBlackList()
{
    Packet_Monitor_Notice_Black_List pkt;
    pkt.m_dbid = *(unsigned int*)((char*)this + 0);
    pkt.m_idByChannel = *(unsigned int*)((char*)this + 0x20);
    unsigned char count = 0;
    GetBlackList(count, pkt.m_charNos);
    pkt.m_count = count;
    SendTcpGameserver(&pkt);
}

unsigned int CUser::GetMemberEnterCallerId()
{
    return *(unsigned int*)((char*)this + 0x1c);
}

char CUser::CheckPrevCallMemberEnter()
{
    if (*(char*)((char*)this + 0x1a) < 1)
    {
        return 0;
    }
    if (*(int*)((char*)this + 0x1c) == 0)
    {
        return 0;
    }
    return 1;
}

void CUser::ResetRequestMemberEnter()
{
    *(unsigned int*)((char*)this + 0x1c) = 0;
    *(char*)((char*)this + 0x1a) = 0;
}

char CUser::RecordCallMemberEnter(unsigned int callerId, unsigned short count)
{
    char old = *(char*)((char*)this + 0x1a);
    if (old == 0)
    {
        *(unsigned int*)((char*)this + 0x1c) = callerId;
        *(char*)((char*)this + 0x1a) = (char)count;
    }
    return old == 0;
}

unsigned int CUser::GetMemberDBFlag()
{
    if (*(int*)((char*)this + 0x14) == 0)
    {
        return 0;
    }
    return ((CMember*)*(int*)((char*)this + 0x14))->GetMemberDBFlag();
}

int CUser::GetConnLowerMemberCnt()
{
    if (*(int*)((char*)this + 0x14) == 0)
    {
        return 0;
    }
    CMember* member = (CMember*)*(int*)((char*)this + 0x14);
    if (member->GetMemberKey() == 0 || (GetMemberDBFlag() & 4) == 0)
    {
        return 0;
    }
    return member->GetConnLowerMemberCnt();
}

CMember* CUser::GetMember()
{
    return *(CMember**)((char*)this + 0x14);
}

void CUser::SetMemberRegisterFlag(bool flag)
{
    if (*(int*)((char*)this + 0x14) != 0)
    {
        ((CMember*)*(int*)((char*)this + 0x14))->SetMemberRegisterFlag(flag);
    }
}

char CUser::IsAbleToRegisterMember()
{
    if (*(int*)((char*)this + 0x14) == 0)
    {
        return 1;
    }
    return ((CMember*)*(int*)((char*)this + 0x14))->IsAbleToRegisterMember();
}

unsigned int CUser::GetMemberKey()
{
    if (*(int*)((char*)this + 0x14) == 0)
    {
        return 0;
    }
    return ((CMember*)*(int*)((char*)this + 0x14))->GetMemberKey();
}

