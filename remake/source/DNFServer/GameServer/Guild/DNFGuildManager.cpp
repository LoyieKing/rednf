// df_guild_r — DNFGuildManager (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFGuildManager.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CGuildManager::LoadGuildAgit(unsigned int guildKey, CServerHandler* handler)
{
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        guild->LoadGuildAgit(handler, guildKey);
    }
}

void CGuildManager::Process()
{
}

void CGuildManager::DBSaveProcess(CApplication* app, bool force)
{
}

void CGuildManager::CargoLock()
{
}

void CGuildManager::CargoUnlock()
{
}

#pragma pack(push,1)
struct STAttendanceInfo_Layout
{
    unsigned int m0;
    unsigned int m4;
    unsigned int m8;
    unsigned int mc;
    unsigned int m10;
    unsigned int m14;
    unsigned int m18;
};
#pragma pack(pop)
STAttendanceInfo::STAttendanceInfo()
{
    ((STAttendanceInfo_Layout*)this)->m0 = 0;
    ((STAttendanceInfo_Layout*)this)->m4 = 0;
    ((STAttendanceInfo_Layout*)this)->m8 = 0;
    ((STAttendanceInfo_Layout*)this)->mc = 0;
    ((STAttendanceInfo_Layout*)this)->m10 = 0;
    ((STAttendanceInfo_Layout*)this)->m14 = 0;
    ((STAttendanceInfo_Layout*)this)->m18 = 4294967295;
}

CGuildWar* CGuildManager::GetGuildWar()
{
    return &m_guildWar;
}

unsigned int CGuildManager::GetMaxGuildExp1()
{
    return 10000;
}

unsigned int CGuildManager::GetMaxGuildExp2()
{
    return 20000;
}

bool CGuildManager::IsCargoLock()
{
    return m_field40 != 0;
}

CGuildManager::CGuildManager()
{
    m_app = 0;
    m_field40 = 1;
    memset(m_time1, 0, sizeof(m_time1));
    memset(m_time2, 0, sizeof(m_time2));
    m_scheduler.SetSpecialDayHour(1, 5);
}

CGuildManager::~CGuildManager()
{
}

void CGuildManager::Init(CApplication* app)
{
    m_app = app;
    m_guilds.clear();
}

int CGuildManager::InsertGuild(unsigned int guildKey, CGuild* guild)
{
    if (guild == 0)
    {
        return 0;
    }
    m_guilds.insert(std::make_pair(guildKey, guild));
    return 1;
}

void CGuildManager::DeleteGuild(unsigned int guildKey)
{
    if (m_guilds.empty())
    {
        return;
    }
    std::map<unsigned int, CGuild*>::iterator it = m_guilds.find(guildKey);
    if (it == m_guilds.end())
    {
        DNF_LOG_SCOPE_LINE(0xcd,"./log/Guild",
            "[DELETE_ERR]  Guild Key : %d\tCurr Guild Load Cnt : %d\n",
            guildKey, (int)m_guilds.size());
        return;
    }
    DNF_LOG_SCOPE_LINE(0xc4,"./log/Guild",
        "[DELETE]  Guild Key : %d\tGuild Name : %s\tGuild State:%d\tCurr Guild Load Cnt : %d\n",
        guildKey, it->second->GetGuildName(),
        it->second->GetGuildDBFlag() & 0xffff, (int)m_guilds.size() - 1);
    delete it->second;
    m_guilds.erase(it);
}

void CGuildManager::DeleteGuild(CGuild* guild)
{
    if (m_guilds.empty() || guild == 0)
    {
        return;
    }
    if (m_guilds.erase(guild->GetGuildKey()) == 1)
    {
        DNF_LOG_SCOPE_LINE(0xe0,"./log/Guild",
            "[DELETE]  Guild Key : %d\tGuild Name : %s\tCurr Guild Load Cnt : %d\n",
            guild->GetGuildKey(), guild->GetGuildName(), (int)m_guilds.size());
        delete guild;
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xe7,"./log/Guild",
            "[DELETE_ERR]  Guild Key : %d\tCurr Guild Load Cnt : %d\n",
            guild->GetGuildKey(), (int)m_guilds.size());
    }
}

CGuild* CGuildManager::FindGuild(unsigned int guildKey)
{
    if (m_guilds.empty())
    {
        return 0;
    }
    std::map<unsigned int, CGuild*>::iterator it = m_guilds.find(guildKey);
    return it == m_guilds.end() ? 0 : it->second;
}

bool CGuildManager::IsEmptyGuild(unsigned int guildKey)
{
    if (m_guilds.empty())
    {
        return 0;
    }
    std::map<unsigned int, CGuild*>::iterator it = m_guilds.find(guildKey);
    if (it != m_guilds.end())
    {
        CGuild* guild = it->second;
        if (guild != 0)
        {
            return guild->IsEmpty();
        }
    }
    return 0;
}

CGuild* CGuildManager::GuildMemLogin(unsigned int guildKey, CUser* user)
{
    if (user == 0 || m_app == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() : m_pclApp , pclUser == NULL\n");
    }
    if (guildKey == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() : dwGuildKey == 0\n");
    }
    CServerHandler* handler = m_app->Get_ServerHandler();
    if (handler == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() pclServerHandler == NULL\n");
    }
    CGuild* guild = FindGuild(guildKey);
    if (guild == 0)
    {
        guild = CreateGuild(guildKey, handler, user->GetUniqCharNo());
    }
    else
    {
        guild->QueryGuild(handler, user->GetUniqCharNo());
        guild->SendGuildInfoToMemberOnly(user);
        guild->CheckGuildSkill();
        AttendGuild(guildKey, user->GetUniqCharNo());
    }
    STTodayGuildMember* today = (STTodayGuildMember*)GetTodayMember(guildKey);
    if (today == 0)
    {
        guild->QueryTodayGuildMember(handler);
    }
    else
    {
        guild->SetTodayGuildMember(*today);
        guild->NotifyTodayGuildMember(user);
    }
    if (guild->InsertGuildMember(user->GetUniqCharNo(), user) == 1)
    {
        user->QueryGuildMember(handler);
    }
    return guild;
}

void CGuildManager::GuildMemLogout(unsigned int guildKey, CUser* user)
{
    if (user == 0 || m_app == 0)
    {
        throw CDNFException(
            "CGuildManager::GuildMemLogout()\t0 == pclUser || 0 == m_pclApp\n");
    }
    if (guildKey == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogout()\t0 == dwGuildKey\n");
    }
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        if (user->GetGameServer() != 0)
        {
            guild->DBGuildMemberSave(user, user->GetGameServer()->m_field9,
                                     m_app->Get_ServerHandler(), 1);
        }
        if (guild->DeleteGuildMember(user->GetUniqCharNo(), user) == 1)
        {
            guild->InsertGuildMemberChanglableInfo(user->GetUniqCharNo());
            if (!IsEmptyGuild(guildKey))
            {
                guild->NoticeGuildMemberLogin_Out(user, 0);
            }
            else
            {
                if (user->GetGameServer() != 0)
                {
                    guild->DBGuildSave(user->GetGameServer()->m_field9,
                                       m_app->Get_ServerHandler(), 0);
                }
                DeleteGuild(guildKey);
            }
        }
    }
}

int CGuildManager::LoadGuild(unsigned int guildKey, STGuildDBInfoOnly& info, char* name)
{
    CGuild* guild = FindGuild(guildKey);
    if (guild == 0)
    {
        return 0;
    }
    guild->LoadGuild(info, name);
    guild->CheckGuildSkill();
    return 1;
}

CGuild* CGuildManager::CreateGuild(unsigned int guildKey, CServerHandler* handler,
                                   unsigned int masterId)
{
    CGuild* guild = new CGuild(guildKey);
    guild->QueryGuild(handler, masterId);
    InsertGuild(guildKey, guild);
    return guild;
}

CGuild* CGuildManager::GuildEnter(unsigned int guildKey, ST_Notice_Guild_Enter& info)
{
    if (m_app == 0)
    {
        throw CDNFException("CGuildManager::GuildEnter()\t0 == m_pclApp\n");
    }
    if (guildKey == 0)
    {
        throw CDNFException("CGuildManager::GuildEnter()\t0 == dwGuildKey\n");
    }
    CServerHandler* handler = m_app->Get_ServerHandler();
    if (handler == 0)
    {
        throw CDNFException("CGuildManager::GuildEnter() pclServerHandler == NULL\n");
    }
    CUser* user = m_app->Get_UserManager()->FindUser_CharNo(*(unsigned int*)((char*)&info + 8));
    if (user != 0)
    {
        user->SendSetGuildKeyToUser(guildKey, *(unsigned int*)((char*)&info + 8));
    }
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        if (user == 0)
        {
            if (guild->IsSetGuildDBFlag(4) && guild->IsSetGuildDBFlag(0x10))
            {
                guild->QueryUnconnGuildMemberProxy(handler, *(unsigned int*)((char*)&info + 8));
            }
            return guild;
        }
        user->QueryGuildMember(handler);
        guild->InsertGuildMember(user->GetUniqCharNo(), user);
        if (guild->LoadGuildOneMemberProxy(user) != 1)
        {
            guild->IncTotalCnt_Of_GuildDBInfo();
        }
        guild->SendGuildInfoToMembers(false);
        user->MakeGameServerSendUserInfoPacket(guildKey);
        return guild;
    }
    if (user == 0)
    {
        return 0;
    }
    guild = new CGuild(guildKey);
    guild->QueryGuild(handler, user->GetUniqCharNo());
    InsertGuild(guildKey, guild);
    user->QueryGuildMember(handler);
    guild->InsertGuildMember(user->GetUniqCharNo(), user);
    user->MakeGameServerSendUserInfoPacket(guildKey);
    return guild;
}

CGuild* CGuildManager::GuildSecede(unsigned int guildKey, ST_Notice_Guild_Secede& info)
{
    if (m_app == 0)
    {
        throw CDNFException("CGuildManager::GuildSecede()\t0 == m_pclApp\n");
    }
    if (guildKey == 0)
    {
        throw CDNFException("CGuildManager::GuildSecede()\t0 == dwGuildKey\n");
    }
    CUser* user = m_app->Get_UserManager()->FindUser_CharNo(*(unsigned int*)((char*)&info + 8));
    CGuild* guild = FindGuild(guildKey);
    if (guild == 0)
    {
        if (user != 0)
        {
            char* accId = NumberToString(*(unsigned int*)((char*)&info + 4), 0);
            DNF_LOG_SCOPE_LINE(0x2a1,"./log/Except",
                "GUILD : CGuildManager::GuildSecede() pclGuild == NULL But pclUser != NULL( Guild Key : %d, Acc Id : %s, Char Id : %d )\n",
                guildKey, accId, *(unsigned int*)((char*)&info + 8));
        }
    }
    else
    {
        if (user != 0)
        {
            if (guild->DeleteGuildMember(user->GetUniqCharNo(), user) != 1)
            {
                return 0;
            }
            user->SendSetGuildKeyToUser(0, *(unsigned int*)((char*)&info + 8));
            user->ResetGuild();
        }
        guild->SecedeProxyMember(info);
        if (guild->IsSubGuildMaster(*(unsigned int*)((char*)&info + 8)) != 0)
        {
            guild->SetSubGuildMaster(*(unsigned int*)((char*)&info + 8), false);
        }
    }
    return guild;
}

void CGuildManager::GuildDismiss(CGuild* guild)
{
    if (m_app == 0)
    {
        throw CDNFException("CGuildManager::GuildDismiss()\t0 == m_pclApp\n");
    }
    if (guild == 0)
    {
        throw CDNFException("CGuildManager::GuildDismiss()\t0 == pclGuild\n");
    }
    DeleteGuild(guild);
}

void CGuildManager::SendGuildInfoToMembers(unsigned int guildKey, bool flag)
{
    if (guildKey == 0)
    {
        throw CDNFException(
            std::string("CGuildManager::SendGuildInfoToMembers()\t0 == dwGuildKey\n"));
    }
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        guild->SendGuildInfoToMembers(flag);
        return;
    }
    throw CDNFException(std::string("CGuildManager::SendGuildInfoToMembers() pclGuild == NULL\n"));
}

void CGuildManager::DBGuildProcess(CServerHandler* handler, bool flag)
{
    if (handler == 0)
    {
        return;
    }
    time_t now = time(0);
    tm* t = localtime(&now);
    int onTime = m_scheduler.IsOnTimeSpecialDayHour(t->tm_mday, t->tm_hour, t->tm_min);
    unsigned char groupNo = m_app->Get_ServerGroup();
    for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
         it != m_guilds.end(); ++it)
    {
        CGuild* guild = it->second;
        if (guild != 0)
        {
            unsigned int key = guild->GetGuildKey();
            if (key % 10 == (unsigned int)g_guildDBProcessDay)
            {
                guild->DBGuildSaveProcess(handler);
            }
        }
    }
    g_guildDBProcessDay++;
    if (g_guildDBProcessDay > 9)
    {
        g_guildDBProcessDay = 0;
    }
}

void CGuildManager::DBLoadAllLoginGuild(CServerHandler* handler)
{
    if (m_guilds.empty() || handler == 0)
    {
        return;
    }
    for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
         it != m_guilds.end(); ++it)
    {
        if (it->second != 0)
        {
            it->second->QueryGuild(handler, 0);
        }
    }
}

void CGuildManager::DBGuildAndGuildMemberSave(CServerHandler* handler)
{
    if (m_guilds.empty() || handler == 0)
    {
        return;
    }
    unsigned char group = m_app->Get_ServerGroup();
    for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
         it != m_guilds.end(); ++it)
    {
        if (it->second != 0)
        {
            it->second->DBGuildSave(group, handler, 0);
            it->second->DBSaveGuildMembers(group, handler, 2);
        }
    }
}

bool CGuildManager::IsGuildWarEventOn(unsigned char group)
{
    return m_guildWar.IsGuildWarEventOn();
}

bool CGuildManager::IsGuildWarEnterableChar(unsigned char group, unsigned int charNo)
{
    if (group == 6)
    {
        if (IsGuildWarEventOn(group) == 1)
        {
            return m_guildWar.IsGuildWarEnterableGuild(charNo) == 1;
        }
        return 0;
    }
    return 1;
}

void CGuildManager::ProcessByMinute()
{
    m_guildWar.DBSaveProcess(m_app);
    if (!m_guilds.empty())
    {
        for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
             it != m_guilds.end(); ++it)
        {
            if (it->second != 0)
            {
                it->second->UpdateChangableInfoProcess();
            }
        }
        RefreshTodayMember(false);
        RefreshAttendanceInfo(false);
    }
}

void CGuildManager::ProcessBySecond()
{
}

void CGuildManager::SetGuildExpTable(unsigned int* table)
{
    for (int i = 0; i < 20; i++)
    {
        *(unsigned int*)((char*)this + i * 4 + 0x70) = table[i];
    }
}

unsigned int CGuildManager::GetGuildExpWithLevel(unsigned char level)
{
    return (unsigned int)level * 1000;
}

int CGuildManager::GetGuildLevelWithExp(unsigned int exp)
{
    static const unsigned int g_guildExpLevelTable[17] = {
        0x546, 0x131e, 0x2f76, 0x6257, 0xb692, 0x13aa3, 0x201d9, 0x325db,
        0x4c89a, 0x716e2, 0xa4794, 0xe7e87, 0x140087, 0x1b00b6, 0x231a87,
        0x2c3b06, 0x3513a1,
    };
    if (exp < g_guildExpLevelTable[0x10])
    {
        for (int i = 0; i <= 0x10; i++)
        {
            if (g_guildExpLevelTable[i] > exp)
            {
                return i;
            }
        }
    }
    return 0x10;
}

void CGuildManager::InsertTodayMember(unsigned int guildKey, STTodayGuildMember& member)
{
    // ORIG：find 后仅当不存在才 insert（不覆盖已存在项）
    std::map<unsigned int, STTodayGuildMember>::iterator it = m_todayMembers.find(guildKey);
    if (it == m_todayMembers.end())
    {
        m_todayMembers.insert(std::make_pair(guildKey, member));
    }
}

STTodayGuildMember* CGuildManager::GetTodayMember(unsigned int guildKey)
{
    std::map<unsigned int, STTodayGuildMember>::iterator it = m_todayMembers.find(guildKey);
    if (it != m_todayMembers.end())
    {
        return &it->second;
    }
    else
    {
        return 0;
    }
}

void CGuildManager::RefreshTodayMember(bool flag)
{
    time_t now = time(0);
    tm* t = localtime(&now);
    if (!flag && *(unsigned int*)((char*)this + 0x7c) == (unsigned int)t->tm_mday &&
        t->tm_hour != 6)
    {
        return;
    }
    m_todayMembers.clear();
    for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
         it != m_guilds.end(); ++it)
    {
        if (it->second != 0)
        {
            it->second->QueryTodayGuildMember(m_app->Get_ServerHandler());
        }
    }
    *(tm*)((char*)this + 0x70) = *t;
}

void CGuildManager::AttendGuild(unsigned int guildKey, unsigned int charNo)
{
    bool attended = false;
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it != m_attendance.end())
    {
        if (std::find(it->second.begin(), it->second.end(), charNo) == it->second.end())
        {
            it->second.push_back(charNo);
            attended = true;
        }
    }
    else
    {
        std::vector<unsigned int> vec;
        vec.push_back(charNo);
        m_attendance.insert(std::make_pair(guildKey, vec));
        attended = true;
    }
    if (attended)
    {
        int phase = CheckAchieveAttendance(guildKey);
        if (phase != 0xffffffff)
        {
            RewardAttendance(guildKey, charNo, phase);
        }
    }
}

int CGuildManager::CheckAchieveAttendance(unsigned int guildKey)
{
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it != m_attendance.end())
    {
        static const int guild_att_phase[9] = { 5, 10, 20, 35, 60, 100, 150, 220, 300 };
        for (int phase = 8; phase >= 0; phase--)
        {
            if ((int)it->second.size() == guild_att_phase[phase])
            {
                return phase;
            }
        }
    }
    return -1;
}

int CGuildManager::GetAttendancePhase(unsigned int guildKey)
{
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it == m_attendance.end())
    {
        return 0;
    }
    return (int)it->second.size();
}

int CGuildManager::GetAttendanceExp(unsigned int guildKey, int phase)
{
    static const int guild_att_exp_tbl[153] = {
        1,3,4,8,10,18,29,46,70,77,112,158,218,236,307,386,0,
        1,4,4,9,11,18,29,46,70,78,113,159,219,236,307,387,0,
        2,6,8,17,21,36,59,91,140,155,224,316,437,472,614,773,0,
        2,10,12,25,31,53,88,138,209,232,337,475,655,708,920,1159,0,
        5,16,21,42,52,90,146,229,348,387,561,792,1093,1179,1534,1933,0,
        7,26,32,67,83,143,234,367,557,620,899,1266,1748,1888,2454,3092,0,
        9,33,41,84,104,179,292,458,697,774,1122,1583,2184,2360,3067,3865,0,
        13,46,56,117,145,251,409,642,975,1084,1572,2217,3059,3303,4295,5411,0,
        14,52,65,135,166,286,468,734,1115,1239,1797,2533,3496,3775,4907,6184,0,
    };
    CGuild* guild = FindGuild(guildKey);
    if (guild == 0)
    {
        return 0;
    }
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it == m_attendance.end())
    {
        return 0;
    }
    return guild_att_exp_tbl[phase * 0x11 + (guild->GetGuildLevel() & 0xff)];
}

void CGuildManager::GetAttendanceInfo(unsigned int guildKey, STAttendanceInfo& info)
{
    static const int guild_att_phase_tbl[9] = { 5, 10, 20, 35, 60, 100, 150, 220, 300 };
    static const int guild_att_exp_tbl[153] = {
        1,3,4,8,10,18,29,46,70,77,112,158,218,236,307,386,0,
        1,4,4,9,11,18,29,46,70,78,113,159,219,236,307,387,0,
        2,6,8,17,21,36,59,91,140,155,224,316,437,472,614,773,0,
        2,10,12,25,31,53,88,138,209,232,337,475,655,708,920,1159,0,
        5,16,21,42,52,90,146,229,348,387,561,792,1093,1179,1534,1933,0,
        7,26,32,67,83,143,234,367,557,620,899,1266,1748,1888,2454,3092,0,
        9,33,41,84,104,179,292,458,697,774,1122,1583,2184,2360,3067,3865,0,
        13,46,56,117,145,251,409,642,975,1084,1572,2217,3059,3303,4295,5411,0,
        14,52,65,135,166,286,468,734,1115,1239,1797,2533,3496,3775,4907,6184,0,
    };
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        *(unsigned int*)((char*)&info + 4) =
            (unsigned int)guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff;
        std::map<unsigned int, std::vector<unsigned int> >::iterator it =
            m_attendance.find(guildKey);
        if (it != m_attendance.end())
        {
            *(unsigned int*)&info = (unsigned int)it->second.size();
            int phase = GetAttendancePhase(guildKey);
            *(int*)((char*)&info + 0x18) = phase;
            if (phase >= 0 && phase < 9)
            {
                *(unsigned int*)((char*)&info + 0x10) = guild_att_phase_tbl[phase];
                unsigned int level = guild->GetGuildLevel();
                *(unsigned int*)((char*)&info + 0x14) =
                    guild_att_exp_tbl[phase * 0x11 + (level & 0xff)];
            }
            int next = phase + 1;
            if (next >= 0 && next < 9)
            {
                *(unsigned int*)((char*)&info + 8) = guild_att_phase_tbl[next];
                unsigned int level = guild->GetGuildLevel();
                *(unsigned int*)((char*)&info + 0xc) =
                    guild_att_exp_tbl[next * 0x11 + (level & 0xff)];
            }
        }
    }
}

void CGuildManager::RefreshAttendanceInfo(bool flag)
{
    time_t now = time(0);
    tm* t = localtime(&now);
    if (!flag && *(unsigned int*)((char*)this + 0xc0) == (unsigned int)t->tm_mday &&
        t->tm_hour != 6)
    {
        return;
    }
    m_attendance.clear();
    m_app->Get_UserManager()->RefreshGuildAttendanceInfo();
    *(tm*)((char*)this + 0xb4) = *t;
}

void CGuildManager::RewardAttendance(unsigned int guildKey, unsigned int charNo, int flag)
                                     
{
    if ((int)flag != -1 && (int)flag < 9)
    {
        int exp = GetAttendanceExp(guildKey, flag);
        if (exp != 0)
        {
            CGuild* guild = FindGuild(guildKey);
            if (guild != 0)
            {
                guild->AddGuildExp((unsigned int)exp);
                guild->NotifyAllAchieveAttendance(charNo, (unsigned int)exp);
            }
        }
    }
}
