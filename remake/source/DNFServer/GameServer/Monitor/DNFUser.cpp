// df_monitor_r — DNFUser（从 MonitorTypes/App/Table 拆分）
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
    : m_dbid(0), m_uniqCharNo(0), m_gameServer(0), m_tcpGameServer(0), m_posState(0),
      m_member(0), m_field18(0), m_memberEnterCount(0), m_memberEnterCallerId(0), m_idByChannel(-1),
      m_job(0xff), m_growthType(0xff), m_level(-1), m_sex(1), m_blackListDBFlag(0)
{
    m_data2[8] = 0;
    m_eventIdx = 0;
    memset(m_dataC2, 0, 0x1e);
    memset(m_dataC2b, 0, 7);
    m_channelCount = 0;
    m_channelInfoMap.clear();
}

CUser::~CUser()
{
    m_gameServer = 0;
    m_tcpGameServer = 0;
    m_dbid = 0;
    m_uniqCharNo = 0;
    m_idByChannel = -1;
    m_job = 0xff;
    m_growthType = 0xff;
    m_level = -1;
    memset(m_dataC2, 0, 0x1e);
    ResetMemberInfo();
    m_posState = 0;
    m_blackListDBFlag = 0;
    m_sex = 1;
    m_channelCount = 0;
    m_channelInfoMap.clear();
}

void* CUser::operator new(unsigned int size) { return ::operator new(size); }

unsigned int CUser::GetUniqCharNo() { return m_uniqCharNo; }

void CUser::AttachMember(CMember* member)
{
    m_member = member;
}

void CUser::operator delete(void* p) { ::operator delete(p); }

void* CUser::GetGameServer() { return m_gameServer; }

void* CUser::GetTcpGameServer() { return m_tcpGameServer; }

unsigned int CUser::GetDBID() { return m_dbid; }

short CUser::GetLevel() { return (short)m_level; }

char CUser::GetJob() { return m_job; }

char CUser::GetGrowthType() { return m_growthType; }

unsigned int CUser::GetIdByChannel() { return m_idByChannel; }

char* CUser::GetCharName() { return m_dataC2; }

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
    if (m_member != 0)
    {
        if (m_member->GetMemberKey() != 0 &&
            (GetMemberDBFlag() & 4) != 0)
        {
            return (unsigned char)m_member->GetUpperMemberExpLevel();
        }
    }
    return 0;
}

void CUser::SendTcpGameserver(PacketHeader* pkt)
{
    if (m_tcpGameServer != 0)
    {
        char* buf = m_tcpGameServer->makePacketHeader(
            pkt->packetId, pkt->packetSize);
        if (buf != 0)
        {
            memcpy(buf + 10, (char*)pkt + 10, (unsigned int)pkt->packetSize - 10U);
            m_tcpGameServer->SendToGameServer(buf);
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
    return (m_channelCount != 0 && (int)m_channelInfoMap.size() == m_channelCount) ? 1 : 0;
}

void CUser::GetChannelUserCount(STPvPChannelInfo* out, unsigned char& count)
{
    int idx = 0;
    std::map<int, ChannelInfo>::iterator it = m_channelInfoMap.begin();
    while (it != m_channelInfoMap.end() && (int)(unsigned char)count > idx)
    {
        out[idx].m_channel = it->first;
        ChannelInfo& info = it->second;
        out[idx].m_countA = info.m_a;
        out[idx].m_countB = info.m_b;
        out[idx].m_countC = info.m_c;
        ++it;
        ++idx;
    }
    count = (unsigned char)idx;
}

void CUser::MemberEnterProcess()
{
    if (m_memberEnterCallerId == 0)
    {
        return;
    }
    m_memberEnterCount--;
    register bool b = m_memberEnterCount <= 0;
    if (b)
    {
        m_memberEnterCallerId = 0;
        m_memberEnterCount = 0;
    }
    return;
}

void CUser::SetBuddyCharName(int dbid, const std::string& name)
{
    m_buddyHandle.setBuddyCharName(dbid, name);
}

int CUser::ChangeCharNameToBlackList(unsigned int dbid, char* name)
{
    if (m_blackList.empty())
    {
        return 0;
    }
    else
    {
        std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(dbid);
        if (it != m_blackList.end())
        {
            CBlackUser* user = it->second;
            user->ChangeCharName(name);
            return 1;
        }
        return 0;
    }
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
    return m_buddyHandle.addDB(handler, name);
}

int CUser::DelBuddyDB(CServerHandler* handler, char* name)
{
    return m_buddyHandle.delDB(handler, name);
}

int CUser::AddBuddy(STBuddyDBInfo& info)
{
    return m_buddyHandle.add(std::string((char*)&info), info);
}

char CUser::DelBuddy(char* name)
{
    return (char)m_buddyHandle.del(std::string(name));
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

void CUser::SetBlackListDBFlag(unsigned short flag) { m_blackListDBFlag |= flag; }

void CUser::SetDBID(unsigned int dbid)
{
    m_dbid = dbid;
}

void CUser::SetUniqCharNo(unsigned int charNo)
{
    m_uniqCharNo = charNo;
}

void CUser::SetIdByChannel(int channel)
{
    m_idByChannel = channel;
}

void CUser::SetGameServer(CGameServer* server) { m_gameServer = server; }

void CUser::PrintBuddys(char* out)
{
    m_buddyHandle.printBuddys(out);
}

void CUser::SetUserPosState(unsigned char state)
{
    m_posState = (char)state;
}

void CUser::SetUserChangableInfo(short level, char flag)
{
    m_level = level;
    m_growthType = flag;
}

void CUser::SetUserInfo_CharNo(char a, char b, short level, unsigned int charNo, char* name)
{
    m_job = a;
    m_growthType = b;
    m_level = level;
    m_uniqCharNo = charNo;
    memcpy(m_dataC2, name, 0x1d);
    m_buddyHandle.reset(this, true);
}

void CUser::SetSex(unsigned char sex)
{
    m_sex = sex;
}

void CUser::SetSsn(char* ssn)
{
    memcpy(m_dataC2b, ssn, 6);
}

void CUser::SetEvent_idx(unsigned int idx)
{
    m_eventIdx = idx;
}

void CUser::Event_idx_modify_state()
{
    m_data2[8] = 1;
}

void CUser::SetTcpGameServer(CTcpGameServer* server)
{
    m_tcpGameServer = server;
}

void CUser::QueryBuddyInfo(CServerHandler* handler)
{
    Packet_DBMW_Query_Buddy_Info pkt;
    pkt.m_charNo = GetUniqCharNo();
    handler->SendToDB(&pkt);
}

void CUser::ResetMemberInfo()
{
    m_member = 0;
    m_field18 = 0;
    m_memberEnterCount = 0;
    m_memberEnterCallerId = 0;
}

void CUser::ResetCharInfo(bool flag)
{
    m_uniqCharNo = 0;
    m_job = 0xff;
    m_growthType = 0xff;
    m_level = -1;
    memset(m_dataC2, 0, 0x1e);
    ResetMemberInfo();
    m_buddyHandle.reset(0, flag);
}

void CUser::ResetBlackList(int flag)
{
    if (m_blackList.empty())
    {
    }
    else
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
    return;
}

void CUser::ResetBuddyList(bool flag)
{
    m_buddyHandle.reset(0, flag);
}

int CUser::GetBuddysCharNo(unsigned int* out)
{
    return m_buddyHandle.getBuddysCharNo(out);
}

void CUser::GetBuddiesCharNo(std::vector<unsigned int>& out)
{
    unsigned int buf[32];
    memset(buf, 0, 0x80);
    int count = m_buddyHandle.getBuddysCharNo(buf);
    for (int i = 0; i < count; i++)
    {
        out.push_back(buf[i]);
    }
}

int CUser::GetBuddys(CBuddy** out)
{
    return m_buddyHandle.getBuddys(out);
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
        pkt.m_flag1 = flag1;
        pkt.m_flag2 = flag2;
        memcpy(pkt.m_name, name, 0x1d);
        pkt.m_flag3 = (unsigned char)flag3;
        ((CServerInterface*)GetGameServer())->SendToServer(
            (char*)&pkt, pkt.packetSize);
    }
}

unsigned short CUser::GetBuddyDBFlag()
{
    return m_buddyHandle.GetBuddyDBFlag();
}

unsigned short CUser::GetBlackListDBFlag()
{
    return m_blackListDBFlag;
}

std::map<unsigned int, CBlackUser*>* CUser::GetMapBlackList()
{
    return &m_blackList;
}

void CUser::GetBlackList(unsigned char& count, STBlackUserDBType* out)
{
    count = 0;
    if (m_blackList.empty())
    {
    }
    else
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end(); ++it)
        {
            memcpy(out[(unsigned int)count].m_name, (*it).second->GetName(), 0x1d);
            out[(unsigned int)count].m_occurTime = (*it).second->GetOccurTime();
            out[(unsigned int)count].m_dbid = (*it).first;
            count++;
            if (9 < count)
            {
                return;
            }
        }
    }
    return;
}

void CUser::GetBlackList(unsigned char& count, unsigned int* out)
{
    count = 0;
    if (m_blackList.empty())
    {
    }
    else
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end(); ++it)
        {
            out[(unsigned int)count] = (*it).first;
            count++;
            if (9 < count)
            {
                return;
            }
        }
    }
    return;
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
    return m_blackList.insert(std::make_pair(charNo, user)).second;
}

char CUser::RegisterToBlackList(unsigned int charNo, char* name, unsigned int time)
{
    CBlackUser* user = new CBlackUser;
    user->SetBlackUser(name, time);
    return m_blackList.insert(std::make_pair(charNo, user)).second;
}

int CUser::DeleteToBlackList(unsigned int charNo)
{
    if (m_blackList.empty())
    {
        return 0;
    }
    else
    {
        std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(charNo);
        if (it != m_blackList.end())
        {
            delete it->second;
            m_blackList.erase(it);
            return 1;
        }
        return 0;
    }
}

void CUser::SendBlackList()
{
    Packet_Monitor_Notice_Black_List pkt;
    pkt.m_dbid = m_dbid;
    pkt.m_idByChannel = (unsigned int)m_idByChannel;
    GetBlackList(pkt.m_count, pkt.m_charNos);
    SendTcpGameserver(&pkt);
}

unsigned int CUser::GetMemberEnterCallerId()
{
    return m_memberEnterCallerId;
}

char CUser::CheckPrevCallMemberEnter()
{
    if (m_memberEnterCount < 1)
    {
        return 0;
    }
    if (m_memberEnterCallerId == 0)
    {
        return 0;
    }
    return 1;
}

void CUser::ResetRequestMemberEnter()
{
    m_memberEnterCallerId = 0;
    m_memberEnterCount = 0;
}

char CUser::RecordCallMemberEnter(unsigned int callerId, unsigned short count)
{
    if (m_memberEnterCount != 0) return 0;
    m_memberEnterCallerId = callerId;
    m_memberEnterCount = (char)(unsigned int)count;
    return 1;
}

unsigned short CUser::GetMemberDBFlag()
{
    if (m_member != 0)
    {
        return m_member->GetMemberDBFlag();
    }
    return 0;
}

int CUser::GetConnLowerMemberCnt()
{
    if (m_member != 0)
    {
        if (m_member->GetMemberKey() != 0 &&
            (GetMemberDBFlag() & 4) != 0)
        {
            return m_member->GetConnLowerMemberCnt();
        }
    }
    return 0;
}

CMember* CUser::GetMember()
{
    if (m_member)
        return m_member;
    return 0;
}

void CUser::SetMemberRegisterFlag(bool flag)
{
    if (m_member != 0)
    {
        m_member->SetMemberRegisterFlag(flag);
    }
}

bool CUser::IsAbleToRegisterMember()
{
    if (m_member != 0)
    {
        return m_member->IsAbleToRegisterMember();
    }
    return 1;
}

unsigned int CUser::GetMemberKey()
{
    if (m_member != 0)
    {
        return m_member->GetMemberKey();
    }
    return 0;
}
