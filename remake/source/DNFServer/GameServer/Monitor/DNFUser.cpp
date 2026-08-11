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
{
    ((RA_UINT<0>*)this)->v = 0;
    ((RA_UINT<4>*)this)->v = 0;
    ((RA_UINT<8>*)this)->v = 0;
    ((RA_UINT<12>*)this)->v = 0;
    ((RA_S8<16>*)this)->v = 0;
    ((RA_UINT<20>*)this)->v = 0;
    ((RA_U16<24>*)this)->v = 0;
    ((RA_S8<26>*)this)->v = 0;
    ((RA_UINT<28>*)this)->v = 0;
    ((RA_UINT<32>*)this)->v = 0xffffffff;
    ((RA_S8<66>*)this)->v = 0xff;
    ((RA_S8<67>*)this)->v = 0xff;
    ((RA_U16<68>*)this)->v = 0xffff;
    ((RA_S8<70>*)this)->v = 1;
    m_field68 = 0;
    ((RA_S8<176>*)this)->v = 0;
    ((RA_UINT<180>*)this)->v = 0;
    memset((char*)this + 0x24, 0, 0x1e);
    memset((char*)this + 0x47, 0, 7);
    m_channelCount = 0;
    m_channelInfoMap.clear();
}

CUser::~CUser()
{
    ((RA_UINT<8>*)this)->v = 0;
    ((RA_UINT<12>*)this)->v = 0;
    ((RA_UINT<0>*)this)->v = 0;
    ((RA_UINT<4>*)this)->v = 0;
    ((RA_UINT<32>*)this)->v = 0xffffffff;
    ((RA_S8<66>*)this)->v = 0xff;
    ((RA_S8<67>*)this)->v = 0xff;
    ((RA_U16<68>*)this)->v = 0xffff;
    memset((char*)this + 0x24, 0, 0x1e);
    ResetMemberInfo();
    ((RA_S8<16>*)this)->v = 0;
    m_field68 = 0;
    ((RA_S8<70>*)this)->v = 1;
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

unsigned int CUser::GetDBID() { return ((RA_UINT<0>*)this)->v; }

short CUser::GetLevel() { return (short)m_level; }

char CUser::GetJob() { return m_job; }

char CUser::GetGrowthType() { return m_growthType; }

unsigned int CUser::GetIdByChannel() { return m_idByChannel; }

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
    if (((RA_INT<20>*)this)->v != 0)
    {
        CMember* member = (CMember*)((RA_INT<20>*)this)->v;
        if (member->GetMemberKey() != 0 && (GetMemberDBFlag() & 4) != 0)
        {
            return (unsigned char)member->GetUpperMemberExpLevel();
        }
    }
    return 0;
}

void CUser::SendTcpGameserver(PacketHeader* pkt)
{
    if (((RA_INT<12>*)this)->v != 0)
    {
        CTcpGameServer* tcp = (CTcpGameServer*)((RA_INT<12>*)this)->v;
        char* buf = tcp->makePacketHeader(*(unsigned short*)pkt,
                                          ((RA_U16<2>*)pkt)->v);
        if (buf != 0)
        {
            memcpy(buf + 10, (char*)pkt + 10, ((RA_U16<2>*)pkt)->v - 10);
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
    return (m_channelCount != 0 && (int)m_channelInfoMap.size() == m_channelCount) ? 1 : 0;
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
    if (((RA_INT<28>*)this)->v != 0)
    {
        ((RA_S8<26>*)this)->v = ((RA_S8<26>*)this)->v - 1;
        if ((signed char)((RA_S8<26>*)this)->v <= 0)
        {
            ((RA_INT<28>*)this)->v = 0;
            ((RA_S8<26>*)this)->v = 0;
        }
    }
    return;
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
    ((RA_S16<68>*)this)->v = level;
    ((RA_S8<67>*)this)->v = flag;
}

void CUser::SetUserInfo_CharNo(char a, char b, short level, unsigned int charNo, char* name)
{
    ((RA_S8<66>*)this)->v = a;
    ((RA_S8<67>*)this)->v = b;
    ((RA_S16<68>*)this)->v = level;
    ((RA_UINT<4>*)this)->v = charNo;
    memcpy((char*)this + 0x24, name, 0x1d);
    ((CBuddyHandle*)((char*)this + 0x6c))->reset(this, true);
}

void CUser::SetSex(unsigned char sex)
{
    m_sex = sex;
}

void CUser::SetSsn(char* ssn)
{
    memcpy((char*)this + 0x47, ssn, 6);
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
    ((RA_UINT<10>*)&pkt)->v = GetUniqCharNo();
    handler->SendToDB(&pkt);
}

void CUser::ResetMemberInfo()
{
    ((RA_UINT<20>*)this)->v = 0;
    ((RA_U16<24>*)this)->v = 0;
    ((RA_S8<26>*)this)->v = 0;
    ((RA_UINT<28>*)this)->v = 0;
}

void CUser::ResetCharInfo(bool flag)
{
    ((RA_UINT<4>*)this)->v = 0;
    ((RA_S8<66>*)this)->v = 0xff;
    ((RA_S8<67>*)this)->v = 0xff;
    ((RA_U16<68>*)this)->v = 0xffff;
    memset((char*)this + 0x24, 0, 0x1e);
    ResetMemberInfo();
    ((CBuddyHandle*)((char*)this + 0x6c))->reset(0, flag);
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
            (char*)&pkt, ((RA_U16<2>*)&pkt)->v);
    }
}

unsigned short CUser::GetBuddyDBFlag()
{
    return m_buddyHandle.GetBuddyDBFlag();
}

unsigned short CUser::GetBlackListDBFlag()
{
    return m_field68;
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
            out[(unsigned int)count].m_field24 = (*it).second->GetOccurTime();
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
    pkt.m_dbid = ((RA_UINT<0>*)this)->v;
    pkt.m_idByChannel = ((RA_UINT<32>*)this)->v;
    unsigned char count = 0;
    GetBlackList(count, pkt.m_charNos);
    pkt.m_count = count;
    SendTcpGameserver(&pkt);
}

unsigned int CUser::GetMemberEnterCallerId()
{
    return m_memberEnterCallerId;
}

char CUser::CheckPrevCallMemberEnter()
{
    if (((RA_S8<26>*)this)->v < 1)
    {
        return 0;
    }
    if (((RA_INT<28>*)this)->v == 0)
    {
        return 0;
    }
    return 1;
}

void CUser::ResetRequestMemberEnter()
{
    m_memberEnterCallerId = 0;
    m_field1a = 0;
}

char CUser::RecordCallMemberEnter(unsigned int callerId, unsigned short count)
{
    char old = ((RA_S8<26>*)this)->v;
    if (old == 0)
    {
        ((RA_UINT<28>*)this)->v = callerId;
        ((RA_S8<26>*)this)->v = (char)count;
    }
    return old == 0;
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
    if (((RA_INT<20>*)this)->v == 0)
    {
        return 0;
    }
    CMember* member = (CMember*)((RA_INT<20>*)this)->v;
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
    if (((RA_INT<20>*)this)->v != 0)
    {
        ((CMember*)((RA_INT<20>*)this)->v)->SetMemberRegisterFlag(flag);
    }
}

bool CUser::IsAbleToRegisterMember()
{
    if (((RA_INT<20>*)this)->v != 0)
    {
        return ((CMember*)((RA_INT<20>*)this)->v)->IsAbleToRegisterMember();
    }
    return 1;
}

unsigned int CUser::GetMemberKey()
{
    if (((RA_INT<20>*)this)->v != 0)
    {
        return ((CMember*)((RA_INT<20>*)this)->v)->GetMemberKey();
    }
    return 0;
}
