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
    memset((char*)this + 6, 0, 0x1ae);
    m_registerTime = 0;
    m_dayHourTime = 0;
    m_state1b8 = 0;
}

void CMember::QueryMember(CServerHandler* handler)
{
    handler->QueryMember(m_key);
    m_flag |= 2;
}

unsigned int* CMember::GetMemberDBInfoW() { return (unsigned int*)((char*)this + 6); }

void CMember::NoticeMemberLogin_Out(CUser* user, char flag)
{
    bool invalid = (user == 0 || user->GetGameServer() == 0);
    if (!invalid && (m_flag & 4) != 0)
    {
        Packet_Monitor_Notice_Member_Member_Login_out pkt;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
        if (member != 0)
        {
            member->GetUniqCharNo();
            if (user->IsBlackUser(0) != 1)
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
                member->GetUniqCharNo();
                if (user->IsBlackUser(0) != 1)
                {
                    pkt.m_flag = flag;
                    pkt.m_idByChannel = user->GetIdByChannel();
                    pkt.m_uniqCharNo = user->GetUniqCharNo();
                    pkt.m_channelNo =
                        (member->GetGameServer() != 0)
                            ? ((CServerInterface*)member->GetGameServer())->GetChannelNo()
                            : 0xff;
                    pkt.m_type = 1;
                    memcpy(pkt.m_charName, member->GetCharName(), 0x1d);
                    pkt.m_expLevel = user->GetUpperMemberExpLevel();
                    pkt.m_uniqCharNo2 = member->GetUniqCharNo();
                    user->SendTcpGameserver(&pkt);
                }
            }
        }
        unsigned int count = (unsigned int)m_dbInfo.m_count27;
        if (count != 0)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
                if (m != 0)
                {
                    m->GetUniqCharNo();
                    if (user->IsBlackUser(0) != 1)
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
    bool flag = ::CheckDailyScheduleTimeOver(6, m_registerTime);
    SetMemberRegisterFlag(flag);
    if (IsAbleToRegisterMember())
    {
        flag = ::CheckDayHourScheduleTimeOver(3, 6, m_dayHourTime);
        SetMemberRegisterFlag(flag);
    }
    if (!IsAbleToRegisterMember())
    {
        tm* t1 = localtime((time_t*)&m_registerTime);
        int sec1 = t1->tm_sec, min1 = t1->tm_min, hour1 = t1->tm_hour;
        int mday1 = t1->tm_mday, mon1 = t1->tm_mon, year1 = t1->tm_year;
        tm* t2 = localtime((time_t*)&m_dayHourTime);
        int sec2 = t2->tm_sec, min2 = t2->tm_min, hour2 = t2->tm_hour;
        int mday2 = t2->tm_mday, mon2 = t2->tm_mon, year2 = t2->tm_year;
        DNF_LOG_SCOPE_LINE(0x336,"./log/MemberModify",
            "MKey(%d)\tRF(0)\tRT(%04d.%02d.%02d %02d:%02d:%02d)\tDT(%04d.%02d.%02d %02d:%02d:%02d)",
            GetMemberKey(), year1 + 0x76c, mon1 + 1, mday1, hour1, min1, sec1, year2 + 0x76c,
            mon2 + 1, mday2, hour2, min2, sec2);
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
    if (len < 0x100 && (m_flag & 4) != 0 && !IsEmpty())
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
        unsigned short totalSize = (unsigned short)len + 0x16a;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
        if (member != 0)
        {
            pkt.m_idByChannel = member->GetIdByChannel();
            pkt.m_uniqCharNo = member->GetUniqCharNo();
            member->SendToGameserver((char*)&pkt, totalSize);
        }
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_uniqCharNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, totalSize);
        unsigned int count2 = (unsigned int)m_dbInfo.m_count27;
        if (count2 != 0)
        {
            for (unsigned int i = 0; i < count2; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
                if (m != 0)
                {
                    pkt.m_idByChannel = m->GetIdByChannel();
                    pkt.m_uniqCharNo = m->GetUniqCharNo();
                    m->SendToGameserver((char*)&pkt, totalSize);
                }
            }
        }
    }
}

void CMember::NoticeChatMsgToMemberMembers(char* msg, int len, CUser* user)
{
    if (len < 0x100 && (m_flag & 4) != 0 && !IsEmpty())
    {
        Packet_Monitor_Member_Chat_ToUser pkt;
        memcpy(pkt.m_charName, user->GetCharName(), 0x1d);
        pkt.m_msgLen = (unsigned char)len;
        memcpy(pkt.m_msg, msg, len);
        unsigned short totalSize = (unsigned short)len + 0x31;
        CUser* member = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
        if (member != 0)
        {
            pkt.m_idByChannel = member->GetIdByChannel();
            pkt.m_uniqCharNo = member->GetUniqCharNo();
            member->SendToGameserver((char*)&pkt, totalSize);
        }
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_uniqCharNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, totalSize);
        unsigned int count = (unsigned int)m_dbInfo.m_count27;
        if (count != 0)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                CUser* m = m_memberManager->FindMemberUser(m_dbInfo.m_member.m_field0);
                if (m != 0)
                {
                    pkt.m_idByChannel = m->GetIdByChannel();
                    pkt.m_uniqCharNo = m->GetUniqCharNo();
                    m->SendToGameserver((char*)&pkt, totalSize);
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
        if ((unsigned int)info.m_count27 > limit)
        {
            info.m_count27 = (unsigned char)limit;
        }
        memcpy((char*)this + 6, &info, (unsigned int)info.m_count27 * 0x27 + 0x28);
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
            return ((RA_INT<6>*)this)->v;
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
    const char* p = (const char*)this + 0x2e;
    while (count-- != 0)
    {
        if (*(unsigned int*)p == charNo)
        {
            return (int)p;
        }
        p += 0x27;
    }
    return 0;
}

unsigned int CMember::GetLowerMemberCount() const
{
    return m_dbInfo.m_count27;
}

unsigned int* CMember::GetLowerMember_Proxy() const
{
    return (unsigned int*)((char*)this + 0x2e);
}

int CMember::IncConnUpperMemberExp(unsigned int maxExp)
{
    m_dbInfo.m_member.m_field23 = m_dbInfo.m_member.m_field23 + 1;
    if (maxExp < m_dbInfo.m_member.m_field23)
    {
        m_dbInfo.m_member.m_field23 = m_dbInfo.m_member.m_field23 - 1;
        return 0;
    }
    return m_dbInfo.m_member.m_field23;
}

int CMember::IncConnLowerMemberExp(unsigned int uCharNo, unsigned int maxExp)
{
    for (int i = 0; i <= 9; i++)
    {
        unsigned int* proxy = (unsigned int*)((char*)this + i * 0x27 + 0x2e);
        if (*proxy == uCharNo)
        {
            *(int*)((char*)this + i * 0x27 + 0x51) =
                *(int*)((char*)this + i * 0x27 + 0x51) + 1;
            if (maxExp < *(unsigned int*)((char*)this + i * 0x27 + 0x51))
            {
                *(int*)((char*)this + i * 0x27 + 0x51) =
                    *(int*)((char*)this + i * 0x27 + 0x51) - 1;
            }
            return 0;
        }
    }
    return 0;
}

int CMember::IncConnLowerMemberExp(int index, unsigned int uCharNo, unsigned int maxExp)
{
    if (index < (int)(unsigned int)(unsigned char)((RA_S8<45>*)this)->v)
    {
        unsigned int* proxy = (unsigned int*)((char*)this + index * 0x27 + 0x2e);
        if (*proxy == uCharNo)
        {
            *(int*)((char*)this + index * 0x27 + 0x51) =
                *(int*)((char*)this + index * 0x27 + 0x51) + 1;
            if (maxExp < *(unsigned int*)((char*)this + index * 0x27 + 0x51))
            {
                *(int*)((char*)this + index * 0x27 + 0x51) =
                    *(int*)((char*)this + index * 0x27 + 0x51) - 1;
                return 0;
            }
            return *(int*)((char*)this + index * 0x27 + 0x51);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x28c,"./log/Member2Except",
                "CMember::IncConnLowerMemberExp  ,  stMemberLowerProxy.m_uCharId(%d) != "
                "uCharNo(%d)",
                *proxy, uCharNo);
            return 0;
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x284,"./log/Member2Except",
            "CMember::IncConnLowerMemberExp  ,  index(%d) >= "
            "m_stMemberDBInfo.m_lowerCnt(%d)",
            index, (unsigned int)(unsigned char)((RA_S8<45>*)this)->v);
        return 0;
    }
}

void CMember::NoticeLevelUpToLowers(unsigned int level)
{
    unsigned char lowerCnt = ((RA_U8<45>*)this)->v;
    if (lowerCnt != 0)
    {
        Packet_Monitor_Notice_MemberExp_LevelUp pkt;
        for (int i = 0; i < lowerCnt; i++)
        {
            unsigned int charId = *(unsigned int*)((char*)this + i * 0x27 + 0x2e);
            CUser* user = m_memberManager->FindMemberUser(charId);
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
    return (unsigned int*)((char*)this + 6);
}

unsigned int* CMember::GetUpperMember_Proxy() const
{
    if ((((RA_U16<4>*)this)->v & 4) != 0)
    {
        if (IsThereUpper() != 0)
        {
            return (unsigned int*)((char*)this + 6);
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
    return m_memberManager->GetMemberExpLevel(m_dbInfo.m_member.m_field23);
}

int CMember::GetConnLowerMemberCnt()
{
    int count = (int)m_dbInfo.m_count27;
    if (count != 0)
    {
        int cnt = 0;
        int i = 0;
        for (i = 0; i < count; i++)
        {
            CUser* user = m_memberManager->FindMemberUser(m_dbInfo.m_lowers[i].m_field0);
            if (user == 0)
            {
                ;
            }
            else
            {
                cnt++;
            }
        }
        return cnt;
    }
    return 0;
}

int CMember::InsertUpperMember(unsigned int charNo, unsigned char level, const char* name,
                               bool flag)
{
    if (IsThereUpper() != 0)
    {
        return 0;
    }
    ((RA_U8<10>*)this)->v = level;
    ((RA_UINT<6>*)this)->v = charNo;
    memcpy((char*)this + 0xb, name, 0x1d);
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
    p->m_field0 = charNo;
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
    if (count > 0)
    {
        char* p = (char*)this + 0x2e;
        unsigned char idx = 0;
        do
        {
            if (*(unsigned int*)p == charNo)
            {
                memcpy(p, p + 0x27, (unsigned int)(~(unsigned char)idx) * 0x27 + 0x186);
                m_dbInfo.m_count27--;
                if (flag)
                {
                    SetMemberDeleteTime(time(0));
                }
                break;
            }
            p += 0x27;
            idx++;
        } while (count-- != 0);
        DebugPrintMemberMember("DELETE_LOWER_MEMBER");
    }
}

unsigned char* CMember::GetMemberDBInfo() const
{
    return (unsigned char*)((char*)this + 6);
}

int CMember::DeleteMemberByName(char* name, unsigned int& outKey)
{
    unsigned int* proxy = GetUpperMember_Proxy();
    if (proxy != 0 && strcmp((char*)proxy + 5, name) == 0)
    {
        outKey = *proxy;
        memset((char*)this + 6, 0, 0x27);
        SetMemberDeleteTime(time(0));
        return 1;
    }
    char* p = (char*)this + 0x2e;
    int idx = 0;
    unsigned int count = (unsigned int)m_dbInfo.m_count27;
    if (count == 0)
    {
        return 0;
    }
    while (count != 0)
    {
        count--;
        if (strcmp(p + 5, name) == 0)
        {
            outKey = *(unsigned int*)p;
            memcpy(p, p + 0x27, (unsigned int)(~(unsigned char)idx) * 0x27 + 0x186);
            m_dbInfo.m_count27--;
            SetMemberDeleteTime(time(0));
            return 2;
        }
        p += 0x27;
        idx++;
    }
    return 3;
}

STMemberDBInfo::STMemberDBInfo() : m_count27(0) {}

STMemberListInfo::STMemberListInfo() : m_count(0) {}

ST_MemberInfo::ST_MemberInfo()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field20 = 0;
    m_field21 = 0;
    m_field22 = 0;
    m_field26 = 0;
    memset(m_name, 0, 0x1e);
}

void ST_MemberProxy::Reset()
{
    m_field0 = 0;
    m_flag4 = 0;
    memset(m_name, 0, 0x1e);
    m_field23 = 0;
}
