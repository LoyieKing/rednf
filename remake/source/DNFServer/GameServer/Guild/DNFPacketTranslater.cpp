// df_guild_r — DNFPacketTranslater (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Guild_Change_Power_War_Point.h"

#include "DNFPacketTranslater.h"
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

#define THROW_IF_NO_APP(msg) \
    if (m_pclApp == 0) \
    { \
        throw CDNFException(msg); \
    }

CApplication* CPacketTranslater::m_pclApp = 0;

struct GuildPacketBodyView
{
    char m_base[0xa];
    unsigned char m_field_a;
    unsigned char m_field_b;
};

#define STUB_HANDLER(name) \
    void CPacketTranslater::name(PacketHeader* pkt) {}

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp != 0)
    {
        CGameServer* gs;
        if ((gs = m_pclApp->FindGameServer(*(int*)(pb + 0xe))) == 0)
        {
            char* mid = NumberToString(*(unsigned int*)(pb + 0xe), 0);
            DNF_LOG_SCOPE_LINE(0x65,"./log/Channel", "Not Found M_ID(%s) Channel No(%d)", mid,
                (unsigned int)(unsigned char)pb[0x12]);
        }
        else
        {
            CTcpGameServer* tgs = m_pclApp->FindTcpGameServer(*(unsigned int*)(pb + 0xe));
            if (tgs != 0)
            {
                CUserManager* um = m_pclApp->Get_UserManager();
                CUser* user = um->FindUser(*(unsigned int*)(pb + 0xe));
                if (user == 0)
                {
                    CUser* nu = um->CreateUser(*(unsigned int*)(pb + 0xe), 0, "",
                                               *(int*)(pb + 10), gs);
                    char* mid = NumberToString(*(unsigned int*)(pb + 0xe), 0);
                    DNF_LOG_SCOPE_LINE(0x76,"./log/User", "Current user count : %d\tConnected User DB ID : %s\n",
                        um->Size(), mid);
                    nu->SetSex((unsigned char)pb[0x23]);
                    nu->SetSsn(pb + 0x2c);
                    nu->SetTcpGameServer(tgs);
                    CMemoryCashManager* mc = m_pclApp->Get_MemoryCashManager();
                    if (mc->QueryCashMemoryBlackList(nu) != 1)
                    {
                        RequestBlackListToDBMW(*(unsigned int*)(pb + 0xe));
                    }
                }
                else
                {
                    char* mid = NumberToString(user->GetDBID(), 0);
                    DNF_LOG_SCOPE_LINE(0x70,"./log/User",
                        "DOUBLE CONNECTED : Already User DB ID(%s)\tChannel(%d)\tCurrent Connect User DB ID(%s)\tChannel(%d)\n",
                        mid, (unsigned int)user->GetGameServer()->GetChannelNo(),
                        NumberToString(*(unsigned int*)(pb + 0xe), 1),
                        (unsigned int)gs->GetChannelNo());
                }
            }
        }
    }
}
    catch (CDNFException& e)
    {
        CMyFileLog log("OnLogin", 0x85);
        log("./log/Except", "CPacketTranslater::OnLogin() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x8a);
        log("./log/Except", "CPacketTranslater::OnLogin() Exception Break\n");
    }
}

void CPacketTranslater::OnLogout(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    try
    {
        if (m_pclApp != 0)
        {
            CUserManager* um = m_pclApp->Get_UserManager();
            CUser* user;
            if ((user = um->FindUser(*(unsigned int*)(pb + 10))) != 0)
            {
                char* mid = NumberToString(*(unsigned int*)(pb + 10), 0);
                CMyFileLog log("OnLogout", 0xaa);
                log("./log/User",
                    "LOGOUT : User DB ID(%s), Char No(%d), Guild K(%d)GFlag(%d), name(%s), F.O.C(%d)\n",
                    mid, user->GetUniqCharNo(), user->GetGuildKey(), user->GetGuildMemFlag(),
                    user->GetCharName(), (unsigned int)(unsigned char)pb[0x17]);
                if (user->GetGuildKey() != 0)
                {
                    m_pclApp->Get_GuildManager()->GuildMemLogout(user->GetGuildKey(), user);
                }
                if (user->GetUniqCharNo() != 0)
                {
                    um->DeleteUser_CharNo(user->GetUniqCharNo());
                    std::string charName(user->GetCharName());
                    um->DeleteUser_CharName(charName);
                }
                user->ResetCharInfo();
                if (pb[0x17] == 0)
                {
                    CMemoryCashManager* mc = m_pclApp->Get_MemoryCashManager();
                    mc->InsertCashMemorySetCharacterObject(user);
                    mc->SetUserObject(user);
                    user->ResetBlackList();
                    if (um->DeleteUser(user) != 1)
                    {
                        char* mid2 = NumberToString(*(unsigned int*)(pb + 10), 0);
                        CMyFileLog log2("OnLogout", 0xd6);
                        log2("./log/User",
                            "[NO USER] Disconnected User DB ID : %s, Char No : %d , char name:%s\n",
                            mid2, user->GetUniqCharNo(), user->GetCharName());
                    }
                }
                else
                {
                    user->SetUserPosState(2);
                }
            }
        }
    }
    catch (std::exception& e)
    {
        printf("CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
        CMyFileLog log("OnLogout", 0xe5);
        log("./log/Except",
            "CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnLogout() Exception Break");
        CMyFileLog log(__FUNCTION__, 0xeb);
        log("./log/Except",
            "CPacketTranslater::OnLogout() Exception Break\n");
    }
}

void CPacketTranslater::OnHeartBeat(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    try
    {
        if (m_pclApp != 0)
        {
            CServerHandler* handler = m_pclApp->m_serverHandler;
            if (handler != 0)
            {
                unsigned char idx = ((GuildPacketBodyView*)pb)->m_field_a;
                if (idx == 0xc8)
                {
                    handler->ResetDBHeartBeat();
                    if (!handler->IsConnectedDBServer())
                    {
                        handler->SetDBConnectFlag(true);
                        handler->SendDBMWConnectionCheck();
                        DNF_LOG_SCOPE_LINE(0x10d, "./log/Udp", "DB Server Connection Complete!");
                    }
                }
                else if (idx != 0 && idx <= 0xbe)
                {
                    handler->ResetHeartBeat(idx);
                    if (!handler->IsConnectedGameServer(idx))
                    {
                        handler->SetConnectFlag(idx, true);
                        Packet_Tcp_Server_Connect connectPkt;
                        connectPkt.m_field_a = 0xcb;
                        CServerInterface* gs = handler->GetGameServer((unsigned int)idx);
                        if (gs != 0)
                        {
                            gs->SendToServer((char*)&connectPkt, 0xb);
                        }
                        else
                        {
                            DNF_LOG_SCOPE_LINE(0x129,"./log/Except",
                                "CPacketTranslater::OnHeartBeat() => Channel Index : %d\n",
                                (unsigned int)idx);
                        }
                    }
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x130,"./log/Except", "[ERROR - HEART BEAT] Channel Index(%d) Over.",
                        (unsigned int)idx);
                }
            }
        }
    }
    catch (std::exception& e)
    {
        printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x137, "./log/Except",
            "CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnHeartBeat() Exception Break");
        DNF_LOG_SCOPE_LINE(0x13d, "./log/Except",
            "CPacketTranslater::OnHeartBeat() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyUserInfo(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    {
        DNF_LOG_SCOPE_LINE(0x150,"./log/Reboot", "[GAME SERVER] Channel No : %d\n",
            (unsigned int)(unsigned char)pb[0xb]);
    }
    THROW_IF_NO_APP("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
    CGameServer* gs;
    if ((gs = m_pclApp->FindGameServer((int)(unsigned char)pb[0xb])) == 0)
    {
        throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
    }
    CTcpGameServer* tgs = m_pclApp->FindTcpGameServer(*(unsigned int*)(pb + 6));
    if (tgs != 0)
    {
        CUserManager* um = m_pclApp->Get_UserManager();
        int count = (int)(unsigned char)pb[10];
        for (int i = 0; i < count; i++)
        {
            char* rec = pb + i * 0x4e + 0xc;
            unsigned int dbid = *(unsigned int*)(rec + 0);
            CUser* user = um->FindUser(dbid);
            if (user == 0)
            {
                user = um->CreateUser(dbid, *(unsigned int*)(rec + 4), rec + 0x14,
                                      *(int*)(rec + 8), gs);
                user->SetUserInfo_CharNo((char)rec[0x10], (char)rec[0x11],
                                         *(short*)(rec + 0x12), *(unsigned int*)(rec + 4),
                                         rec + 0x14);
                user->SetSex((unsigned char)rec[0x46]);
                user->SetSsn(rec + 0x47);
                user->SetTcpGameServer(tgs);
                RequestBlackListToDBMW(dbid);
                if (*(unsigned int*)(rec + 0xc) != 0)
                {
                    m_pclApp->Get_GuildManager()->GuildMemLogin(*(unsigned int*)(rec + 0xc),
                                                                user);
                }
            }
            else
            {
                char* mid = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x166, "./log/Except", "CPacketTranslater::OnReplyUserInfo() : %s\n", mid);
            }
        }
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
        CMyFileLog log("OnReplyUserInfo", 0x187);
        log("./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyUserInfo() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x18d);
        log("./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnCharLogin(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            return;
        }
        if (m_pclApp->Get_GuildManager()->IsGuildWarEnterableChar(
                (unsigned char)pb[0x3d], *(unsigned int*)(pb + 0x13)) != true)
        {
            Packet_Monitor_UDP_User_Getout pktGetout;
            *(unsigned int*)((char*)&pktGetout + 0xa) = *(unsigned int*)(pb + 0xa);
            m_pclApp->Get_ServerHandler()->SendToGameServer((unsigned char)pb[0xe], &pktGetout);
        }
        CUserManager* um = m_pclApp->Get_UserManager();
        CUser* user = um->FindUser(*(unsigned int*)(pb + 10));
        if (user == 0)
        {
            char* mid = NumberToString(*(unsigned int*)(pb + 10), 0);
            DNF_LOG_SCOPE_LINE(0x1d9,"./log/User",
                "[CHAR_LOGIN_ERR]\tDB ID : %s\tChar Key : %d\tGuild Key : %d\tJob : %d\tname : %s\n",
                mid, *(unsigned int*)(pb + 0xf), *(unsigned int*)(pb + 0x13),
                (int)(char)pb[0x17], pb + 0x1f);
            return;
        }
        user->SetUserInfo_CharNo((char)pb[0x17], (char)pb[0x18],
                                 *(short*)(pb + 0x19), *(unsigned int*)(pb + 0xf), pb + 0x1f);
        char* mid = NumberToString(*(unsigned int*)(pb + 10), 0);
        DNF_LOG_SCOPE_LINE(0x1b1,"./log/User",
            "[CHAR_LOGIN]\tDB ID(%s)\tChar Key(%d)\tGuild K(%d)\tMember K(%d)\tJob(%d)\tname(%s)\tCh No(%d)\tpvp(%d)\n",
            mid, *(unsigned int*)(pb + 0xf), *(unsigned int*)(pb + 0x13),
            *(unsigned int*)(pb + 0x1b), (int)(char)pb[0x17], pb + 0x1f,
            (unsigned int)(unsigned char)pb[0xe], 0);
        user->SetUserPosState(3);
        if (*(unsigned int*)(pb + 0x13) != 0)
        {
            CGuild* guild = m_pclApp->Get_GuildManager()->GuildMemLogin(
                *(unsigned int*)(pb + 0x13), user);
            if (guild != 0)
            {
                guild->NoticeGuildMemberLogin_Out(user, 1);
            }
        }
        if (um->InsertUser_CharName(pb + 0x1f, user) != 1)
        {
            char* mid2 = NumberToString(*(unsigned int*)(pb + 10), 0);
            CMyFileLog log2("OnCharLogin", 0x1ce);
            log2("./log/Except",
                 "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!", mid2, pb + 0x1f);
        }
        if (um->InsertUser_CharNo(*(unsigned int*)(pb + 0xf), user) != 1)
        {
            CMyFileLog log("OnCharLogin", 0x1d2);
            log("./log/Except",
                "Insert Fail!\tChar ID : %d\t\xc4\xb3\xb8\xaf\xc5\xcd \xc0\xcc\xb8\xa7:%s\n",
                *(unsigned int*)(pb + 0xf), pb + 0x1f);
        }
        m_pclApp->Get_MemoryCashManager()->DeleteCashObjecct(user->GetDBID());
    }
    catch (std::exception& e)
    {
        printf("CPacketTranslater::OnCharLogin() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1e5, "./log/Except",
            "CPacketTranslater::OnCharLogin() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCharLogin() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1eb, "./log/Except",
            "CPacketTranslater::OnCharLogin() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyQueryGuild(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (pb[10] == 1)
    {
        int rc = m_pclApp->Get_GuildManager()->LoadGuild(
            *(unsigned int*)(pb + 0xb), *(STGuildDBInfoOnly*)(pb + 0x13), pb + 0xd0);
        (void)rc;
        m_pclApp->Get_GuildManager()->SendGuildInfoToMembers(
            *(unsigned int*)(pb + 0xb), true);
        m_pclApp->Get_GuildManager()->AttendGuild(
            *(unsigned int*)(pb + 0xb), *(unsigned int*)(pb + 0xf));
        if (pb[0xb1] != 0)
        {
            CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(
                *(unsigned int*)(pb + 0xb));
            CServerHandler* handler = m_pclApp->Get_ServerHandler();
            if (guild != 0)
            {
                guild->LoadGuildAgit(handler, *(unsigned int*)(pb + 0xb));
            }
        }
    }
    else
    {
        m_pclApp->Get_GuildManager()->SendGuildInfoToMembers(
            *(unsigned int*)(pb + 0xb), true);
        DNF_LOG_SCOPE_LINE(0x21d,"./log/Except",
            "[DB ERROR]CPacketTranslater::OnReplyQueryGuild() packet->bSuccess : %d,guildKey(%d)",
            (unsigned int)(unsigned char)pb[10], *(unsigned int*)(pb + 0xb));
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyQueryGuild() Exception Break : %s\n", e.what());
        CMyFileLog log("OnReplyQueryGuild", 0x223);
        log("./log/Except", "CPacketTranslater::OnReplyQueryGuild() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyQueryGuild() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x229);
        log("./log/Except", "CPacketTranslater::OnReplyQueryGuild() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyQueryGuildMember(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnDBReplyQueryGuildMember()\tpclApp is NULL")
    char* pb = (char*)pkt;
    if (pb[10] == 1)
    {
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xf));
        if (user == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyQueryGuildMember()\tpclUser is NULL\n");
        }
        user->LoadGuildMember(*(unsigned int*)(pb + 0xb),
                              *(STGuildMemerDBInfo*)(pb + 0x13));
        user->SendGuildMemberDBInfo(*(STGuildMemerDBInfo*)(pb + 0x13));
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x24b,"./log/Except",
            "[DB ERROR]CPacketTranslater::OnDBReplyQueryGuildMember() packet->bSuccess : %d\n",
            (unsigned int)(unsigned char)pb[10]);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break : %s\n", e.what());
        CMyFileLog log("OnDBReplyQueryGuildMember", 0x251);
        log("./log/Except", "CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x257);
        log("./log/Except", "CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildEnter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildEnter : 0 == m_pclApp")
    char* pb = (char*)pkt;
    char* mid = NumberToString(*(unsigned int*)(pb + 0xe), 0);
    DNF_LOG_SCOPE_LINE(0x283,"./log/Web",
        "Packet_Monitor_Notice_Guild_Enter: guildkey : %d, m_id : %s , charid : %d, guildname : %s, charname : %s\n",
        *(unsigned int*)(pb + 10), mid, *(unsigned int*)(pb + 0x12), pb + 0x16, pb + 0x2d);
    CGuild* guild = m_pclApp->Get_GuildManager()->GuildEnter(
        *(unsigned int*)(pb + 10), *(ST_Notice_Guild_Enter*)(pb + 10));
    if (guild != 0)
    {
        pb[0x4f] = 1;
        *(unsigned int*)(pb + 0x4b) = *(unsigned int*)(pb + 0x12);
        guild->NoticeEnterToGuildMember(pb + 10);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildEnter() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildEnter", 0x290);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildEnter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildEnter() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x296);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildEnter() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildSecede(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildSecede : 0 == m_pclApp")
    char* pb = (char*)pkt;
    try
    {
        CUser* user =
            m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0x12));
        if (user != 0)
        {
            Packet_Guild_Exp_Book_Delete expDel;
            *(unsigned int*)((char*)&expDel + 0xa) = user->GetIdByChannel();
            *(unsigned int*)((char*)&expDel + 0xe) = *(unsigned int*)(pb + 0x12);
            *(unsigned int*)((char*)&expDel + 0x12) = *(unsigned int*)(pb + 0xe);
            *(unsigned int*)((char*)&expDel + 0x16) =
                (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
            user->SendTcpGameserver((PacketHeader*)&expDel);
        }
        else
        {
            Packet_No_Cache noCache;
            *(unsigned int*)((char*)&noCache + 0xa) = *(unsigned int*)(pb + 0xe);
            *(unsigned int*)((char*)&noCache + 0xe) =
                (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
            *(unsigned int*)((char*)&noCache + 0x12) = 1;
            m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
            *(unsigned int*)((char*)&noCache + 0x12) = 2;
            m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);

            Packet_DBMW_Query_Msg query;
            *(unsigned int*)((char*)&query + 0xa) = 0x4f00;
            *(unsigned int*)((char*)&query + 0xe) = 2;
            sprintf((char*)&query + 0x12,
                    "upDate charac_info set guild_secede = 1 where charac_no = %u",
                    *(unsigned int*)(pb + 0x12));
            m_pclApp->Get_ServerHandler()->SendToDB(&query);
        }

        CGuild* guild = m_pclApp->Get_GuildManager()->GuildSecede(
            *(unsigned int*)(pb + 0xa), *(ST_Notice_Guild_Secede*)(pb + 0xa));
        if (guild != 0)
        {
            guild->NoticeSecedeToGuildMember(pb + 0xa);
            guild->SendGuildInfoToMembers(false);
        }
    }
    catch (std::exception& e)
    {
        CMyFileLog log("OnNoticeGuildSecede", 0x2f9);
        log("./log/Except",
            "CPacketTranslater::OnNoticeGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x2ff);
        log("./log/Except",
            "CPacketTranslater::OnNoticeGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildMarkChange(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildMarkChange : 0 == m_pclApp")
    char* pb = (char*)pkt;
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 10));
    if (guild != 0)
    {
        guild->NoticeMarkChangeToGuildMember(*(unsigned int*)(pb + 0xe));
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildMarkChange() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildMarkChange", 0x337);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildMarkChange() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildMarkChange() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x33d);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildMarkChange() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildCreate(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildCreate : 0 == m_pclApp");
    char* pb = (char*)pkt;
    CServerHandler* handler;
    if ((handler = m_pclApp->Get_ServerHandler()) == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() pclServerHandler == NULL\n");
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe));
    if (user != 0)
    {
        unsigned int uniqCharNo = user->GetUniqCharNo();
        m_pclApp->Get_GuildManager()->CreateGuild(*(unsigned int*)(pb + 0xa), handler,
                                                  uniqCharNo);
        user->QueryGuildMember(handler);
        Packet_Monitor_Notice_Guild_Create_ToUser notice;
        memcpy((char*)&notice + 0xa, pb + 0xa, 0x1f);
        *(unsigned int*)((char*)&notice + 0x29) = user->GetIdByChannel();
        user->SendToGameserver((char*)&notice, 0x2d);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildCreate() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildCreate", 0x37f);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildCreate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildCreate() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x385);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildCreate() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildDismiss(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildDismiss : 0 == m_pclApp")
    char* pb = (char*)pkt;
    DNF_LOG_SCOPE_LINE(0x3a9,"./log/Web", "Packet_Monitor_Notice_Guild_Dismiss: guildkey : %d\n",
        *(unsigned int*)(pb + 10));
    Packet_No_Cache noCache;
    *(unsigned int*)((char*)&noCache + 0xa) = 0;
    *(unsigned int*)((char*)&noCache + 0xe) =
        (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
    *(unsigned int*)((char*)&noCache + 0x12) = 1;
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
    *(unsigned int*)((char*)&noCache + 0x12) = 2;
    m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 10));
    if (guild != 0)
    {
        guild->DismissGuildMemberAndNotice((int)m_pclApp->Get_ServerGroup());
        m_pclApp->Get_GuildManager()->GuildDismiss(guild);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildDismiss() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildDismiss", 0x3c9);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildDismiss() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildDismiss() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x3cf);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildDismiss() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildChatMsg(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildChatMsg : 0 == m_pclApp")
    char* pb = (char*)pkt;
    if (*(unsigned int*)(pb + 0xe) == 0 || *(unsigned int*)(pb + 10) == 0 || pb[0x12] == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnNoticeGuildChatMsg : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 10));
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
    if (user != 0 && guild != 0)
    {
        guild->NoticeChatMsgToGuildMembers(*(unsigned int*)(pb + 10), pb + 0x13,
                                           (int)(unsigned char)pb[0x12],
                                           (const char*)user->GetCharName());
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildChatMsg", 0x405);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x40b);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildChatMsg() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildChatMsgHyperLink(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildChatMsgHyperLink : 0 == m_pclApp");
    char* pb = (char*)pkt;
    if (*(int*)(pb + 0xe) != 0 && *(int*)(pb + 0xa) != 0 && (unsigned char)pb[0x14b] != 0)
    {
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa));
        if (user != 0)
        {
            CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
            if (guild != 0)
            {
                const char* name = user->GetCharName();
                guild->NoticeChatMsgToGuildMembersHyperLink(
                    *(unsigned int*)(pb + 0xa), pb + 0x14c, (unsigned char)pb[0x14b],
                    (unsigned char)pb[0x12], (hyperlink_item_info*)(pb + 0x13), name);
            }
        }
    }
    else
    {
        throw CDNFException(
            "CPacketTranslater::OnNoticeGuildChatMsgHyperLink : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break : %s\n", e.what());
        CMyFileLog log("OnNoticeGuildChatMsgHyperLink", 0x430);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x436);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break\n");
    }
}

void CPacketTranslater::OnIncreaseGuildExp(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnIncreaseGuildExp : 0 == m_pclApp")
    char* pb = (char*)pkt;
    if (*(unsigned int*)(pb + 0xe) == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnIncreaseGuildExp : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
    if (guild != 0)
    {
        unsigned int oldExp = guild->GetGuildExp();
        unsigned int addExp = *(unsigned int*)(pb + 0x12);
        DNF_LOG_SCOPE_LINE(0x453,"./log/Guild",
            "GUILD EXP : char no(%d) guild key(%d), add exp(%d), guild exp(%d), book(%d)",
            *(unsigned int*)(pb + 10), *(unsigned int*)(pb + 0xe), addExp, oldExp,
            (int)(char)pb[0x17]);
        unsigned int max1 = m_pclApp->Get_GuildManager()->GetMaxGuildExp1();
        unsigned int max2 = m_pclApp->Get_GuildManager()->GetMaxGuildExp2();
        unsigned int level = guild->GetGuildLevel();
        int expLevel = m_pclApp->Get_GuildManager()->GetGuildLevelWithExp(oldExp);
        if (level == (unsigned int)expLevel)
        {
            if (pb[0x17] == 0)
            {
                unsigned int next = m_pclApp->Get_GuildManager()->GetGuildExpWithLevel(level + 1);
                unsigned int limit = next < max1 ? next : max1;
                guild->AddGuildExpUntilLimit(addExp, limit);
            }
            else if (pb[0x17] == 1 || pb[0x17] == 2)
            {
                guild->AddGuildExpUntilLimit(addExp, max2);
            }
            if (pb[0x16] != 0)
            {
                if (m_pclApp->Get_UserManager()->FindUser_CharNo(
                        *(unsigned int*)(pb + 0xa)) != 0)
                {
                    guild->SendGuildInfoToMembers(false);
                }
            }
        }
        else
        {
            CMyFileLog log2("OnIncreaseGuildExp", 0x462);
            log2("./log/Guild",
                 "OnIncreaseGuildExp : guild key(%d), curr guild exp(%d),lev(%d), next guild exp(%d), exp lev(%d)",
                 *(unsigned int*)(pb + 0xe), oldExp, level, oldExp + addExp, expLevel);
        }
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnIncreaseGuildExp() Exception Break : %s\n", e.what());
        CMyFileLog log("OnIncreaseGuildExp", 0x498);
        log("./log/Except", "CPacketTranslater::OnIncreaseGuildExp() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnIncreaseGuildExp() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x49e);
        log("./log/Except", "CPacketTranslater::OnIncreaseGuildExp() Exception Break\n");
    }
}

void CPacketTranslater::OnBuyGuildSkill(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnBuyGuildSkill : 0 == m_pclApp");
        unsigned int charNo = *(unsigned int*)((char*)pkt + 0xa);
        CUser* user;
        if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
        {
            throw CDNFException("CPacketTranslater::OnBuyGuildSkill : 0 == pclUser");
        }
        unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xe);
        if (guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnBuyGuildSkill : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild != 0)
        {
            bool isMaster = guild->IsGuildMaster(charNo) || guild->IsSubGuildMaster(charNo);
            if (isMaster &&
                guild->BuyGuildSkill(*(int*)((char*)pkt + 0x12), *(int*)((char*)pkt + 0x16),
                                     *(short*)((char*)pkt + 0x1a),
                                     *(unsigned int*)((char*)pkt + 0x1c)))
            {
                char saveFlag = (char)guild->GetDBSaveFlag();
                guild->SendGuildInfoToMembers(false);
                unsigned char group =
                    user->GetGameServer() != 0 ? user->GetGameServer()->GetGroupNo() : 0;
                guild->DBGuildSave(group, m_pclApp->Get_ServerHandler(), 1);
                if (saveFlag != 0)
                {
                    guild->EnableDBSaveFlag();
                    guild->DBGuildSave(group, m_pclApp->Get_ServerHandler(), 0);
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnBuyGuildSkill() Exception Break : %s\n", e.what());
        CMyFileLog log("OnBuyGuildSkill", 0x4f0);
        log("./log/Except",
            "CPacketTranslater::OnBuyGuildSkill() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnBuyGuildSkill() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x4f6);
        log("./log/Except", "CPacketTranslater::OnBuyGuildSkill() Exception Break\n");
    }
}

void CPacketTranslater::OnSetGuildMemberGradeFromWeb(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnSetGuildMemberGradeFromWeb : 0 == m_pclApp");
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xe);
    unsigned int masterCharNo = *(unsigned int*)((char*)pkt + 0xa);
    unsigned char newGrade = *(unsigned char*)((char*)pkt + 0x12);
    unsigned int targetCharNo = *(unsigned int*)((char*)pkt + 0x13);
    if (guildKey == 0)
    {
        DNF_LOG_SCOPE_LINE(0x515,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : 0 == packet->m_uGuildKey(%d)",
            (unsigned int)(guildKey == 0));
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x51a,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : no guild(%d)", masterCharNo);
        return;
    }
    if (!guild->IsGuildMaster(masterCharNo) && !guild->IsSubGuildMaster(masterCharNo))
    {
        DNF_LOG_SCOPE_LINE(0x520,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : no guild master(%d)", masterCharNo);
        return;
    }
    if (newGrade == 1)
    {
        DNF_LOG_SCOPE_LINE(0x526,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change guild master(%d)",
            (int)(char)newGrade);
        return;
    }
    if (newGrade == 2)
    {
        if (4 < guild->GetCurSubGuildMasterCnt())
        {
            DNF_LOG_SCOPE_LINE(0x52e,"./log/GuildModify",
                "CPacketTranslater::OnSetGuildMemberGradeFromWeb : over sub guild master count");
            return;
        }
        if (!guild->IsGuildMaster(masterCharNo))
        {
            DNF_LOG_SCOPE_LINE(0x535,"./log/GuildModify",
                "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change sub guild master except master");
            return;
        }
    }
    if (guild->IsGuildMaster(targetCharNo))
    {
        DNF_LOG_SCOPE_LINE(0x53d,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change guild master(%d)",
            (int)(char)newGrade);
        return;
    }
    if (targetCharNo == masterCharNo)
    {
        DNF_LOG_SCOPE_LINE(0x542,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change myself(%d)", targetCharNo);
        return;
    }
    if (newGrade == 2)
    {
        if (!guild->IsGuildMaster(masterCharNo))
        {
            DNF_LOG_SCOPE_LINE(0x54c,"./log/GuildModify",
                "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change sub guild master except guild master(%d)",
                masterCharNo);
            return;
        }
        guild->SetSubGuildMaster(targetCharNo, true);
        guild->SendGuildInfoToMembers(false);
    }
    else if (guild->IsSubGuildMaster(targetCharNo))
    {
        if (!guild->IsGuildMaster(masterCharNo))
        {
            DNF_LOG_SCOPE_LINE(0x55c,"./log/GuildModify",
                "CPacketTranslater::OnSetGuildMemberGradeFromWeb : can't change sub guild master except guild master(%d)",
                masterCharNo);
            return;
        }
        guild->SetSubGuildMaster(targetCharNo, false);
        guild->SendGuildInfoToMembers(false);
    }
    guild->ChangeUnconnectedGuildMemberGrade(targetCharNo, (int)(char)newGrade);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(targetCharNo);
    if (user != 0)
    {
        user->ChangeGuildMemberGrade(newGrade);
        Packet_Monitor_Notify_GuildMemberGrade notify;
        *(unsigned int*)((char*)&notify + 0xa) = user->GetIdByChannel();
        *(unsigned int*)((char*)&notify + 0xe) = user->GetUniqCharNo();
        *(unsigned char*)((char*)&notify + 0x12) = newGrade;
        user->SendToGameserver((char*)&notify, *(unsigned short*)((char*)&notify + 2));
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnSetGuildMemberGradeFromWeb", 0x578);
        log("./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x57d);
        log("./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break\n");
    }
}

void CPacketTranslater::OnSetGuildMemberGrade(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnSetGuildMemberGrade : 0 == m_pclApp")
    char* pb = (char*)pkt;
    CUser* caller;
    if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x5f3,"./log/GuildModify", "CPacketTranslater::OnSetGuildMemberGrade : no pclRequester(%d)",
            *(unsigned int*)(pb + 0xa));
        return;
    }
    Packet_Monitor_Set_Sub_Guild_Master_Reply reply;
    *(unsigned int*)((char*)&reply + 0xa) = caller->GetIdByChannel();
    *(unsigned int*)((char*)&reply + 0xe) = caller->GetUniqCharNo();
    int errCode = 0;
    memcpy((char*)&reply + 0x17, pb + 0x12, 0x1d);
    unsigned char grade = (unsigned char)pb[0x30];
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild = 0;
    if (guildKey == 0 ||
        (guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        errCode = 100;
        caller->SendToGameserver((char*)&reply, 0x3a);
        return;
    }
    bool authorized = guild->IsGuildMaster(*(unsigned int*)(pb + 0xa)) == 1 ||
                      caller->IsSubGuildMaster();
    if (!authorized)
    {
        errCode = 0x56;
    }
    if (grade == 1)
    {
        errCode = 0x58;
    }
    if (grade == 2)
    {
        if (guild->GetCurSubGuildMasterCnt() < 5)
        {
            if (guild->IsGuildMaster(*(unsigned int*)(pb + 0xa)) != 1)
            {
                errCode = 0x66;
            }
        }
        else
        {
            errCode = 0x59;
        }
    }
    CUser* target = m_pclApp->Get_UserManager()->FindUser_CharName(pb + 0x12);
    if (target == 0)
    {
        if (errCode == 0)
        {
            Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade dbPkt;
            unsigned char group = 0;
            if (caller->GetGameServer() != 0)
            {
                group = caller->GetGameServer()->GetGroupNo();
            }
            *(unsigned char*)((char*)&dbPkt + 0xa) = group;
            *(unsigned int*)((char*)&dbPkt + 0xb) = guildKey;
            *(unsigned char*)((char*)&dbPkt + 0xf) = grade;
            *(unsigned int*)((char*)&dbPkt + 0x10) = *(unsigned int*)(pb + 0xa);
            *(unsigned char*)((char*)&dbPkt + 0x14) =
                *(unsigned char*)((char*)caller->GetGuildMemDBInfo() + 0x15);
            memcpy((char*)&dbPkt + 0x15, pb + 0x12, 0x1d);
            m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
            return;
        }
    }
    else if (errCode == 0)
    {
        if (guild->IsGuildMaster(target->GetUniqCharNo()) != 1)
        {
            if (target == caller)
            {
                errCode = 0x55;
            }
            else
            {
                if (*(unsigned char*)((char*)target->GetGuildMemDBInfo() + 0x15) == grade)
                {
                    errCode = 0x65;
                }
                else
                {
                    if (grade == 2)
                    {
                        if (*(char*)((char*)caller->GetGuildMemDBInfo() + 0x15) == 1)
                        {
                            guild->SetSubGuildMaster(target->GetUniqCharNo(), true);
                            *(unsigned char*)((char*)target->GetGuildMemDBInfo() + 0x15) = 2;
                            guild->ChangeUnconnectedGuildMemberGrade(target->GetUniqCharNo(), 2);
                            guild->SendGuildInfoToMembers(false);
                        }
                        else
                        {
                            errCode = 0x66;
                        }
                    }
                    else
                    {
                        if (guild->IsSubGuildMaster(target->GetUniqCharNo()))
                        {
                            if (*(char*)((char*)caller->GetGuildMemDBInfo() + 0x15) == 1)
                            {
                                guild->SetSubGuildMaster(target->GetUniqCharNo(), false);
                                guild->SendGuildInfoToMembers(false);
                            }
                            else
                            {
                                errCode = 0x66;
                            }
                        }
                    }
                    if (errCode == 0)
                    {
                        target->ChangeGuildMemberGrade(grade);
                        Packet_Monitor_Notify_GuildMemberGrade notify;
                        *(unsigned char*)((char*)&notify + 0x12) = grade;
                        *(unsigned int*)((char*)&notify + 0xa) = target->GetUniqCharNo();
                        *(unsigned int*)((char*)&notify + 0xe) = target->GetIdByChannel();
                        target->SendToGameserver((char*)&notify, 0x13);
                        if (target->GetGameServer() == 0)
                        {
                            errCode = 3;
                        }
                        else
                        {
                            guild->DBGuildMemberSave(target, target->GetGameServer()->GetGroupNo(),
                                                     m_pclApp->Get_ServerHandler(), 3);
                        }
                    }
                }
            }
        }
        else
        {
            errCode = 0x57;
        }
    }
    *(int*)((char*)&reply + 0x12) = errCode;
    caller->SendToGameserver((char*)&reply, 0x3a);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnSetGuildMemberGrade", 0x680);
        log("./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x685);
        log("./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break\n");
    }
}

void CPacketTranslater::OnUpdateChangableCharInfo(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnUpdateChangableCharInfo : 0 == m_pclApp");
    char* pb = (char*)pkt;
    CUser* user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa));
    if (user != 0)
    {
        user->SetUserChangableInfo(*(short*)(pb + 0xf), (char)pb[0x11]);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
        CMyFileLog log("OnUpdateChangableCharInfo", 0x6a4);
        log("./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x6aa);
        log("./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break\n");
    }
}

void CPacketTranslater::OnLoadGuildWarEnterableGuilds(PacketHeader* pkt)
{
    (void)pkt;
    char buf[0x30];
    (void)buf;
}

void CPacketTranslater::OnNoticeGuildWarPointChange(PacketHeader* pkt)
{
    try
    {
    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeGuildWarStart : 0 == m_pclApp");
    }
    CGuildWar* war = m_pclApp->Get_GuildManager()->GetGuildWar();
    if (war->IsGuildWarEventOn() == 1)
    {
        char* pb = (char*)pkt;
        if (war->IsGuildWarEnterableGuild(*(unsigned int*)(pb + 0xa)) == 1)
        {
            war->AddGuildWarPoint(*(unsigned int*)(pb + 0xa), (int)(char)pb[0xe]);
        }
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildWarEnd Exception Break : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0x705);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildWarEnd Exception Break");
        CMyFileLog log(__FUNCTION__, 0x70a);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildWarStart(PacketHeader* pkt)
{
    (void)pkt;
    char* pb;
    (void)pb;
}

void CPacketTranslater::OnNoticeGuildWarEnd(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildWarEnd : 0 == m_pclApp");
    CGuildWar* war = m_pclApp->Get_GuildManager()->GetGuildWar();
    if (war->IsGuildWarEventOn() == 1)
    {
        char* pb = (char*)pkt;
        war->SetGuildWarEvent(false, (unsigned char)pb[10]);
        if (war->Rank() != 1)
        {
            throw CDNFException(
                "CPacketTranslater::OnNoticeGuildWarEnd : false == Rank() : May be m_vtGuildWarInfo is empty!");
        }
        if (war->SameRankWork() != 1)
        {
            throw CDNFException(
                "CPacketTranslater::OnNoticeGuildWarEnd : false == SameRankWork() : May be m_vtGuildWarInfo is empty!");
        }
        unsigned char group = m_pclApp->Get_ServerGroup();
        Packet_Notice_DB_Guild_War_End dbPkt;
        *(unsigned char*)((char*)&dbPkt + 0xa) = group;
        war->GetGuildWarInfo((unsigned int*)((char*)&dbPkt + 0xb),
                             (unsigned int*)((char*)&dbPkt + 0x33),
                             (unsigned short*)((char*)&dbPkt + 0x5b));
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
        Packet_Monitor_Event_End monPkt;
        *(unsigned int*)((char*)&monPkt + 0xa) = 9;
        OnEventEnd(&monPkt);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnNoticeGuildWarEnd", 0x76d);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x772);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildWarInfo(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildWarInfo : 0 == m_pclApp");
    CGuildWar* war = m_pclApp->Get_GuildManager()->GetGuildWar();
    char* pb = (char*)pkt;
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x78d,"./log/Except",
            "[USER] CPacketTranslater::OnRequestGuildWarInfo : pclUser == 0!\tchar id(%d)",
            *(unsigned int*)(pb + 0xe));
    }
    else
    {
        war->GetGuildWarInfo((ST_Guild_War_Rank_Info*)(pb + 0x12));
        user->SendToGameserver((char*)pb, 0x15c);
    }
}

void CPacketTranslater::OnCharacterDelete(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnCharacterDelete : 0 == m_pclApp");
    char* pb = (char*)pkt;
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x7bb,"./log/GuildModify",
            "CPacketTranslater::OnCharacterDelete : 0 == pclGuild, Char Key = %d (Maybe Requester was logout)",
            *(unsigned int*)(pb + 0xe));
    }
    else
    {
        ST_Notice_Guild_Secede notice;
        *(unsigned int*)((char*)&notice + 0) = *(unsigned int*)(pb + 0x12);
        *(unsigned int*)((char*)&notice + 8) = *(unsigned int*)(pb + 0xe);
        *(unsigned short*)((char*)&notice + 0xc) = 1;
        memcpy((char*)&notice + 0xe, guild->GetGuildName(), 0x16);
        m_pclApp->Get_GuildManager()->GuildSecede(*(unsigned int*)(pb + 0x12), notice);
        guild->SendGuildInfoToMembers(false);
        m_pclApp->Get_UserManager()->DeleteBlackUserOnCharacDelete(*(unsigned int*)(pb + 0xe));
    }
}

void CPacketTranslater::OnCallGuildMembers(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildMembers : 0 == m_pclApp")
    char* pb = (char*)pkt;
    try
    {
        if (*(unsigned int*)(pb + 0xe) == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : packet->m_uGuildKey == 0");
        }
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 10));
        if (user == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : 0 == pclUser");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
        if (guild == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : 0 == pclGuild");
        }
        guild->ReplyGuildMembers(user);
    }
    catch (std::exception& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        CMyFileLog log("OnCallGuildMembers", 0x812);
        log("./log/Except",
            "CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x818);
        log("./log/Except",
            "CPacketTranslater::OnCallGuildMembers() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildAllMembers(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnCallGuildAllMembers : 0 == m_pclApp")
        char* pb = (char*)pkt;
        if (*(unsigned int*)(pb + 0xe) == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnCallGuildAllMembers : packet->m_uGuildKey == 0");
        }
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 10));
        if (user != 0)
        {
            CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
            if (guild != 0)
            {
                guild->CallGuildAllMembersProxy(user, m_pclApp->Get_ServerHandler());
            }
            else
            {
                throw CDNFException(
                    "CPacketTranslater::OnCallGuildAllMembers : 0 == pclGuild");
            }
        }
        else
        {
            throw CDNFException("CPacketTranslater::OnCallGuildAllMembers : 0 == pclUser");
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        CMyFileLog log("OnCallGuildAllMembers", 0x851);
        log("./log/Except",
            "CPacketTranslater::OnCallGuildAllMembers() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x857);
        log("./log/Except", "CPacketTranslater::OnCallGuildAllMembers() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildAllMembers(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDBReplyGuildAllMembers : 0 == m_pclApp");
        char* pb = (char*)pkt;
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        if (guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyGuildAllMembers : packet->m_uGuildKey == 0");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyGuildAllMembers : 0 == pclGuild ,the guild is out");
        }
        guild->LoadGuildAllMembersProxy((STGuildMemberProxy*)(pb + 0x14),
                                        (unsigned char)pb[0x12], (unsigned char)pb[0x13]);
        CUser* user = 0;
        if ((unsigned char)pb[0x12] == 2)
        {
            user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe));
            if (user != 0)
            {
                guild->SetGuildDBFlag(0x10);
                guild->ReplyGuildAllMembers(user);
                DNF_LOG_SCOPE_LINE(0x87b,"./log/GuildModify", "OnDBReplyGuildAllMembers gKey(%d), cKey(%d)", guildKey,
                    *(unsigned int*)(pb + 0xe));
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        CMyFileLog log("OnDBReplyGuildAllMembers", 0x886);
        log("./log/Except",
            "CPacketTranslater::OnDBReplyGuildAllMembers() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x88c);
        log("./log/Except", "CPacketTranslater::OnDBReplyGuildAllMembers() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyUnconnGuildMember(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDBReplyUnconnGuildMember : 0 == m_pclApp");
        char* pb = (char*)pkt;
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        if (guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyUnconnGuildMember : packet->m_uGuildKey == 0");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild != 0)
        {
            guild->LoadGuildOneMemberProxy(*(STGuildMemberProxy*)(pb + 0x12));
            return;
        }
        throw CDNFException(
            "CPacketTranslater::OnDBReplyUnconnGuildMember : 0 == pclGuild ,the guild is out");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        CMyFileLog log("OnDBReplyUnconnGuildMember", 0x8b5);
        log("./log/Except",
            "CPacketTranslater::OnDBReplyUnconnGuildMember() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        CMyFileLog log(__FUNCTION__, 0x8bb);
        log("./log/Except", "CPacketTranslater::OnDBReplyUnconnGuildMember() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildInvite(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnMonitorSendGuildLetter : 0 == m_pclApp");
    Packet_Guild_Call_Guild_Invite_To_Caller callerPkt;
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xe);
    unsigned int charNo = *(unsigned int*)((char*)pkt + 0xa);
    if (guildKey == 0)
    {
        throw CDNFException("CPacketTranslater::OnCallGuildInvite : packet->m_uGuildKey == 0");
    }
    CUser* caller;
    if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x8db,"./log/Except",
            "CPacketTranslater::OnCallGuildInvite : 0 == pclCaller, Char Key = %d", charNo);
        return;
    }
    *(unsigned int*)((char*)&callerPkt + 0xa) = charNo;
    *(unsigned int*)((char*)&callerPkt + 0xe) = caller->GetIdByChannel();
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x8e4,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : 0 == pclGuild, Guild Key = %d", guildKey);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x22;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    if (guild->IsSetGuildDBFlag(4) != 1)
    {
        DNF_LOG_SCOPE_LINE(0x8eb,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
            guildKey);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x22;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    if (!guild->IsGuildMaster(charNo) && !guild->IsSubGuildMaster(charNo) &&
        *(char*)((char*)caller->GetGuildMemDBInfo() + 0x15) != 0x03)
    {
        DNF_LOG_SCOPE_LINE(0x8f4,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
            guildKey, charNo);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x24;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    if (300 < (guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff) + 1)
    {
        DNF_LOG_SCOPE_LINE(0x8fd,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : cnt(%d), Guild Key = %d",
            guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff, guildKey);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x26;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    CUser* invited =
        m_pclApp->Get_UserManager()->FindUser_CharName(std::string((char*)pkt + 0x12));
    if (invited == 0)
    {
        DNF_LOG_SCOPE_LINE(0x905,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : 0 == pclInvitedUser, Char Key = %d", charNo);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 3;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    if (caller->IsBlackUser(invited->GetUniqCharNo()) != 0 ||
        invited->IsBlackUser(caller->GetUniqCharNo()) != 0)
    {
        DNF_LOG_SCOPE_LINE(0x90d,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : 0 == pclInvitedUser, Char Key = %d", charNo);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x4d;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        return;
    }
    if (invited->GetGuildKey() == 0)
    {
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
        invited->SetGuildInviteFact(guild->GetGuildKey(), caller->GetUniqCharNo(), 1);
        Packet_Guild_Call_Guild_Invite_To_Invited invitedPkt;
        *(unsigned int*)((char*)&invitedPkt + 0xa) = invited->GetUniqCharNo();
        *(unsigned int*)((char*)&invitedPkt + 0xe) = invited->GetIdByChannel();
        memcpy((char*)&invitedPkt + 0x12, caller->GetCharName(), 0x1d);
        memcpy((char*)&invitedPkt + 0x2f, guild->GetGuildName(), 0x16);
        invited->SendToGameserver((char*)&invitedPkt, 0x47);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x916,"./log/GuildModify",
            "CPacketTranslater::OnCallGuildInvite : 0 != pclInvitedUser.GetGuildKey(), Char Key = %d",
            charNo);
        *(unsigned int*)((char*)&callerPkt + 0x12) = 0x27;
        caller->SendToGameserver((char*)&callerPkt, 0x16);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnCallGuildInvite", 0x92d);
        log("./log/Except", "CPacketTranslater::OnCallGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x932);
        log("./log/Except", "CPacketTranslater::OnCallGuildInvite() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyGuildInvite(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnReplyGuildInvite : 0 == m_pclApp");
    unsigned int charNo = *(unsigned int*)((char*)pkt + 0xa);
    CUser* invited;
    if ((invited = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x94d,"./log/Except",
            "CPacketTranslater::OnReplyGuildInvite : 0 == pclInvitedUser, Char Key = %d", charNo);
        return;
    }
    Packet_Guild_Reply_Guild_Invite_To_Invited invitedPkt;
    *(unsigned int*)((char*)&invitedPkt + 0xa) = charNo;
    *(unsigned int*)((char*)&invitedPkt + 0xe) = invited->GetIdByChannel();
    int callerId = (int)invited->GetGuildInviteCallerId();
    int guildId = (int)invited->GetGuildInviteGuildId();
    if (callerId == 0 || guildId == 0)
    {
        *(unsigned int*)((char*)&invitedPkt + 0x12) = 0x67;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild((unsigned int)guildId)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x962,"./log/GuildModify",
            "CPacketTranslater::OnReplyGuildInvite : 0 == pclGuild, Guild Key = %d", guildId);
        *(unsigned int*)((char*)&invitedPkt + 0x12) = 0x22;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        CUser* caller;
        if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo((unsigned int)callerId)) != 0)
        {
            Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
            *(int*)((char*)&callerPkt + 0xa) = callerId;
            *(unsigned int*)((char*)&callerPkt + 0xe) = caller->GetIdByChannel();
            *(unsigned int*)((char*)&callerPkt + 0x12) = 0x22;
            caller->SendToGameserver((char*)&callerPkt, 0x34);
        }
        return;
    }
    if ((guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff) + 1 < 0x12d)
    {
        if (*(char*)((char*)pkt + 0xe) == 1)
        {
            Packet_DBMW_Save_Guild_Join joinPkt;
            unsigned int dbid = invited->GetDBID();
            unsigned char group = 0;
            if (invited->GetGameServer() != 0)
            {
                group = invited->GetGameServer()->GetGroupNo();
            }
            *(unsigned int*)((char*)&joinPkt + 0xa) =
                ((unsigned int)group << 24) | ((unsigned int)guildId & 0xffffff);
            *(unsigned char*)((char*)&joinPkt + 0xe) = (unsigned char)((unsigned int)guildId >> 24);
            *(unsigned char*)((char*)&joinPkt + 0xf) = (unsigned char)(dbid & 0xff);
            *(unsigned short*)((char*)&joinPkt + 0x10) = (unsigned short)((dbid >> 8) & 0xffff);
            *(unsigned char*)((char*)&joinPkt + 0x12) = (unsigned char)(dbid >> 24);
            *(unsigned char*)((char*)&joinPkt + 0x13) = (unsigned char)((unsigned int)callerId & 0xff);
            *(unsigned short*)((char*)&joinPkt + 0x14) =
                (unsigned short)(((unsigned int)callerId >> 8) & 0xffff);
            *(unsigned char*)((char*)&joinPkt + 0x16) = (unsigned char)((unsigned int)callerId >> 24);
            *(unsigned int*)((char*)&joinPkt + 0x17) = invited->GetUniqCharNo();
            memcpy((char*)&joinPkt + 0x1b, invited->GetCharName(), 0x1d);
            *(unsigned char*)((char*)&joinPkt + 0x38) = invited->GetJob();
            *(unsigned char*)((char*)&joinPkt + 0x39) = invited->GetGrowthType();
            *(unsigned char*)((char*)&joinPkt + 0x3a) = invited->GetLevel();
            *(unsigned char*)((char*)&joinPkt + 0x3b) = invited->GetSex();
            memcpy((char*)&joinPkt + 0x3c, invited->GetSsn(), 2);
            m_pclApp->Get_ServerHandler()->GetDBServer()->SendToServer((char*)&joinPkt, 0x40);
        }
        else
        {
            CUser* caller;
            if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo((unsigned int)callerId)) != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
                *(int*)((char*)&callerPkt + 0xa) = callerId;
                *(unsigned int*)((char*)&callerPkt + 0xe) = caller->GetIdByChannel();
                *(unsigned int*)((char*)&callerPkt + 0x12) =
                    (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe);
                memcpy((char*)&callerPkt + 0x16, invited->GetCharName(), 0x1d);
                caller->SendToGameserver((char*)&callerPkt, 0x34);
            }
        }
        invited->SetGuildInviteFact(0, 0, 0xff);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x977,"./log/GuildModify",
            "CPacketTranslater::OnReplyGuildInvite : cnt(%d), Guild Key = %d",
            guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff, guildId);
        *(unsigned int*)((char*)&invitedPkt + 0x12) = 0x26;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        CUser* caller;
        if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo((unsigned int)callerId)) != 0)
        {
            Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
            *(int*)((char*)&callerPkt + 0xa) = callerId;
            *(unsigned int*)((char*)&callerPkt + 0xe) = caller->GetIdByChannel();
            *(unsigned int*)((char*)&callerPkt + 0x12) = 0x26;
            caller->SendToGameserver((char*)&callerPkt, 0x34);
        }
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnReplyGuildInvite", 0x9b8);
        log("./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x9bd);
        log("./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWGuildJoin(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnReplyGuildInvite : 0 == m_pclApp");
        char* pb = (char*)pkt;
        CUser* joinUser;
        if ((joinUser = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0x12))) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x9d8, "./log/GuildModify", "CPacketTranslater::OnDBMWGuildJoin() 0 == pclJoinUser");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xa));
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x9dc, "./log/GuildModify", "CPacketTranslater::OnDBMWGuildJoin() 0 == pclGuild");
        }
        if (*(int*)(pb + 0x16) == 0)
        {
            GuildJoin(guild, joinUser, *(unsigned int*)(pb + 0xe));
        }
        else
        {
            CUser* caller;
            if ((caller = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe))) != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
                *(unsigned int*)((char*)&callerPkt + 0xa) = *(unsigned int*)(pb + 0xe);
                *(unsigned int*)((char*)&callerPkt + 0xe) = caller->GetIdByChannel();
                *(unsigned int*)((char*)&callerPkt + 0x12) = *(unsigned int*)(pb + 0x16);
                caller->SendToGameserver((char*)&callerPkt, 0x34);
            }
            if (joinUser != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Invited invitedPkt;
                *(unsigned int*)((char*)&invitedPkt + 0xa) = *(unsigned int*)(pb + 0x12);
                *(unsigned int*)((char*)&invitedPkt + 0xe) = joinUser->GetIdByChannel();
                *(unsigned int*)((char*)&invitedPkt + 0x12) = *(unsigned int*)(pb + 0x16);
                joinUser->SendToGameserver((char*)&invitedPkt, 0x16);
            }
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBMWGuildJoin", 0xa2c);
        log("./log/Except",
            "CPacketTranslater::OnReplyGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xa31);
        log("./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break\n");
    }
}

void CPacketTranslater::GuildJoin(CGuild* guild, CUser* user, unsigned int dbid)
{
    if (guild != 0 && user != 0)
    {
        user->SetGuildMemFlag(4);
        user->SendSetGuildKeyToUser(guild->GetGuildKey(), user->GetUniqCharNo());
        if (guild->InsertGuildMember(user->GetUniqCharNo(), user) != 1)
        {
            DNF_LOG_SCOPE_LINE(0xa41, "./log/GuildMember", "[INSERT_ERR]\tChar Key : %d\t Insert False\n",
                user->GetUniqCharNo());
        }
        if (guild->IsSetGuildDBFlag(4) != 0)
        {
            if (guild->LoadGuildOneMemberProxy(user) != 1)
            {
                guild->IncTotalCnt_Of_GuildDBInfo();
            }
            DNF_LOG_SCOPE_LINE(0xa4a,"./log/GuildModify", "GUILD JOIN guild(%s) char(%s)",
                guild->GetGuildName(), user->GetCharName());
            ST_Notice_Guild_Enter info;
            memset(&info, 0, sizeof(info));
            *(unsigned int*)((char*)&info + 0) = guild->GetGuildKey();
            *(unsigned int*)((char*)&info + 4) = user->GetDBID();
            *(unsigned int*)((char*)&info + 8) = user->GetUniqCharNo();
            memcpy((char*)&info + 0x23, user->GetCharName(), 0x1d);
            memcpy((char*)&info + 0xc, guild->GetGuildName(), 0x16);
            guild->NoticeEnterToGuildMember((char*)&info);
        }
        user->MakeGameServerSendUserInfoPacket(guild->GetGuildKey());
        guild->SendGuildInfoToMembers(false);
    }
}

void CPacketTranslater::GuildJoin(CGuild* guild, STGuildJoinInfo* joinInfo, unsigned int dbid)
{
    if (guild != 0)
    {
        if (guild->IsSetGuildDBFlag(4) != 0 || guild->IsSetGuildDBFlag(0x10) != 0)
        {
            STGuildMemberProxy proxy;
            memset(&proxy, 0, sizeof(proxy));
            *(unsigned int*)((char*)&proxy + 0x10) = *(unsigned int*)((char*)joinInfo + 0x10);
            memcpy((char*)&proxy + 0x14, (char*)joinInfo + 0x14, 0x1d);
            if (guild->LoadGuildOneMemberProxy(proxy) != 1)
            {
                guild->IncTotalCnt_Of_GuildDBInfo();
            }
            DNF_LOG_SCOPE_LINE(0xa71,"./log/GuildModify", "GUILD JOIN guild(%s) char(%s)",
                guild->GetGuildName(), (char*)joinInfo + 0x14);
        }
        ST_Notice_Guild_Enter info;
        memset(&info, 0, sizeof(info));
        *(unsigned int*)((char*)&info + 0) = guild->GetGuildKey();
        *(unsigned int*)((char*)&info + 4) = *(unsigned int*)((char*)joinInfo + 8);
        *(unsigned int*)((char*)&info + 8) = dbid;
        memcpy((char*)&info + 0x23, (char*)joinInfo + 0x14, 0x1d);
        memcpy((char*)&info + 0xc, guild->GetGuildName(), 0x16);
        guild->NoticeEnterToGuildMember((char*)&info);
        guild->SendGuildInfoToMembers(false);
    }
}

void CPacketTranslater::OnMonitorSendGuildLetter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnMonitorSendGuildLetter : 0 == m_pclApp");
    char* pb = (char*)pkt;
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    unsigned int guildId = *(unsigned int*)(pb + 0xe);
    const char* msg = pb + 0x12;
    Packet_Monitor_Reply_Guild_Mail reply;
    {
        DNF_LOG_SCOPE_LINE(0xa98, "./log/GuildLetter", "charNo(%d),guildId(%d),msg(%s)", charNo, guildId, msg);
    }
    if (guildId == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnMonitorSendGuildLetter : packet->m_uGuildKey == 0");
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xaa0,"./log/GuildModify",
            "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclUser, Char Key = %d", charNo);
        return;
    }
    *(unsigned int*)((char*)&reply + 0xa) = charNo;
    *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildId);
    if (guild != 0)
    {
        if (!(guild->IsGuildMaster(charNo) || guild->IsSubGuildMaster(charNo)))
        {
            DNF_LOG_SCOPE_LINE(0xab0,"./log/GuildModify",
                "CPacketTranslater::OnMonitorSendGuildLetter : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
                guildId, charNo);
            *(unsigned char*)((char*)&reply + 0x12) = 0x24;
            user->SendToGameserver((char*)&reply, 0x13);
        }
        else
        {
            CServerInterface* gs = user->GetGameServer();
            if (gs == 0)
            {
                DNF_LOG_SCOPE_LINE(0xab8,"./log/GuildModify",
                    "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclUser->GetGameServer(), g(%d), c(%d)",
                    guildId, charNo);
                *(unsigned char*)((char*)&reply + 0x12) = 1;
                user->SendToGameserver((char*)&reply, 0x13);
            }
            else
            {
                SendPacketGuildMail(gs->GetGroupNo(), charNo, guildId, "", msg, 0xffffffff);
            }
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xaa8,"./log/GuildModify",
            "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclGuild, Guild Key = %d", guildId);
        *(unsigned char*)((char*)&reply + 0x12) = 0x22;
        user->SendToGameserver((char*)&reply, 0x13);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnMonitorSendGuildLetter", 0xac2);
        log("./log/Except", "CPacketTranslater::OnMonitorSendGuildLetter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xac7);
        log("./log/Except", "CPacketTranslater::OnMonitorSendGuildLetter() Exception Break\n");
    }
}

void CPacketTranslater::SendPacketGuildMail(unsigned char group, unsigned int charNo,
                                            unsigned int guildKey, const char* title,
                                            const char* content,
                                            unsigned int param)
{
    Packet_DBMW_Send_Guild_Mail pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = charNo;
    *(unsigned int*)((char*)&pkt + 0xf) = guildKey;
    *(unsigned char*)((char*)&pkt + 0x12) = group;
    size_t len = strlen(content);
    memcpy((char*)&pkt + 0x13, content, len < 0x100 ? len : 0xff);
    len = strlen(title);
    memcpy((char*)&pkt + 0x113, title, len < 0x11 ? len : 0x10);
    *(unsigned int*)((char*)&pkt + 0x124) = param;
    m_pclApp->Get_ServerHandler()->SendToDB(&pkt);
}

void CPacketTranslater::OnDBMWReplySendGuildLetter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnDBMWReplySendGuildLetter : 0 == m_pclApp");
    char* pb = (char*)pkt;
    Packet_Monitor_Reply_Guild_Mail reply;
    if (*(unsigned int*)(pb + 0xe) == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnDBMWReplySendGuildLetter : packet->m_uGuildKey == 0");
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb0e,"./log/GuildModify",
            "CPacketTranslater::OnDBMWReplySendGuildLetter : 0 == pclUser, Char Key = %d", charNo);
    }
    else
    {
        *(unsigned int*)((char*)&reply + 0xa) = charNo;
        *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
        *(unsigned char*)((char*)&reply + 0x12) = *(unsigned char*)(pb + 0x12);
        user->SendToGameserver((char*)&reply, 0x13);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBMWReplySendGuildLetter", 0xb19);
        log("./log/Except", "CPacketTranslater::OnDBMWReplySendGuildLetter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xb1e);
        log("./log/Except", "CPacketTranslater::OnDBMWReplySendGuildLetter() Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildSecede(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildSecede : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Secede resp;
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int requesterCharNo = *(unsigned int*)((char*)pkt + 0xe);
    if (guildKey == 0)
    {
        throw CDNFException("CPacketTranslater::OnCallGuildInvite : packet->m_uGuildKey == 0");
    }
    CUser* requester;
    if ((requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb3d,"./log/Except",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclRequester, Char Key = %d",
            requesterCharNo);
        return;
    }
    *(unsigned int*)((char*)&resp + 0xa) = requesterCharNo;
    *(unsigned int*)((char*)&resp + 0xe) = requester->GetIdByChannel();
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild != 0)
    {
        if (guild->IsSetGuildDBFlag(4) != 1)
        {
            DNF_LOG_SCOPE_LINE(0xb4e,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildSecede : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
                guildKey);
            *(unsigned int*)((char*)&resp + 0x12) = 0x22;
            requester->SendToGameserver((char*)&resp, 0x52);
            return;
        }
        Packet_DB_Request_Guild_Secede dbPkt;
        *(unsigned int*)((char*)&dbPkt + 0xa) = guildKey;
        *(unsigned int*)((char*)&dbPkt + 0xe) = requesterCharNo;
        size_t nameLen = *(size_t*)((char*)pkt + 0x12);
        *(unsigned char*)((char*)&dbPkt + 0x13) = (unsigned char)nameLen;
        memcpy((char*)&dbPkt + 0x17, (char*)pkt + 0x16, nameLen);
        (void)guild->GetMasterId();
        if (*(int*)((char*)pkt + 0x12) == 0)
        {
            *(unsigned char*)((char*)&dbPkt + 0x12) = 1;
            const char* name = requester->GetCharName();
            memcpy((char*)&dbPkt + 0x17, name, strlen(name));
        }
        else
        {
            if (!guild->IsGuildMaster(requesterCharNo) &&
                !guild->IsSubGuildMaster(requesterCharNo))
            {
                DNF_LOG_SCOPE_LINE(0xb60,"./log/GuildModify",
                    "CPacketTranslater::OnRequestGuildSecede : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
                    guildKey, requesterCharNo);
                *(unsigned int*)((char*)&resp + 0x12) = 0x24;
                requester->SendToGameserver((char*)&resp, 0x52);
                return;
            }
            *(unsigned char*)((char*)&dbPkt + 0x12) = 2;
        }
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
        return;
    }
    DNF_LOG_SCOPE_LINE(0xb46,"./log/GuildModify",
        "CPacketTranslater::OnRequestGuildSecede : 0 == pclGuild, Guild Key = %d", guildKey);
    *(unsigned int*)((char*)&resp + 0x12) = 0x22;
    requester->SendToGameserver((char*)&resp, 0x52);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnRequestGuildSecede", 0xb7e);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xb83);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildSecede(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildSecede : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Secede resp;
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int requesterCharNo = *(unsigned int*)((char*)pkt + 0xe);
    unsigned int targetCharNo = *(unsigned int*)((char*)pkt + 0x12);
    int secedeType = *(int*)((char*)pkt + 0x16);
    unsigned char secedeFlag = *(unsigned char*)((char*)pkt + 0x1a);
    unsigned int dbid = *(unsigned int*)((char*)pkt + 0x3d);
    CUser* requester;
    if ((requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb9e,"./log/Except",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclCaller, Char Key = %d",
            requesterCharNo);
        return;
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xba3,"./log/GuildModify",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclGuild, Char Key = %d (Maybe Requester was logout)",
            requesterCharNo);
        return;
    }
    *(unsigned int*)((char*)&resp + 0xa) = guildKey;
    *(unsigned short*)((char*)&resp + 0xe) = guild->GetTotalCnt_Of_GuildDBInfo();
    *(unsigned int*)((char*)&resp + 0x10) = requesterCharNo;
    *(unsigned int*)((char*)&resp + 0x14) = requester->GetIdByChannel();
    *(unsigned char*)((char*)&resp + 0x18) = secedeFlag;
    if (secedeType == 0 || secedeType == 1)
    {
        if (requesterCharNo != targetCharNo)
        {
            CUser* target;
            if ((target = m_pclApp->Get_UserManager()->FindUser_CharNo(targetCharNo)) == 0)
            {
                Packet_No_Cache noCache;
                *(unsigned int*)((char*)&noCache + 0xa) = dbid;
                *(unsigned int*)((char*)&noCache + 0xe) =
                    (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
                *(unsigned int*)((char*)&noCache + 0x12) = 1;
                m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
                *(unsigned int*)((char*)&noCache + 0x12) = 2;
                m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);
                Packet_DBMW_Query_Msg query;
                *(unsigned int*)((char*)&query + 0xa) = 2;
                *(unsigned int*)((char*)&query + 0xe) = 0x4f00;
                char sql[4097];
                sprintf(sql, "upDate charac_info set guild_secede = 1 where charac_no = %u",
                        targetCharNo);
                m_pclApp->Get_ServerHandler()->SendToDB(&query);
            }
            else
            {
                Packet_Guild_Exp_Book_Delete expDel;
                *(unsigned int*)((char*)&expDel + 0xa) = target->GetIdByChannel();
                *(unsigned int*)((char*)&expDel + 0xe) = target->GetUniqCharNo();
                *(unsigned int*)((char*)&expDel + 0x12) =
                    (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
                *(unsigned int*)((char*)&expDel + 0x16) = target->GetDBID();
                target->SendTcpGameserver((PacketHeader*)&expDel);
            }
        }
        ST_Notice_Guild_Secede notice;
        memset(&notice, 0, sizeof(notice));
        *(unsigned int*)((char*)&notice + 0) = guildKey;
        *(unsigned int*)((char*)&notice + 4) = targetCharNo;
        *(unsigned short*)((char*)&notice + 8) = (unsigned short)(signed char)secedeFlag;
        memcpy((char*)&notice + 0xe, (char*)pkt + 0x1f, 0x1d);
        memcpy((char*)&notice + 0x2b, guild->GetGuildName(), 0x16);
        guild->NoticeSecedeToGuildMember((char*)&notice);
        m_pclApp->Get_GuildManager()->GuildSecede(guildKey, notice);
        guild->SendGuildInfoToMembers(false);
        if (guild != 0 && secedeType == 1)
        {
            if (guild->GetTotalCnt_Of_GuildDBInfo() != 0)
            {
                DNF_LOG_SCOPE_LINE(0xbf3,"./log/GuildModify",
                    "CPacketTranslater::OnDBReplyGuildSecede : Guild(%d) Deleted, rest member count(%d)",
                    guild->GetGuildKey(), guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff);
            }
            m_pclApp->Get_GuildManager()->GuildDismiss(guild);
        }
        *(unsigned int*)((char*)&resp + 0x1a) = 0;
        requester->SendToGameserver((char*)&resp, 0x52);
    }
    else
    {
        *(unsigned int*)((char*)&resp + 0x1a) = (unsigned int)secedeType;
        requester->SendToGameserver((char*)&resp, 0x52);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBReplyGuildSecede", 0xbff);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xc04);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildMasterDelegate(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildMasterDelegate : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Master_Delegate resp;
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int requesterCharNo = *(unsigned int*)((char*)pkt + 0xe);
    if (guildKey != 0)
    {
        CUser* requester;
        if ((requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterCharNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc23,"./log/Except",
                "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclRequester, Char Key = %d",
                requesterCharNo);
            return;
        }
        *(unsigned int*)((char*)&resp + 0xa) = requesterCharNo;
        *(unsigned int*)((char*)&resp + 0xe) = requester->GetIdByChannel();
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc2c,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclGuild, Guild Key = %d",
                guildKey);
            *(unsigned int*)((char*)&resp + 0x12) = 0x22;
            requester->SendToGameserver((char*)&resp, 0x16);
            return;
        }
        if (guild->IsSetGuildDBFlag(4) == 1)
        {
            if (guild->IsGuildMaster(requesterCharNo))
            {
                CUser* delegatee = m_pclApp->Get_UserManager()->FindUser_CharName(
                    std::string((char*)pkt + 0x12));
                if (delegatee != 0 && !guild->IsSubGuildMaster(delegatee->GetUniqCharNo()))
                {
                    DNF_LOG_SCOPE_LINE(0xc49,"./log/GuildModify",
                        "CPacketTranslater::OnRequestGuildMasterDelegate : Delegatee is Not Sub Guild Master(%d)(%d)",
                        guildKey, delegatee->GetUniqCharNo());
                    *(unsigned int*)((char*)&resp + 0x12) = 0x56;
                    requester->SendToGameserver((char*)&resp, 0x16);
                    return;
                }
                if (requester->GetGameServer() != 0)
                {
                    Packet_DB_Request_Guild_Master_Delegate dbPkt;
                    *(unsigned int*)((char*)&dbPkt + 0xa) = guildKey;
                    *(unsigned int*)((char*)&dbPkt + 0xe) = requesterCharNo;
                    *(unsigned char*)((char*)&dbPkt + 0x12) =
                        requester->GetGameServer()->GetGroupNo();
                    memcpy((char*)&dbPkt + 0x13, (char*)pkt + 0x12, 0x1d);
                    m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0xc3d,"./log/GuildModify",
                    "CPacketTranslater::OnRequestGuildMasterDelegate : Requester is Not Guild Master(%d)(%d)",
                    guildKey, requesterCharNo);
                *(unsigned int*)((char*)&resp + 0x12) = 0x24;
                requester->SendToGameserver((char*)&resp, 0x16);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xc34,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildMasterDelegate : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
                guildKey);
            *(unsigned int*)((char*)&resp + 0x12) = 0x22;
            requester->SendToGameserver((char*)&resp, 0x16);
        }
    }
    else
    {
        throw CDNFException(
            "CPacketTranslater::OnDBReplyGuildMasterDelegate : packet->m_uGuildKey == 0");
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnRequestGuildMasterDelegate", 0xc5d);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xc62);
        log("./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildMasterDelegate(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildMasterDelegate : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Master_Delegate resp;
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int requesterCharNo = *(unsigned int*)((char*)pkt + 0xe);
    unsigned int delegateeCharNo = *(unsigned int*)((char*)pkt + 0x12);
    int result = *(int*)((char*)pkt + 0x34);
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnDBReplyGuildMasterDelegate : packet->m_uGuildKey == 0");
    }
    CUser* requester;
    if ((requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xc82,"./log/GuildModify",
            "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclRequester, Char Key = %d",
            requesterCharNo);
        return;
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xc89,"./log/GuildModify",
            "CPacketTranslater::OnDBReplyGuildMasterDelegate : 0 == pclGuild, Guild Key = %d",
            guildKey);
        return;
    }
    if (guild->IsSetGuildDBFlag(4) == 1)
    {
        if (result == 0)
        {
            if (guild->IsSubGuildMaster(delegateeCharNo) == 0)
            {
                DNF_LOG_SCOPE_LINE(0xc9b,"./log/GuildModify",
                    "CPacketTranslater::OnDBReplyGuildMasterDelegate : Delegatee(%d) is not sub guild master, Guild Key = %d",
                    delegateeCharNo, guildKey);
            }
            else
            {
                guild->SetSubGuildMaster(delegateeCharNo, false);
            }
            if (guild->ChangeGuildMaster(m_pclApp->Get_ServerHandler(), requester,
                                         delegateeCharNo) != 1)
            {
                DNF_LOG_SCOPE_LINE(0xca5,"./log/GuildModify",
                    "CPacketTranslater::OnDBReplyGuildMasterDelegate : ERR Guild'state is not GUILD_DB_LOAD_STATE, Guild Key = %d",
                    guildKey);
            }
            guild->ChangeUnconnectedGuildMemberGrade(requesterCharNo, 3);
            requester->ChangeGuildMemberGrade(3);
            guild->SendGuildInfoToMembers(false);
            guild->NoticeGuildMasterDelegateToMembers((char*)pkt + 0x16);
        }
        if (requester != 0)
        {
            *(unsigned int*)((char*)&resp + 0xa) = requesterCharNo;
            *(unsigned int*)((char*)&resp + 0xe) = requester->GetIdByChannel();
            *(int*)((char*)&resp + 0x12) = result;
            requester->SendToGameserver((char*)&resp, 0x16);
        }
        CUser* delegatee;
        if ((delegatee = m_pclApp->Get_UserManager()->FindUser_CharNo(delegateeCharNo)) != 0)
        {
            Packet_Notice_Has_Been_Guild_Master notice;
            *(unsigned int*)((char*)&notice + 0xa) = delegateeCharNo;
            *(unsigned int*)((char*)&notice + 0xe) = delegatee->GetIdByChannel();
            delegatee->SendToGameserver((char*)&notice, 0x12);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xc8e,"./log/GuildModify",
            "CPacketTranslater::OnDBReplyGuildMasterDelegate : Guild'state is not GUILD_DB_LOAD_STATE, Guild Key = %d",
            guildKey);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBReplyGuildMasterDelegate", 0xccc);
        log("./log/Except", "CPacketTranslater::OnDBReplyGuildMasterDelegate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xcd1);
        log("./log/Except", "CPacketTranslater::OnDBReplyGuildMasterDelegate() Exception Break\n");
    }
}

void CPacketTranslater::OnGuildMasterDelegateFromWeb(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnGuildMasterDelegateFromWeb : 0 == m_pclApp");
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int requesterCharNo = *(unsigned int*)((char*)pkt + 0xe);
    unsigned int delegateeCharNo = *(unsigned int*)((char*)pkt + 0x12);
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnGuildMasterDelegateFromWeb : packet->m_uGuildKey == 0");
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xcf0,"./log/GuildModify",
            "CPacketTranslater::OnGuildMasterDelegateFromWeb : 0 == pclGuild, Guild Key = %d",
            guildKey);
        return;
    }
    if (guild->IsSetGuildDBFlag(4) == 1)
    {
        if (guild->IsGuildMaster(requesterCharNo))
        {
            CUser* requester;
            if ((requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterCharNo)) == 0)
            {
                DNF_LOG_SCOPE_LINE(0xd04,"./log/GuildModify",
                    "CPacketTranslater::OnGuildMasterDelegateFromWeb : 0 == pclRequester, Char Key = %d",
                    requesterCharNo);
            }
            if (guild->IsSubGuildMaster(delegateeCharNo) == 0)
            {
                DNF_LOG_SCOPE_LINE(0xd10,"./log/GuildModify",
                    "CPacketTranslater::OnGuildMasterDelegateFromWeb : Delegatee(%d) is not sub guild master, Guild Key = %d",
                    delegateeCharNo, guildKey);
            }
            else
            {
                guild->SetSubGuildMaster(delegateeCharNo, false);
                if (guild->ChangeGuildMaster(m_pclApp->Get_ServerHandler(), requester,
                                             delegateeCharNo) != 1)
                {
                    DNF_LOG_SCOPE_LINE(0xd1b,"./log/GuildModify",
                        "CPacketTranslater::OnGuildMasterDelegateFromWeb : ERR : Guild'state is not GUILD_DB_LOAD_STATE, Guild Key = %d",
                        guildKey);
                }
                guild->ChangeUnconnectedGuildMemberGrade(requesterCharNo, 3);
                if (requester != 0)
                {
                    requester->ChangeGuildMemberGrade(3);
                }
                guild->SendGuildInfoToMembers(false);
                guild->NoticeGuildMasterDelegateToMembers((char*)pkt + 0x16);
                CUser* delegatee;
                if ((delegatee = m_pclApp->Get_UserManager()->FindUser_CharNo(delegateeCharNo)) != 0)
                {
                    Packet_Notice_Has_Been_Guild_Master notice;
                    *(unsigned int*)((char*)&notice + 0xa) = delegateeCharNo;
                    *(unsigned int*)((char*)&notice + 0xe) = delegatee->GetIdByChannel();
                    delegatee->SendToGameserver((char*)&notice, 0x12);
                }
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xcfd,"./log/GuildModify",
                "CPacketTranslater::OnGuildMasterDelegateFromWeb : Requester is Not Guild Master(%d)(%d)",
                guildKey, requesterCharNo);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xcf6,"./log/GuildModify",
            "CPacketTranslater::OnGuildMasterDelegateFromWeb : Guild'state is not GUILD_DB_LOAD_STATE, Guild Key = %d",
            guildKey);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildMasterDelegateFromWeb", 0xd39);
        log("./log/Except", "CPacketTranslater::OnGuildMasterDelegateFromWeb() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xd3e);
        log("./log/Except", "CPacketTranslater::OnGuildMasterDelegateFromWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnCheckGuildMemberConnectionFromWeb(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCheckGuildMemberConnectionFromWeb : 0 == m_pclApp");
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xa);
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb : packet->m_uGuildKey == 0");
    }
    Packet_Answer_Guild_Member_Connection_From_Web resp;
    *(unsigned int*)((char*)&resp + 0xa) = guildKey;
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    short count;
    if (guild == 0 || guild->IsSetGuildDBFlag(4) != 1)
    {
        count = 0;
    }
    else
    {
        count = (short)guild->ReplyGuildMembersToWeb(
            (STGuildMemberWebConnInfo*)((char*)&resp + 0x10));
    }
    unsigned short size = (unsigned short)(count * 5 + 0x10);
    *(unsigned short*)((char*)&resp + 2) = size;
    unsigned short port = *(unsigned short*)((char*)pkt + 4);
    unsigned int ip = *(unsigned int*)((char*)pkt + 6);
    if (m_pclApp->Get_UdpHandler()->SendToClient((char*)&resp, (int)size, port, 0, ip) != 1)
    {
        throw CDNFException(strerror(errno));
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnCheckGuildMemberConnectionFromWeb", 0xd6e);
        log("./log/Except", "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xd73);
        log("./log/Except", "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildLevelUp(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildLevelUp : 0 == m_pclApp")
    char* pb = (char*)pkt;
    if (*(unsigned int*)(pb + 0x12) == 0)
    {
        throw CDNFException("CPacketTranslater::OnCallGuildLevelUp : packet->m_uGuildKey == 0");
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd90, "./log/Except",
            "CPacketTranslater::OnCallGuildLevelUp : 0 == pclUser, Char Key = %d",
            *(unsigned int*)(pb + 0xe));
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0x12));
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd94, "./log/Except",
            "CPacketTranslater::OnCallGuildLevelUp : 0 == pclGuild, Guild Key = %d",
            *(unsigned int*)(pb + 0x12));
        return;
    }
    if (*(unsigned int*)(pb + 0x17) > guild->GetGuildFund())
    {
        DNF_LOG_SCOPE_LINE(0xd9a, "./log/GuildModify",
            "CPacketTranslater::OnCallGuildLevelUp(%d : %d) Guild Fund Shortage",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0x12));
        return;
    }
    guild->SubGuildFund(*(unsigned int*)(pb + 0x17));
    unsigned char ret = (unsigned char)guild->GuildLevelUp(m_pclApp->Get_ServerHandler(), user);
    if (ret != 0)
    {
        DNF_LOG_SCOPE_LINE(0xda3, "./log/GuildModify",
            "CPacketTranslater::OnCallGuildLevelUp(%d : %d) ERR(%d)",
            *(unsigned int*)(pb + 0x12), *(unsigned int*)(pb + 0xe), (int)ret);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnCallGuildLevelUp", 0xda7);
        log("./log/Except", "CPacketTranslater::OnCallGuildLevelUp() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xdac);
        log("./log/Except", "CPacketTranslater::OnCallGuildLevelUp() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildInfo(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildInfo : 0 == m_pclApp")
    char* pb = (char*)pkt;
    try
    {
        CUser* user;
        if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe))) == 0)
        {
            CMyFileLog log("OnCallGuildInfo", 0xdc5);
            log("./log/Except",
                "CPacketTranslater::OnCallGuildInfo() pclUser = 0, Char Key : %d\n",
                *(unsigned int*)(pb + 0xe));
            return;
        }
        Packet_Monitor_Call_Guild_Info_Reply replyPkt;
        *(unsigned int*)((char*)&replyPkt + 0xa) = *(unsigned int*)(pb + 0xa);
        *(unsigned int*)((char*)&replyPkt + 0xe) = *(unsigned int*)(pb + 0xe);
        *(unsigned char*)((char*)&replyPkt + 0x12) = pb[0x12];
        for (int i = 0; i < (unsigned char)pb[0x12]; i++)
        {
            unsigned int guildKey = *(unsigned int*)(pb + 0x13 + i * 4);
            if (guildKey == 0)
            {
                CMyFileLog log("OnCallGuildInfo", 0xdd3);
                log("./log/Guild",
                    "CPacketTranslater::OnCallGuildInfo : packet->m_uGuildKey[%d] == 0", i);
                continue;
            }
            CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
            if (guild == 0)
            {
                continue;
            }
            *(unsigned int*)((char*)&replyPkt + 0x13 + i * 0x1d) = guildKey;
            *(unsigned char*)((char*)&replyPkt + 0x17 + i * 0x1d) = guild->GetGuildLevel();
            *(unsigned char*)((char*)&replyPkt + 0x18 + i * 0x1d) = guild->GetGuildRank();
            memcpy((char*)&replyPkt + 0x19 + i * 0x1d, guild->GetGuildName(), 0x16);
        }
        unsigned short totalLen =
            (unsigned short)((unsigned char)pb[0x12] * 0x1d + 0x13);
        *(unsigned short*)((char*)&replyPkt + 0x2) = totalLen;
        user->SendToGameserver((char*)&replyPkt, totalLen);
    }
    catch (std::exception& e)
    {
        CMyFileLog log("OnCallGuildInfo", 0xde9);
        log("./log/Except",
            "CPacketTranslater::OnCallGuildInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xdee);
        log("./log/Except",
            "CPacketTranslater::OnCallGuildInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildMailArrive(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildMailArrive : 0 == m_pclApp");
    char* pb = (char*)pkt;
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    for (int i = 0; i < (int)(unsigned int)(unsigned char)pb[10]; i++)
    {
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild != 0)
        {
            guild->SendToGuildForMail();
        }
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildMailArrive() Exception Break : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0xe10);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildMailArrive() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildMailArrive() Exception Break");
        CMyFileLog log(__FUNCTION__, 0xe15);
        log("./log/Except", "CPacketTranslater::OnNoticeGuildMailArrive() Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorManagerConnectOK(PacketHeader* pkt)
{
    (void)pkt;
    THROW_IF_NO_APP("CPacketTranslater::OnMonitorManagerConnectOK : 0 == m_pclApp");
    m_pclApp->Get_ServerHandler()->SetManagerConnectFlag(true);
    DNF_LOG_SCOPE_LINE(0xe2d, "./log/Manager", "Manager Server Connect Success");
    puts("** Manager Server Connect Success **");
}

void CPacketTranslater::OnEventStart(PacketHeader* pkt)
{
    THROW_IF_NO_APP("CPacketTranslater::OnEventStart : 0 == m_pclApp");
    char* pb = (char*)pkt;
    DNF_LOG_SCOPE_LINE(0xe4c,"./log/Web",
        "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n",
        *(unsigned int*)(pb + 0xa), (unsigned int)*(unsigned short*)(pb + 0xe),
        (unsigned int)*(unsigned short*)(pb + 0x10));
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
}

void CPacketTranslater::OnEventEnd(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnEventEnd : 0 == m_pclApp");
    char* pb = (char*)pkt;
    unsigned int eventCode = *(unsigned int*)(pb + 10);
    DNF_LOG_SCOPE_LINE(0xe6b, "./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n", eventCode);
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() 예외 발생 : %s\n", e.what());
        CMyFileLog log("OnEventEnd", 0xe72);
        log("./log/Except", "CPacketTranslater::OnEventEnd() 예외 발생 : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() 예외 발생");
        CMyFileLog log(__FUNCTION__, 0xe78);
        log("./log/Except", "CPacketTranslater::OnEventEnd() 예외 발생\n");
    }
}

void CPacketTranslater::OnRegisterToBlackList(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xe8f, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == m_pclApp");
        return;
    }
    Packet_DBMW_Register_To_BlackList dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xa) = *(unsigned int*)(pb + 0xa);
    memcpy((char*)&dbPkt + 0xe, pb + 0xe, 0x1d);
    CUser* requester = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa));
    if (requester == 0)
    {
        DNF_LOG_SCOPE_LINE(0xe9f, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == pclUser");
        return;
    }
    char* targetName = pb + 0xe;
    if (strcmp(requester->GetCharName(), targetName) != 0 && requester->GetBlackListSize() < 10)
    {
        CUser* target = m_pclApp->Get_UserManager()->FindUser_CharName(targetName);
        if (target != 0)
        {
            if (requester->RegisterToBlackList(target->GetUniqCharNo(), targetName) == 1)
            {
                target->GetUniqCharNo();
            }
        }
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnRegisterToBlackList", 0xed1);
        log("./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xed6);
        log("./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDeleteToBlackList(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xeeb, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == m_pclApp");
        return;
    }
    Packet_DMBW_Delete_To_BlackList dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xa) = *(unsigned int*)(pb + 0xa);
    memcpy((char*)&dbPkt + 0xe, pb + 0xe, 0x1d);
    CUser* target = m_pclApp->Get_UserManager()->FindUser_CharName(pb + 0xe);
    if (target != 0)
    {
        CUser* requester;
        if ((requester = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
        {
            DNF_LOG_SCOPE_LINE(0xefc, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == pclUser");
        }
        else
        {
            if (requester->DeleteToBlackList(target->GetUniqCharNo()) == 1)
            {
                target->GetUniqCharNo();
            }
        }
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDeleteToBlackList", 0xf16);
        log("./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xf1b);
        log("./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnRequestBlackList(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf2f, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == m_pclApp");
        return;
    }
    Packet_Request_Result_BlackList reply;
    *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)(pb + 0xe);
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf39, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == pclUser");
        return;
    }
    unsigned char count = 0;
    user->GetBlackList(count, (STBlackUserDBType*)((char*)&reply + 0xf));
    *(unsigned char*)((char*)&reply + 0xe) = count;
    user->SendToGameserver((char*)&reply, 0x19f);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnRequestBlackList", 0xf45);
        log("./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xf4a);
        log("./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWResisterToBlackList(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf5e,"./log/BlackList",
            "CPacketTranslater::OnDBMWResisterToBlackList : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf65,"./log/BlackList",
            "CPacketTranslater::OnDBMWResisterToBlackList : 0 == pclUser");
        return;
    }
    Packet_Register_To_BlackList_RESULT reply;
    *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
    memcpy((char*)&reply + 0xe, pb + 0xe, 0x1d);
    if (*(int*)(pb + 0x2c) == -1)
    {
        *(unsigned char*)((char*)&reply + 0x30) = 3;
        user->SendToGameserver((char*)&reply, 0x31);
        return;
    }
    if (user->IsBlackUser(*(unsigned int*)(pb + 0x2c)) == 0)
    {
        if (user->GetBlackListSize() < 10)
        {
            user->RegisterToBlackList(*(unsigned int*)(pb + 0x2c), pb + 0xe);
            *(unsigned char*)((char*)&reply + 0x30) = 1;
            *(unsigned int*)((char*)&reply + 0x2c) = *(unsigned int*)(pb + 0x2c);
            user->SendToGameserver((char*)&reply, 0x31);
        }
        else
        {
            *(unsigned char*)((char*)&reply + 0x30) = 4;
            user->SendToGameserver((char*)&reply, 0x31);
        }
    }
    else
    {
        *(unsigned char*)((char*)&reply + 0x30) = 2;
        user->SendToGameserver((char*)&reply, 0x31);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBMWResisterToBlackList", 0xf8d);
        log("./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xf92);
        log("./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWDeleteToBlackList(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xfa6,"./log/BlackList",
            "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xfad,"./log/BlackList",
            "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == pclUser");
        return;
    }
    Packet_Delete_To_BlackList_Result reply;
    *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
    memcpy((char*)&reply + 0xe, pb + 0xe, 0x1d);
    if (*(int*)(pb + 0x2c) == -1)
    {
        *(unsigned char*)((char*)&reply + 0x30) = 3;
        user->SendToGameserver((char*)&reply, 0x31);
        return;
    }
    if (user->DeleteToBlackList(*(unsigned int*)(pb + 0x2c)) != 1)
    {
        *(unsigned char*)((char*)&reply + 0x30) = 2;
        user->SendToGameserver((char*)&reply, 0x31);
    }
    *(unsigned char*)((char*)&reply + 0x30) = 1;
    *(unsigned int*)((char*)&reply + 0x2c) = *(unsigned int*)(pb + 0x2c);
    user->SendToGameserver((char*)&reply, 0x31);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBMWDeleteToBlackList", 0xfca);
        log("./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xfcf);
        log("./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::RequestBlackListToDBMW(unsigned int charNo)
{
    try
    {
        Packet_DBMW_Request_BlackList pkt;
        *(unsigned int*)((char*)&pkt + 0xa) = charNo;
        *(unsigned char*)((char*)&pkt + 0xb) = 0xcb;
        m_pclApp->Get_ServerHandler()->SendToDB(&pkt);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("RequestBlackListToDBMW", 0xfe7);
        log("./log/Except",
            "CPacketTranslater::RequestBlackListToDBMW Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0xfec);
        log("./log/Except", "CPacketTranslater::RequestBlackListToDBMW Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWResponseBlackListOnLogin(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1000,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1007,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == pclUser");
        return;
    }
    int i = 0;
    while (*(int*)(pb + i * 0x28 + 0x32) != 0)
    {
        if (user->IsBlackUser(*(unsigned int*)(pb + i * 0x28 + 0xe)) != 1)
        {
            user->RegisterToBlackList(*(unsigned int*)(pb + i * 0x28 + 0xe),
                                      pb + i * 0x28 + 0x12,
                                      *(unsigned int*)(pb + i * 0x28 + 0x32));
        }
        i++;
    }
    user->SetBlackListDBFlag(4);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBMWResponseBlackListOnLogin", 0x101b);
        log("./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1020);
        log("./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1036,"./log/GuildModify",
            "CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x103c,"./log/GuildModify",
            "CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade : 0 == pclGuild(%d)",
            guildKey);
        return;
    }
    Packet_Monitor_Set_Sub_Guild_Master_Reply reply;
    int result = 0;
    if ((unsigned char)pb[0x30] == 0xff)
    {
        result = 2;
    }
    else if ((unsigned char)pb[0x30] == 0xfe)
    {
        result = 0x66;
    }
    else if ((unsigned char)pb[0x31] == 1)
    {
        result = 0x58;
    }
    else if ((unsigned char)pb[0x30] == (unsigned char)pb[0x31])
    {
        result = 0x65;
    }
    else
    {
        if ((unsigned char)pb[0x30] == 2)
        {
            if (guild->GetCurSubGuildMasterCnt() < 5)
            {
                guild->SetSubGuildMaster(*(unsigned int*)(pb + 0x32), true);
                guild->SendGuildInfoToMembers(false);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x105b,"./log/GuildModify", "CRITICAL ERR sub guild master cnt over(%d)",
                    (unsigned int)guild->GetCurSubGuildMasterCnt() & 0xff);
                result = 0x59;
            }
        }
        else if ((unsigned char)pb[0x31] == 2)
        {
            guild->SetSubGuildMaster(*(unsigned int*)(pb + 0x32), false);
            guild->SendGuildInfoToMembers(false);
        }
        if (result == 0)
        {
            guild->ChangeUnconnectedGuildMemberGrade(*(unsigned int*)(pb + 0x32),
                                                     (unsigned char)pb[0x30]);
        }
    }
    {
        DNF_LOG_SCOPE_LINE(0x1073,"./log/GuildModify",
            "Change Guild(%d) Grade(%d) Prev Grade(%d) result(%d) unconnected charNo(%d) RequestNo(%d)",
            guildKey, (unsigned int)(unsigned char)pb[0x30],
            (unsigned int)(unsigned char)pb[0x31], result, *(unsigned int*)(pb + 0x32),
            *(unsigned int*)(pb + 0xe));
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe));
    if (user != 0)
    {
        *(unsigned int*)((char*)&reply + 0xa) = result;
        *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0x12) = user->GetUniqCharNo();
        *(unsigned char*)((char*)&reply + 0x16) = *(unsigned char*)(pb + 0x30);
        memcpy((char*)&reply + 0x17, pb + 0x12, 0x1d);
        user->SendToGameserver((char*)&reply, 0x3a);
    }
}

void CPacketTranslater::OnNotifyMessageToGuild(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x109d, "./log/Except", "CPacketTranslater::OnNotifyMessageToGuild : 0 == m_pclApp");
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xa))) != 0)
    {
        if ((unsigned char)pb[0xe] != 1)
        {
            if ((unsigned char)pb[0xe] != 2)
            {
                return;
            }
            Packet_Web_Notify_Message_To_Guild notify;
            memcpy(&notify, pb, 0x74);
            *(unsigned char*)((char*)&notify + 0xa) = m_pclApp->Get_ServerGroup();
            m_pclApp->Get_ServerHandler()->SendToDB(&notify);
        }
        guild->SetGuildMessage(pb + 0xf);
        guild->NotifyMessageToGuildMember();
    }
}

void CPacketTranslater::OnRequestGuildCreate(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x10d8, "./log/GuildModify", "CPacketTranslater::OnRequestGuildCreate : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x10df, "./log/GuildModify", "CPacketTranslater::OnRequestGuildCreate : 0 == pclUser");
        return;
    }
    Packet_Reply_Guild_Create reply;
    *(unsigned int*)((char*)&reply + 0xa) = charNo;
    *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
    if (user->GetGuildKey() != 0)
    {
        *(unsigned int*)((char*)&reply + 0x12) = 0x20;
        user->SendToGameserver((char*)&reply, 0x2d);
        return;
    }
    if (user->GetGameServer() == 0)
    {
        *(unsigned int*)((char*)&reply + 0x12) = 1;
        user->SendToGameserver((char*)&reply, 0x2d);
        return;
    }
    Packet_DBMW_Request_Guild_Create dbPkt;
    *(unsigned char*)((char*)&dbPkt + 0xa) = user->GetGameServer()->GetGroupNo();
    *(unsigned int*)((char*)&dbPkt + 0xb) = user->GetDBID();
    *(unsigned int*)((char*)&dbPkt + 0xf) = user->GetUniqCharNo();
    memcpy((char*)&dbPkt + 0x13, user->GetCharName(), 0x1d);
    *(unsigned char*)((char*)&dbPkt + 0x31) = user->GetJob();
    *(unsigned char*)((char*)&dbPkt + 0x32) = user->GetGrowthType();
    *(unsigned char*)((char*)&dbPkt + 0x33) = user->GetLevel();
    *(unsigned char*)((char*)&dbPkt + 0x34) = user->GetSex();
    memcpy((char*)&dbPkt + 0x35, user->GetSsn(), 2);
    memcpy((char*)&dbPkt + 0x38, pb + 0xe, 0x16);
    memcpy((char*)&dbPkt + 0x4f, pb + 0x25, 0xc);
    m_pclApp->Get_ServerHandler()->GetDBServer()->SendToServer((char*)&dbPkt, 0x5c);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnRequestGuildCreate", 0x1110);
        log("./log/Except", "CPacketTranslater::OnRequestGuildCreate Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1115);
        log("./log/Except", "CPacketTranslater::OnRequestGuildCreate Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildCreate(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x112c, "./log/GuildModify", "CPacketTranslater::OnDBReplyGuildCreate : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1133,"./log/GuildModify",
            "CPacketTranslater::OnDBReplyGuildCreate : 0 == pclRequestUser");
        return;
    }
    Packet_Notice_GuildName_On_Guild_Create notice;
    Packet_Reply_Guild_Create reply;
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    *(unsigned int*)((char*)&notice + 0xa) = charNo;
    *(unsigned int*)((char*)&reply + 0xa) = charNo;
    unsigned int channel = user->GetIdByChannel();
    *(unsigned int*)((char*)&notice + 0xe) = channel;
    *(unsigned int*)((char*)&reply + 0xe) = channel;
    int result = *(int*)(pb + 0x12);
    *(int*)((char*)&reply + 0x12) = result;
    memcpy((char*)&reply + 0x16, pb + 0x16, 0x16);
    if (result == 0)
    {
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler == 0)
        {
            throw CDNFException("CGuildManager::GuildMemLogin() pclServerHandler == NULL\n");
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->CreateGuild(guildKey, handler, charNo);
        unsigned int uniqCharNo = user->GetUniqCharNo();
        if (guild->InsertGuildMember(uniqCharNo, user) == 1)
        {
            user->QueryGuildMember(handler);
        }
        uniqCharNo = user->GetUniqCharNo();
        user->SendSetGuildKeyToUser(guildKey, uniqCharNo);
        *(unsigned int*)((char*)&notice + 0x12) = guildKey;
        memcpy((char*)&notice + 0x16, pb + 0x16, 0x16);
        *(unsigned char*)((char*)&notice + 0x2d) = m_pclApp->Get_ServerGroup();
        m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&notice);
        m_pclApp->Get_GuildManager()->AttendGuild(guildKey, charNo);
        guild->QueryTodayGuildMember(m_pclApp->Get_ServerHandler());
    }
    user->SendTcpGameserver(&reply);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnDBReplyGuildCreate", 0x116d);
        log("./log/Except", "CPacketTranslater::OnDBReplyGuildCreate Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1172);
        log("./log/Except", "CPacketTranslater::OnDBReplyGuildCreate Exception Break\n");
    }
}

void CPacketTranslater::OnPowerWarStartInfo(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x118a, "./log/Power", "CPacketTranslater::OnPowerWarStartInfo : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (pm->IsPowerWarOn() != 0)
        {
            Packet_Monitor_Event_Start start;
            *(unsigned int*)((char*)&start + 0xa) = 0x1e;
            m_pclApp->Get_ServerHandler()->SendToGameServer((unsigned char)*(unsigned int*)(pb + 0xa),
                                                            &start);
        }
        const char* state = pm->IsPowerWarOn() == 0 ? "END!" : "START!";
        DNF_LOG_SCOPE_LINE(0x119a,"./log/Power", "OnPowerWarStartInfo(%d) - power war %s ", *(unsigned int*)(pb + 0xa),
            state);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPowerWarStartInfo Exception Break", 0x119e, 0x11a3);
}

void CPacketTranslater::OnChangePowerWarPoint(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x11ba, "./log/Power", "CPacketTranslater::OnChangePowerWarScore : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (pm->IsPowerWarOn() == 1)
        {
            if ((unsigned char)pb[10] != 1 && (unsigned char)pb[10] != 2)
            {
                if ((unsigned char)pb[10] == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x11c9,"./log/Power",
                        "ChangePowerWarPoint Invalid Power Side Income(side:%d, 0:%d, 1:%d, 2:%d, 3:%d, pp:%d)",
                        (unsigned int)(unsigned char)pb[10], *(unsigned int*)(pb + 0xb),
                        *(unsigned int*)(pb + 0xf), *(unsigned int*)(pb + 0x13),
                        *(unsigned int*)(pb + 0x17), (unsigned int)(unsigned char)pb[0x2b]);
                    return;
                }
                DNF_LOG_SCOPE_LINE(0x11ce,"./log/Power",
                    "ChangePowerWarPoint Invalid Power Side Income(side:%d, 0:%d, 1:%d, 2:%d, 3:%d, pp:%d)",
                    (unsigned int)(unsigned char)pb[10], *(unsigned int*)(pb + 0xb),
                    *(unsigned int*)(pb + 0xf), *(unsigned int*)(pb + 0x13),
                    *(unsigned int*)(pb + 0x17), (unsigned int)(unsigned char)pb[0x2b]);
                pb[10] = (char)((unsigned char)pb[10] == 3 ? 1 : 2);
            }
            int winScore = pm->IncPowerScore((ENUM_POWER_SIDE_TYPE)(unsigned char)pb[10],
                                             (unsigned char)pb[0x2b]);
            ENUM_POWER_SIDE_TYPE loseSide =
                (unsigned char)pb[10] == 1 ? (ENUM_POWER_SIDE_TYPE)2 : (ENUM_POWER_SIDE_TYPE)1;
            int loseScore = pm->IncPowerScore(loseSide, (unsigned char)pb[0x2c]);
            {
                DNF_LOG_SCOPE_LINE(0x11e1,"./log/Power", "win side(%d), win pp(%d, %d), lose pp(%d, %d)",
                    (unsigned int)(unsigned char)pb[10], (unsigned int)(unsigned char)pb[0x2b],
                    winScore, (unsigned int)(unsigned char)pb[0x2c], loseScore);
            }
            pm->UpdatePowerWarInfo(1, (ENUM_POWER_SIDE_TYPE)(unsigned char)pb[10],
                                   (unsigned char)pb[0x2b], (unsigned int*)(pb + 0xb));
            pm->UpdatePowerWarInfo(0, loseSide, (unsigned char)pb[0x2c],
                                   (unsigned int*)(pb + 0x1b));
            std::vector<unsigned int> users;
            users.reserve(8);
            ((Packet_Guild_Change_Power_War_Point*)pb)->GetUserList(users);
            for (unsigned int i = 0; i < users.size(); i++)
            {
                pm->SendPowerWarProcessInfo(users.at(i));
            }
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnChangePowerWarScore Exception Break", 0x1217, 0x121c);
}

void CPacketTranslater::OnPacketJoinPower(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        Packet_Answer_Join_Power reply;
        *(unsigned int*)((char*)&reply + 0xa) = 0;
        *(unsigned int*)((char*)&reply + 0x12) = *(unsigned int*)(pb + 0xa);
        *(unsigned char*)((char*)&reply + 0x16) = *(unsigned char*)(pb + 0x12);
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1237, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == m_pclApp");
            return;
        }
        unsigned int charNo = *(unsigned int*)(pb + 0xa);
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x123d, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == pclRequestUser");
            return;
        }
        *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
        unsigned int guildKey = *(unsigned int*)(pb + 0xe);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1244, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == pclGuild");
            *(unsigned int*)((char*)&reply + 0xa) = 100;
            user->SendToGameserver((char*)&reply, 0x17);
            return;
        }
        if (guild->IsSubGuildMaster(charNo) == 1 || guild->IsGuildMaster(charNo) == 1)
        {
            unsigned char side = (unsigned char)pb[0x12] == 1 ? 3 : 4;
            guild->SetPowerSide(side);
            guild->IncPowerJoinCount();
            CServerInterface* gs = user->GetGameServer();
            guild->DBGuildSave(gs->GetGroupNo(), m_pclApp->Get_ServerHandler(), 0);
            *(unsigned char*)((char*)&reply + 0x16) = side;
            user->SendToGameserver((char*)&reply, 0x17);
            guild->SendGuildInfoToMembers(false);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x124f,"./log/Power",
                "CPacketTranslater::OnPacketJoinPower : %d is not guild master or sub master(g:%d)",
                charNo, guildKey);
            *(unsigned int*)((char*)&reply + 0xa) = 0x56;
            user->SendToGameserver((char*)&reply, 0x17);
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnPacketJoinPower", 0x1291);
        log("./log/Except",
            "CPacketTranslater::OnPacketJoinPower Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1296);
        log("./log/Except", "CPacketTranslater::OnPacketJoinPower Exception Break\n");
    }
}

void CPacketTranslater::OnPacketSecedePower(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12a9, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == m_pclApp");
        return;
    }
    Packet_Answer_Secede_Power reply;
    *(unsigned int*)((char*)&reply + 0xa) = 0;
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    *(unsigned int*)((char*)&reply + 0x12) = charNo;
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12b5, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == pclRequestUser");
        return;
    }
    *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guildKey == 0 || guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12bc, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == pclGuild");
        *(unsigned int*)((char*)&reply + 0xa) = 100;
        user->SendToGameserver((char*)&reply, 0x17);
        return;
    }
    if (guild->GetPowerSide() != 0)
    {
        if (guild->IsSubGuildMaster(charNo) == 1 || guild->IsGuildMaster(charNo) == 1)
        {
            if (m_pclApp->GetPowerManager()->IsPowerWarOn() == 0)
            {
                guild->SetPowerSide(0);
                guild->SetPowerSecedeTime((unsigned int)time(0));
                CServerInterface* gs = user->GetGameServer();
                guild->DBSavePowerSecedeTime(gs->GetGroupNo(),
                                             m_pclApp->Get_ServerHandler());
                guild->SendGuildInfoToMembers(false);
                user->SendToGameserver((char*)&reply, 0x17);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x12db,"./log/Power",
                    "CPacketTranslater::OnPacketSecedePower : %d power war on(g:%d)", charNo,
                    guildKey);
                *(unsigned int*)((char*)&reply + 0xa) = 0x7f;
                user->SendToGameserver((char*)&reply, 0x17);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x12d1,"./log/Power",
                "CPacketTranslater::OnPacketSecedePower : %d is not guild master or sub master(g:%d)",
                charNo, guildKey);
            *(unsigned int*)((char*)&reply + 0xa) = 0x56;
            user->SendToGameserver((char*)&reply, 0x17);
        }
        return;
    }
    DNF_LOG_SCOPE_LINE(0x12c8,"./log/Power",
        "CPacketTranslater::OnPacketSecedePower : %d did not join power side(g:%d)", charNo,
        guildKey);
    *(unsigned int*)((char*)&reply + 0xa) = 0x82;
    user->SendToGameserver((char*)&reply, 0x17);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnPacketSecedePower", 0x12ff);
        log("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1304);
        log("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break\n");
    }
}

void CPacketTranslater::OnSetPowerWarCfg(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1317, "./log/Power", "CPacketTranslater::OnSetPowerWarCfg : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        pm->SetPowerWarEndKillPoint(*(unsigned short*)(pb + 0xa));
        pm->SetPowerWarRewardInfo(*(int*)(pb + 0xc), *(int*)(pb + 0x10), *(int*)(pb + 0x14),
                                  *(int*)(pb + 0x18));
        DNF_LOG_SCOPE_LINE(0x1321,"./log/Power",
            "CPacketTranslater::OnSetPowerWarCfg EndKillPoint:%d, BasicPowerWarPoint:%d, FirstGradeBonusPowerWarPoint:%d, BonusDecreasePoint:%d, RankingBonusGrade:%d",
            (unsigned int)*(unsigned short*)(pb + 0xa), *(int*)(pb + 0xc), *(int*)(pb + 0x10),
            *(int*)(pb + 0x14), *(int*)(pb + 0x18));
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break", 0x132b, 0x1330);
}

void CPacketTranslater::OnLoadFromDBOnGuildBooting(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1343,"./log/Power",
            "CPacketTranslater::OnLoadFromDBOnGuildBooting : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        pm->SetPowerInfo((char)pb[0x12], *(int*)(pb + 0xa), *(int*)(pb + 0xe));
        pm->SendPowerWarInfo();
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break", 0x134f, 0x1354);
}

void CPacketTranslater::OnGMPowerWarStart(PacketHeader* pkt)
{
    (void)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x13f9, "./log/Power", "CPacketTranslater::OnGMPowerWarStart : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (pm->IsPowerWarOn() == 0)
        {
            pm->StartPowerWarEvent();
            Packet_Monitor_Event_Start start;
            *(unsigned int*)((char*)&start + 0xa) = 0x1e;
            CServerHandler* handler;
            if ((handler = m_pclApp->Get_ServerHandler()) == 0)
            {
                throw CDNFException(
                    "CGuildManager::OnGMPowerWarStart() pclServerHandler == NULL\n");
            }
            handler->SendAllTcpGameServer(&start);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGMPowerWarStart Exception Break", 0x140e, 0x1413);
}

void CPacketTranslater::OnGMPowerWarEnd(PacketHeader* pkt)
{
    (void)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1426, "./log/Power", "CPacketTranslater::OnGMPowerWarEnd : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (pm->IsPowerWarOn() == 1)
        {
            pm->SendPowerWarScore();
            pm->EndPowerWarEvent();
            Packet_Monitor_Event_End end;
            *(unsigned int*)((char*)&end + 0xa) = 0x1e;
            CServerHandler* handler;
            if ((handler = m_pclApp->Get_ServerHandler()) == 0)
            {
                throw CDNFException(
                    "CGuildManager::OnGMPowerWarStart() pclServerHandler == NULL\n");
            }
            handler->SendAllTcpGameServer(&end);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGMPowerWarEnd Exception Break", 0x143d, 0x1442);
}

void CPacketTranslater::OnInnerPacketLogin(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1455, "./log/Except", "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
        return;
    }
    try
    {
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler->GetTcpDBServer()->GetSock() == *(int*)(pb + 6))
        {
            handler->GetTcpDBServer()->Connected();
        }
        else
        {
            CTcpGameServer* tgs = handler->CreateTcpGameServer(*(unsigned int*)(pb + 6));
            if (tgs != 0)
            {
                char* buf = tgs->makePacketHeader(8000, 0xc);
                if (buf != 0)
                {
                    buf[10] = 1;
                    buf[11] = m_pclApp->Get_ServerGroup();
                    tgs->SendToGameServer(buf);
                }
                char* buf2 = tgs->makePacketHeader(0x3ea, 0xb);
                if (buf2 != 0)
                {
                    buf2[10] = 0xcb;
                    tgs->SendToGameServer(buf2);
                }
            }
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnInnerPacketLogin Exception Break", 0x1489, 0x148e);
}

void CPacketTranslater::OnInnerPacketLogout(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x149e, "./log/Except", "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
        return;
    }
    try
    {
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler->GetTcpDBServer()->GetSock() == *(int*)(pb + 6))
        {
            handler->GetTcpDBServer()->DisConnected();
        }
        else
        {
            unsigned int group = *(unsigned int*)(pb + 6);
            CTcpGameServer* tgs = m_pclApp->FindTcpGameServer(group);
            m_pclApp->OnTcpGameServerDown(tgs);
            unsigned char ch = tgs->GetChannelNo();
            if (ch != 0)
            {
                m_pclApp->Get_ServerHandler()->UnregistGameServer((unsigned int)ch);
            }
            m_pclApp->Get_ServerHandler()->DeleteTcpGameServer(group);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break", 0x14be, 0x14c3);
}

void CPacketTranslater::OnChangeGuildName(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14dc, "./log/Except", "CPacketTranslater::OnChangeGuildName : 0 == m_pclApp");
        return;
    }
    Packet_Reply_Change_Guild_Name reply;
    *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)(pb + 0xe);
    memcpy((char*)&reply + 0x16, pb + 0x12, 0x16);
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14e7, "./log/GuildModify", "CPacketTranslater::OnChangeGuildName : 0 == pclRequestUser");
        return;
    }
    *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
    unsigned int guildKey = *(unsigned int*)(pb + 10);
    CGuild* guild = 0;
    if (guildKey == 0 ||
        (guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14ef, "./log/GuildModify", "CPacketTranslater::OnChangeGuildName : 0 == pclGuild");
        *(unsigned char*)((char*)&reply + 0x12) = 100;
        user->SendTcpGameserver(&reply);
        return;
    }
    if (guild->IsGuildMaster(*(unsigned int*)(pb + 0xe)) == 1)
    {
        guild->ChangeGuildName(pb + 0x12, 1);
        unsigned char group = 0;
        if (user->GetGameServer() != 0)
        {
            group = user->GetGameServer()->GetGroupNo();
        }
        guild->DBGuildSave(group, m_pclApp->Get_ServerHandler(), 0);
        guild->SendGuildNameChangeToMembers();
        *(unsigned int*)((char*)&reply + 0xa) = guildKey;
        *(unsigned char*)((char*)&reply + 0x12) = 0;
        user->SendTcpGameserver(&reply);
        static const char letterText[0x121] =
            "\xe8\xae\x8a\xe6\x9b\xb4\xe5\x85\xac\xe6\x9c\x83\xe5\x90\x8d\xe7\xa8\xb1\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe5\x85\xac\xe6\x9c\x83\xe5\x90\x8d"
            "\xe7\xa8\xb1\xe5\xb7\xb2\xe8\xae\x8a\xe6\x9b\xb4\xe7\x82\xba\x20\x25\x73\xe3\x80\x82\x00\x00\x00";
        char mailContent[0x100];
        memset(mailContent, 0, sizeof(mailContent));
        sprintf(mailContent, letterText + 0xff, pb + 0x12);
        SendPacketGuildMail(group, user->GetUniqCharNo(), guildKey, letterText,
                            mailContent, 0xffffffff);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x14f7,"./log/GuildModify",
            "CPacketTranslater::OnChangeGuildName : %d is not guild master or sub master(g:%d)",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 10));
        *(unsigned char*)((char*)&reply + 0x12) = 0x56;
        user->SendTcpGameserver(&reply);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnChangeGuildName", 0x1527);
        log("./log/Except", "CPacketTranslater::OnChangeGuildName Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x152c);
        log("./log/Except", "CPacketTranslater::OnChangeGuildName Exception Break\n");
    }
}

void CPacketTranslater::OnChangeCharName(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1544, "./log/Except", "CPacketTranslater::OnChangeGuildName : 0 == m_pclApp");
        return;
    }
    if (*(int*)(pb + 0x12) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x154c, "./log/ServerUnify", "CPacketTranslater::OnChangeCharName : packet->m_uGuildKey");
        return;
    }
    Packet_UnChangable_GuildInfo_Save save;
    *(unsigned int*)((char*)&save + 0xa) = *(unsigned int*)(pb + 0x12);
    *(unsigned int*)((char*)&save + 0xe) = *(unsigned int*)(pb + 0xe);
    memcpy((char*)&save + 0x12, pb + 0x16, 0x1d);
    m_pclApp->Get_ServerHandler()->SendToDB(&save);
    STTodayGuildMember* today =
        m_pclApp->Get_GuildManager()->GetTodayMember(*(unsigned int*)(pb + 0x12));
    if (today != 0 && *(int*)today == *(int*)(pb + 0xe))
    {
        memset((char*)today + 4, 0, 0x1e);
        strncpy((char*)today + 4, pb + 0x16, 0x1d);
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xa));
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1563, "./log/ServerUnify", "CPacketTranslater::OnChangeCharName : 0 == pclGuild");
        return;
    }
    if (guild->ChangeGuildMemberCharName(*(unsigned int*)(pb + 0xe), pb + 0x16) == 1)
    {
        CUser* user;
        if ((user = m_pclApp->Get_UserManager()->FindUser(*(unsigned int*)(pb + 0xa))) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1572,"./log/ServerUnify",
                "CPacketTranslater::OnChangeGuildName : 0 == pclRequestUser");
        }
        else
        {
            user->ChangeCharName(pb + 0x16);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x156b,"./log/ServerUnify",
            "CPacketTranslater::OnChangeGuildName : false == pclGuild->ChangeGuildMemberCharName()");
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnChangeCharName", 0x157f);
        log("./log/Except", "CPacketTranslater::OnChangeCharName Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1584);
        log("./log/Except", "CPacketTranslater::OnChangeCharName Exception Break\n");
    }
}

void CPacketTranslater::OnChangeGuildNameFromWeb(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x159d,"./log/Except",
            "CPacketTranslater::OnChangeGuildNameFromWeb : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guildKey == 0 || guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x15a5,"./log/GuildModify",
            "CPacketTranslater::OnChangeGuildNameFromWeb : 0 == pclGuild");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    if (guild->IsSubGuildMaster(charNo) == 1 || guild->IsGuildMaster(charNo) == 1)
    {
        guild->ChangeGuildName(pb + 0x12, 0);
        guild->SendGuildInfoToMembers(false);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x15ab,"./log/GuildModify",
            "CPacketTranslater::OnChangeGuildNameFromWeb : %d is not guild master or sub master(g:%d)",
            charNo, guildKey);
    }
}

void CPacketTranslater::OnInCreaseDecreasePowerWarPoint(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x15d3,"./log/Except",
            "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guildKey == 0 || guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x15db,"./log/Power",
            "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : 0 == pclGuild");
        return;
    }
    int point = *(int*)(pb + 0x12);
    if (point == 0)
    {
        DNF_LOG_SCOPE_LINE(0x15e2,"./log/Power",
            "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : Invalid Power War Point Reached(characNo:%d, PowerWarPoint:%d)",
            *(unsigned int*)(pb + 0xe), point);
        return;
    }
    if (point < 1)
    {
        unsigned int neg = ((unsigned int)point >> 0x1f);
        guild->SubPowerWarPoint(((unsigned int)point ^ neg) - neg);
    }
    else
    {
        guild->AddPowerWarPoint((unsigned int)point);
    }
    guild->SendGuildInfoToMembers(false);
}

void CPacketTranslater::OnCreateGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1608, "./log/Except", "CPacketTranslater::OnCreateGuildAgit : 0 == m_pclApp");
        return;
    }
    else
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1610, "./log/GuildAgit", "CPacketTranslater::OnCreateGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(*(unsigned int*)(pb + 0xe)) == 1)
        {
            if (guild->IsExistGuildAgit() == 0)
            {
                if (guild->GetGuildFund() < *(unsigned int*)(pb + 0x16))
                {
                    DNF_LOG_SCOPE_LINE(0x1623,"./log/GuildAgit",
                        "CPacketTranslater::OnCreateGuildAgit : %d guild fund shortage",
                        guildKey);
                }
                else
                {
                    guild->CreateGuildAgit(m_pclApp->Get_ServerHandler(), guildKey,
                                           *(unsigned int*)(pb + 0xe),
                                           *(unsigned int*)(pb + 0x12),
                                           *(unsigned int*)(pb + 0x16));
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x161c,"./log/GuildAgit",
                    "CPacketTranslater::OnCreateGuildAgit : %d guild already have guild agit",
                    guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1616,"./log/GuildAgit",
                "CPacketTranslater::OnCreateGuildAgit : %d is not guild master(g:%d)",
                *(unsigned int*)(pb + 0xe), guildKey);
        }
    }
}

void CPacketTranslater::OnDeleteGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1647, "./log/Except", "CPacketTranslater::OnDeleteGuildAgit : 0 == m_pclApp");
        return;
    }
    else
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x164f, "./log/GuildAgit", "CPacketTranslater::OnDeleteGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(*(unsigned int*)(pb + 0xe)) == 1)
        {
            if (guild->IsExistGuildAgit() == 1)
            {
                if (guild->GetGuildCargo()->IsEmpty() == 1)
                {
                    guild->DeleteGuildAgit(m_pclApp->Get_ServerHandler(), guildKey,
                                           *(unsigned int*)(pb + 0xe));
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x1663,"./log/GuildAgit",
                        "CPacketTranslater::OnDeleteGuildAgit : %d guild cargo is not empty",
                        guildKey);
                    CUser* user =
                        m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xe));
                    if (user == 0)
                    {
                        DNF_LOG_SCOPE_LINE(0x166b,"./log/GuildCargo",
                            "CPacketTranslater::OnDeleteGuildAgit : 0 == pclUser(%d)",
                            *(unsigned int*)(pb + 0xe));
                    }
                    else
                    {
                        Packet_Channel_Delete_Guild_Agit reply;
                        *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
                        *(unsigned int*)((char*)&reply + 0xe) = user->GetUniqCharNo();
                        *(unsigned int*)((char*)&reply + 0x12) = user->GetUniqCharNo();
                        *(unsigned char*)((char*)&reply + 0x16) = 0xd4;
                        user->SendToGameserver((char*)&reply, 0x17);
                    }
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x165b,"./log/GuildAgit",
                    "CPacketTranslater::OnDeleteGuildAgit : %d guild do not have guild agit",
                    guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1655,"./log/GuildAgit",
                "CPacketTranslater::OnDeleteGuildAgit : %d is not guild master(g:%d)",
                *(unsigned int*)(pb + 0xe), guildKey);
        }
    }
}

void CPacketTranslater::OnDBCreateGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1696, "./log/Except", "CPacketTranslater::OnDBCreateGuildAgit : 0 == m_pclApp");
        return;
    }
    if (*(int*)(pb + 0x12) == 0)
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x16a9, "./log/GuildAgit", "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
        }
        else
        {
            guild->SetGuildAgitFlag(true);
            guild->GetGuildCargo()->SetGuildInfo(guild->GetGuildKey());
            STGuildCargoDBInfo cargoInfo;
            memset(&cargoInfo, 0, 0x18dc);
            *(unsigned int*)((char*)&cargoInfo + 0x18d8) = 8;
            guild->GetGuildCargo()->SetGuildCargoDBInfo(cargoInfo);
            guild->NotifyCreateGuildAgitToGuildMember(*(unsigned int*)(pb + 0xe));
            guild->SendGuildAgitInfoToMembers();
            guild->SendGuildInfoToMembers(false);
        }
        return;
    }
    DNF_LOG_SCOPE_LINE(0x16a0,"./log/GuildAgit",
        "CPacketTranslater::OnDBCreateGuildAgit / Failure! Create Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
        *(unsigned int*)(pb + 0xa), *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0x12));
}

void CPacketTranslater::OnDBDeleteGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x16dd, "./log/Except", "CPacketTranslater::OnDBDeleteGuildAgit : 0 == m_pclApp");
        return;
    }
    if (*(int*)(pb + 0x12) == 0)
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x16ef, "./log/GuildAgit", "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
        }
        else
        {
            guild->SetGuildAgitFlag(false);
            guild->NotifyDeleteGuildAgitToGuildMember(*(unsigned int*)(pb + 0xe));
            guild->SendGuildInfoToMembers(false);
        }
        return;
    }
    DNF_LOG_SCOPE_LINE(0x16e6,"./log/GuildAgit",
        "CPacketTranslater::OnDBDeleteGuildAgit / Failure! Delete Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
        *(unsigned int*)(pb + 0xa), *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0x12));
}

void CPacketTranslater::OnPowerWarProcessInfo(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1717, "./log/Except", "CPacketTranslater::OnPowerWarProcessInfo : 0 == m_pclApp");
        return;
    }
    try
    {
        m_pclApp->GetPowerManager()->SendPowerWarProcessInfo(*(unsigned int*)(pb + 0xa));
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPowerWarProcessInfo Exception Break", 0x1722, 0x1727);
}

void CPacketTranslater::OnDBLoadGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x173a);
            log("./log/Except", "CPacketTranslater::OnDBLoadGuildAgit : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x1743);
            log("./log/GuildAgit", "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
        }
        else
        {
            guild->SetGuildAgitInfo(*(STGuildAgitDBInfo*)(pb + 0xe));
            guild->SendGuildAgitInfoToMembers();
        }
    }
    catch (std::exception& e)
    {
        CMyFileLog log("OnDBLoadGuildAgit", 0x174f);
        log("./log/Except",
            "CPacketTranslater::OnDBLoadGuildAgit Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1754);
        log("./log/Except",
            "CPacketTranslater::OnDBLoadGuildAgit Exception Break\n");
    }
}

void CPacketTranslater::OnUpgradeGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1767, "./log/Except", "CPacketTranslater::OnUpgradeGuildAgit : 0 == m_pclApp");
        return;
    }
    else
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(6000, "./log/GuildAgit", "CPacketTranslater::OnCreateGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(*(unsigned int*)(pb + 0xe)) == 1)
        {
            if (guild->IsExistGuildAgit() == 1)
            {
                if (guild->GetGuildFund() < *(unsigned int*)(pb + 0x16))
                {
                    DNF_LOG_SCOPE_LINE(0x1783,"./log/GuildAgit",
                        "CPacketTranslater::OnCreateGuildAgit : %d guild fund shortage",
                        guildKey);
                }
                else
                {
                    guild->UpgradeGuildAgit(m_pclApp->Get_ServerHandler(), guildKey,
                                            *(unsigned int*)(pb + 0xe),
                                            *(unsigned int*)(pb + 0x12),
                                            *(unsigned int*)(pb + 0x16));
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x177c,"./log/GuildAgit",
                    "CPacketTranslater::OnDeleteGuildAgit : %d guild already have guild agit",
                    guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1776,"./log/GuildAgit",
                "CPacketTranslater::OnCreateGuildAgit : %d is not guild master(g:%d)",
                *(unsigned int*)(pb + 0xe), guildKey);
        }
    }
}

void CPacketTranslater::OnDBUpgradeGuildAgit(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x17a8, "./log/Except", "CPacketTranslater::OnDBUpgradeGuildAgit : 0 == m_pclApp");
        return;
    }
    if (*(int*)(pb + 0x12) == 0)
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x17b9,"./log/GuildAgit",
                "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
        }
        else
        {
            guild->SetGuildAgitLevelUp();
            guild->SendGuildAgitInfoToMembers();
        }
        return;
    }
    DNF_LOG_SCOPE_LINE(0x17b2,"./log/GuildAgit",
        "CPacketTranslater::OnDBUpgradeGuildAgit / Failure! Upgrade Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
        *(unsigned int*)(pb + 0xa), *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0x12));
}

void CPacketTranslater::OnGuildExpLimit(PacketHeader* pkt)
{
    (void)pkt;
}

void CPacketTranslater::OnWriteGuildMemberMemo(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x17eb,"./log/GuildMember",
            "CPacketTranslater::OnWriteGuildMemberMemo : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa));
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x17f5, "./log/GuildMember", "CPacketTranslater::OnWriteGuildMemberMemo : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x17fc, "./log/GuildMember", "CPacketTranslater::OnWriteGuildMemberMemo : 0 == pclGuild");
        return;
    }
    Packet_DB_Write_Guild_Member_Memo dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xa) = *(unsigned int*)(pb + 0xa);
    *(unsigned int*)((char*)&dbPkt + 0xe) = guildKey;
    size_t len = strlen(pb + 0x12);
    memcpy((char*)&dbPkt + 0x12, pb + 0x12, len < 0x15 ? len : 0x14);
    m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
    guild->WriteGuildMemberMemo(user, pb + 0x12);
    guild->NotifyMemoToGuildMember(user, pb + 0x12);
}

void CPacketTranslater::OnLoadGuildCargo(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1831, "./log/GuildCargo", "CPacketTranslater::OnLoadGuildCargo : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1838, "./log/GuildCargo", "CPacketTranslater::OnLoadGuildCargo : 0 == pclGuild");
        return;
    }
    guild->GetGuildCargo()->SetGuildCargoDBInfo(*(STGuildCargoDBInfo*)(pb + 0xe));
}

void CPacketTranslater::OnLoadGuildCargoHistory(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1861,"./log/GuildCargo",
                "CPacketTranslater::OnLoadGuildCargoHistory : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild;
        if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1868,"./log/GuildCargo",
                "CPacketTranslater::OnLoadGuildCargoHistory : 0 == pclGuild");
            return;
        }
        guild->GetGuildCargo()->SetGuildCargoHistory(*(unsigned int*)(pb + 0xe),
                                                     (STGuildCargoLog*)(pb + 0x12));
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnLoadGuildCargoHistory", 0x1875);
        log("./log/Except",
            "CPacketTranslater::OnLoadGuildCargoHistory Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x187a);
        log("./log/Except", "CPacketTranslater::OnLoadGuildCargoHistory Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargo(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x188c, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == m_pclApp");
            return;
        }
        unsigned int charNo = *(unsigned int*)(pb + 0xa);
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1896, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == pclUser");
            return;
        }
        unsigned int guildKey = *(unsigned int*)(pb + 0xe);
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x189e, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == pclGuild");
            return;
        }
        Packet_Guild_Cargo_Response reply;
        *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0xe) = guildKey;
        CGuildCargo* cargo = guild->GetGuildCargo();
        if (cargo->IsLoadComplete() == 1)
        {
            unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
            if (grade == 3 || grade == 1 || grade == 2)
            {
                if (m_pclApp->Get_GuildManager()->IsCargoLock() == 0)
                {
                    *(unsigned char*)((char*)&reply + 0x18ee) = 0xc1;
                    memcpy((char*)&reply + 0x12, cargo->GetGuildCargoDBInfo(), 0x18dc);
                }
                else
                {
                    *(unsigned char*)((char*)&reply + 0x18ee) = 0xcc;
                    DNF_LOG_SCOPE_LINE(0x18c2, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo GUILD CARGO LOCKED!");
                }
                user->SendTcpGameserver(&reply);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x18b7,"./log/GuildCargo",
                    "CPacketTranslater::OnGuildCargo : Access Deny(%d,%d,%d)", charNo, guildKey,
                    (unsigned int)grade);
                *(unsigned char*)((char*)&reply + 0x18ee) = 0x24;
                user->SendTcpGameserver(&reply);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x18aa,"./log/GuildCargo", "CPacketTranslater::OnGuildCargo : Guild(%d,%d) Not Loaded",
                charNo, guildKey);
            *(unsigned char*)((char*)&reply + 0x18ee) = 0xc3;
            user->SendTcpGameserver(&reply);
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargo", 0x18d5);
        log("./log/Except",
            "CPacketTranslater::OnGuildCargo Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x18da);
        log("./log/Except", "CPacketTranslater::OnGuildCargo Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoHistory(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x18ec, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x18f6, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x18fe, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == pclGuild");
        return;
    }
    if (m_pclApp->Get_GuildManager()->IsCargoLock() == 0)
    {
        unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
        if (grade == 3 || grade == 1 || grade == 2)
        {
            Packet_Guild_Cargo_History_Response reply;
            *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
            *(unsigned int*)((char*)&reply + 0xe) = guildKey;
            guild->GetGuildCargo()->GetHistory((STGuildCargoLog*)((char*)&reply + 0x12));
            user->SendTcpGameserver(&reply);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1910,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoHistory : Access Deny(%d,%d,%d)", charNo,
                guildKey, (unsigned int)grade);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1906,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoHistory GUILD CARGO LOCKED!(%d,%d)", charNo,
            guildKey);
    }
}

void CPacketTranslater::OnGuildCargoCheckPushItem(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1935,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == m_pclApp");
            return;
        }
        Packet_Channel_Check_Guild_Cargo_Push_Item reply;
        unsigned int charNo = *(unsigned int*)(pb + 0xe);
        *(unsigned int*)((char*)&reply + 0xe) = charNo;
        *(unsigned int*)((char*)&reply + 0x13) = *(unsigned int*)(pb + 0x12);
        *(unsigned int*)((char*)&reply + 0x17) = *(unsigned int*)(pb + 0x16);
        *(unsigned int*)((char*)&reply + 0x1b) = *(unsigned int*)(pb + 0x1a);
        *(unsigned char*)((char*)&reply + 0x23) = *(unsigned char*)(pb + 0x22);
        *(unsigned int*)((char*)&reply + 0x1f) = *(unsigned int*)(pb + 0x1e);
        *(unsigned short*)((char*)&reply + 0x25) = *(unsigned short*)(pb + 0x24);
        *(unsigned char*)((char*)&reply + 0x24) = *(unsigned char*)(pb + 0x23);
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x194a,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == pclUser");
            return;
        }
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1952,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == pclGuild");
            return;
        }
        *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
        if (m_pclApp->Get_GuildManager()->IsCargoLock() != 0)
        {
            DNF_LOG_SCOPE_LINE(0x195c,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem GUILD CARGO LOCKED!(%d,%d)", charNo,
                guildKey);
            *(unsigned char*)((char*)&reply + 0x12) = 0xcc;
            user->SendTcpGameserver(&reply);
            return;
        }
        if (guild->IsExistGuildAgit() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x1967,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoPushItem : (%d,%d) No GuildAgit", charNo, guildKey);
            *(unsigned char*)((char*)&reply + 0x12) = 0xb5;
            user->SendTcpGameserver(&reply);
            return;
        }
        CGuildCargo* cargo = guild->GetGuildCargo();
        if (cargo->IsLoadComplete() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x1971,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoPushItem : Guild(%d,%d) Not Loaded", charNo,
                guildKey);
            *(unsigned char*)((char*)&reply + 0x12) = 0xc3;
            user->SendTcpGameserver(&reply);
            return;
        }
        unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
        if (grade == 3 || grade == 1 || grade == 2)
        {
            int result = cargo->CheckInsertItem(*(int*)(pb + 0x16), *(int*)(pb + 0x1a),
                                                *(int*)(pb + 0x12), *(unsigned char*)(pb + 0x22),
                                                *(int*)(pb + 0x1e));
            *(unsigned char*)((char*)&reply + 0x12) = (unsigned char)result;
            user->SendTcpGameserver(&reply);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x197f,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : Access Deny(%d,%d,%d)", charNo,
                guildKey, (unsigned int)grade);
            *(unsigned char*)((char*)&reply + 0x12) = 0x24;
            user->SendTcpGameserver(&reply);
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargoCheckPushItem", 0x1994);
        log("./log/Except",
            "CPacketTranslater::OnGuildCargoCheckPushItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1999);
        log("./log/Except", "CPacketTranslater::OnGuildCargoCheckPushItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoPushItem(PacketHeader* pkt)
{
    try
    {
        unsigned int group = *(unsigned int*)((char*)pkt + 0xa);
        unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xe);
        int slot = *(int*)((char*)pkt + 0x12);
        int count = *(int*)((char*)pkt + 0x16);
        unsigned char itemType = *(unsigned char*)((char*)pkt + 0x1a);
        DnfItemInfo* item = (DnfItemInfo*)((char*)pkt + 0x1b);
        unsigned char fst = *(unsigned char*)((char*)pkt + 0x50);
        unsigned short fsn = *(unsigned short*)((char*)pkt + 0x51);
        char* itemDesc = (char*)CGuildCargo::PrintDnfItemInfo(*item);
        CMyFileLog log0("OnGuildCargoPushItem", 0x19b3);
        log0("./log/GuildCargo", "PUSH ITEM(g:%d,cn:%d,sn:%d,sl:%d,fsn:%d,fst:%d,it:%d,%s)",
             group, guildKey, *(unsigned int*)((char*)pkt + 0x12),
             *(unsigned int*)((char*)pkt + 0x16), (unsigned int)fsn, (unsigned int)fst,
             (unsigned int)itemType, itemDesc);
        Packet_Channel_Guild_Cargo_Push_Item resp;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19c3, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == m_pclApp");
            return;
        }
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(group);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19cd, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == pclUser");
            return;
        }
        *(unsigned int*)((char*)&resp + 0xa) = user->GetIdByChannel();
        CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19d7, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == pclGuild");
            *(unsigned char*)((char*)&resp + 0x12) = 100;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        if (m_pclApp->Get_GuildManager()->IsCargoLock())
        {
            DNF_LOG_SCOPE_LINE(0x19e2,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem GUILD CARGO LOCKED!(%d,%d)",
                group, guildKey);
            *(unsigned char*)((char*)&resp + 0x12) = 0xcc;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        if (guild->IsExistGuildAgit() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x19ed,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : (%d,%d) No GuildAgit",
                group, guildKey);
            *(unsigned char*)((char*)&resp + 0x12) = 0xb5;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        CGuildCargo* cargo = guild->GetGuildCargo();
        if (cargo->IsLoadComplete() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x19f7,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : Guild(%d,%d) Not Loaded",
                group, guildKey);
            *(unsigned char*)((char*)&resp + 0x12) = 0xc3;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
        if (grade != 3 && grade != 1 && grade != 2)
        {
            DNF_LOG_SCOPE_LINE(0x1a05,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : Access Deny(%d,%d,%d)",
                guildKey, group, (unsigned int)grade);
            *(unsigned char*)((char*)&resp + 0x12) = 0x24;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        int result = cargo->InsertItem(*item, slot, count, itemType, (int)guildKey);
        *(unsigned char*)((char*)&resp + 0x12) = (unsigned char)result;
        *(int*)((char*)&resp + 0x16) = slot;
        if (result == 0xc1)
        {
            cargo->InsertHistory((ENUM_GUILD_CARGO_BEHAVIOR)1, (int)guildKey, user->GetCharName(),
                                 *(int*)((char*)pkt + 0x1c), *(int*)((char*)pkt + 0x21),
                                 (RandomOption*)((char*)pkt + 0x38));
            CServerHandler* handler = m_pclApp->Get_ServerHandler();
            cargo->SendHistoryToDBMW(handler, (ENUM_GUILD_CARGO_BEHAVIOR)1, (int)guildKey,
                                     user->GetCharName(), slot, 0, *item);
            cargo->SendGuildCargoToDBMW(handler, (int)guildKey);
            cargo->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)1);
        }
        user->SendTcpGameserver((PacketHeader*)&resp);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargoPushItem", 0x1a48);
        log("./log/Except",
            "CPacketTranslater::OnGuildCargoPushItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1a4d);
        log("./log/Except", "CPacketTranslater::OnGuidCargoPushItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoPopItem(PacketHeader* pkt)
{
    try
    {
    unsigned int group = *(unsigned int*)((char*)pkt + 0xa);
    unsigned int guildKey = *(unsigned int*)((char*)pkt + 0xe);
    int slot = *(int*)((char*)pkt + 0x12);
    int id = *(int*)((char*)pkt + 0x16);
    int count = *(int*)((char*)pkt + 0x1a);
    unsigned char itemType = *(unsigned char*)((char*)pkt + 0x1e);
    unsigned int uit = *(unsigned int*)((char*)pkt + 0x1f);
    CMyFileLog log0("OnGuildCargoPopItem", 0x1a65);
    log0("./log/GuildCargo", "POP ITEM(g:%d,cn:%d,sn:%d,id:%d,cnt:%d,it:%d,uit:%d)",
         group, guildKey, slot, id, count, (unsigned int)itemType, uit);
    Packet_Channel_Guild_Cargo_Pop_Item resp;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a74, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(group);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a7e, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == pclUser");
        return;
    }
    *(unsigned int*)((char*)&resp + 0xa) = user->GetIdByChannel();
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a88, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == pclGuild");
        *(unsigned char*)((char*)&resp + 0x12) = 100;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    if (m_pclApp->Get_GuildManager()->IsCargoLock())
    {
        DNF_LOG_SCOPE_LINE(0x1a92,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem GUILD CARGO LOCKED!(%d,%d)",
            group, guildKey);
        *(unsigned char*)((char*)&resp + 0x12) = 0xcc;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    if (guild->IsExistGuildAgit() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1a9d,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : (%d,%d) No GuildAgit",
            group, guildKey);
        *(unsigned char*)((char*)&resp + 0x12) = 0xb5;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    CGuildCargo* cargo = guild->GetGuildCargo();
    if (cargo->IsLoadComplete() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1aa7,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : Guild(%d,%d) Not Loaded",
            group, guildKey);
        *(unsigned char*)((char*)&resp + 0x12) = 0xc3;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
    if (grade != 3 && grade != 1 && grade != 2)
    {
        DNF_LOG_SCOPE_LINE(0x1ab5,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : Access Deny(%d,%d,%d)",
            guildKey, group, (unsigned int)grade);
        *(unsigned char*)((char*)&resp + 0x12) = 0x24;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    DnfItemInfo poppedItem;
    int result = cargo->DeleteItem(poppedItem, slot, count, itemType, id, (int)guildKey);
    *(unsigned char*)((char*)&resp + 0x12) = (unsigned char)result;
    if (result == 0xc1)
    {
        memcpy((char*)&resp + 0x16, &poppedItem, 0x35);
        cargo->InsertHistory((ENUM_GUILD_CARGO_BEHAVIOR)2, (int)guildKey, user->GetCharName(),
                             count, id, (RandomOption*)((char*)&poppedItem + 0x1d));
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        cargo->SendHistoryToDBMW(handler, (ENUM_GUILD_CARGO_BEHAVIOR)2, (int)guildKey,
                                 user->GetCharName(), slot, 0, poppedItem);
        cargo->SendGuildCargoToDBMW(handler, (int)guildKey);
        cargo->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)2);
    }
    user->SendTcpGameserver((PacketHeader*)&resp);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargoPopItem", 0x1af8);
        log("./log/Except",
            "CPacketTranslater::OnGuildCargoPopItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1afd);
        log("./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoMoveItem(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    Packet_Channel_Guild_Cargo_Move_Item reply;
    unsigned int charNo = *(unsigned int*)(pb + 0xe);
    unsigned int fromSlot = *(unsigned int*)(pb + 0x12);
    unsigned int toSlot = *(unsigned int*)(pb + 0x16);
    {
        DNF_LOG_SCOPE_LINE(0x1b15,"./log/GuildCargo", "MOVE ITEM(g:%d,cn:%d,fsn:%d,dsn:%d",
            *(unsigned int*)(pb + 0xa), charNo, fromSlot, toSlot);
    }
    *(unsigned int*)((char*)&reply + 0xe) = charNo;
    *(unsigned int*)((char*)&reply + 0x13) = fromSlot;
    *(unsigned int*)((char*)&reply + 0x17) = toSlot;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b21,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b2b, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoMoveItem : 0 == pclUser");
        return;
    }
    *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xa));
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b35, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoMoveItem : 0 == pclGuild");
        *(unsigned char*)((char*)&reply + 0x12) = 100;
        user->SendTcpGameserver(&reply);
        return;
    }
    if (m_pclApp->Get_GuildManager()->IsCargoLock() != 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b40,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem GUILD CARGO LOCKED!(%d,%d)",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0xa));
        *(unsigned char*)((char*)&reply + 0x12) = 0xcc;
        user->SendTcpGameserver(&reply);
        return;
    }
    if (guild->IsExistGuildAgit() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1b4b,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : (%d,%d) No GuildAgit",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0xa));
        *(unsigned char*)((char*)&reply + 0x12) = 0xb5;
        user->SendTcpGameserver(&reply);
        return;
    }
    CGuildCargo* cargo = guild->GetGuildCargo();
    if (cargo->IsLoadComplete() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1b55,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : Guild(%d,%d) Not Loaded",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0xa));
        *(unsigned char*)((char*)&reply + 0x12) = 0xc3;
        user->SendTcpGameserver(&reply);
        return;
    }
    unsigned char grade = *(unsigned char*)((char*)user->GetGuildMemDBInfo() + 0x15);
    if (grade == 3 || grade == 1 || grade == 2)
    {
        DnfItemInfo fromItem;
        DnfItemInfo toItem;
        int result = cargo->MoveItem(fromItem, toItem, *(int*)(pb + 0x12),
                                     *(int*)(pb + 0x1a), *(int*)(pb + 0x16),
                                     *(int*)(pb + 0x1e), *(int*)(pb + 0xe));
        *(unsigned char*)((char*)&reply + 0x12) = (unsigned char)result;
        if (result == 0xc1)
        {
            cargo->SendGuildCargoToDBMW(m_pclApp->Get_ServerHandler(), *(int*)(pb + 0xe));
            cargo->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)3);
        }
        user->SendTcpGameserver(&reply);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1b63,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : Access Deny(%d,%d,%d)",
            *(unsigned int*)(pb + 0xe), *(unsigned int*)(pb + 0xa), (unsigned int)grade);
        *(unsigned char*)((char*)&reply + 0x12) = 0x24;
        user->SendTcpGameserver(&reply);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargoMoveItem", 0x1bb6);
        log("./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1bbb);
        log("./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoUpgrade(PacketHeader* pkt)
{
    try
    {
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1bd1, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xe);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1bdb, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1be2, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == pclGuild");
        return;
    }
    if (guild->IsGuildMaster(charNo) == 1)
    {
        if (guild->IsExistGuildAgit() == 1)
        {
            int curCapacity = guild->GetGuildCargo()->GetCapacity();
            if (*(int*)(pb + 0x12) == curCapacity)
            {
                unsigned int reqFund = *(unsigned int*)(pb + 0x1a);
                unsigned int guildFund = guild->GetGuildFund();
                if (guildFund < reqFund)
                {
                    DNF_LOG_SCOPE_LINE(0x1c00,"./log/GuildCargo",
                        "CPacketTranslater::OnGuildCargoUpgrade guild fund shortage(c:%d,n:%d, gcapa:%d) Error! Guild(%d), User(%d), ReqFund(%d), CurrFund(%d)",
                        *(unsigned int*)(pb + 0x12), *(unsigned int*)(pb + 0x16), curCapacity,
                        guildKey, charNo, reqFund, guildFund);
                }
                else
                {
                    guild->SubGuildFund(reqFund);
                    guild->SendGuildInfoToMembers(true);
                    guild->GetGuildCargo()->SetCapacity(*(unsigned int*)(pb + 0x16));
                    CTcpDBServer* tcpDb = m_pclApp->Get_ServerHandler()->GetTcpDBServer();
                    char* buf = tcpDb->makePacketHeader(0x714, 0x16);
                    *(unsigned int*)(buf + 0xa) = guildKey;
                    *(unsigned int*)(buf + 0xe) = charNo;
                    *(unsigned int*)(buf + 0x12) = *(unsigned int*)(pb + 0x16);
                    tcpDb->SendToServer(buf);
                    Packet_Channel_Guild_Cargo_Upgrade reply;
                    *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
                    *(unsigned int*)((char*)&reply + 0xe) = charNo;
                    *(unsigned char*)((char*)&reply + 0x12) = 0xc1;
                    user->SendToGameserver((char*)&reply, 0x13);
                    guild->GetGuildCargo()->SendGuildCargo(user);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x1bf7,"./log/GuildCargo",
                    "CPacketTranslater::OnGuildCargoUpgrade Capacity(c:%d,n:%d, gcapa:%d) Error! Guild(%d), User(%d)",
                    *(unsigned int*)(pb + 0x12), *(unsigned int*)(pb + 0x16), curCapacity,
                    guildKey, charNo);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1bf0,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoUpgrade : %d guild agit error", guildKey);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1be9,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoUpgrade : %d is not guild master(g:%d)", charNo,
            guildKey);
    }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildCargoUpgrade", 0x1c34);
        log("./log/Except", "CPacketTranslater::OnGuildApplyOriginalPowerSide Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1c39);
        log("./log/Except", "CPacketTranslater::OnGuildApplyOriginalPowerSide Exception Break\n");
    }
}

void CPacketTranslater::OnGuildRequestGuildBoardOpen(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c4a,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xf));
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c54,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xb);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c5b,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == pclGuild");
        return;
    }
    CGuildBoard* board = guild->GetGuildBoard();
    bool sendNow = false;
    if (board->isWebGuildBoardAction() == 0 && (unsigned char)pb[10] == 0)
    {
        if (board->getGuildBoardDBLoadState() == 0)
        {
            sendNow = true;
        }
    }
    else
    {
        sendNow = true;
    }
    if (!sendNow)
    {
        board->sendGuildBoardData(*(unsigned int*)(pb + 0xb), *(unsigned int*)(pb + 0xf),
                                  0x232a, user);
        return;
    }
    if (board->getGuildBoardDBLoadState() == 0)
    {
        Packet_DB_Load_Request_Guild_Board_Open dbPkt;
        *(unsigned int*)((char*)&dbPkt + 0xa) = *(unsigned int*)(pb + 0xb);
        *(unsigned int*)((char*)&dbPkt + 0xe) = *(unsigned int*)(pb + 0xf);
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
        board->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)1);
        return;
    }
    board->setWebGuildBoardAction(false);
    bool needDB = false;
    if (board->getGuildBoardDBLoadState() == 2 && board->isGuildBoardDBAccess() != 0)
    {
        needDB = true;
    }
    if (needDB)
    {
        Packet_DB_Load_Request_Guild_Board_Open dbPkt;
        *(unsigned int*)((char*)&dbPkt + 0xa) = *(unsigned int*)(pb + 0xb);
        *(unsigned int*)((char*)&dbPkt + 0xe) = *(unsigned int*)(pb + 0xf);
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
        board->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)1);
    }
    else
    {
        board->sendGuildBoardData(*(unsigned int*)(pb + 0xb), *(unsigned int*)(pb + 0xf),
                                  0x232a, user);
    }
}

void CPacketTranslater::OnDBLoadReplyGuildBoardOpen(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c9d,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0x11);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1ca7,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xd);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1cae,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == pclGuild");
        return;
    }
    CGuildBoard* board = guild->GetGuildBoard();
    board->setGuildBoardData(guildKey, charNo, guild, (int)(char)pb[0x15],
                             (STGuildBoardDBInfo*)(pb + 0x16));
    if (pb[0xc] != 0)
    {
        board->sendGuildBoardData(guildKey, charNo, 0x232a, user);
        board->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)2);
        board->setGuildBoardDBAccess();
    }
}

void CPacketTranslater::OnGuildRequestGuildBoardWrite(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1cd3,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa));
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1cdd,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == pclUser");
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1ce4,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == pclGuild");
        return;
    }
    Packet_DB_Load_Request_Guild_Board_Write dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xb) = *(unsigned int*)(pb + 0xa);
    *(unsigned int*)((char*)&dbPkt + 0xf) = *(unsigned int*)(pb + 0xe);
    *(unsigned int*)((char*)&dbPkt + 0x13) = *(unsigned int*)(pb + 0x12);
    memcpy((char*)&dbPkt + 0x17, pb + 0x18, 0x78);
    *(unsigned char*)((char*)&dbPkt + 0x9b) = *(unsigned char*)(pb + 0x16);
    *(unsigned char*)((char*)&dbPkt + 0x9c) = *(unsigned char*)(pb + 0x17);
    memcpy((char*)&dbPkt + 0x9e, user->GetCharName(), 0x1e);
    m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
}

void CPacketTranslater::OnDBLoadReplyGuildBoardWrite(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d0c,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0x10);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d16,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xc);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d1d,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == pclGuild");
        return;
    }
    if (*(short*)(pb + 0xa) == 0)
    {
        CGuildBoard* board = guild->GetGuildBoard();
        board->setGuildBoardData(guildKey, charNo, guild, 1,
                                 (STGuildBoardDBInfo*)(pb + 0x14));
        if (*(int*)(pb + 0x94) != 0)
        {
            board->sendGuildBoardData(guildKey, charNo, 0x232e, user);
        }
    }
    else
    {
        Packet_Guild_Reply_Guild_Board reply;
        *(unsigned short*)((char*)&reply + 0xa) = *(unsigned short*)(pb + 0xa);
        *(unsigned short*)((char*)&reply + 0xc) = 0x232e;
        *(unsigned int*)((char*)&reply + 0xf) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0x13) = user->GetUniqCharNo();
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_LINE(0x1d37,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : Write Fail!!");
    }
}

void CPacketTranslater::OnGuildRequestGuildBoardDelete(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d52,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == m_pclApp");
        return;
    }
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa));
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d5c,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == pclUser");
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d63,"./log/GuildBoard",
            "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == pclGuild");
        return;
    }
    Packet_DB_Load_Request_Guild_Board_Delete dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xb) = *(unsigned int*)(pb + 0xa);
    *(unsigned int*)((char*)&dbPkt + 0xf) = *(unsigned int*)(pb + 0xe);
    *(unsigned int*)((char*)&dbPkt + 0x13) = *(unsigned int*)(pb + 0x12);
    m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
}

void CPacketTranslater::OnDBLoadReplyGuildBoardDelete(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d84,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0x10);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d8e,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xc);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d95,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == pclGuild");
        return;
    }
    if (*(short*)(pb + 0xa) == 0)
    {
        guild->GetGuildBoard()->deleteGuildBoardData(*(unsigned int*)(pb + 0x14), guildKey,
                                                     charNo);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1d9e,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : Delete Fail!!");
    }
    Packet_Guild_Reply_Guild_Board_Delete reply;
    *(unsigned short*)((char*)&reply + 0xa) = *(unsigned short*)(pb + 0xa);
    *(unsigned int*)((char*)&reply + 0xc) = user->GetIdByChannel();
    *(unsigned int*)((char*)&reply + 0x10) = user->GetUniqCharNo();
    user->SendTcpGameserver(&reply);
}

void CPacketTranslater::OnWebGuildBoardWrite(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    unsigned int charNo = *(unsigned int*)(pb + 0xe);
    unsigned int no = *(unsigned int*)(pb + 0x12);
    {
        DNF_LOG_SCOPE_LINE(0x1dbe,"./log/GuildBoard",
            "CPacketTranslater::OnWebGuildBoardWrite Receive Data: GuildKey : %u, CharacID : %u, NO : %u",
            guildKey, charNo, no);
    }
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1dc2, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardWrite : 0 == m_pclApp");
        return;
    }
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1dc9, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardWrite : 0 == pclGuild");
        return;
    }
    Packet_DB_Load_Request_Web_Guild_Board_Write dbPkt;
    *(unsigned int*)((char*)&dbPkt + 0xa) = guildKey;
    *(unsigned int*)((char*)&dbPkt + 0xe) = charNo;
    *(unsigned int*)((char*)&dbPkt + 0x12) = no;
    m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
}

void CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1ded,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xc);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1df4,"./log/GuildBoard",
            "CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite : 0 == pclGuild");
        return;
    }
    if (*(short*)(pb + 0xa) == 0)
    {
        guild->GetGuildBoard()->setGuildBoardData(guildKey, *(unsigned int*)(pb + 0x10),
                                                  guild, 1,
                                                  (STGuildBoardDBInfo*)(pb + 0x14));
        guild->GetGuildBoard()->setWebGuildBoardAction(true);
    }
}

void CPacketTranslater::OnWebGuildBoardDelete(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        unsigned int charNo = *(unsigned int*)(pb + 0xe);
        unsigned int no = *(unsigned int*)(pb + 0x12);
        {
            DNF_LOG_SCOPE_LINE(0x1e15,"./log/GuildBoard",
                "CPacketTranslater::OnWebGuildBoardDelete Receive Data: GuildKey : %u, CharacID : %u, NO : %u",
                guildKey, charNo, no);
        }
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e19, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardDelete : 0 == m_pclApp");
            return;
        }
        CGuild* guild;
        if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e20, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardDelete : 0 == pclGuild");
            return;
        }
        guild->GetGuildBoard()->deleteGuildBoardData(no, guildKey, charNo);
        guild->GetGuildBoard()->setWebGuildBoardAction(true);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnWebGuildBoardDelete", 0x1e29);
        log("./log/Except",
            "CPacketTranslater::OnWebGuildBoardDelete Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1e2e);
        log("./log/Except", "CPacketTranslater::OnWebGuildBoardDelete Exception Break\n");
    }
}

void CPacketTranslater::OnGuildApplyOriginalPowerSide(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1e44,"./log/Power",
            "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xa);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1e4e, "./log/Power", "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == pclUser");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xe);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1e55, "./log/Power", "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == pclGuild");
        return;
    }
    unsigned char side = (unsigned char)pb[0x12];
    if (!((side == 3 || side == 4) && side == guild->GetPowerSide()))
    {
        DNF_LOG_SCOPE_LINE(0x1e5d,"./log/Power",
            "CPacketTranslater::OnGuildApplyOriginalPowerSide Guild(%d), GuildOrigPowerSide(%d), PacketPowerSide(%d)",
            guildKey, (unsigned int)guild->GetPowerSide() & 0xff, (unsigned int)side);
        return;
    }
    unsigned char newSide = side == 3 ? 1 : 2;
    guild->SetPowerSide(newSide);
    CServerInterface* gs = user->GetGameServer();
    guild->DBGuildSave(gs->GetGroupNo(), m_pclApp->Get_ServerHandler(), 0);
    guild->SendGuildInfoToMembers(false);
    Packet_Guild_Apply_Origial_Power_Side_Reply reply;
    *(unsigned int*)((char*)&reply + 0xa) = user->GetIdByChannel();
    *(unsigned int*)((char*)&reply + 0xe) = charNo;
    *(unsigned int*)((char*)&reply + 0x12) = guildKey;
    *(unsigned char*)((char*)&reply + 0x16) = newSide;
    user->SendToGameserver((char*)&reply, 0x17);
}

void CPacketTranslater::OnNotifyNewGroupMail(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        CUser* user = 0;
        Packet_Monitor_Notify_New_Mail mail;
        for (int i = 0; i < *(int*)(pb + 0xa); i++)
        {
            user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + i * 4 + 0xe));
            if (user != 0)
            {
                *(unsigned int*)((char*)&mail + 0xa) = *(unsigned int*)(pb + i * 4 + 0xe);
                *(unsigned int*)((char*)&mail + 0xe) = user->GetIdByChannel();
                user->SendToGameserver((char*)&mail, 0x12);
            }
        }
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnNotifyNewGroupMail", 0x1e9f);
        log("./log/Except",
            "CPacketTranslater::OnNotifyNewGroupMail() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x1ea4);
        log("./log/Except", "CPacketTranslater::OnNotifyNewGroupMail() Exception Break\n");
    }
}

void CPacketTranslater::OnGameServerRegist(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    stServerInfo info;
    memset(&info, 0, sizeof(info));
    info.m_group = (unsigned char)pb[0xb];
    info.m_field1 = (unsigned char)pb[0xc];
    info.m_field2 = (unsigned char)pb[10];
    *(unsigned short*)((char*)&info + 0x14) = *(unsigned short*)(pb + 0x1d);
    strncpy(info.m_name, pb + 0xd, 0x10);
    CServerHandler* handler = m_pclApp->Get_ServerHandler();
    CTcpGameServer* tgs = handler->GetTcpGameServer(*(unsigned int*)(pb + 6));
    if (tgs != 0)
    {
        DNF_LOG_SCOPE_LINE(0x1ec6,"./log/GameServer", "Get Packet - OnGameServerRegist from Channel:%d",
            (unsigned int)(unsigned char)info.m_field1);
        if (tgs->GetChannelNo() == 0)
        {
            char* reply = tgs->makePacketHeader(0x1f42, 0xc);
            if (reply != 0)
            {
                if (handler->RegistGameServer(&info) == 1)
                {
                    tgs->SetChannelNo((unsigned char)pb[0xc]);
                    CGameServer* gs = handler->GetGameServer(*(unsigned int*)(pb + 6));
                    gs->SetSocket(*(unsigned int*)(pb + 6));
                    reply[0xb] = 0;
                    DNF_LOG_SCOPE_LINE(0x1eeb,"./log/GameServer", "Game server regist success. Channel: %d",
                        (unsigned int)(unsigned char)info.m_field1);
                }
                else
                {
                    reply[0xb] = 1;
                    DNF_LOG_SCOPE_LINE(0x1ede,"./log/GameServer",
                        "Game server regist failed. Channel: %d is already exist.",
                        (unsigned int)(unsigned char)info.m_field1);
                }
                reply[10] = 1;
                tgs->SendToGameServer(reply);
            }
        }
    }
}

void CPacketTranslater::OnAddGuildFund(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f03, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f09, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
        return;
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f0f, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pGuild");
        return;
    }
    if ((unsigned char)pb[10] == 1)
    {
        int result = guild->IsAddableGuildFund(*(unsigned int*)(pb + 0xb)) ? 1 : 0;
        if (result == 0)
        {
            guild->AddGuildFund(*(unsigned int*)(pb + 0xb));
            guild->GetGuildBoard()->sendMessageToDBMW_GuildFund(m_pclApp->Get_ServerHandler(),
                                                                *(int*)(pb + 0xb), user);
        }
        Packet_Add_Guild_Fund_Result reply;
        *(unsigned char*)((char*)&reply + 0xa) = (unsigned char)result;
        *(unsigned int*)((char*)&reply + 0xb) = *(unsigned int*)(pb + 0xf);
        *(unsigned int*)((char*)&reply + 0xf) = user->GetIdByChannel();
        user->SendToGameserver((char*)&reply, 0x17);
        guild->SendGuildInfoToManagers();
    }
    else if ((unsigned char)pb[10] == 2)
    {
        *(unsigned int*)((char*)guild->GetGuildDBInfo() + 0xa0) = *(unsigned int*)(pb + 0xb);
        guild->SendGuildInfoToMemberOnly(user);
    }
    else
    {
        guild->AddGuildFund(*(unsigned int*)(pb + 0xb));
    }
}

void CPacketTranslater::OnRefreshGuildInfo(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f4b, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = m_pclApp->Get_UserManager()->FindUser_CharNo(*(unsigned int*)(pb + 0xa))) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f51, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
        return;
    }
    CGuild* guild = m_pclApp->Get_GuildManager()->FindGuild(*(unsigned int*)(pb + 0xe));
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f57, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pGuild");
        return;
    }
    guild->SendGuildInfoToMemberOnly(user);
}

void CPacketTranslater::OnReplyTodayGuildMember(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f75, "./log/Guild", "CPacketTranslater::OnReplyTodayGuildMember : 0 == m_pclApp");
        return;
    }
    try
    {
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild;
        if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f7b, "./log/Guild", "CPacketTranslater::OnReplyTodayGuildMember : 0 == pGuild");
            return;
        }
        m_pclApp->Get_GuildManager()->InsertTodayMember(guildKey,
                                                       *(STTodayGuildMember*)(pb + 0xe));
        guild->SetTodayGuildMember(*(STTodayGuildMember*)(pb + 0xe));
        guild->NotifyAllTodayGuildMember();
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnReplyTodayGuildMember Exception Break", 0x1f8a, 0x1f8f);
}

void CPacketTranslater::OnApproveJoinGuild(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1fa0, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) != 0)
    {
        unsigned int charNo = *(unsigned int*)(pb + 0xe);
        if (guild->IsGuildMaster(charNo) == 1 || guild->IsSubGuildMaster(charNo) == 1)
        {
            Packet_DB_Request_Approve_Join_Guild dbPkt;
            *(unsigned int*)((char*)&dbPkt + 0xa) = guildKey;
            *(unsigned int*)((char*)&dbPkt + 0xe) = charNo;
            *(unsigned int*)((char*)&dbPkt + 0x12) = *(unsigned int*)(pb + 0x12);
            *(unsigned char*)((char*)&dbPkt + 0x16) = m_pclApp->Get_ServerGroup();
            m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1fad, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild authority error");
        }
    }
}

void CPacketTranslater::OnDBResponseApproveJoinGuild(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1fd1, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1fd7, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == pGuild");
        return;
    }
    unsigned int requesterNo = *(unsigned int*)(pb + 0x16);
    CUser* requester = m_pclApp->Get_UserManager()->FindUser_CharNo(requesterNo);
    if (requester == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1fdd, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
        return;
    }
    unsigned int joinerNo = *(unsigned int*)(pb + 0x12);
    CUser* joiner = m_pclApp->Get_UserManager()->FindUser_CharNo(joinerNo);
    Packet_Approve_Join_Guild_Result reply;
    *(unsigned int*)((char*)&reply + 0xa) = guildKey;
    *(unsigned int*)((char*)&reply + 0xe) = requester->GetUniqCharNo();
    *(unsigned int*)((char*)&reply + 0x12) = requester->GetIdByChannel();
    *(unsigned int*)((char*)&reply + 0x16) = *(unsigned int*)(pb + 0x16);
    if (guildKey == 0)
    {
        if (joiner == 0)
        {
            GuildJoin(guild, (STGuildJoinInfo*)(pb + 0x1a), joinerNo);
        }
        else
        {
            GuildJoin(guild, joiner, joinerNo);
        }
    }
    requester->SendToGameserver((char*)&reply, 0x1a);
}

void CPacketTranslater::OnGuildAttendanceInfo(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x200e, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = *(unsigned int*)(pb + 0xa);
        CGuild* guild;
        if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x2014, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == pGuild");
            return;
        }
        unsigned int charNo = *(unsigned int*)(pb + 0xe);
        CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x201a, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == pUser");
            return;
        }
        Packet_Guild_Attendance_Info_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = user->GetUniqCharNo();
        *(unsigned int*)((char*)&reply + 0xe) = user->GetIdByChannel();
        *(unsigned char*)((char*)&reply + 0x12) = *(unsigned char*)(pb + 0x12);
        m_pclApp->Get_GuildManager()->GetAttendanceInfo(
            guildKey, *(STAttendanceInfo*)((char*)&reply + 0x13));
        user->SendToGameserver((char*)&reply, 0x2f);
    }
    catch (CDNFException& e)
    {
        CMyFileLog log("OnGuildAttendanceInfo", 0x2029);
        log("./log/Except",
            "CPacketTranslater::OnApproveJoinGuild Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog log(__FUNCTION__, 0x202e);
        log("./log/Except", "CPacketTranslater::OnApproveJoinGuild Exception Break\n");
    }
}

void CPacketTranslater::OnGuildDebug(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x2040, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == m_pclApp");
        return;
    }
    unsigned int guildKey = *(unsigned int*)(pb + 0xa);
    CGuild* guild;
    if ((guild = m_pclApp->Get_GuildManager()->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x2046, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == pGuild");
        return;
    }
    unsigned int charNo = *(unsigned int*)(pb + 0xe);
    CUser* user = m_pclApp->Get_UserManager()->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x204c, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == pUser");
    }
    else if (*(int*)(pb + 0x12) == 0)
    {
        m_pclApp->Get_GuildManager()->RefreshAttendanceInfo(true);
    }
    else if (*(int*)(pb + 0x12) == 1)
    {
        m_pclApp->Get_GuildManager()->RefreshTodayMember(true);
    }
}

void CPacketTranslater::OnRenew_GM_List(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && pb != 0)
    {
        if (((GuildPacketBodyView*)pb)->m_field_a == 0)
        {
            gm->clearGmList();
        }
        for (int i = 0; i < (char)((GuildPacketBodyView*)pb)->m_field_b; i++)
        {
            gm->AppendGM_Sys(*(unsigned int*)(pb + i * 4 + 0xc), (char)pb[i + 0x5c]);
        }
    }
}

Packet_Tcp_Server_Connect::Packet_Tcp_Server_Connect()
    : PacketHeader(0x3f8, 0xb)
{
    (void)0;
}

#pragma pack(push,1)
struct Packet_Monitor_UDP_User_Getout_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
};
#pragma pack(pop)
Packet_Monitor_UDP_User_Getout::Packet_Monitor_UDP_User_Getout()
    : PacketHeader(0x3ee, 0xe)
{
    ((Packet_Monitor_UDP_User_Getout_Layout*)this)->ma = 0;
}

Packet_Monitor_Notice_Guild_Create_ToUser::Packet_Monitor_Notice_Guild_Create_ToUser()
    : PacketHeader(0x40c, 0x2d), m_member(0xffffffff)
{
}

Packet_Monitor_Call_Guild_Info_Reply::Packet_Monitor_Call_Guild_Info_Reply()
    : PacketHeader(0x40d, 0xad6)
{
    *(unsigned int*)((char*)this + 0x10) = 0;
    *(unsigned int*)((char*)this + 0xe) = 0;
    *(unsigned char*)((char*)this + 0x12) = 0x0;
    for (int i = 0; i < 0x5f; i++) { new ((char*)this + 0x13 + i * 0x1d) STGuildCallInfo; }
}

#pragma pack(push,1)
struct Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
    unsigned int mb;
    char pad0xf[0x23];
    unsigned char m32;
};
#pragma pack(pop)
Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade::
    Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade()
    : PacketHeader(0x42b, 0x33)
{
    ((Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade_Layout*)this)->ma = 0;
    ((Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade_Layout*)this)->mb = 0;
    ((Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade_Layout*)this)->m32 = 255;
    memset((char*)this + 0x14, 0, 0x1e);
}

#pragma pack(push,1)
struct Packet_Monitor_Notify_GuildMemberGrade_Layout
{
    char pad0x0[0x12];
    unsigned char m12;
};
#pragma pack(pop)
Packet_Monitor_Notify_GuildMemberGrade::Packet_Monitor_Notify_GuildMemberGrade()
    : PacketHeader(0x42c, 0x13)
{
    ((Packet_Monitor_Notify_GuildMemberGrade_Layout*)this)->m12 = 255;
}

#pragma pack(push,1)
struct Packet_Monitor_Set_Sub_Guild_Master_Reply_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
};
#pragma pack(pop)
Packet_Monitor_Set_Sub_Guild_Master_Reply::Packet_Monitor_Set_Sub_Guild_Master_Reply()
    : PacketHeader(0x430, 0x3a)
{
    ((Packet_Monitor_Set_Sub_Guild_Master_Reply_Layout*)this)->ma = 0;
}

#pragma pack(push,1)
struct Packet_DB_Write_Guild_Member_Memo_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Write_Guild_Member_Memo::Packet_DB_Write_Guild_Member_Memo()
    : PacketHeader(0x4d3, 0x27)
{
    ((Packet_DB_Write_Guild_Member_Memo_Layout*)this)->ma = 0;
    ((Packet_DB_Write_Guild_Member_Memo_Layout*)this)->me = 0;
    memset((char*)this + 0x12, 0, 0x15);
}

#pragma pack(push,1)
struct Packet_Web_Notify_Message_To_Guild_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned char me;
};
#pragma pack(pop)
Packet_Web_Notify_Message_To_Guild::Packet_Web_Notify_Message_To_Guild()
    : PacketHeader(0x42d, 0x74)
{
    ((Packet_Web_Notify_Message_To_Guild_Layout*)this)->ma = 0;
    ((Packet_Web_Notify_Message_To_Guild_Layout*)this)->me = 0;
    memset((char*)this + 0xf, 0, 0x65);
}

Packet_Notice_DB_Guild_War_End::Packet_Notice_DB_Guild_War_End()
    : PacketHeader(0x412, 0x6f)
{
    memset((char*)this + 0xb, 0, 0x28);
    memset((char*)this + 0x33, 0, 0x28);
    memset((char*)this + 0x5b, 0, 0x14);
}

#pragma pack(push,1)
struct Packet_Channel_Delete_Guild_Agit_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned char m16;
};
#pragma pack(pop)
Packet_Channel_Delete_Guild_Agit::Packet_Channel_Delete_Guild_Agit()
    : PacketHeader(0x6de, 0x17)
{
    ((Packet_Channel_Delete_Guild_Agit_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Delete_Guild_Agit_Layout*)this)->me = 0;
    ((Packet_Channel_Delete_Guild_Agit_Layout*)this)->m12 = 0;
    ((Packet_Channel_Delete_Guild_Agit_Layout*)this)->m16 = 0;
}

#pragma pack(push,1)
struct Packet_Guild_Apply_Origial_Power_Side_Reply_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned char m16;
};
#pragma pack(pop)
Packet_Guild_Apply_Origial_Power_Side_Reply::Packet_Guild_Apply_Origial_Power_Side_Reply()
    : PacketHeader(0x6b2, 0x17)
{
    ((Packet_Guild_Apply_Origial_Power_Side_Reply_Layout*)this)->ma = 0;
    ((Packet_Guild_Apply_Origial_Power_Side_Reply_Layout*)this)->me = 0;
    ((Packet_Guild_Apply_Origial_Power_Side_Reply_Layout*)this)->m12 = 0;
    ((Packet_Guild_Apply_Origial_Power_Side_Reply_Layout*)this)->m16 = 0;
}

Packet_Answer_Join_Power::Packet_Answer_Join_Power()
    : PacketHeader(0x6a8, 0x17)
{
    (void)0;
}

Packet_Answer_Secede_Power::Packet_Answer_Secede_Power()
    : PacketHeader(0x6a9, 0x17)
{
    (void)0;
}

Packet_Monitor_Notify_New_Mail::Packet_Monitor_Notify_New_Mail()
    : PacketHeader(0x514, 0x12)
{
    (void)0;
}

Packet_Register_To_BlackList_RESULT::Packet_Register_To_BlackList_RESULT()
    : PacketHeader(0x5dc, 0x31)
{
    memset((char*)this + 0xe, 0, 0x1e);
}

Packet_Delete_To_BlackList_Result::Packet_Delete_To_BlackList_Result()
    : PacketHeader(0x5dd, 0x31)
{
    memset((char*)this + 0xe, 0, 0x1e);
}

Packet_Request_Result_BlackList::Packet_Request_Result_BlackList()
    : PacketHeader(0x5e0, 0x19f)
{
    for (int i = 0; i < 9; ++i)
        new ((char*)this + 0xf + i * 0x28) STBlackUserDBType;
}

#pragma pack(push,1)
struct Packet_DBMW_Register_To_BlackList_Layout
{
    char pad0x0[0x2c];
    unsigned int m2c;
};
#pragma pack(pop)
Packet_DBMW_Register_To_BlackList::Packet_DBMW_Register_To_BlackList()
    : PacketHeader(0x5de, 0x34)
{
    ((Packet_DBMW_Register_To_BlackList_Layout*)this)->m2c = 4294967295;
    memset((char*)this + 0xe, 0, 0x1e);
}

#pragma pack(push,1)
struct Packet_DMBW_Delete_To_BlackList_Layout
{
    char pad0x0[0x2c];
    unsigned int m2c;
};
#pragma pack(pop)
Packet_DMBW_Delete_To_BlackList::Packet_DMBW_Delete_To_BlackList()
    : PacketHeader(0x5df, 0x30)
{
    ((Packet_DMBW_Delete_To_BlackList_Layout*)this)->m2c = 4294967295;
    memset((char*)this + 0xe, 0, 0x1e);
}

Packet_DBMW_Request_BlackList::Packet_DBMW_Request_BlackList()
    : PacketHeader(0x5e1, 0xf)
{
    (void)0;
}

Packet_Monitor_Reply_Guild_Mail::Packet_Monitor_Reply_Guild_Mail()
    : PacketHeader(0x432, 0x13)
{
    (void)0;
}

Packet_DBMW_Send_Guild_Mail::Packet_DBMW_Send_Guild_Mail()
    : PacketHeader(0x432, 0x129)
{
    memset((char*)this + 0x113, 0, 0x11);
    memset((char*)this + 0x13, 0, 0x100);
}

Packet_Guild_Call_Guild_Invite_To_Caller::Packet_Guild_Call_Guild_Invite_To_Caller()
    : PacketHeader(0x434, 0x16)
{
    (void)0;
}

Packet_Guild_Call_Guild_Invite_To_Invited::Packet_Guild_Call_Guild_Invite_To_Invited()
    : PacketHeader(0x435, 0x47)
{
    memset((char*)this + 0x12, 0, 0x1e);
    memset((char*)this + 0x30, 0, 0x17);
}

Packet_Guild_Reply_Guild_Invite_To_Invited::Packet_Guild_Reply_Guild_Invite_To_Invited()
    : PacketHeader(0x436, 0x16)
{
    (void)0;
}

Packet_Guild_Reply_Guild_Invite_To_Caller::Packet_Guild_Reply_Guild_Invite_To_Caller()
    : PacketHeader(0x437, 0x34)
{
    memset((char*)this + 0x16, 0, 0x1e);
}

Packet_DBMW_Save_Guild_Join::Packet_DBMW_Save_Guild_Join()
    : PacketHeader(0x438, 0x40)
{
    memset((char*)this + 0x1b, 0, 0x1e);
    memset((char*)this + 0x3d, 0, 3);
}

Packet_Guild_Reply_Guild_Secede::Packet_Guild_Reply_Guild_Secede()
    : PacketHeader(0x439, 0x52)
{
    memset((char*)this + 0x1d, 0, 0x1e);
    memset((char*)this + 0x3b, 0, 0x17);
}

Packet_DB_Request_Guild_Secede::Packet_DB_Request_Guild_Secede()
    : PacketHeader(0x439, 0x39)
{
    memset((char*)this + 0x17, 0, 0x1e);
}

Packet_Guild_Reply_Guild_Master_Delegate::Packet_Guild_Reply_Guild_Master_Delegate()
    : PacketHeader(0x43b, 0x16)
{
    (void)0;
}

Packet_DB_Request_Guild_Master_Delegate::Packet_DB_Request_Guild_Master_Delegate()
    : PacketHeader(0x43b, 0x31)
{
    memset((char*)this + 0x13, 0, 0x1e);
}

Packet_Notice_Has_Been_Guild_Master::Packet_Notice_Has_Been_Guild_Master()
    : PacketHeader(0x43e, 0x12)
{
    (void)0;
}

#pragma pack(push,1)
struct Packet_Reply_Change_Guild_Name_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    char pad0xe[0x4];
    unsigned int m12;
};
#pragma pack(pop)
Packet_Reply_Change_Guild_Name::Packet_Reply_Change_Guild_Name()
    : PacketHeader(0x444, 0x2e)
{
    ((Packet_Reply_Change_Guild_Name_Layout*)this)->ma = 0;
    ((Packet_Reply_Change_Guild_Name_Layout*)this)->m12 = 0;
    memset((char*)this + 0x17, 0, 0x17);
}

#pragma pack(push,1)
struct Packet_UnChangable_GuildInfo_Save_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_UnChangable_GuildInfo_Save::Packet_UnChangable_GuildInfo_Save()
    : PacketHeader(0x452, 0x30)
{
    ((Packet_UnChangable_GuildInfo_Save_Layout*)this)->ma = 0;
    ((Packet_UnChangable_GuildInfo_Save_Layout*)this)->me = 0;
    memset((char*)this + 0x12, 0, 0x1e);
}

#pragma pack(push,1)
struct Packet_Answer_Guild_Member_Connection_From_Web_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned short me;
};
#pragma pack(pop)
Packet_Answer_Guild_Member_Connection_From_Web::
    Packet_Answer_Guild_Member_Connection_From_Web()
    : PacketHeader(0x443, 0x5ec)
{
    ((Packet_Answer_Guild_Member_Connection_From_Web_Layout*)this)->ma = 0;
    ((Packet_Answer_Guild_Member_Connection_From_Web_Layout*)this)->me = 0;
    memset((char*)this + 0x10, 0, 0x5dc);
}

Packet_Reply_Guild_Create::Packet_Reply_Guild_Create()
    : PacketHeader(0x43f, 0x2d)
{
    memset((char*)this + 0x16, 0, 0x17);
}

Packet_DBMW_Request_Guild_Create::Packet_DBMW_Request_Guild_Create()
    : PacketHeader(0x43f, 0x5c)
{
    memset((char*)this + 0x13, 0, 0x17);
    memset((char*)this + 0x35, 0, 3);
    memset((char*)this + 0x4f, 0, 0xd);
    memset((char*)this + 0x38, 0, 0x17);
}

#pragma pack(push,1)
struct Packet_Notice_GuildName_On_Guild_Create_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    char pad0x16[0x17];
    unsigned char m2d;
};
#pragma pack(pop)
Packet_Notice_GuildName_On_Guild_Create::Packet_Notice_GuildName_On_Guild_Create()
    : PacketHeader(0x445, 0x2e)
{
    ((Packet_Notice_GuildName_On_Guild_Create_Layout*)this)->ma = 0;
    ((Packet_Notice_GuildName_On_Guild_Create_Layout*)this)->me = 4294967295;
    ((Packet_Notice_GuildName_On_Guild_Create_Layout*)this)->m12 = 0;
    ((Packet_Notice_GuildName_On_Guild_Create_Layout*)this)->m2d = 255;
    memset((char*)this + 0x16, 0, 0x17);
}

void Packet_Guild_Change_Power_War_Point::GetUserList(std::vector<unsigned int>& list)
{
    for (int i = 0; i < 8; ++i)
    {
        if (*(int*)((char*)this + i * 4 + 0xb) != 0)
        {
            list.push_back(*(unsigned int*)((char*)this + i * 4 + 0xb));
        }
    }
}

Packet_Guild_Cargo_Response::Packet_Guild_Cargo_Response()
    : PacketHeader(0x70a, 0x18ef), m_a(0xffffffff), m_b(0), m_flag(0)
{
    memset((char*)this + 0x12, 0, 0x18dc);
}

Packet_Guild_Cargo_History_Response::Packet_Guild_Cargo_History_Response()
    : PacketHeader(0x70b, 0x972)
{
    *(unsigned int*)((char*)this + 0x10) = 0xffffffff;
    *(unsigned int*)((char*)this + 0xe) = 0;
    for (int i = 0; i < 0x32; i++) { new ((char*)this + 0x12 + i * 0x30) STGuildCargoLog; }
    memset((char*)this + 0x12, 0, 0x960);
}

#pragma pack(push,1)
struct Packet_Channel_Check_Guild_Cargo_Push_Item_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned char m12;
    unsigned int m13;
    unsigned int m17;
    unsigned int m1b;
    unsigned int m1f;
    unsigned char m23;
    unsigned char m24;
    unsigned short m25;
};
#pragma pack(pop)
Packet_Channel_Check_Guild_Cargo_Push_Item::Packet_Channel_Check_Guild_Cargo_Push_Item()
    : PacketHeader(0x70c, 0x27)
{
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->me = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m12 = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m13 = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m17 = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m1b = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m1f = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m23 = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m24 = 0;
    ((Packet_Channel_Check_Guild_Cargo_Push_Item_Layout*)this)->m25 = 0;
}

Packet_Channel_Guild_Cargo_Push_Item::Packet_Channel_Guild_Cargo_Push_Item()
    : PacketHeader(0x70d, 0x4f), m_a(0xffffffff), m_b(0), m_c(0), m_d(0)
{
    m_flag = 0;
    m_e = 0;
    memset((char*)this + 0x17, 0, 0x35);
}

Packet_Channel_Guild_Cargo_Pop_Item::Packet_Channel_Guild_Cargo_Pop_Item()
    : PacketHeader(0x70e, 0x50), m_a(0xffffffff), m_b(0), m_c(0)
{
    m_flag = 0;
    m_d = 0;
    memset((char*)this + 0x16, 0, 0x35);
}

#pragma pack(push,1)
struct Packet_Channel_Guild_Cargo_Move_Item_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned char m12;
    unsigned int m13;
    unsigned int m17;
};
#pragma pack(pop)
Packet_Channel_Guild_Cargo_Move_Item::Packet_Channel_Guild_Cargo_Move_Item()
    : PacketHeader(0x70f, 0x1b)
{
    ((Packet_Channel_Guild_Cargo_Move_Item_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Guild_Cargo_Move_Item_Layout*)this)->me = 0;
    ((Packet_Channel_Guild_Cargo_Move_Item_Layout*)this)->m12 = 0;
    ((Packet_Channel_Guild_Cargo_Move_Item_Layout*)this)->m13 = 0;
    ((Packet_Channel_Guild_Cargo_Move_Item_Layout*)this)->m17 = 0;
}

#pragma pack(push,1)
struct Packet_Channel_Guild_Cargo_Upgrade_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned char m12;
};
#pragma pack(pop)
Packet_Channel_Guild_Cargo_Upgrade::Packet_Channel_Guild_Cargo_Upgrade()
    : PacketHeader(0x713, 0x13)
{
    ((Packet_Channel_Guild_Cargo_Upgrade_Layout*)this)->ma = 4294967295;
    ((Packet_Channel_Guild_Cargo_Upgrade_Layout*)this)->me = 0;
    ((Packet_Channel_Guild_Cargo_Upgrade_Layout*)this)->m12 = 0;
}

Packet_DBMW_Query_Msg::Packet_DBMW_Query_Msg()
    : PacketHeader(0x177d, 0x1013)
{
    m_fieldA = 0;
    m_fieldE = 0;
    int i = 0;
    for (i = 0; i <= 0x1000; i++) { m_data[i] = 0x0; }
}

#pragma pack(push,1)
struct Packet_Guild_Exp_Book_Delete_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    char pad0x12[0x4];
    unsigned int m16;
};
#pragma pack(pop)
Packet_Guild_Exp_Book_Delete::Packet_Guild_Exp_Book_Delete()
    : PacketHeader(0x1b6c, 0x1a)
{
    ((Packet_Guild_Exp_Book_Delete_Layout*)this)->ma = 4294967295;
    ((Packet_Guild_Exp_Book_Delete_Layout*)this)->me = 0;
    ((Packet_Guild_Exp_Book_Delete_Layout*)this)->m16 = 0;
}

#pragma pack(push,1)
struct Packet_No_Cache_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
};
#pragma pack(pop)
Packet_No_Cache::Packet_No_Cache()
    : PacketHeader(0x1b6d, 0x16)
{
    ((Packet_No_Cache_Layout*)this)->ma = 0;
    ((Packet_No_Cache_Layout*)this)->me = 0;
    ((Packet_No_Cache_Layout*)this)->m12 = 0;
}

#pragma pack(push,1)
struct Packet_Add_Guild_Fund_Result_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
    unsigned int mb;
    unsigned int mf;
    unsigned int m13;
};
#pragma pack(pop)
Packet_Add_Guild_Fund_Result::Packet_Add_Guild_Fund_Result()
    : PacketHeader(0x1bbd, 0x17)
{
    ((Packet_Add_Guild_Fund_Result_Layout*)this)->ma = 0;
    ((Packet_Add_Guild_Fund_Result_Layout*)this)->mb = 0;
    ((Packet_Add_Guild_Fund_Result_Layout*)this)->mf = 0;
    ((Packet_Add_Guild_Fund_Result_Layout*)this)->m13 = 0;
}

#pragma pack(push,1)
struct Packet_Approve_Join_Guild_Result_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned int m16;
};
#pragma pack(pop)
Packet_Approve_Join_Guild_Result::Packet_Approve_Join_Guild_Result()
    : PacketHeader(0x1bc3, 0x1a)
{
    ((Packet_Approve_Join_Guild_Result_Layout*)this)->ma = 0;
    ((Packet_Approve_Join_Guild_Result_Layout*)this)->me = 0;
    ((Packet_Approve_Join_Guild_Result_Layout*)this)->m12 = 0;
    ((Packet_Approve_Join_Guild_Result_Layout*)this)->m16 = 0;
}
