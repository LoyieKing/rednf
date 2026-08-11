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
    *(unsigned short*)((char*)this + 0x18a) |= flag;
}

unsigned char CPowerManager::IsPowerWarOn()
{
    return ((const CPowerWar*)((char*)this + 0x14c))->IsPowerWarOn();
}

CPowerManager::CPowerManager()
    : m_field4(0)
{
    m_winnerSide = 0;
    m_powerWarEndKillPoint = 0x3c;
    m_sideCount = 3;
    m_field18a = 0;
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
    *(char*)((char*)this + 0x184) = side;
    if (IsPowerWarOn() != 1)
    {
        ((CPower*)((char*)this + 0x74))->SetScore(score1);
        ((CPower*)((char*)this + 0xe0))->SetScore(score2);
    }
    SetPowerDBFlag(4);
}

void CPowerManager::SendPowerWarInfo()
{
    Packet_Notice_Power_War_Info pkt;
    *(unsigned char*)((char*)&pkt + 0x12) = *(unsigned char*)((char*)this + 0x184);
    *(int*)((char*)&pkt + 0xa) = ((CPower*)((char*)this + 0x74))->GetScore();
    *(int*)((char*)&pkt + 0xe) = ((CPower*)((char*)this + 0xe0))->GetScore();
    (*(CApplication**)((char*)this + 4))->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
}

void CPowerManager::SendPowerWarScore()
{
    CApplication* app = *(CApplication**)((char*)this + 4);
    CServerHandler* handler = app->Get_ServerHandler();
    if (handler == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Reply_Power_War_Score pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    *(unsigned int*)((char*)&pkt + 0xe) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllTcpGameServer(&pkt);
    DNF_LOG_SCOPE_LINE(0x75,"./log/Power", "Power Point : A(%d)  B(%d)",
        (int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1),
        (int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2));
}

void CPowerManager::SendPowerWarEndTime(int time)
{
    CApplication* app = *(CApplication**)((char*)this + 4);
    CServerHandler* handler = app->Get_ServerHandler();
    if (handler == 0)
    {
        throw CDNFException(
            "CGuildManager::OnChangePowerWarScore() pclServerHandler == NULL\n");
    }
    Packet_Notice_Power_War_End_Time pkt;
    *(unsigned char*)((char*)&pkt + 0xa) = (unsigned char)time;
    *(unsigned int*)((char*)&pkt + 0xb) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    *(unsigned int*)((char*)&pkt + 0xf) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    handler->SendAllUdpGameServer((char*)&pkt, 0x13);
}

int CPowerManager::IncPowerScore(ENUM_POWER_SIDE_TYPE side, int score)
{
    if (((const CPowerWar*)((char*)this + 0x14c))->IsPowerWarOn() == 0)
    {
        return ((CPower*)((char*)this + 8 + side * 0x6c))->GetScore();
    }
    return ((CPower*)((char*)this + 8 + side * 0x6c))->IncScore(score);
}

int CPowerManager::GetPowerScore(ENUM_POWER_SIDE_TYPE side)
{
    return ((CPower*)((char*)this + 8 + side * 0x6c))->GetScore();
}

void CPowerManager::StartPowerWarEvent()
{
    ((CPowerWar*)((char*)this + 0x14c))->CPowerWar::setEvent();
    for (int i = 0; i < 3; i++)
    {
        ((CPower*)((char*)this + i * 0x6c + 8))->InitPower();
    }
    DNF_LOG_SCOPE_LINE(0xb9, "./log/Power", "CPowerManager::StartPowerWarEvent");
}

int CPowerManager::ComputeWinnerSide()
{
    int a = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    int b = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    if (b < a)
    {
        return 1;
    }
    a = GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    b = GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    if (a < b)
    {
        return 2;
    }
    return 0;
}

void CPowerManager::EndPowerWarEvent()
{
    CMyFileLog logTop(__FUNCTION__, 0xd5);
    logTop("./log/Power", "CPowerManager::EndPowerWarEvent TOP");
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
    ((CPowerWar*)((char*)this + 0x14c))->CPowerWar::resetEvent();
    CMyFileLog logBottom(__FUNCTION__, 0xfd);
    logBottom("./log/Power", "CPowerManager::EndPowerWarEvent BOTTOM");
}

void CPowerManager::CleanPowerWar()
{
    for (int i = 0; i < 3; i++)
    {
        ((CPower*)((char*)this + i * 0x6c + 8))->CleanPower();
    }
    DNF_LOG_SCOPE_LINE(0x170, "./log/Power", "CleanPower");
}

void CPowerManager::CalcPowerWarRank(bool flag)
{
    const char* s = flag ? "All" : "Winner";
    DNF_LOG_SCOPE_LINE(0x17c, "./log/Power", "CPowerManager::CalcPowerWarRank(%s)", s);
    if (flag)
    {
        ((CPower*)((char*)this + 0x74))->CalcPowerWarRank();
        ((CPower*)((char*)this + 0xe0))->CalcPowerWarRank();
    }
    else if (*(char*)((char*)this + 0x184) == 0 ||
             *(char*)((char*)this + 0x184) > 2)
    {
        DNF_LOG_SCOPE_LINE(0x187,"./log/Power", "invalid winner side income(%d)",
            (int)*(char*)((char*)this + 0x184));
    }
    else
    {
        ((CPower*)((char*)this + (unsigned char)*(char*)((char*)this + 0x184) * 0x6c + 8))
            ->CalcPowerWarRank();
    }
}

void CPowerManager::RewardGuildPowerWarPoint()
{
    CMyFileLog logTop(__FUNCTION__, 0x198);
    logTop("./log/PowerResult", "CPowerManager::RewardGuildPowerWarPoint");
    char winnerSide = *(char*)((char*)this + 0x184);
    if (winnerSide == 0 || winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x19c, "./log/PowerResult", "invalid winner side income(%d)", (int)winnerSide);
        return;
    }
    CApplication* app = *(CApplication**)((char*)this + 4);
    CGuildManager* gm = app->Get_GuildManager();
    int reward1 = *(int*)((char*)this + 0x18c);
    int reward2 = *(int*)((char*)this + 0x190);
    int reward3 = *(int*)((char*)this + 0x194);
    int reward4 = *(int*)((char*)this + 0x198);
    ((CPower*)((char*)this + (unsigned char)winnerSide * 0x6c + 8))
        ->RewardGuildPowerWarPoint(*gm, true, reward1, reward2, reward3, reward4);
    if (winnerSide == 1)
    {
        ((CPower*)((char*)this + 0xe0))
            ->RewardGuildPowerWarPoint(*gm, false, reward1, reward2, reward3, reward4);
    }
    else
    {
        ((CPower*)((char*)this + 0x74))
            ->RewardGuildPowerWarPoint(*gm, false, reward1, reward2, reward3, reward4);
    }
}

void CPowerManager::SaveDBPowerWarRank()
{
    char winnerSide = *(char*)((char*)this + 0x184);
    if (winnerSide != 0 && winnerSide < 3)
    {
        CMyFileLog logStart(__FUNCTION__, 0x1b6);
        logStart("./log/PowerResult", "POWER WAR RESULT DB SAVE START");
        CApplication* app = *(CApplication**)((char*)this + 4);
        Packet_DB_Save_Power_War_User_Rank userPkt;
        unsigned char group = app->Get_ServerHandler()->GetServerGroupNo();
        *(unsigned char*)((char*)&userPkt + 0xb) = group;
        STUserRank userRanks[500];
        for (int side = 1; side < 3; side++)
        {
            memset(userRanks, 0, 4000);
            *(unsigned char*)((char*)&userPkt + 0xc) = (unsigned char)side;
            CPowerWarCharacInfo* characInfo =
                ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarCharacInfo();
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
                *(unsigned char*)((char*)&userPkt + 0xa) = 1;
                *(unsigned int*)((char*)&userPkt + 0x11) = count;
                memcpy((char*)&userPkt + 0x15, userRanks, count << 3);
                app->Get_ServerHandler()->SendToDB(&userPkt);
            }
            else
            {
                *(unsigned char*)((char*)&userPkt + 0xa) = (unsigned char)(side == 1);
                *(unsigned int*)((char*)&userPkt + 0x11) = 0xfa;
                memcpy((char*)&userPkt + 0x15, userRanks, 2000);
                app->Get_ServerHandler()->SendToDB(&userPkt);
                *(unsigned char*)((char*)&userPkt + 0xc) = 0xfb;
                *(unsigned char*)((char*)&userPkt + 0xa) = 0;
                *(unsigned int*)((char*)&userPkt + 0x11) = count - 0xfa;
                memcpy((char*)&userPkt + 0x15, (char*)userRanks + 2000, (count - 0xfa) * 8);
                app->Get_ServerHandler()->SendToDB(&userPkt);
            }
        }
        Packet_DB_Save_Power_War_Guild_Rank guildPkt;
        *(unsigned char*)((char*)&guildPkt + 0xa) = app->Get_ServerHandler()->GetServerGroupNo();
        for (int side = 1; side < 3; side++)
        {
            *(unsigned char*)((char*)&guildPkt + 0xb) = (unsigned char)side;
            *(unsigned int*)((char*)&guildPkt + 0xc) = 0;
            memset((char*)&guildPkt + 0x10, 0, 800);
            CPowerWarGuildInfo* guildInfo =
                ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarGuildInfo();
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
        *(unsigned char*)((char*)&statuePkt + 0xa) =
            app->Get_ServerHandler()->GetServerGroupNo();
        std::vector<STPowerWarCharacInfo*> users;
        CPowerWarCharacInfo* s1 =
            ((CPower*)((char*)this + 0x74))->GetPowerWarCharacInfo();
        CPowerWarCharacInfo* s2 =
            ((CPower*)((char*)this + 0xe0))->GetPowerWarCharacInfo();
        s1->GetStatueRankingUsers(users);
        s2->GetStatueRankingUsers(users);
        std::sort(users.begin(), users.end(), STPowerWarCharacInfo::Compare);
        for (int i = 0; i < 3; i++)
        {
            if (i < (int)users.size())
            {
                unsigned int charNo = *(unsigned int*)users[i]->m_data;
                *(unsigned int*)((char*)&statuePkt + 0xb + i * 4) = charNo;
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
    CApplication* app = *(CApplication**)((char*)this + 4);
    Packet_DB_Save_Power_War_Point pkt;
    *(char*)((char*)&pkt + 0xb) = GetWinnerSide();
    *(unsigned int*)((char*)&pkt + 0xc) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    *(unsigned int*)((char*)&pkt + 0x10) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    *(unsigned char*)((char*)&pkt + 0xa) = app->Get_ServerGroup();
    app->Get_ServerHandler()->SendToDB(&pkt);
    DNF_LOG_SCOPE_LINE(0x28e,"./log/Power", "winner:%d, A:%d, B:%d, svr group:%d",
        (int)GetWinnerSide(), (int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1),
        (int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2),
        (unsigned int)app->Get_ServerGroup());
}

void CPowerManager::LoadPowerWarCfg(char* path)
{
    ((CPowerWar*)((char*)this + 0x14c))->LoadPowerWarTableFile(path);
}

void CPowerManager::SetPowerWarEndKillPoint(unsigned short point)
{
    ((CPowerWar*)((char*)this + 0x14c))->setPowerWarEndKillPoint(point);
}

unsigned short CPowerManager::GetPowerWarEndKillPoint()
{
    return ((const CPowerWar*)((char*)this + 0x14c))->getPowerWarEndKillPoint();
}

void CPowerManager::ProcessByMinute()
{
}

void CPowerManager::RewardBonusPoint()
{
    char winnerSide = *(char*)((char*)this + 0x184);
    if (winnerSide != 0 && winnerSide < 3)
    {
        ((CPower*)((char*)this + (unsigned char)winnerSide * 0x6c + 8))
            ->GetPowerWarCharacInfo()->CalcBonus();
    }
}

void CPowerManager::SaveDBPowerWarBonusPoint()
{
    char winnerSide = *(char*)((char*)this + 0x184);
    if (winnerSide == 0 || winnerSide > 2)
    {
        return;
    }
    Packet_DB_Save_Power_War_Bonus_Point pkt;
    CPowerWarCharacInfo* info =
        ((CPower*)((char*)this + winnerSide * 0x6c + 8))->GetPowerWarCharacInfo();
    info->GetBonus(pkt);
    if (*(int*)((char*)&pkt + 0xa) != 0)
    {
        CApplication* app = *(CApplication**)((char*)this + 4);
        app->Get_ServerHandler()->SendToDB(&pkt);
    }
}

void CPowerManager::SaveDBPowerWarPointReward()
{
    char winnerSide = *(char*)((char*)this + 0x184);
    if (winnerSide == 0 || winnerSide > 2)
    {
        DNF_LOG_SCOPE_LINE(0x357, "./log/Power", "invalid winner side income(%d)", (int)winnerSide);
        return;
    }
    CApplication* app = *(CApplication**)((char*)this + 4);
    for (int side = 1; side < 3; side++)
    {
        CPowerWarGuildInfo* info =
            ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarGuildInfo();
        int count = info->GetPowerWarPointDBSaveCount();
        if (count != 0)
        {
            Packet_DB_Save_Power_War_Point_Reward pkt;
            *(unsigned char*)((char*)&pkt + 0xa) =
                app->Get_ServerHandler()->GetServerGroupNo();
            info->MakePacketDBPowerWarPoint(&pkt);
            app->Get_ServerHandler()->SendToDB(&pkt);
        }
    }
}

void CPowerManager::UpdatePowerWarInfo(bool flag, ENUM_POWER_SIDE_TYPE side, int score, unsigned int* p)
{
    if (*(CApplication**)((char*)this + 4) == 0)
    {
        return;
    }
    for (int i = 0; i <= 3; i++)
    {
        if (p[i] != 0)
        {
            CUser* user = (*(CApplication**)((char*)this + 4))
                              ->Get_UserManager()
                              ->FindUser_CharNo(p[i]);
            if (user != 0)
            {
                ((CPower*)((char*)this + (int)side * 0x6c + 8))
                    ->UpdatePowerWarInfo(score, p[i], user->GetGuildKey());
            }
        }
    }
}

void CPowerManager::SetPowerWarRewardInfo(int a, int b, int c, int d)
{
    *(int*)((char*)this + 0x18c) = a;
    *(int*)((char*)this + 0x190) = b;
    *(int*)((char*)this + 0x194) = c;
    *(int*)((char*)this + 0x198) = d;
}

unsigned int CPowerManager::GetUserPowerWarPoint(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarCharacInfo()
        ->GetUserPowerWarPoint(charNo);
}

unsigned int CPowerManager::GetGuildRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int guildKey)
{
    return ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarGuildInfo()
        ->GetGuildRanking(guildKey);
}

unsigned int CPowerManager::GetUserRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int charNo)
{
    return ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarCharacInfo()
        ->GetUserRanking(charNo);
}

void CPowerManager::PrintDebugInfo()
{
    CMyFileLog logA(__FUNCTION__, 0x3c9);
    logA("./log/PowerResult", "----- POWER A");
    ((CPower*)((char*)this + 0x74))->GetPowerWarGuildInfo()->PrintDebugInfo();
    CMyFileLog logB(__FUNCTION__, 0x3cf);
    logB("./log/PowerResult", "----- POWER B");
    ((CPower*)((char*)this + 0xe0))->GetPowerWarGuildInfo()->PrintDebugInfo();
}

void CPowerManager::SendPowerWarProcessInfo(unsigned int charNo)
{
    CApplication* app = *(CApplication**)((char*)this + 4);
    CUser* user = app->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3e2,"./log/Power", "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclUser(%d)",
            charNo);
        return;
    }
    unsigned int guildKey = user->GetGuildKey();
    CGuild* guild = app->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x3ea,"./log/Power",
            "CPacketTranslater::SendPowerWarProcessInfo : 0 == pclGuild(%d), pclUser(%d)",
            guildKey, charNo);
        return;
    }
    Packet_Channel_Power_War_Process_Info pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = user->GetIdByChannel();
    *(unsigned int*)((char*)&pkt + 0xe) = charNo;
    *(unsigned int*)((char*)&pkt + 0x12) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)1);
    *(unsigned int*)((char*)&pkt + 0x16) = (unsigned int)GetPowerScore((ENUM_POWER_SIDE_TYPE)2);
    unsigned int side = (unsigned int)guild->GetPowerSide() & 0xff;
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
            (unsigned int)guild->GetPowerSide() & 0xff);
        return;
    }
    *(unsigned int*)((char*)&pkt + 0x1a) =
        GetUserPowerWarPoint((ENUM_POWER_SIDE_TYPE)side, charNo);
    if (IsPowerWarOn() == 0)
    {
        *(unsigned int*)((char*)&pkt + 0x1e) = 0;
        *(unsigned int*)((char*)&pkt + 0x22) = 0;
    }
    else
    {
        *(unsigned int*)((char*)&pkt + 0x1e) =
            GetGuildRankingInPower((ENUM_POWER_SIDE_TYPE)side, guildKey);
        *(unsigned int*)((char*)&pkt + 0x22) =
            GetUserRankingInPower((ENUM_POWER_SIDE_TYPE)side, charNo);
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
    CMyFileLog log2(__FUNCTION__, 0x41f);
    log2("./log/PowerResult", "SEND POWER WAR END INFO END");
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
        ((CPower*)((char*)this + side * 0x6c + 8))->GetPowerWarCharacInfo()
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
            CUser* user = (*(CApplication**)((char*)this + 4))->Get_UserManager()
                ->FindUser_CharNo(*(unsigned int*)characInfo->m_data);
            if (user != 0)
            {
                guild = (*(CApplication**)((char*)this + 4))->Get_GuildManager()
                    ->FindGuild(user->GetGuildKey());
                if (guild != 0)
                {
                    charNo = user->GetUniqCharNo();
                    guildKey = user->GetGuildKey();
                    isWinner = (guild->GetPowerSide() == GetWinnerSide()) ? 1 : 0;
                    CPower* power = (CPower*)((char*)this + side * 0x6c + 8);
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
    CApplication* app = *(CApplication**)((char*)this + 4);
    (void)app->GetPowerManager();
    *(unsigned int*)((char*)&pkt + 0xa) = user->GetIdByChannel();
    *(unsigned int*)((char*)&pkt + 0xe) = b;
    *(unsigned char*)((char*)&pkt + 0x12) = c;
    *(unsigned int*)((char*)&pkt + 0x13) = d;
    *(unsigned int*)((char*)&pkt + 0x17) = e;
    *(unsigned int*)((char*)&pkt + 0x1b) = f;
    *(unsigned int*)((char*)&pkt + 0x1f) = g;
    *(unsigned int*)((char*)&pkt + 0x23) = h;
    user->SendToGameserver((char*)&pkt, 0x27);
}

#pragma pack(push,1)
struct STUserPoint_Layout
{
    unsigned int m0;
    unsigned int m4;
};
#pragma pack(pop)
STUserPoint::STUserPoint()
{
    ((STUserPoint_Layout*)this)->m0 = 0;
    ((STUserPoint_Layout*)this)->m4 = 0;
}

Packet_DB_Save_Power_War_Bonus_Point::Packet_DB_Save_Power_War_Bonus_Point()
    : PacketHeader(0x6f4, 0x7de)
{
    *(unsigned int*)((char*)this + 0x10) = 0;
    for (int i = 0; i < 0xfa; i++) { new ((char*)this + 0xe + i * 0x8) STUserPoint; }
    memset((char*)this + 0xe, 0, 2000);
}

Packet_DB_Save_Power_War_Point::Packet_DB_Save_Power_War_Point()
    : PacketHeader(0x6ad, 0x14)
{
    (void)0;
}

#pragma pack(push,1)
struct STGuildRank_Layout
{
    unsigned int m0;
    unsigned int m4;
};
#pragma pack(pop)
STGuildRank::STGuildRank()
{
    ((STGuildRank_Layout*)this)->m0 = 0;
    ((STGuildRank_Layout*)this)->m4 = 0;
}

Packet_DB_Save_Power_War_Guild_Rank::Packet_DB_Save_Power_War_Guild_Rank()
    : PacketHeader(0x6d6, 0x330)
{
    *(unsigned char*)((char*)this + 0x10) = 0x0;
    *(unsigned char*)((char*)this + 0xb) = 0x0;
    *(unsigned int*)((char*)this + 0xc) = 0;
    for (int i = 0; i < 0x64; i++) { new ((char*)this + 0x10 + i * 0x8) STGuildRank; }
    memset((char*)this + 0x10, 0, 800);
}

#pragma pack(push,1)
struct STUserRank_Layout
{
    unsigned int m0;
    unsigned int m4;
};
#pragma pack(pop)
STUserRank::STUserRank()
{
    ((STUserRank_Layout*)this)->m0 = 0;
    ((STUserRank_Layout*)this)->m4 = 0;
}

Packet_DB_Save_Power_War_User_Rank::Packet_DB_Save_Power_War_User_Rank()
    : PacketHeader(0x6d7, 0x7e5)
{
    *(unsigned char*)((char*)this + 0x10) = 0x0;
    *(unsigned char*)((char*)this + 0xb) = 0x0;
    *(unsigned char*)((char*)this + 0xc) = 0x0;
    *(unsigned int*)((char*)this + 0xd) = 0;
    *(unsigned int*)((char*)this + 0x11) = 0;
    for (int i = 0; i < 0xfa; i++) { new ((char*)this + 0x15 + i * 0x8) STUserRank; }
    memset((char*)this + 0x15, 0, 2000);
}

#pragma pack(push,1)
struct Packet_DB_Save_Power_War_Statue_Ranker_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
};
#pragma pack(pop)
Packet_DB_Save_Power_War_Statue_Ranker::Packet_DB_Save_Power_War_Statue_Ranker()
    : PacketHeader(0x6da, 0x17)
{
    ((Packet_DB_Save_Power_War_Statue_Ranker_Layout*)this)->ma = 0;
    memset((char*)this + 0xb, 0, 0xc);
}

#pragma pack(push,1)
struct STPowerWarPointInfo_Layout
{
    unsigned int m0;
    unsigned int m4;
};
#pragma pack(pop)
STPowerWarPointInfo::STPowerWarPointInfo()
{
    ((STPowerWarPointInfo_Layout*)this)->m0 = 0;
    ((STPowerWarPointInfo_Layout*)this)->m4 = 0;
}

Packet_DB_Save_Power_War_Point_Reward::Packet_DB_Save_Power_War_Point_Reward()
    : PacketHeader(0x6d9, 0x7df)
{
    *(unsigned char*)((char*)this + 0x10) = 0x0;
    *(unsigned int*)((char*)this + 0xb) = 0;
    for (int i = 0; i < 0xfa; i++) { new ((char*)this + 0xf + i * 0x8) STPowerWarPointInfo; }
    memset((char*)this + 0xf, 0, 2000);
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
    return ((PowerManagerLayout*)this)->m_winnerSide;
}

void CPowerManager::SetWinnerSide(char side)
{
    ((PowerManagerLayout*)this)->m_winnerSide = side;
}
