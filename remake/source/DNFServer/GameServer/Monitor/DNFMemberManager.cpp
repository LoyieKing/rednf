// df_monitor_r — DNFMemberManager（从 MonitorTypes/App/Table 拆分）
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

#include "DNFMemberManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFMember.h"
#include "DNFMemberConfig.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFUser.h"
#include "DNFUserManager.h"

CMemberManager::CMemberManager()
    : m_app(0), m_userMgr(0), m_memberConfig(0), m_memberExpTbl(0)
{
    m_scheduler.SetSpecialHour(6);
}

CMemberManager::~CMemberManager()
{
    m_app = 0;
    m_userMgr = 0;
    m_memberConfig = 0;
    m_memberExpTbl = 0;
    m_members.clear();
}

void CMemberManager::Init(CApplication* app, CUserManager* userMgr, CMemberConfig* memberConfig,
                          CMemberExpTbl* memberExpTbl)
{
    m_app = app;
    m_userMgr = userMgr;
    m_memberConfig = memberConfig;
    m_memberExpTbl = memberExpTbl;
}

void CMemberManager::MemberRegisterFlagProcess()
{
    time_t t = time(0);
    struct tm* lt = localtime(&t);
    if (m_scheduler.IsOnTimeSpecialHour(lt->tm_hour, lt->tm_min))
    {
        for (std::map<unsigned int, CMember*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CMember* member = it->second;
            member->CheckMemberRegisterFlag();
        }
        register int min = lt->tm_min;
        register int hour = lt->tm_hour;
        CMyFileLog log(__FUNCTION__, 0x19a);
        log("./log/MemberModify", "CMemberManager::MemberRegisterFlagProcess(%d,%d)",
            hour, min);
    }
}

char CMemberManager::LoadMemberFromCash(CUser* user, CMember* member)
{
    if (member == 0 || user == 0)
    {
        return 0;
    }
    InsertMember(member->GetMemberKey(), member);
    user->AttachMember(member);
    member->NoticeMemberLogin_Out(user, 1);
    return 1;
}

int CMemberManager::DeleteMember(unsigned int key, bool cash)
{
    if (m_members.empty())
    {
        return 0;
    }
    std::map<unsigned int, CMember*>::iterator it = m_members.find(key);
    if (it != m_members.end())
    {
        if (cash && it->second != 0)
        {
            delete it->second;
        }
        if (m_app != 0)
        {
            m_app->Call_ResetUserMemberInfo(key);
        }
        m_members.erase(it);
        return 1;
    }
    if (cash)
    {
        CMyFileLog log(__FUNCTION__, 0xbb);
        log("./log/Member", "[DELETE_CASH_PROCESS] Member Key : %d", key);
    }
    return 0;
}

void CMemberManager::MemberMemLogout(unsigned int key, CUser* user, bool cash)
{
    if (user == 0 || m_app == 0)
    {
        throw CDNFException("CMemberManager::MemberMemLogout\t0 == pclUser || 0 == m_pclApp\n");
    }
    if (key == 0)
    {
        DNF_LOG_SCOPE_LINE(0x23b,"./log/MemberMember",
            "CMemberManager::MemberMemLogout()\tMemberKey == 0\tchar id(%d), Maybe after logout, this user connect at character screen, and logout again! check User.log!",
            user->GetUniqCharNo());
        return;
    }
    CMember* member = FindMember(key);
    if (member == 0)
    {
        DNF_LOG_SCOPE_LINE(0x241,"./log/Except",
            "CMemberManager::MemberMemLogout()\t0 == pclMember\tMemberKey(%d)", key);
        return;
    }
    member->NoticeMemberLogin_Out(user, 0);
    char ok = (char)DeleteMember(key, cash);
    if (ok != 1)
    {
        DNF_LOG_SCOPE_LINE(0x24b,"./log/MemberMember",
            "<Delete Member Error> CMemberManager::MemberMemLogout\tdeleteOrCash(%d), Member Key(%d)",
            (unsigned int)cash, key);
    }
}

void CMemberManager::SendToDBMemberUpdateCharInfo(CServerHandler* handler, unsigned int key,
                                                  unsigned char flag)
{
    if (handler != 0)
    {
        Packet_Monitor_SAVE_Member_Update_Char_Info pkt;
        pkt.m_uniqCharNo = key;
        pkt.m_flag = flag;
        handler->SendToDB(&pkt);
    }
}

unsigned int CMemberManager::GetLowerMemberEnterLimit(unsigned int level)
{
    ST_MemberConfig* info = m_memberConfig->GetMemberInfo();
    int idx = level / 10;
    if (idx > 0xa)
    {
        return 0;
    }
    return (unsigned int)info[idx].m_c;
}

int CMemberManager::IsPossableMemberEnter(CUser* u1, CMember* m1, CUser* u2, CMember* m2,
                                          int mode)
{
    if (u2 == 0 || u1 == 0)
    {
        return 0x31;
    }
    if (u1->GetLevel() < 0x12)
    {
        return 0x2e;
    }
    if (m2 != 0)
    {
        if (m2->IsThereUpper())
        {
            return (mode == 2) ? 0x35 : 0x2b;
        }
        int upper = m2->GetUpperMember_CharId();
        if (upper == 0)
        {
            return 0x2d;
        }
        if (upper != -1 && 0 < upper)
        {
            return (mode == 2) ? 0x35 : 0x2b;
        }
    }
    if (m1 != 0)
    {
        unsigned int charNo = u2->GetUniqCharNo();
        if (m1->FindLowerMember(charNo))
        {
            return (mode == 2) ? 0x33 : 0x32;
        }
        unsigned int lowerCount = m1->GetLowerMemberCount();
        short level = u1->GetLevel();
        unsigned int limit = GetLowerMemberEnterLimit((unsigned int)level);
        if (limit <= lowerCount)
        {
            return (mode == 2) ? 0x2a : 0x34;
        }
    }
    return 0;
}

bool CMemberManager::LoadMember(unsigned int key, STMemberDBInfo& info, unsigned int a,
                                unsigned int b, CServerHandler* handler)
{
    CMember* member = FindMember(key);
    if (member == 0)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x26d,"./log/Except",
            "[MEMBER]\tCMemberManager::LoadMember()\tpclMember is Null, member key(%d)\n", key);
        return 0;
    }
    CUser* user = FindMemberUser(key);
    if (user == 0)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x273,"./log/Except",
            "[MEMBER]\tCMemberManager::LoadMember()\tpclUser is Null, member key(%d)\n", key);
        return 0;
    }
    member->LoadMember(info, user->GetLevel(), a, b);
    if (!member->IsEmpty())
    {
        member->NoticeMemberLogin_Out(user, 1);
        return 1;
    }
    Packet_Monitor_Notice_Delete_Member_Id pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    user->SendToGameserver((char*)&pkt, 0x12);
    SendToDBMemberUpdateCharInfo(handler, key, 0);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x285,"./log/Except",
        "CMemberManager::LoadMember, true == pclMember->IsEmpty()\tChar id(%d), Member Key(%d)",
        user->GetUniqCharNo(), key);
    DeleteMember(user->GetUniqCharNo(), true);
    return 0;
}

CMember* CMemberManager::FindMember(unsigned int key)
{
    std::map<unsigned int, CMember*>::iterator it;
    if (m_members.empty())
    {
        return 0;
    }
    it = m_members.find(key);
    if (m_members.end() != it)
    {
        return it->second;
    }
    return 0;
}

CUser* CMemberManager::FindMemberUser(unsigned int key)
{
    if (m_userMgr == 0)
    {
        return 0;
    }
    return m_userMgr->FindUser_CharNo(key);
}

char CMemberManager::IsEmptyMember(unsigned int key)
{
    std::map<unsigned int, CMember*>::iterator it;
    if (m_members.empty())
    {
        return 1;
    }
    it = m_members.find(key);
    if (m_members.end() != it)
    {
        if (it->second != 0)
        {
            return it->second->IsEmpty();
        }
    }
    return 0;
}

CMemberExpTbl* CMemberManager::GetMemberExpTable()
{
    return m_memberExpTbl;
}

void CMemberManager::SaveMemberExp(CServerHandler* handler, unsigned int memberKey,
                                   unsigned int upperCharId, unsigned int exp)
{
    Packet_DB_Save_Member_Exp pkt;
    pkt.m_memberKey = memberKey;
    pkt.m_upperCharId = upperCharId;
    pkt.m_exp = exp;
    handler->SendToDB(&pkt);
}

unsigned char CMemberManager::IsMemberExpLevelUp(unsigned int exp)
{
    return m_memberExpTbl->IsMemberExpLevelUp(exp);
}

void CMemberManager::NoticeLevelUpToLowers(unsigned int upperCharId, unsigned int exp)
{
    CMember* upper;
    if ((upper = FindMember(upperCharId)) != 0)
    {
        upper->NoticeLevelUpToLowers((unsigned char)m_memberExpTbl->GetMemberExpLevel(exp));
        DNF_LOG_SCOPE_LINE(0x180,"./log/MemberModify", "Member Level Up! Char Id(%d), Exp(%d), Level(%d)",
            upperCharId, exp, (unsigned int)(unsigned char)m_memberExpTbl->GetMemberExpLevel(exp));
    }
}

CMember* CMemberManager::CreateMemberQuery(unsigned int key, CUser* user, CServerHandler* handler)
{
    if (user == 0 || handler == 0)
    {
        return 0;
    }
    CMember* member = new CMember(key, this);
    member->QueryMember(handler);
    InsertMember(key, member);
    user->AttachMember(member);
    return member;
}

int CMemberManager::InsertMember(unsigned int key, CMember* member)
{
    if (member != 0)
    {
        std::pair<std::map<unsigned int, CMember*>::iterator, bool> r =
            m_members.insert(std::make_pair(key, member));
        if (!r.second)
        {
            DNF_LOG_SCOPE_LINE(0x83, "./log/Member", "[INSERT_ERR] Member Key : %d\tAlready Member Exist", key);
        }
        return 1;
    }
    DNF_LOG_SCOPE_LINE(0x87, "./log/Member", "[INSERT_ERR] Member Key : %d\tpclMember == 0", key);
    return 0;
}

void CMemberManager::SaveMemberOnConnect(CServerHandler* handler, CUser* u1, CUser* u2,
                                        unsigned char flag)
{
    if (handler != 0 && u1 != 0 && u2 != 0)
    {
        Packet_Monitor_SAVE_Member pkt;
        pkt.m_flag = flag;
        short l1 = u1->GetLevel();
        short l2 = u2->GetLevel();
        if (l1 > l2)
        {
            pkt.m_upperCharNo = u1->GetUniqCharNo();
            pkt.m_lowerCharNo = u2->GetUniqCharNo();
            pkt.m_type = 1;
        }
        else if (l1 < l2)
        {
            pkt.m_upperCharNo = u2->GetUniqCharNo();
            pkt.m_lowerCharNo = u1->GetUniqCharNo();
            pkt.m_type = 2;
        }
        else
        {
            return;
        }
        handler->SendToDB(&pkt);
    }
}

void CMemberManager::SaveMemberOnUnConnect(CServerHandler* handler, unsigned int a,
                                          unsigned int b, unsigned int c, unsigned char flag)
{
    if (handler != 0)
    {
        Packet_Monitor_SAVE_Member pkt;
        pkt.m_flag = flag;
        if (c == 1)
        {
            pkt.m_upperCharNo = b;
            pkt.m_lowerCharNo = a;
            pkt.m_type = 1;
        }
        else
        {
            pkt.m_type = 2;
            if (c != 2)
            {
                DNF_LOG_SCOPE_AT(__FUNCTION__, 0x137,"./log/Member",
                    "CMemberManager::SaveMemberOnUnConnect , isSecederUpperOrLower == 0");
                return;
            }
            pkt.m_upperCharNo = a;
            pkt.m_lowerCharNo = b;
        }
        handler->SendToDB(&pkt);
    }
}

int CMemberManager::RegisterMember(CMember* member, short level, CUser* user, bool flag)
{
    if (member == 0 || user == 0)
    {
        return 0;
    }
    if (level < user->GetLevel())
    {
        if (!member->InsertUpperMember(user->GetUniqCharNo(),
                                       (unsigned char)user->GetLevel(), user->GetCharName(),
                                       flag))
        {
            return 0;
        }
    }
    else if (user->GetLevel() < level)
    {
        if (!member->InsertLowerMember(user->GetUniqCharNo(),
                                       (unsigned char)user->GetLevel(), user->GetCharName(),
                                       flag))
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

CMember* CMemberManager::CreateMemberInJoin(CUser* user)
{
    unsigned int charNo = user->GetUniqCharNo();
    CMember* member = new CMember(charNo, this);
    InsertMember(charNo, member);
    user->AttachMember(member);
    member->SetMemberDBFlag(2);
    member->SetMemberDBFlag(4);
    return member;
}

int CMemberManager::CheckMemberEnter(CUser* u1, CMember* m1, CUser* u2, CMember* m2)
{
    if (u1 == 0 || u2 == 0)
    {
        return 0x31;
    }
    if (u1->GetLevel() < u2->GetLevel())
    {
        return IsPossableMemberEnter(u2, m2, u1, m1, 2);
    }
    if (u1->GetLevel() > u2->GetLevel())
    {
        return IsPossableMemberEnter(u1, m1, u2, m2, 1);
    }
    return 0x2c;
}

char CMemberManager::CheckEmptyMember(CMember* member, CUser* user)
{
    if (user != 0 && member != 0 && member->IsEmpty() != 0)
    {
        Packet_Monitor_Notice_Delete_Member_Id pkt;
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_uniqCharNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, 0x12);
        DeleteMember(user->GetUniqCharNo(), true);
        return 1;
    }
    return 0;
}

char CMemberManager::IsAlreadyMemberMember(unsigned int key, unsigned int charNo)
{
    CMember* member = FindMember(key);
    if (member == 0)
    {
        return 0;
    }
    return member->IsAlreadyMemberMember(charNo);
}

int CMemberManager::GetMemberExpLevel(unsigned int level)
{
    return m_memberExpTbl->GetMemberExpLevel(level);
}

void CMemberManager::GetMemberExpNextLevelNeedExpLevel(unsigned int& exp,
                                                       unsigned int& expNext,
                                                       unsigned char& level)
{
    unsigned int needExp;
    m_memberExpTbl->GetMemberExpLevel(exp, needExp, expNext, level);
    exp = exp - needExp;
    expNext = expNext - needExp;
}

int CMemberManager::MemerMemLogin(unsigned int key, CUser* user)
{
    if (user == 0 || m_app == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() : m_pclApp , pclUser == NULL\n");
    }
    if (key == 0)
    {
        throw CDNFException("CMemberManager::MemerMemLogin() : uMemberKey == 0");
    }
    if (user->GetUniqCharNo() != key)
    {
        DNF_LOG_SCOPE_LINE(0x20c,"../log/Member",
            "CMemberManager::MemerMemLogin() : pclUser->GetUniqCharNo() != uMemberKey\tmember key(%d), char id(%d)",
            key, user->GetUniqCharNo());
    }
    CServerHandler* handler = m_app->Get_ServerHandler();
    if (handler == 0)
    {
        throw CDNFException("CMemberManager::MemerMemLogin() pclServerHandler == NULL\n");
    }
    int result = 0;
    if (FindMember(key) == 0)
    {
        result = (int)CreateMemberQuery(key, user, handler);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x21a,"../log/Member", "CMemberManager::MemerMemLogin() ( is already member error ) : %d",
            key);
    }
    return result;
}

int CMemberManager::IsThereUpperMember(unsigned int charNo)
{
    CMember* member = FindMember(charNo);
    if (member != 0)
    {
        return member->GetUpperMember_CharId();
    }
    return -2;
}
