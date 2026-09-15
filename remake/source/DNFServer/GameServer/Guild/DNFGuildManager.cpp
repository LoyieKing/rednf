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

unsigned int CGuildManager::m_ExpTable[17] = {
    0x546, 0x131e, 0x2f76, 0x6257, 0xb692, 0x13aa3, 0x201d9, 0x325db,
    0x4c89a, 0x716e2, 0xa4794, 0xe7e87, 0x140087, 0x1b00b6, 0x231a87,
    0x2c3b06, 0x3513a1,
};
unsigned int CGuildManager::m_uGuildExpMax1 = 10000;
unsigned int CGuildManager::m_uGuildExpMax2 = 20000;

static const int guild_att_phase[] = {
    5, 10, 20, 35, 60, 100, 150, 220, 300,
};

static const unsigned int guild_att_exp[9][17] = {
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
    m_cargoLock = 1;
}

void CGuildManager::CargoUnlock()
{
    m_cargoLock = 0;
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
    return m_uGuildExpMax1;
}

unsigned int CGuildManager::GetMaxGuildExp2()
{
    return m_uGuildExpMax2;
}

bool CGuildManager::IsCargoLock()
{
    return m_cargoLock;
}

CGuildManager::CGuildManager()
    : m_app(0)
{
    m_scheduler.SetSpecialDayHour(1, 5);
    m_cargoLock = 1;
    memset(m_time1, 0, sizeof(m_time1));
    memset(m_time2, 0, sizeof(m_time2));
}

CGuildManager::~CGuildManager()
{
    m_app = 0;
}

void CGuildManager::Init(CApplication* app)
{
    m_app = app;
    m_guilds.clear();
}

bool CGuildManager::InsertGuild(unsigned int guildKey, CGuild* guild)
{
    if (guild != 0)
    {
        return m_guilds.insert(std::make_pair(guildKey, guild)).second;
    }
    return 0;
}

int CGuildManager::DeleteGuild(unsigned int guildKey)
{
    if (m_guilds.empty())
    {
        return 0;
    }
    else
    {
        std::map<unsigned int, CGuild*>::iterator it = m_guilds.find(guildKey);
        if (it != m_guilds.end())
        {
            DNF_LOG_SCOPE_LINE(0xc4,"./log/Guild",
                "[DELETE]  Guild Key : %d\tGuild Name : %s\tGuild State:%d\tCurr Guild Load Cnt : %d\n",
                guildKey, it->second->GetGuildName(),
                it->second->GetGuildDBFlag() & 0xffff, (int)m_guilds.size() - 1);
            delete it->second;
            m_guilds.erase(it);
            return 1;
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xcd,"./log/Guild",
                "[DELETE_ERR]  Guild Key : %d\tCurr Guild Load Cnt : %d\n",
                guildKey, m_guilds.size());
            return 0;
        }
    }
}

int CGuildManager::DeleteGuild(CGuild* guild)
{
    if (m_guilds.empty() || guild == 0)
    {
        return 0;
    }
    else
    {
        if (m_guilds.erase(guild->GetGuildKey()) == 1)
        {
            DNF_LOG_SCOPE_LINE(0xe0,"./log/Guild",
                "[DELETE]  Guild Key : %d\tGuild Name : %s\tCurr Guild Load Cnt : %d\n",
                guild->GetGuildKey(), guild->GetGuildName(), (int)m_guilds.size());
            delete guild;
            return 1;
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xe7,"./log/Guild",
                "[DELETE_ERR]  Guild Key : %d\tCurr Guild Load Cnt : %d\n",
                guild->GetGuildKey(), (int)m_guilds.size());
            return 0;
        }
    }
}

CGuild* CGuildManager::FindGuild(unsigned int guildKey)
{
    std::map<unsigned int, CGuild*>::iterator it;
    if (m_guilds.empty())
    {
        return 0;
    }
    else
    {
        it = m_guilds.find(guildKey);
        if (m_guilds.end() != it)
        {
            return it->second;
        }
        return 0;
    }
}

bool CGuildManager::IsEmptyGuild(unsigned int guildKey)
{
    std::map<unsigned int, CGuild*>::iterator it;
    CGuild* guild;
    if (m_guilds.empty())
    {
        return 1;
    }
    else
    {
        it = m_guilds.find(guildKey);
        if (m_guilds.end() != it)
        {
            if ((guild = it->second) != 0)
            {
                return guild->IsEmpty();
            }
        }
        return 0;
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0012 | guild | 与ORIG差异=DIFF | CGuildManager::GuildMemLogin | 详见 function_reports/guild/_ZN13CGuildManager13GuildMemLoginEjP5CUser.md
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
    CGuild* guild;
    CServerHandler* handler;
    if (!(handler = m_app->Get_ServerHandler()))
    {
        throw CDNFException("CGuildManager::GuildMemLogin() pclServerHandler == NULL\n");
    }
    if (!(guild = FindGuild(guildKey)))
    {
        guild = CreateGuild(guildKey, handler, user->GetUniqCharNo());
    }
    else
    {
        guild->QueryGuild(handler, user->GetUniqCharNo());
        guild->SendGuildInfoToMemberOnly(user);
        guild->CheckGuildSkill();
        m_app->Get_GuildManager()->AttendGuild(guildKey, user->GetUniqCharNo());
    }
    STTodayGuildMember* today = m_app->Get_GuildManager()->GetTodayMember(guildKey);
    if (today == 0)
    {
        guild->QueryTodayGuildMember(m_app->Get_ServerHandler());
    }
    else
    {
        guild->SetTodayGuildMember(*today);
        guild->NotifyTodayGuildMember(user);
    }
    // [2026-09-15 修复/P354] ORIG 0x8078?? 该处为 `call; xor $1,%eax; test %al,%al;
    //   jne <skip>`（bool callee 的「否定条件 + 空 then + else」形）；正写 `== 1` 只产
    //   `test %al; je`（缺 xor 段，且块序相反）。
    if (guild->InsertGuildMember(user->GetUniqCharNo(), user) != 1)
    {
    }
    else
    {
        user->QueryGuildMember(handler);
    }
    return guild;
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0014 | guild | 与ORIG差异=DIFF | CGuildManager::GuildMemLogout | 详见 function_reports/guild/_ZN13CGuildManager14GuildMemLogoutEjP5CUser.md
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
    register bool bNull = (guild == 0);
    if (bNull)
    {
        return;
    }
    if (user->GetGameServer() != 0)
    {
        guild->DBGuildMemberSave(user, user->GetGameServer()->GetGroupNo(),
                                 m_app->Get_ServerHandler(), 1);
    }
    if (!guild->DeleteGuildMember(user->GetUniqCharNo(), user))
    {
        return;
    }
    guild->InsertGuildMemberChanglableInfo(user->GetUniqCharNo());
    if (IsEmptyGuild(guildKey))
    {
        if (user->GetGameServer() != 0)
        {
            guild->DBGuildSave(user->GetGameServer()->GetGroupNo(),
                               m_app->Get_ServerHandler(), 0);
        }
        DeleteGuild(guildKey);
    }
    else
    {
        guild->NoticeGuildMemberLogin_Out(user, 0);
    }
}

bool CGuildManager::LoadGuild(unsigned int guildKey, STGuildDBInfoOnly& info, char* name)
{
    CGuild* guild;
    if ((guild = FindGuild(guildKey)) == 0)
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
    CServerHandler* handler;
    if (!(handler = m_app->Get_ServerHandler()))
    {
        throw CDNFException("CGuildManager::GuildEnter() pclServerHandler == NULL\n");
    }
    CUser* user = m_app->Get_UserManager()->FindUser_CharNo(info.m_charNo);
    if (user != 0)
    {
        user->SendSetGuildKeyToUser(guildKey, info.m_charNo);
    }
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        if (user != 0)
        {
            user->QueryGuildMember(handler);
            guild->InsertGuildMember(user->GetUniqCharNo(), user);
            if (!guild->LoadGuildOneMemberProxy(user))
            {
                guild->IncTotalCnt_Of_GuildDBInfo();
            }
            guild->SendGuildInfoToMembers(false);
            user->MakeGameServerSendUserInfoPacket(guildKey);
        }
        else if (guild->IsSetGuildDBFlag(4) && guild->IsSetGuildDBFlag(0x10))
        {
            guild->QueryUnconnGuildMemberProxy(handler, info.m_charNo);
        }
    }
    else if (user != 0)
    {
        guild = new CGuild(guildKey);
        guild->QueryGuild(handler, user->GetUniqCharNo());
        InsertGuild(guildKey, guild);
        user->QueryGuildMember(handler);
        guild->InsertGuildMember(user->GetUniqCharNo(), user);
        user->MakeGameServerSendUserInfoPacket(guildKey);
    }
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
    CUser* user = m_app->Get_UserManager()->FindUser_CharNo(info.m_charNo);
    CGuild* guild = FindGuild(guildKey);
    if (guild != 0)
    {
        if (user != 0)
        {
            if (guild->DeleteGuildMember(user->GetUniqCharNo(), user) != 1)
            {
                return 0;
            }
            user->SendSetGuildKeyToUser(0, info.m_charNo);
            user->ResetGuild();
        }
        guild->SecedeProxyMember(info);
        if (guild->IsSubGuildMaster(info.m_charNo) != 0)
        {
            guild->SetSubGuildMaster(info.m_charNo, false);
        }
    }
    else if (user != 0)
    {
        register unsigned int charId = info.m_charNo;
        register char* accId = NumberToString(info.m_dbid, 0);
        DNF_LOG_SCOPE_LINE(0x2a1,"./log/Except",
            "GUILD : CGuildManager::GuildSecede() pclGuild == NULL But pclUser != NULL( Guild Key : %d, Acc Id : %s, Char Id : %d )\n",
            guildKey, accId, charId);
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
    }
    else
    {
        throw CDNFException(std::string("CGuildManager::SendGuildInfoToMembers() pclGuild == NULL\n"));
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0013 | guild | 与ORIG差异=DIFF | CGuildManager::DBGuildProcess | 详见 function_reports/guild/_ZN13CGuildManager14DBGuildProcessEP14CServerHandlerb.md
void CGuildManager::DBGuildProcess(CServerHandler* handler, bool flag)
{
    if (handler == 0)
    {
        return;
    }
    static unsigned int save_order = 0;
    CGuild* guild;
    time_t now = time(NULL);
    tm* t = localtime(&now);
    char onTime = m_scheduler.IsOnTimeSpecialDayHour(t->tm_mday, t->tm_hour, t->tm_min);
    unsigned char groupNo = m_app->Get_ServerGroup();
    for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
         it != m_guilds.end(); ++it)
    {
        guild = it->second;
        if (guild != 0)
        {
            unsigned int key = guild->GetGuildKey() % 10;
            if (key == save_order)
            {
                guild->DBGuildSaveProcess(handler);
            }
        }
    }
    save_order++;
    if (save_order > 9)
    {
        save_order = 0;
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
    if (group != 6)
    {
        return 1;
    }
    else
    {
        if (!IsGuildWarEventOn(group))
        {
            return 0;
        }
        else
        {
            return m_guildWar.IsGuildWarEnterableGuild(charNo);
        }
    }
}

void CGuildManager::ProcessByMinute()
{
    m_guildWar.DBSaveProcess(m_app);
    if (m_guilds.empty())
    {
        return;
    }
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

void CGuildManager::ProcessBySecond()
{
}

void CGuildManager::SetGuildExpTable(unsigned int* table)
{
    for (int i = 0; i < 17; i++)
    {
        m_ExpTable[i] = table[i];
    }
}

unsigned int CGuildManager::GetGuildExpWithLevel(unsigned char level)
{
    if (level > 0xf)
    {
        return m_ExpTable[0x10];
    }
    return m_ExpTable[level];
}

int CGuildManager::GetGuildLevelWithExp(unsigned int exp)
{
    int level;
    if (exp >= m_ExpTable[0x10])
    {
        return 0x10;
    }
    level = 0;
    for (int i = 0; i <= 0x10; i++)
    {
        if (m_ExpTable[i] > exp)
        {
            level = i;
            break;
        }
    }
    return level;
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
    struct CGM_Time1 { char pad[0x70]; tm t; };
    time_t now;
    tm* t;
    time(&now);
    t = localtime(&now);
    if (flag || (unsigned int)((CGM_Time1*)this)->t.tm_mday != (unsigned int)t->tm_mday &&
        t->tm_hour == 6)
    {
        m_todayMembers.clear();
        for (std::map<unsigned int, CGuild*>::iterator it = m_guilds.begin();
             it != m_guilds.end(); ++it)
        {
            if (it->second != 0)
            {
                it->second->QueryTodayGuildMember(m_app->Get_ServerHandler());
            }
        }
        ((CGM_Time1*)this)->t = *t;
    }
}

void CGuildManager::AttendGuild(unsigned int guildKey, unsigned int charNo)
{
    bool attended = false;
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it == m_attendance.end())
    {
        std::vector<unsigned int> vec;
        vec.push_back(charNo);
        m_attendance.insert(std::make_pair(guildKey, vec));
        attended = true;
    }
    else
    {
        std::vector<unsigned int>::iterator found =
            std::find(it->second.begin(), it->second.end(), charNo);
        if (found == it->second.end())
        {
            it->second.push_back(charNo);
            attended = true;
        }
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
        for (int phase = 8; phase >= 0; phase--)
        {
            if (it->second.size() == guild_att_phase[phase])
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
    if (it != m_attendance.end())
    {
        for (int phase = 8; phase >= 0; phase--)
        {
            if (it->second.size() >= guild_att_phase[phase])
            {
                return phase;
            }
        }
    }
    return -1;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-NEAR-0006 已消除：多维索引二维化）
int CGuildManager::GetAttendanceExp(unsigned int guildKey, int phase)
{
    CGuild* guild = FindGuild(guildKey);
    if (guild == 0)
    {
        return 0;
    }
    std::map<unsigned int, std::vector<unsigned int> >::iterator it =
        m_attendance.find(guildKey);
    if (it != m_attendance.end())
    {
        unsigned int cnt = it->second.size();
        register int p = phase;
        return guild_att_exp[p][guild->GetGuildLevel() & 0xff];
    }
    return 0;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0015 已消除）
//   [GuildA] 本件残余 1 条（ORIG then 块尾多一条 `jmp` 跳过对齐 nop）属「差 1 / 空差集」
//   族；修法是**外层守卫反转**：`if (guild != 0) { body }` → `if (!guild) { return; }`
//   后**保留原复合块 `{ body }`**（裸块不可删 —— 删掉就回到 ndiff=1，它正是复现 ORIG
//   块布局/尾 jmp 的关键）。同族反例：同一 TU 的 `LoadGuild`、`AddGuildFund`、
//   `DBGuildSaveProcess` 属「该 jmp 不可复现」型，加 return/裸块均无收益。
void CGuildManager::GetAttendanceInfo(unsigned int guildKey, STAttendanceInfo& info)
{
    CGuild* guild = FindGuild(guildKey);
    if (!guild)
    {
        return;
    }
    {
        info.m1 = (unsigned int)guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff;
        std::map<unsigned int, std::vector<unsigned int> >::iterator it =
            m_attendance.find(guildKey);
        if (it != m_attendance.end())
        {
            info.m0 = (unsigned int)it->second.size();
            int phase = GetAttendancePhase(guildKey);
            info.m6 = phase;
            if (phase >= 0 && phase < 9)
            {
                info.m4 = guild_att_phase[phase];
                register int p = phase;
                info.m5 = guild_att_exp[p][guild->GetGuildLevel() & 0xff];
            }
            phase++;
            if (phase >= 0 && phase < 9)
            {
                info.m2 = guild_att_phase[phase];
                register int p = phase;
                info.m3 = guild_att_exp[p][guild->GetGuildLevel() & 0xff];
            }
        }
    }
}

void CGuildManager::RefreshAttendanceInfo(bool flag)
{
    struct CGM_Time2 { char pad[0xb4]; tm t; };
    time_t now;
    tm* t;
    time(&now);
    t = localtime(&now);
    if (flag || (unsigned int)((CGM_Time2*)this)->t.tm_mday != (unsigned int)t->tm_mday &&
        t->tm_hour == 6)
    {
        m_attendance.clear();
        m_app->Get_UserManager()->RefreshGuildAttendanceInfo();
        ((CGM_Time2*)this)->t = *t;
    }
}

void CGuildManager::RewardAttendance(unsigned int guildKey, unsigned int charNo, int flag)
                                     
{
    if ((int)flag != -1 && (int)flag < 9)
    {
        unsigned int exp = GetAttendanceExp(guildKey, flag);
        if (exp != 0)
        {
            CGuild* guild = FindGuild(guildKey);
            if (guild != 0)
            {
                guild->AddGuildExp(exp);
                guild->NotifyAllAchieveAttendance(guild_att_phase[flag], exp);
            }
        }
    }
}
