// df_guild_r — PowerManager (split from source/guild per ORIG layout)
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

#include "PowerManager.h"
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
#include "DNFGuildManager.h"
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
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

struct PowerManagerLayout
{
    char pad0[4];
    CApplication* m_app;   // +4
    char rest[0x184 - 8];
    char m_winnerSide;     // +0x184
    char rest2[0x198 - 0x185];
};

void CPowerManager::Process()
{
}

void CPowerManager::SetPowerDBFlag(unsigned short flag)
{
    m_powerDBFlag |= flag;
}

bool CPowerManager::IsPowerWarOn()
{
    return m_powerWar.IsPowerWarOn();
}

CPowerManager::CPowerManager()
    : m_app(0)
{
    m_winnerSide = 0;
    m_powerWarEndKillPoint = 0x3c;
    m_sideCount = 3;
    m_powerDBFlag = 0;
}

CPowerManager::~CPowerManager()
{
}

void CPowerManager::InitPowerManager(char* path, CApplication* app)
{
    ((PowerManagerLayout*)this)->m_app = app;
    LoadPowerWarCfg(path);
}

void CPowerManager::SetPowerInfo(char side, int score1, int score2)
{
    m_winnerSide = side;
    if (!IsPowerWarOn())
    {
        m_power[1].SetScore(score1);
        m_power[2].SetScore(score2);
    }
    SetPowerDBFlag(4);
}

void CPowerManager::SendPowerWarInfo()
{
    Packet_Notice_Power_War_Info pkt;
    pkt.m_winner = m_winnerSide;
    pkt.m_scoreA = m_power[1].GetScore();
    pkt.m_scoreB = m_power[2].GetScore();
    ((CApplication*)m_app)->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
}

void CPowerManager::SendPowerWarScore()
{
    CServerHandler* handler;
    if ((handler = ((CApplication*)m_app)->Get_ServerHandler()) == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Reply_Power_War_Score pkt;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllTcpGameServer(&pkt);
    DNF_LOG_SCOPE_LINE(0x75,"./log/Power", "Power Point : A(%d)  B(%d)",
        pkt.m_scoreA, pkt.m_scoreB);
}

void CPowerManager::SendPowerWarEndTime(int time)
{
    CServerHandler* handler;
    if ((handler = ((CApplication*)m_app)->Get_ServerHandler()) == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Notice_Power_War_End_Time pkt;
    pkt.m_time = (unsigned char)time;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllUdpGameServer((char*)&pkt, 0x13);
}

int CPowerManager::IncPowerScore(ENUM_POWER_SIDE_TYPE side, int score)
{
    if (m_powerWar.IsPowerWarOn() != 0)
    {
        return m_power[side].IncScore(score);
    }
    return m_power[side].GetScore();
}

int CPowerManager::GetPowerScore(ENUM_POWER_SIDE_TYPE side)
{
    return m_power[side].GetScore();
}

void CPowerManager::StartPowerWarEvent()
{
    m_powerWar.CPowerWar::setEvent();
    for (int i = 0; i < 3; i++)
    {
        m_power[i].InitPower();
    }
    DNF_LOG_SCOPE_LINE(0xb9, "./log/Power", "CPowerManager::StartPowerWarEvent");
}

int CPowerManager::ComputeWinnerSide()
{
    if (GetPowerScore((ENUM_POWER_SIDE_TYPE)1) > GetPowerScore((ENUM_POWER_SIDE_TYPE)2))
    {
        return 1;
    }
    if (GetPowerScore((ENUM_POWER_SIDE_TYPE)1) < GetPowerScore((ENUM_POWER_SIDE_TYPE)2))
    {
        return 2;
    }
    return 0;
}

void CPowerManager::EndPowerWarEvent()
{
    {
        CMyFileLog logTop(__FUNCTION__, 0xd5);
        logTop("./log/Power", "CPowerManager::EndPowerWarEvent TOP");
    }
    SetWinnerSide(ComputeWinnerSide());
    RewardBonusPoint();
    SaveDBPowerWarPoint();
    CalcPowerWarRank(true);
    RewardGuildPowerWarPoint();
    PrintDebugInfo();
    SaveDBPowerWarRank();
    SendPowerWarInfo();
    SendPowerWarEndInfo();
    CleanPowerWar();
    m_powerWar.CPowerWar::resetEvent();
    CMyFileLog logBottom(__FUNCTION__, 0xfd);
    logBottom("./log/Power", "CPowerManager::EndPowerWarEvent BOTTOM");
}

void CPowerManager::CleanPowerWar()
{
    for (int i = 0; i < 3; i++)
    {
        m_power[i].CleanPower();
    }
    DNF_LOG_SCOPE_LINE(0x170, "./log/Power", "CleanPower");
}

void CPowerManager::CalcPowerWarRank(bool flag)
{
    DNF_LOG_SCOPE_LINE(0x17c, "./log/Power", "CPowerManager::CalcPowerWarRank(%s)",
        flag ? "All" : "Winner");
    if (flag)
    {
        m_power[1].CalcPowerWarRank();
        m_power[2].CalcPowerWarRank();
    }
    else if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x187,"./log/Power", "invalid winner side income(%d)",
            (int)m_winnerSide);
    }
    else
    {
        m_power[m_winnerSide].CalcPowerWarRank();
    }
}

void CPowerManager::RewardGuildPowerWarPoint()
{
    CMyFileLog logTop(__FUNCTION__, 0x198);
    logTop("./log/PowerResult", "CPowerManager::RewardGuildPowerWarPoint");
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x19c, "./log/PowerResult", "invalid winner side income(%d)", (int)m_winnerSide);
        return;
    }
    m_power[m_winnerSide].RewardGuildPowerWarPoint(
        *((CApplication*)m_app)->Get_GuildManager(), true,
        m_reward1, m_reward2, m_reward3, m_reward4);
    if (m_winnerSide == 1)
    {
        m_power[2].RewardGuildPowerWarPoint(
            *((CApplication*)m_app)->Get_GuildManager(), false,
            m_reward1, m_reward2, m_reward3, m_reward4);
    }
    else
    {
        m_power[1].RewardGuildPowerWarPoint(
            *((CApplication*)m_app)->Get_GuildManager(), false,
            m_reward1, m_reward2, m_reward3, m_reward4);
    }
}

void CPowerManager::SaveDBPowerWarRank()
{
    char winnerSide = m_winnerSide;
    if (winnerSide != 0 && winnerSide < 3)
    {
        CMyFileLog logStart(__FUNCTION__, 0x1b6);
        logStart("./log/PowerResult", "POWER WAR RESULT DB SAVE START");
        CApplication* app = *(CApplication**)&m_app;
        Packet_DB_Save_Power_War_User_Rank userPkt;
        unsigned char group = app->Get_ServerHandler()->GetServerGroupNo();
        userPkt.m_b = group;
        STUserRank userRanks[500];
        for (int side = 1; side < 3; side++)
        {
            memset(userRanks, 0, 4000);
            userPkt.m_c = (unsigned char)side;
            CPowerWarCharacInfo* characInfo =
                m_power[side].GetPowerWarCharacInfo();
            unsigned int count = 0;
            characInfo->GetAllUserRankingInfo(count, userRanks);
            {
                DNF_LOG_SCOPE_LINE(0x1d1,"./log/PowerResult", "SaveDBPowerWarRank() PowerSide %d, User Rank Count is %d",
                    side, count);
            }
            if (count == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1d6,"./log/PowerResult",
                    "SaveDBPowerWarRank() PowerSide %d, User Rank Count is 0", side, count);
            }
            else if (count < 0xfb)
            {
                userPkt.m_a = 1;
                userPkt.m_e = count;
                memcpy(userPkt.m_ranks, userRanks, count << 3);
                app->Get_ServerHandler()->SendToDB(&userPkt);
            }
            else
            {
                userPkt.m_a = (unsigned char)(side == 1);
                userPkt.m_e = 0xfa;
                memcpy(userPkt.m_ranks, userRanks, 2000);
                app->Get_ServerHandler()->SendToDB(&userPkt);
                userPkt.m_c = 0xfb;
                userPkt.m_a = 0;
                userPkt.m_e = count - 0xfa;
                memcpy(userPkt.m_ranks, (char*)userRanks + 2000, (count - 0xfa) * 8);
                app->Get_ServerHandler()->SendToDB(&userPkt);
            }
        }
        Packet_DB_Save_Power_War_Guild_Rank guildPkt;
        guildPkt.m_a = app->Get_ServerHandler()->GetServerGroupNo();
        for (int side = 1; side < 3; side++)
        {
            guildPkt.m_b = (unsigned char)side;
            guildPkt.m_c = 0;
            memset(guildPkt.m_ranks, 0, 800);
            CPowerWarGuildInfo* guildInfo =
                m_power[side].GetPowerWarGuildInfo();
            int count = 0;
            guildInfo->GetAllGuildRankingInfo(count, (STGuildRank*)((char*)&guildPkt + 0x10));
            app->Get_ServerHandler()->SendToDB(&guildPkt);
            {
                DNF_LOG_SCOPE_LINE(0x248,"./log/PowerResult",
                    "SaveDBPowerWarRank() SEND Packet_DB_Save_Power_War_Guild_Rank Power:%d", side);
            }
        }
        Packet_DB_Save_Power_War_Statue_Ranker statuePkt;
        Packet_Notice_Power_War_Rank noticePkt;
        (void)noticePkt;
        statuePkt.m_group = app->Get_ServerHandler()->GetServerGroupNo();
        std::vector<STPowerWarCharacInfo*> users;
        CPowerWarCharacInfo* s1 = m_power[1].GetPowerWarCharacInfo();
        CPowerWarCharacInfo* s2 = m_power[2].GetPowerWarCharacInfo();
        s1->GetStatueRankingUsers(users);
        s2->GetStatueRankingUsers(users);
        std::sort(users.begin(), users.end(), STPowerWarCharacInfo::Compare);
        for (int i = 0; i < 3; i++)
        {
            if (i < (int)users.size())
            {
                unsigned int charNo = *(unsigned int*)users[i]->m_data;
                statuePkt.m_rankers[i] = charNo;
                DNF_LOG_SCOPE_LINE(0x273, "./log/PowerResult", "Last Rank:%d, Charac No:%d", i, charNo);
            }
        }
        {
            DNF_LOG_SCOPE_LINE(0x277,"./log/PowerResult",
                "SaveDBPowerWarRank() SEND Packet_DB_Save_Power_War_Statue_Ranker");
        }
        app->Get_ServerHandler()->SendToDB(&statuePkt);
        {
            DNF_LOG_SCOPE_LINE(0x27b, "./log/PowerResult", "POWER WAR RESULT DB SAVE END");
        }
    }
}

void CPowerManager::SaveDBPowerWarPoint()
{
    Packet_DB_Save_Power_War_Point pkt;
    pkt.m_winner = GetWinnerSide();
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    pkt.m_group = ((CApplication*)m_app)->Get_ServerGroup();
    ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
    DNF_LOG_SCOPE_LINE(0x28e,"./log/Power", "winner:%d, A:%d, B:%d, svr group:%d",
        pkt.m_winner, pkt.m_scoreA, pkt.m_scoreB, (unsigned int)pkt.m_group);
}

void CPowerManager::LoadPowerWarCfg(char* path)
{
    m_powerWar.LoadPowerWarTableFile(path);
}

void CPowerManager::SetPowerWarEndKillPoint(unsigned short point)
{
    m_powerWar.setPowerWarEndKillPoint(point);
}

unsigned short CPowerManager::GetPowerWarEndKillPoint()
{
    return ((const CPowerWar&)m_powerWar).getPowerWarEndKillPoint();
}

void CPowerManager::ProcessByMinute()
{
}

void CPowerManager::RewardBonusPoint()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        return;
    }
    CPowerWarCharacInfo* info = m_power[m_winnerSide].GetPowerWarCharacInfo();
    info->CalcBonus();
}

void CPowerManager::SaveDBPowerWarBonusPoint()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        return;
    }
    Packet_DB_Save_Power_War_Bonus_Point pkt;
    CPowerWarCharacInfo* info = m_power[m_winnerSide].GetPowerWarCharacInfo();
    info->GetBonus(pkt);
    if (pkt.m_bonus != 0)
    {
        ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
    }
}

void CPowerManager::SaveDBPowerWarPointReward()
{
    if (m_winnerSide == 0 || m_winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x357, "./log/Power", "invalid winner side income(%d)", (int)m_winnerSide);
        return;
    }
    for (int side = 1; side < 3; side++)
    {
        CPowerWarGuildInfo* info = m_power[side].GetPowerWarGuildInfo();
        int count = info->GetPowerWarPointDBSaveCount();
        if (count != 0)
        {
            Packet_DB_Save_Power_War_Point_Reward pkt;
            pkt.m_a = ((CApplication*)m_app)->Get_ServerHandler()->GetServerGroupNo();
            info->MakePacketDBPowerWarPoint(&pkt);
            ((CApplication*)m_app)->Get_ServerHandler()->SendToDB(&pkt);
        }
    }
}

void CPowerManager::UpdatePowerWarInfo(bool flag, ENUM_POWER_SIDE_TYPE side, int score, unsigned int* p)
{
    if (m_app == 0)
    {
        return;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (p[i] == 0)
        {
            continue;
        }
        CUser* user = ((CApplication*)m_app)->Get_UserManager()->FindUser_CharNo(p[i]);
        if (user == 0)
        {
            continue;
        }
        unsigned int guildKey = user->GetGuildKey();
        m_power[side].UpdatePowerWarInfo(score, p[i], guildKey);
    }
}

void CPowerManager::SetPowerWarRewardInfo(int a, int b, int c, int d)
{
    m_reward1 = a;
    m_reward2 = b;
    m_reward3 = c;
    m_reward4 = d;
}

unsigned int CPowerManager::GetUserPowerWarPoint(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return m_power[side].GetPowerWarCharacInfo()
        ->GetUserPowerWarPoint(charNo);
}

unsigned int CPowerManager::GetGuildRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int guildKey)
{
    return m_power[side].GetPowerWarGuildInfo()
        ->GetGuildRanking(guildKey);
}

unsigned int CPowerManager::GetUserRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return m_power[side].GetPowerWarCharacInfo()
        ->GetUserRanking(charNo);
}

void CPowerManager::PrintDebugInfo()
{
    {
        CMyFileLog logA(__FUNCTION__, 0x3c9);
        logA("./log/PowerResult", "----- POWER A");
    }
    m_power[1].GetPowerWarGuildInfo()->PrintDebugInfo();
    CMyFileLog logB(__FUNCTION__, 0x3cf);
    logB("./log/PowerResult", "----- POWER B");
    m_power[2].GetPowerWarGuildInfo()->PrintDebugInfo();
}

void CPowerManager::SendPowerWarProcessInfo(unsigned int charNo)
{
    CUser* user =
        ((CApplication*)m_app)->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3e2,"./log/Power", "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclUser(%d)",
            charNo);
        return;
    }
    unsigned int guildKey = user->GetGuildKey();
    CGuild* guild = ((CApplication*)m_app)->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3ea,"./log/Power",
            "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclGuild(%d), pclUser(%d)",
            guildKey, charNo);
        return;
    }
    Packet_Channel_Power_War_Process_Info pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_charNo = charNo;
    pkt.m_scoreA = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    pkt.m_scoreB = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    int side = (int)(unsigned char)guild->GetPowerSide();
    if (side == 3)
    {
        side = 1;
    }
    else if (side == 4)
    {
        side = 2;
    }
    else if (side == 0 || 4 < side)
    {
        DNF_LOG_SCOPE_LINE(0x3fd,"./log/Power", "CPacketTranslater::SendPowerWarProcessInfo : Invalid Power Side(%d)",
            (int)(unsigned char)guild->GetPowerSide());
        return;
    }
    pkt.m_userPoint = GetUserPowerWarPoint((ENUM_POWER_SIDE_TYPE)side, charNo);
    if (IsPowerWarOn())
    {
        pkt.m_guildRank = GetGuildRankingInPower((ENUM_POWER_SIDE_TYPE)side, guildKey);
        pkt.m_userRank = GetUserRankingInPower((ENUM_POWER_SIDE_TYPE)side, charNo);
    }
    else
    {
        pkt.m_guildRank = 0;
        pkt.m_userRank = 0;
    }
    user->SendToGameserver((char*)&pkt, 0x26);
}

void CPowerManager::SendPowerWarEndInfo(int time)
{
}

void CPowerManager::SendPowerWarEndInfo()
{
    DNF_LOG_SCOPE_LINE(0x419, "./log/PowerResult", "SEND POWER WAR END INFO START");
    SendPowerWarEndInfoInSpecificPower(1);
    SendPowerWarEndInfoInSpecificPower(2);
    DNF_LOG_SCOPE_LINE(0x41f, "./log/PowerResult", "SEND POWER WAR END INFO END");
}

void CPowerManager::SendPowerWarEndInfoInSpecificPower(char side)
{
    DNF_LOG_SCOPE_LINE(0x42a, "./log/Power", "SEND POWER WAR END INFO %d Power START", side);
    unsigned int charNo = 0;
    CGuild* guild = 0;
    unsigned char isWinner = 0;
    unsigned int guildRank = 0;
    unsigned int userRank = 1;
    unsigned int guildKey = 0;
    STPowerWarCharacInfo* characInfo = 0;
    STPowerWarGuildInfo* guildInfo = 0;
    std::vector<STPowerWarCharacInfo*>* vec =
        m_power[side].GetPowerWarCharacInfo()
            ->GetCharacInfoVector();
    std::vector<STPowerWarCharacInfo*>::iterator it = vec->begin();
    {
        DNF_LOG_SCOPE_LINE(0x439, "./log/PowerResult", "SORT USER RANK COUNT : %d",
            (int)vec->size());
    }
    for (; it != vec->end(); ++it)
    {
        characInfo = *it;
        if (characInfo == 0)
        {
            DNF_LOG_SCOPE_LINE(0x463, "./log/PowerResult", "CharacInfo is NULL");
        }
        else
        {
            CUser* user = (*(CApplication**)&m_app)->Get_UserManager()
                ->FindUser_CharNo(*(unsigned int*)characInfo->m_data);
            if (user != 0)
            {
                guild = (*(CApplication**)&m_app)->Get_GuildManager()
                    ->FindGuild(user->GetGuildKey());
                if (guild != 0)
                {
                    charNo = user->GetUniqCharNo();
                    guildKey = user->GetGuildKey();
                    isWinner = (guild->GetPowerSide() == GetWinnerSide()) ? 1 : 0;
                    CPower* power = &m_power[side];
                    guildRank = power->GetPowerWarGuildInfo()->GetGuildRanking(guildKey);
                    guildInfo = power->GetPowerWarGuildInfo()->GetSpecificGuildInfo(guildKey);
                    if (guildInfo != 0)
                    {
                        SendPowerWarEndInfoToSpecificUser(user, charNo, isWinner,
                            *(unsigned int*)(characInfo->m_data + 4), userRank,
                            *(unsigned int*)(guildInfo->m_data + 4),
                            *(unsigned int*)(guildInfo->m_data + 8), guildRank);
                    }
                    else
                    {
                        DNF_LOG_SCOPE_LINE(0x45c, "./log/PowerResult",
                            "DBID(%d) CharacNo(%d) UserPP(%d) UserRank(%d)",
                            user->GetDBID(), charNo, *(unsigned int*)(characInfo->m_data + 4),
                            userRank);
                    }
                }
            }
        }
        userRank++;
    }
    {
        DNF_LOG_SCOPE_LINE(0x46b, "./log/PowerResult", "SEND POWER WAR END INFO %d Power END", side);
    }
}

void CPowerManager::SendPowerWarEndInfoToSpecificUser(CUser* user, unsigned int b,
                                                     unsigned char c, unsigned int d,
                                                     unsigned int e, unsigned int f,
                                                     unsigned int g, unsigned int h)
{
    Packet_Notice_Power_war_End_Info pkt;
    CPowerManager* powerManager = ((CApplication*)m_app)->GetPowerManager();
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_b = b;
    pkt.m_c = c;
    pkt.m_d = d;
    pkt.m_e = e;
    pkt.m_f = f;
    pkt.m_g = g;
    pkt.m_h = h;
    user->SendToGameserver((char*)&pkt, 0x27);
}

STUserPoint::STUserPoint()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Bonus_Point::Packet_DB_Save_Power_War_Bonus_Point()
    : PacketHeader(0x6f4, 0x7de), m_bonus(0)
{
    memset(m_points, 0, sizeof(m_points));
}

Packet_DB_Save_Power_War_Point::Packet_DB_Save_Power_War_Point()
    : PacketHeader(0x6ad, 0x14)
{
    (void)0;
}

STGuildRank::STGuildRank()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Guild_Rank::Packet_DB_Save_Power_War_Guild_Rank()
    : PacketHeader(0x6d6, 0x330), m_a(0), m_b(0), m_c(0)
{
    memset(m_ranks, 0, sizeof(m_ranks));
}

STUserRank::STUserRank()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_User_Rank::Packet_DB_Save_Power_War_User_Rank()
    : PacketHeader(0x6d7, 0x7e5), m_a(0), m_b(0), m_c(0), m_d(0), m_e(0)
{
    memset(m_ranks, 0, sizeof(m_ranks));
}

Packet_DB_Save_Power_War_Statue_Ranker::Packet_DB_Save_Power_War_Statue_Ranker()
    : PacketHeader(0x6da, 0x17)
{
    m_group = 0;
    memset(m_rankers, 0, sizeof(m_rankers));
}

STPowerWarPointInfo::STPowerWarPointInfo()
    : m0(0), m4(0)
{
}

Packet_DB_Save_Power_War_Point_Reward::Packet_DB_Save_Power_War_Point_Reward()
    : PacketHeader(0x6d9, 0x7df), m_a(0), m_b(0)
{
    memset(m_points, 0, sizeof(m_points));
}

#pragma pack(push,1)
struct Packet_Channel_Power_War_Process_Info_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned int m16;
    unsigned int m1a;
    unsigned int m1e;
    unsigned int m22;
};
#pragma pack(pop)
Packet_Channel_Power_War_Process_Info::Packet_Channel_Power_War_Process_Info()
    : PacketHeader(0x6e0, 0x26)
{
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->me = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m12 = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m16 = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m1a = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m1e = 0;
    ((Packet_Channel_Power_War_Process_Info_Layout*)this)->m22 = 0;
}

Packet_Notice_Power_War_Info::Packet_Notice_Power_War_Info()
    : PacketHeader(0x6ae, 0x13)
{
    (void)0;
}

Packet_Notice_Power_War_Rank::Packet_Notice_Power_War_Rank()
    : PacketHeader(0x6d8, 0x16)
{
    memset((char*)this + 0xa, 0, 0xc);
}

#pragma pack(push,1)
struct Packet_Notice_Power_war_End_Info_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned char m12;
    unsigned int m13;
    unsigned int m17;
    unsigned int m1b;
    unsigned int m1f;
    unsigned int m23;
};
#pragma pack(pop)
Packet_Notice_Power_war_End_Info::Packet_Notice_Power_war_End_Info()
    : PacketHeader(0x6b0, 0x27)
{
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->ma = 4294967295;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->me = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m12 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m13 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m17 = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m1b = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m1f = 0;
    ((Packet_Notice_Power_war_End_Info_Layout*)this)->m23 = 0;
}

Packet_Notice_Power_War_End_Time::Packet_Notice_Power_War_End_Time()
    : PacketHeader(0x6ac, 0x13)
{
    (void)0;
}

#pragma pack(push,1)
struct Packet_Reply_Power_War_Score_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Reply_Power_War_Score::Packet_Reply_Power_War_Score()
    : PacketHeader(0x6a5, 0x12)
{
    ((Packet_Reply_Power_War_Score_Layout*)this)->ma = 0;
    ((Packet_Reply_Power_War_Score_Layout*)this)->me = 0;
}

char CPowerManager::GetWinnerSide()
{
    return m_winnerSide;
}

void CPowerManager::SetWinnerSide(char side)
{
    m_winnerSide = side;
}
