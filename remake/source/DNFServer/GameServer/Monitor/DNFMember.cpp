// df_monitor_r — DNFMember（从 MonitorTypes/App/Table 拆分）
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

#include "DNFMember.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFMemberManager.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFUser.h"

// ORIG 此处调用全局版本（_Z26CheckDailyScheduleTimeOveril /
// _Z28CheckDayHourScheduleTimeOveriil，定义于 Scheduler.cpp），
// 而非本类成员方法；声明见 VillageAttackedManager.h。
bool CheckDailyScheduleTimeOver(int day, long time);
bool CheckDayHourScheduleTimeOver(int day, int hour, long time);

void CMember::operator delete(void* p) { ::operator delete(p); }

void CMember::operator delete(void* p, unsigned int size) { ::operator delete(p); }

void* CMember::operator new(unsigned int size) { return ::operator new(size); }

CMember::CMember(unsigned int key, CMemberManager* mgr)
    : m_key(key), m_flag(0)
{
    m_memberManager = mgr;
    m_state1b8 = 1;
    m_registerTime = 0;
    m_dayHourTime = 0;
}

CMember::~CMember()
{
    m_key = 0;
    m_flag = 0;
    m_memberManager = 0;
    memset(&m_dbInfo, 0, 0x1ae);
    m_registerTime = 0;
    m_dayHourTime = 0;
    m_state1b8 = 0;
}

void CMember::QueryMember(CServerHandler* handler)
{
    handler->QueryMember(m_key);
    m_flag |= 2;
}

unsigned int* CMember::GetMemberDBInfoW() { return (unsigned int*)&m_dbInfo; }

void CMember::NoticeMemberLogin_Out(CUser* user, char flag)
{
    bool invalid = (user == 0 || user->GetGameServer() == 0);
    if (invalid == 0 && (m_flag & 4) != 0)
    {
        Packet_Monitor_Notice_Member_Member_Login_out pkt;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_charNo);
        if (member != 0)
        {
            if (user->IsBlackUser(member->GetUniqCharNo()) != 1)
            {
                pkt.m_flag = flag;
                pkt.m_idByChannel = member->GetIdByChannel();
                pkt.m_uniqCharNo = member->GetUniqCharNo();
                pkt.m_channelNo =
                    ((CServerInterface*)user->GetGameServer())->GetChannelNo();
                pkt.m_type = 2;
                memcpy(pkt.m_charName, user->GetCharName(), 0x1d);
                pkt.m_expLevel = 0;
                member->SendTcpGameserver(&pkt);
            }
            if (flag == 1)
            {
                if (user->IsBlackUser(member->GetUniqCharNo()) != 1)
                {
                    pkt.m_flag = flag;
                    pkt.m_idByChannel = user->GetIdByChannel();
                    pkt.m_uniqCharNo = user->GetUniqCharNo();
                    if (member->GetGameServer() != 0)
                    {
                        pkt.m_channelNo =
                            ((CServerInterface*)member->GetGameServer())->GetChannelNo();
                    }
                    else
                    {
                        pkt.m_channelNo = 0xff;
                    }
                    pkt.m_type = 1;
                    memcpy(pkt.m_charName, member->GetCharName(), 0x1d);
                    pkt.m_expLevel = user->GetUpperMemberExpLevel();
                    pkt.m_uniqCharNo2 = member->GetUniqCharNo();
                    user->SendTcpGameserver(&pkt);
                }
            }
        }
        int count = (int)m_dbInfo.m_count27;
        if (count != 0)
        {
            for (int i = 0; i < count; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_charNo);
                if (m == 0)
                {
                    continue;
                }
                if (user->IsBlackUser(m->GetUniqCharNo()) != 1)
                {
                    pkt.m_flag = flag;
                    pkt.m_idByChannel = m->GetIdByChannel();
                    pkt.m_uniqCharNo = m->GetUniqCharNo();
                    pkt.m_channelNo =
                        ((CServerInterface*)user->GetGameServer())->GetChannelNo();
                    pkt.m_type = 1;
                    memcpy(pkt.m_charName, user->GetCharName(), 0x1d);
                    pkt.m_expLevel = m->GetUpperMemberExpLevel();
                    pkt.m_uniqCharNo2 = user->GetUniqCharNo();
                    m->SendTcpGameserver(&pkt);
                }
            }
        }
    }
}

char CMember::CheckDailyScheduleTimeOver(int day, long time)
{
    return (char)CheckDailyScheduleTimeOver(day, time);
}

char CMember::CheckDayHourScheduleTimeOver(int day, int hour, long time)
{
    return (char)CheckDayHourScheduleTimeOver(day, hour, time);
}

void CMember::SetMemberRegisterFlag(bool flag)
{
    m_state1b8 = flag;
}

bool CMember::IsAbleToRegisterMember()
{
    return m_state1b8;
}

unsigned int CMember::GetMemberKey()
{
    return m_key;
}

void CMember::CheckMemberRegisterFlag()
{
    SetMemberRegisterFlag(::CheckDailyScheduleTimeOver(6, m_registerTime));
    if (IsAbleToRegisterMember())
    {
        SetMemberRegisterFlag(::CheckDayHourScheduleTimeOver(3, 6, m_dayHourTime));
    }
    if (!IsAbleToRegisterMember())
    {
        tm t1 = *localtime((time_t*)&m_registerTime);
        tm t2 = *localtime((time_t*)&m_dayHourTime);
        DNF_LOG_SCOPE_LINE(0x336,"./log/MemberModify",
            "MKey(%d)\tRF(0)\tRT(%04d.%02d.%02d %02d:%02d:%02d)\tDT(%04d.%02d.%02d %02d:%02d:%02d)",
            GetMemberKey(), t1.tm_year + 0x76c, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour,
            t1.tm_min, t1.tm_sec, t2.tm_year + 0x76c, t2.tm_mon + 1, t2.tm_mday,
            t2.tm_hour, t2.tm_min, t2.tm_sec);
    }
}

char CMember::IsEmpty()
{
    if (!IsThereUpper() && m_dbInfo.m_count27 == 0)
    {
        return 1;
    }
    return 0;
}

void CMember::NoticeChatMsgToMemberMembersHyperLink(char* msg, int len, unsigned char count,
                                                    const hyperlink_item_info* items, CUser* user)
{
    if (len < 0x100 && (m_flag & 4) != 0 && IsEmpty() == 0)
    {
        Packet_Monitor_Member_Chat_ToUser_Hyper_Link pkt;
        memcpy(pkt.m_charName, user->GetCharName(), 0x1d);
        pkt.m_msgLen = (unsigned char)len;
        memcpy(pkt.m_msg, msg, len);
        pkt.m_itemCount = count;
        for (int i = 0; i < (int)count; i++)
        {
            memcpy(pkt.m_items + i * 0x68, (char*)items + i * 0x68, 0x68);
        }
        pkt.packetSize = (unsigned short)len + 0x16a;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_charNo);
        if (member != 0)
        {
            pkt.m_idByChannel = member->GetIdByChannel();
            pkt.m_uniqCharNo = member->GetUniqCharNo();
            member->SendToGameserver((char*)&pkt, pkt.packetSize);
        }
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_uniqCharNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, pkt.packetSize);
        int count2 = (int)m_dbInfo.m_count27;
        if (count2 > 0)
        {
            for (int i = 0; i < count2; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_charNo);
                if (m != 0)
                {
                    pkt.m_idByChannel = m->GetIdByChannel();
                    pkt.m_uniqCharNo = m->GetUniqCharNo();
                    m->SendToGameserver((char*)&pkt, pkt.packetSize);
                }
            }
        }
    }
}

void CMember::NoticeChatMsgToMemberMembers(char* msg, int len, CUser* user)
{
    if (len < 0x100 && (m_flag & 4) != 0 && IsEmpty() == 0)
    {
        Packet_Monitor_Member_Chat_ToUser pkt;
        memcpy(pkt.m_charName, user->GetCharName(), 0x1d);
        pkt.m_msgLen = (unsigned char)len;
        memcpy(pkt.m_msg, msg, len);
        pkt.packetSize = (unsigned short)len + 0x31;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_charNo);
        if (member != 0)
        {
            pkt.m_idByChannel = member->GetIdByChannel();
            pkt.m_uniqCharNo = member->GetUniqCharNo();
            member->SendToGameserver((char*)&pkt, pkt.packetSize);
        }
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_uniqCharNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, pkt.packetSize);
        int count = (int)m_dbInfo.m_count27;
        if (count > 0)
        {
            for (int i = 0; i < count; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_charNo);
                if (m != 0)
                {
                    pkt.m_idByChannel = m->GetIdByChannel();
                    pkt.m_uniqCharNo = m->GetUniqCharNo();
                    m->SendToGameserver((char*)&pkt, pkt.packetSize);
                }
            }
        }
    }
}

void CMember::LoadMember(STMemberDBInfo& info, short level, unsigned int a, unsigned int b)
{
    if ((m_flag & 2) != 0)
    {
        unsigned int limit = m_memberManager->GetLowerMemberEnterLimit(level);
        if ((int)(unsigned int)info.m_count27 > (int)limit)
        {
            info.m_count27 = (unsigned char)limit;
        }
        unsigned int size = (unsigned int)info.m_count27 * 0x27 + 0x28;
        memcpy((char*)this + 6, &info, size);
        m_flag |= 4;
        SetMemberRegisterTime(a);
        SetMemberDeleteTime(b);
        CheckMemberRegisterFlag();
    }
}

int CMember::GetUpperMember_CharId() const
{
    if ((m_flag & 4) != 0)
    {
        if (IsThereUpper() != 0)
        {
            return (int)m_dbInfo.m_member.m_charNo;
        }
        return 0xffffffff;
    }
    return 0;
}

int CMember::FindLowerMember(unsigned int charNo) const
{
    unsigned int count = (unsigned int)m_dbInfo.m_count27;
    if (count == 0)
    {
        return 0;
    }
    const ST_MemberProxy* p = m_dbInfo.m_lowers;
    while (count-- != 0)
    {
        if (p->m_charNo == charNo)
        {
            return (int)p;
        }
        p++;
    }
    return 0;
}

unsigned int CMember::GetLowerMemberCount() const
{
    return m_dbInfo.m_count27;
}

unsigned int* CMember::GetLowerMember_Proxy() const
{
    return (unsigned int*)&m_dbInfo.m_lowers[0];
}

int CMember::IncConnUpperMemberExp(unsigned int maxExp)
{
    m_dbInfo.m_member.m_exp = m_dbInfo.m_member.m_exp + 1;
    if (maxExp < m_dbInfo.m_member.m_exp)
    {
        m_dbInfo.m_member.m_exp = m_dbInfo.m_member.m_exp - 1;
        return 0;
    }
    return m_dbInfo.m_member.m_exp;
}

void CMember::IncConnLowerMemberExp(unsigned int uCharNo, unsigned int maxExp)
{
    for (int i = 0; i <= 9; i++)
    {
        ST_MemberProxy* proxy = &m_dbInfo.m_lowers[i];
        if (proxy->m_charNo == uCharNo)
        {
            proxy->m_exp = proxy->m_exp + 1;
            if (maxExp < proxy->m_exp)
            {
                proxy->m_exp = proxy->m_exp - 1;
            }
            return;
        }
    }
}

int CMember::IncConnLowerMemberExp(int index, unsigned int uCharNo, unsigned int maxExp)
{
    if ((int)(unsigned int)m_dbInfo.m_count27 <= index)
    {
        DNF_LOG_SCOPE_LINE(0x284,"./log/Member2Except",
            "CMember::IncConnLowerMemberExp  ,  index(%d) >= "
            "m_stMemberDBInfo.m_lowerCnt(%d)",
            index, (unsigned int)m_dbInfo.m_count27);
        return 0;
    }
    ST_MemberProxy* proxy = &m_dbInfo.m_lowers[index];
    if (proxy->m_charNo != uCharNo)
    {
        DNF_LOG_SCOPE_LINE(0x28c,"./log/Member2Except",
            "CMember::IncConnLowerMemberExp  ,  stMemberLowerProxy.m_uCharId(%d) != "
            "uCharNo(%d)",
            proxy->m_charNo, uCharNo);
        return 0;
    }
    proxy->m_exp = proxy->m_exp + 1;
    if (maxExp < proxy->m_exp)
    {
        proxy->m_exp = proxy->m_exp - 1;
        return 0;
    }
    return proxy->m_exp;
}

void CMember::NoticeLevelUpToLowers(unsigned int level)
{
    int count = (int)m_dbInfo.m_count27;
    if (count != 0)
    {
        Packet_Monitor_Notice_MemberExp_LevelUp pkt;
        for (int i = 0; i < count; i++)
        {
            CUser* user = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_charNo);
            if (user != 0)
            {
                pkt.m_idByChannel = user->GetIdByChannel();
                pkt.m_uniqCharNo = user->GetUniqCharNo();
                pkt.m_level = level;
                user->SendToGameserver((char*)&pkt, 0x13);
            }
        }
    }
}

unsigned int* CMember::GetUpperMember_Proxy()
{
    return (unsigned int*)&m_dbInfo.m_member;
}

unsigned int* CMember::GetUpperMember_Proxy() const
{
    if ((m_flag & 4) != 0)
    {
        if (IsThereUpper() != 0)
        {
            return (unsigned int*)&m_dbInfo.m_member;
        }
        return 0;
    }
    return 0;
}

void CMember::SetMemberDeleteTime(unsigned int t) { m_dayHourTime = t; }

void CMember::DebugPrintMemberMember(char* out) {}

void CMember::SetMemberRegisterTime(unsigned int t) { m_registerTime = t; }

void CMember::SetMemberDBFlag(unsigned short flag) { m_flag = (unsigned short)(m_flag | flag); }

unsigned short CMember::GetMemberDBFlag() { return m_flag; }

unsigned int CMember::GetUpperMemberExpLevel()
{
    return m_memberManager->GetMemberExpLevel(m_dbInfo.m_member.m_exp);
}

int CMember::GetConnLowerMemberCnt()
{
    CUser* user;
    int count = (int)m_dbInfo.m_count27;
    if (count == 0)
    {
        return 0;
    }
    int cnt = 0;
    for (int i = 0; i < count; i++)
    {
        user = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_charNo);
        if (user == 0)
        {
            continue;
        }
        cnt++;
    }
    return cnt;
}

int CMember::InsertUpperMember(unsigned int charNo, unsigned char level, const char* name,
                               bool flag)
{
    if (IsThereUpper() != 0)
    {
        return 0;
    }
    m_dbInfo.m_member.m_flag4 = level;
    m_dbInfo.m_member.m_charNo = charNo;
    memcpy(m_dbInfo.m_member.m_name, name, 0x1d);
    if (flag)
    {
        SetMemberRegisterTime((unsigned int)time(0));
    }
    return 1;
}

int CMember::InsertLowerMember(unsigned int charNo, unsigned char level, const char* name,
                               bool flag)
{
    int n = (int)m_dbInfo.m_count27;
    if (n + 1 > 0xa)
    {
        return 0;
    }
    ST_MemberProxy* p = &m_dbInfo.m_lowers[n];
    p->m_flag4 = level;
    p->m_charNo = charNo;
    memcpy(p->m_name, name, 0x1d);
    if (flag)
    {
        SetMemberRegisterTime((unsigned int)time(0));
    }
    m_dbInfo.m_count27++;
    return 1;
}

char CMember::IsAlreadyMemberMember(unsigned int charNo) const
{
    if ((unsigned int)GetUpperMember_CharId() == charNo)
    {
        return 1;
    }
    if (FindLowerMember(charNo) != 0)
    {
        return 1;
    }
    return 0;
}

void CMember::DeleteUpperMember(unsigned int charNo, bool flag)
{
    m_dbInfo.m_member.Reset();
    if (flag)
    {
        SetMemberDeleteTime(time(0));
    }
    DebugPrintMemberMember("DELETE_UPPER_MEMBER");
}

void CMember::DeleteLowerMember(unsigned int charNo, bool flag)
{
    int count = (int)m_dbInfo.m_count27;
    if (count <= 0)
    {
        return;
    }
    ST_MemberProxy* p = m_dbInfo.m_lowers;
    unsigned char idx = 0;
    while (count-- != 0)
    {
        if (p->m_charNo == charNo)
        {
            memcpy(p, p + 1, (int)(~(unsigned char)idx) * 0x27 + 0x186);
            m_dbInfo.m_count27--;
            if (flag)
            {
                SetMemberDeleteTime(time(0));
            }
            break;
        }
        p++;
        idx++;
    }
    DebugPrintMemberMember("DELETE_LOWER_MEMBER");
}

unsigned char* CMember::GetMemberDBInfo() const
{
    return (unsigned char*)&m_dbInfo;
}

int CMember::DeleteMemberByName(char* name, unsigned int& outKey)
{
    unsigned int* proxy = ((const CMember*)this)->GetUpperMember_Proxy();
    if (proxy != 0 && strcmp((char*)proxy + 5, name) == 0)
    {
        outKey = *proxy;
        m_dbInfo.m_member.Reset();
        SetMemberDeleteTime(time(0));
        return 1;
    }
    ST_MemberProxy* p = m_dbInfo.m_lowers;
    unsigned char idx = 0;
    int count = (int)m_dbInfo.m_count27;
    if (count <= 0)
    {
        return 0;
    }
    while (count-- != 0)
    {
        if (strcmp(p->m_name, name) == 0)
        {
            outKey = p->m_charNo;
            memcpy(p, p + 1, (int)(~(unsigned char)idx) * 0x27 + 0x186);
            m_dbInfo.m_count27--;
            SetMemberDeleteTime(time(0));
            return 2;
        }
        p++;
        idx++;
    }
    return 3;
}

STMemberDBInfo::STMemberDBInfo() : m_count27(0) {}

STMemberListInfo::STMemberListInfo() : m_count(0) {}

ST_MemberInfo::ST_MemberInfo()
{
    m_channelNo = 0;
    m_flag = 0;
    m_blackFlag = 0;
    m_level = 0;
    m_exp = 0;
    m_expNext = 0;
    memset(m_name, 0, 0x1e);
}

void ST_MemberProxy::Reset()
{
    m_charNo = 0;
    m_flag4 = 0;
    memset(m_name, 0, 0x1e);
    m_exp = 0;
}
