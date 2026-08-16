// df_guild_r — DNFPacketTranslater (split from source/guild per ORIG layout)
#define DNF_GUILD_ODR_TRIVIAL_CARGOLOG_DTOR
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Request_Power_War_Start_Info.h"
#include "Packet_Guild_Change_Power_War_Point.h"
#include "Packet_Guild_Request_Guild_Board_Delete.h"
#include "Packet_Guild_Request_Guild_Board_Open.h"
#include "Packet_Guild_Request_Guild_Board_Write.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Monitor_UDP_Reply_UserInfo.h"
#include "Packet_Monitor_Char_Info.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_Charac_Delete.h"
#include "Packet_Monitor_Buy_Guild_Skill.h"
#include "Packet_Monitor_Guild_Chat.h"
#include "Packet_Monitor_Guild_Chat_Hyper_Link.h"
#include "Packet_Monitor_Increase_Guild_Exp.h"
#include "Packet_Monitor_Set_Sub_Guild_Master.h"
#include "Packet_Monitor_Call_Guild_Members.h"
#include "Packet_Monitor_Call_Guild_All_Members.h"
#include "Packet_Monitor_Call_Guild_Level_Up.h"
#include "Packet_Monitor_Call_Guild_Info.h"
#include "Packet_Monitor_Notice_Guild_Dismiss.h"
#include "Packet_Guild_Call_Guild_Invite.h"
#include "Packet_Guild_Reply_Guild_Invite_From_Invited.h"
#include "Packet_Guild_Request_Guild_Secede.h"
#include "Packet_Guild_Request_Guild_Master_Delegate.h"
#include "Packet_Request_Guild_Create.h"
#include "Packet_Change_Guild_Name.h"
#include "Packet_Change_Char_Name.h"
#include "Packet_Request_BlackList.h"
#include "Packet_Register_To_BlackList.h"
#include "Packet_Delete_To_BlackList.h"
#include "Packet_Add_Guild_Fund.h"
#include "Packet_Guild_Debug.h"
#include "Packet_Refresh_Guild_Info.h"
#include "Packet_Approve_Join_Guild.h"
#include "Packet_Guild_Attendance_Info.h"
#include "Packet_Guild_Write_Guild_Member_Memo.h"
#include "Packet_Notice_Guild_War_Point_Change.h"
#include "Packet_Notice_Guild_War_End.h"
#include "Packet_Notice_Guild_War_Info.h"
#include "Packet_Guild_Create_Guild_Agit.h"
#include "Packet_Guild_Delete_Guild_Agit.h"
#include "Packet_Guild_Upgrade_Guild_Agit.h"
#include "Packet_Guild_Check_Guild_Cargo_Push_Item.h"
#include "Packet_Guild_Guild_Cargo_Push_Item.h"
#include "Packet_Guild_Guild_Cargo_Pop_Item.h"
#include "Packet_Guild_Guild_Cargo_Move_Item.h"
#include "Packet_Guild_Guild_Cargo_Upgrade.h"
#include "Packet_Request_Join_Power.h"
#include "Packet_Request_Secede_Power.h"
#include "Packet_Guild_Apply_Origial_Power_Side.h"
#include "Packet_Notice_Power_War_CFG.h"
#include "Packet_Guild_Power_War_Process_Info.h"
#include "Packet_Guild_Increase_Decrease_Power_War_Point.h"
#include "Packet_Guild_Cargo_Requst.h"
#include "Packet_Guild_Cargo_History_Request.h"
#include "Packet_Sync_GM_List.h"

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

#define STUB_HANDLER(name) \
    void CPacketTranslater::name(PacketHeader* pkt) {}

// ORIG 中 STGuildCargoLog ctor/dtor 为头文件内联弱符号（本 TU 可见定义，
// 仅 memset / 空体），GCC 据此判定 nothrow，数组成员构造不发 EH 清理块。
// 在 GuildCargo.cpp 有强符号定义，链接时优先取强符号，符号内容一致。
inline STGuildCargoLog::STGuildCargoLog()
{
    memset(this, 0, sizeof(STGuildCargoLog));
}

// ORIG 文件级 static（符号 _ZL11LETTER_TEXT）；OnChangeGuildName 使用
static const char LETTER_TEXT[0x121] =
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

void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    try
    {
        Packet_Monitor_UDP_Login* pb = (Packet_Monitor_UDP_Login*)pkt;
        if (m_pclApp != 0)
        {
            CGameServer* gs;
            if ((gs = m_pclApp->FindGameServer((unsigned char)pb->m_channel)) == 0)
            {
                DNF_LOG_SCOPE_LINE(0x65, "./log/Channel",
                    "Not Found M_ID(%s) Channel No(%d)",
                    NumberToString(pb->m_serverNo, 0),
                    (unsigned int)(unsigned char)pb->m_channel);
            }
            else
            {
                CServerInterface* cs;
                CUser* user;
                CTcpGameServer* tgs;
                CUserManager* um;
                CUser* nu;
                if ((tgs = m_pclApp->FindTcpGameServer(pb->m_connNo)) == 0)
                {
                    return;
                }
                else
                {
                    um = &m_pclApp->m_userManager;
                    if ((user = um->FindUser(pb->m_serverNo)) != 0)
                    {
                        cs = user->GetGameServer();
                        DNF_LOG_SCOPE_LINE(0x70, "./log/User",
                            "DOUBLE CONNECTED : Already User DB ID(%s)\tChannel(%d)\tCurrent Connect User DB ID(%s)\tChannel(%d)\n",
                            NumberToString(user->GetDBID(), 0),
                            (unsigned int)cs->GetChannelNo(),
                            NumberToString(pb->m_serverNo, 1),
                            (unsigned int)gs->GetChannelNo());
                    }
                    else
                    {
                        nu = um->CreateUser(pb->m_serverNo, 0, "",
                                            pb->m_guildKey, gs);
                        DNF_LOG_SCOPE_LINE(0x76, "./log/User",
                            "Current user count : %d\tConnected User DB ID : %s\n",
                            um->Size(), NumberToString(pb->m_serverNo, 0));
                        nu->SetSex(pb->m_sex);
                        nu->SetSsn(pb->m_ssn);
                        nu->SetTcpGameServer(tgs);
                        if (!m_pclApp->Get_MemoryCashManager()->QueryCashMemoryBlackList(nu))
                        {
                            RequestBlackListToDBMW(pb->m_serverNo);
                        }
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x85, "./log/Except",
            "CPacketTranslater::OnLogin() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x8a, "./log/Except",
            "CPacketTranslater::OnLogin() Exception Break\n");
    }
}

void CPacketTranslater::OnLogout(PacketHeader* pkt)
{
    Packet_Monitor_UDP_Logout* pb = (Packet_Monitor_UDP_Logout*)pkt;
    try
    {
        if (m_pclApp != 0)
        {
            CUserManager* um = &m_pclApp->m_userManager;
            CUser* user;
            if ((user = um->FindUser(pb->m_dbid)) == 0)
            {
            }
            else
            {
                unsigned int foc = (unsigned int)pb->m_foc;
                char* name = user->GetCharName();
                unsigned int gflag = (unsigned int)user->GetGuildMemFlag();
                unsigned int gkey = user->GetGuildKey();
                unsigned int uniq = user->GetUniqCharNo();
                char* mid = NumberToString(pb->m_dbid, 0);
                CMyFileLog log(__FUNCTION__, 0xaa);
                log("./log/User",
                    "LOGOUT : User DB ID(%s), Char No(%d), Guild K(%d)GFlag(%d), name(%s), F.O.C(%d)\n",
                    mid, uniq, gkey, gflag, name, foc);
                if (user->GetGuildKey() != 0)
                {
                    (&m_pclApp->m_guildManager)->GuildMemLogout(user->GetGuildKey(), user);
                }
                if (user->GetUniqCharNo() != 0)
                {
                    um->DeleteUser_CharNo(user->GetUniqCharNo());
                    um->DeleteUser_CharName(user->GetCharName());
                }
                user->ResetCharInfo();
                if (pb->m_foc == 0)
                {
                    CMemoryCashManager* mc = m_pclApp->Get_MemoryCashManager();
                    mc->InsertCashMemorySetCharacterObject(user);
                    mc->SetUserObject(user);
                    user->ResetBlackList();
                    if (!um->DeleteUser(user))
                    {
                        char* cname = user->GetCharName();
                        unsigned int cno = user->GetUniqCharNo();
                        char* mid2 = NumberToString(pb->m_dbid, 0);
                        CMyFileLog log2(__FUNCTION__, 0xd6);
                        log2("./log/User",
                            "[NO USER] Disconnected User DB ID : %s, Char No : %d , char name:%s\n",
                            mid2, cno, cname);
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
        CMyFileLog log(__FUNCTION__, 0xe5);
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
                unsigned char idx = ((Packet_Sync_GM_List*)pb)->m_field_a;
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
                        CGameServer* gs;
                        if ((gs = handler->GetGameServer((unsigned int)idx)) != 0)
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
                        (unsigned int)((Packet_Sync_GM_List*)pb)->m_field_a);
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
    Packet_Monitor_UDP_Reply_UserInfo* pb = (Packet_Monitor_UDP_Reply_UserInfo*)pkt;
    {
        DNF_LOG_SCOPE_LINE(0x150,"./log/Reboot", "[GAME SERVER] Channel No : %d\n",
            (unsigned int)(unsigned char)pb->m_channel);
    }
    if (m_pclApp != 0)
    {
        CGameServer* gs;
        if ((gs = m_pclApp->FindGameServer((int)(unsigned char)pb->m_channel)) == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
        }
        CTcpGameServer* tgs = m_pclApp->FindTcpGameServer(pb->m_connNo);
        if (tgs == 0)
        {
        }
        else
        {
            CUserManager* um = &m_pclApp->m_userManager;
            int i = 0;
            while (i < (int)(unsigned char)pb->m_count)
            {
                CUser* user = um->FindUser(pb->m_items[i].m_dbid);
                if (user != 0)
                {
                    char* mid = NumberToString(pb->m_items[i].m_dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x166, "./log/Except", "CPacketTranslater::OnReplyUserInfo() : %s\n", mid);
                }
                else
                {
                    ST_MonitorReplyUserInfo* rec = &pb->m_items[i];
                    user = um->CreateUser(rec->m_dbid, rec->m_charNo, rec->m_name,
                                          rec->m_channel, gs);
                    user->SetUserInfo_CharNo(rec->m_job, rec->m_grade, rec->m_level,
                                             rec->m_charNo, rec->m_name);
                    user->SetSex(rec->m_sex);
                    user->SetSsn(rec->m_ssn);
                    user->SetTcpGameServer(tgs);
                    RequestBlackListToDBMW(rec->m_dbid);
                    if (rec->m_memberKey != 0)
                    {
                        (&m_pclApp->m_guildManager)->GuildMemLogin(
                            (unsigned int)rec->m_memberKey, user);
                    }
                }
                i++;
            }
        }
    }
    else
    {
        throw CDNFException("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x187, "./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyUserInfo() Exception Break");
        DNF_LOG_SCOPE_LINE(0x18d, "./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnCharLogin(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            return;
        }
        CUser* user;
        CGuild* guild;
        Packet_Monitor_Char_Info* pb = (Packet_Monitor_Char_Info*)pkt;
        CUserManager* um;
        if ((&m_pclApp->m_guildManager)->IsGuildWarEnterableChar(
                pb->m_war, pb->m_guildKey) != true)
        {
            Packet_Monitor_UDP_User_Getout pktGetout;
            pktGetout.ma = pb->m_serverNo;
            m_pclApp->Get_ServerHandler()->SendToGameServer(pb->m_channel, &pktGetout);
        }
        um = &m_pclApp->m_userManager;
        if ((user = um->FindUser(pb->m_serverNo)) != 0)
        {
            user->SetUserInfo_CharNo((char)pb->m_job, (char)pb->m_pvp, (short)pb->m_level,
                                     pb->m_charKey, pb->m_name);
            DNF_LOG_SCOPE_LINE(0x1b1,"./log/User",
                "[CHAR_LOGIN]\tDB ID(%s)\tChar Key(%d)\tGuild K(%d)\tMember K(%d)\tJob(%d)\tname(%s)\tCh No(%d)\tpvp(%d)\n",
                NumberToString(pb->m_serverNo, 0), pb->m_charKey, pb->m_guildKey,
                pb->m_memberKey, (int)(char)pb->m_job, pb->m_name,
                (unsigned int)pb->m_channel);
            user->SetUserPosState(3);
            if (pb->m_guildKey != 0)
            {
                guild = (&m_pclApp->m_guildManager)->GuildMemLogin(pb->m_guildKey, user);
                guild->NoticeGuildMemberLogin_Out(user, 1);
            }
            if (um->InsertUser_CharName(pb->m_name, user) != 1)
            {
                DNF_LOG_SCOPE_LINE(0x1ce,"./log/Except",
                    "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!",
                    NumberToString(pb->m_serverNo, 0), pb->m_name);
            }
            if (um->InsertUser_CharNo(pb->m_charKey, user) != 1)
            {
                DNF_LOG_SCOPE_LINE(0x1d2,"./log/Except",
                    "Insert Fail!\tChar ID : %d\t\xc4\xb3\xb8\xaf\xc5\xcd \xc0\xcc\xb8\xa7:%s\n",
                    pb->m_charKey, pb->m_name);
            }
            m_pclApp->Get_MemoryCashManager()->DeleteCashObjecct(user->GetDBID());
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1d9,"./log/User",
                "[CHAR_LOGIN_ERR]\tDB ID : %s\tChar Key : %d\tGuild Key : %d\tJob : %d\tname : %s\n",
                NumberToString(pb->m_serverNo, 0), pb->m_charKey, pb->m_guildKey,
                (int)(char)pb->m_job, pb->m_name);
        }
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
        PacketHeader* p = pkt;
        if (((Packet_DB_Reply_Query_Guild*)p)->m_result == 1)
        {
            if (!(&m_pclApp->m_guildManager)->LoadGuild(
                    ((Packet_DB_Reply_Query_Guild*)p)->m_guildId,
                    ((Packet_DB_Reply_Query_Guild*)p)->m_guildInfo,
                    ((Packet_DB_Reply_Query_Guild*)p)->m_rest))
            {
                DNF_LOG_SCOPE_LINE(0x202, "./log/GuildErr",
                    "CPacketTranslater::OnReplyQueryGuild()\tLoadGuild Err(%d)",
                    ((Packet_DB_Reply_Query_Guild*)p)->m_guildId);
            }
            (&m_pclApp->m_guildManager)->SendGuildInfoToMembers(
                ((Packet_DB_Reply_Query_Guild*)p)->m_guildId, true);
            (&m_pclApp->m_guildManager)->AttendGuild(
                ((Packet_DB_Reply_Query_Guild*)p)->m_guildId,
                ((Packet_DB_Reply_Query_Guild*)p)->m_characNo);
            if (((Packet_DB_Reply_Query_Guild*)p)->m_guildInfo.m_agitFlag != 0)
            {
                CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
                    ((Packet_DB_Reply_Query_Guild*)p)->m_guildId);
                CServerHandler* handler = m_pclApp->Get_ServerHandler();
                if (guild != 0)
                {
                    guild->LoadGuildAgit(handler,
                                         ((Packet_DB_Reply_Query_Guild*)p)->m_guildId);
                }
            }
        }
        else
        {
            (&m_pclApp->m_guildManager)->SendGuildInfoToMembers(
                ((Packet_DB_Reply_Query_Guild*)p)->m_guildId, true);
            DNF_LOG_SCOPE_LINE(0x21d, "./log/Except",
                "[DB ERROR]CPacketTranslater::OnReplyQueryGuild() packet->bSuccess : %d,guildKey(%d)",
                (unsigned int)((Packet_DB_Reply_Query_Guild*)p)->m_result,
                ((Packet_DB_Reply_Query_Guild*)p)->m_guildId);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyQueryGuild() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x223, "./log/Except",
            "CPacketTranslater::OnReplyQueryGuild() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyQueryGuild() Exception Break");
        DNF_LOG_SCOPE_LINE(0x229, "./log/Except",
            "CPacketTranslater::OnReplyQueryGuild() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyQueryGuildMember(PacketHeader* pkt)
{
    try
    {
        CUser* user;
        PacketHeader* p = pkt;
        if (((Packet_DB_Reply_Query_Guild_Member*)p)->m_flag == 1)
        {
            CUserManager* um = &m_pclApp->m_userManager;
            if ((user = um->FindUser_CharNo(
                     ((Packet_DB_Reply_Query_Guild_Member*)p)->m_characNo)) != 0)
            {
                user->LoadGuildMember(
                    ((Packet_DB_Reply_Query_Guild_Member*)p)->m_guildId,
                    ((Packet_DB_Reply_Query_Guild_Member*)p)->m_info);
                user->SendGuildMemberDBInfo(
                    ((Packet_DB_Reply_Query_Guild_Member*)p)->m_info);
            }
            else
            {
                throw CDNFException(
                    "CPacketTranslater::OnDBReplyQueryGuildMember()\tpclUser is NULL\n");
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x24b, "./log/Except",
                "[DB ERROR]CPacketTranslater::OnDBReplyQueryGuildMember() packet->bSuccess : %d\n",
                (unsigned int)((Packet_DB_Reply_Query_Guild_Member*)p)->m_flag);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x251, "./log/Except",
            "CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break");
        DNF_LOG_SCOPE_LINE(0x257, "./log/Except",
            "CPacketTranslater::OnDBReplyQueryGuildMember() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildEnter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildEnter : 0 == m_pclApp")
    Packet_Monitor_Notice_Guild_Enter* pb = (Packet_Monitor_Notice_Guild_Enter*)pkt;
    char* mid = NumberToString(pb->m_info.m_dbid, 0);
    DNF_LOG_SCOPE_LINE(0x283,"./log/Web",
        "Packet_Monitor_Notice_Guild_Enter: guildkey : %d, m_id : %s , charid : %d, guildname : %s, charname : %s\n",
        pb->m_info.m_guildKey, mid, pb->m_info.m_charNo, pb->m_info.m_guildName, pb->m_info.m_charName);
    CGuild* guild = (&m_pclApp->m_guildManager)->GuildEnter(
        pb->m_info.m_guildKey, pb->m_info);
    if (guild != 0)
    {
        pb->m_info.m_field_4f = 1;
        pb->m_info.m_field_4b = pb->m_info.m_charNo;
        guild->NoticeEnterToGuildMember((char*)&pb->m_info);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildEnter() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x290, "./log/Except", "CPacketTranslater::OnNoticeGuildEnter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildEnter() Exception Break");
        DNF_LOG_SCOPE_LINE(0x296, "./log/Except", "CPacketTranslater::OnNoticeGuildEnter() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildSecede(PacketHeader* pkt)
{
    Packet_Monitor_Notice_Guild_Secede* pb = (Packet_Monitor_Notice_Guild_Secede*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildSecede : 0 == m_pclApp")
        CUser* user =
            m_pclApp->Get_UserManager()->FindUser_CharNo(pb->m_info.m_charNo);
        if (user != 0)
        {
            Packet_Guild_Exp_Book_Delete expDel;
            expDel.m_channel = user->GetIdByChannel();
            expDel.m_charNo = pb->m_info.m_charNo;
            expDel.m16 = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
            expDel.m_group = pb->m_info.m_dbid;
            user->SendTcpGameserver((PacketHeader*)&expDel);
        }
        else
        {
            Packet_No_Cache noCache;
            noCache.ma = pb->m_info.m_dbid;
            noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
            noCache.m12 = 1;
            m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
            noCache.ma = pb->m_info.m_dbid;
            noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
            noCache.m12 = 2;
            m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);

            Packet_DBMW_Query_Msg query;
            query.m_handleIdx = 2;
            query.m_queryId = 0x4f00;
            sprintf(query.m_data,
                    "upDate charac_info set guild_secede = 1 where charac_no = %u",
                    pb->m_info.m_charNo);
            m_pclApp->Get_ServerHandler()->SendToDB(&query);
        }

        CGuild* guild = (&m_pclApp->m_guildManager)->GuildSecede(
            pb->m_info.m_guildKey, pb->m_info);
        if (guild != 0)
        {
            guild->NoticeSecedeToGuildMember((char*)&pb->m_info);
            guild->SendGuildInfoToMembers(false);
        }
    }
    catch (std::exception& e)
    {
        CMyFileLog log(__FUNCTION__, 0x2f9);
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
    CGuild* guild;
    Packet_Guild_Mark_Change_Notice* pbinfo = (Packet_Guild_Mark_Change_Notice*)pkt;
    DNF_LOG_SCOPE_LINE(0x328, "./log/Web",
        "[GUILD MARK CHANGE] Recv from web server. (guildkey:%d)\n", pbinfo->m_guildKey);
    guild = (&m_pclApp->m_guildManager)->FindGuild(pbinfo->m_guildKey);
    if (guild != 0)
    {
        guild->NoticeMarkChangeToGuildMember(pbinfo->m_guildKey);
    }
    DNF_LOG_SCOPE_LINE(0x331, "./log/Web",
        "[GUILD MARK CHANGE] Not exist guild. (guildkey:%d)\n", pbinfo->m_guildKey);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildMarkChange() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x337, "./log/Except",
            "CPacketTranslater::OnNoticeGuildMarkChange() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildMarkChange() Exception Break");
        DNF_LOG_SCOPE_LINE(0x33d, "./log/Except",
            "CPacketTranslater::OnNoticeGuildMarkChange() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildCreate(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildCreate : 0 == m_pclApp");
    Packet_Monitor_Notice_Guild_Create* pb = (Packet_Monitor_Notice_Guild_Create*)pkt;
    CServerHandler* handler;
    if ((handler = m_pclApp->Get_ServerHandler()) == 0)
    {
        throw CDNFException("CGuildManager::GuildMemLogin() pclServerHandler == NULL\n");
    }
    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_info.m_charNo);
    if (user != 0)
    {
        unsigned int uniqCharNo = user->GetUniqCharNo();
        (&m_pclApp->m_guildManager)->CreateGuild(pb->m_info.m_guildKey, handler,
                                                  uniqCharNo);
        user->QueryGuildMember(handler);
        Packet_Monitor_Notice_Guild_Create_ToUser notice;
        memcpy(&notice.m_info, &pb->m_info, sizeof(pb->m_info));
        notice.m_member = user->GetIdByChannel();
        user->SendToGameserver((char*)&notice, 0x2d);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildCreate() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x37f, "./log/Except", "CPacketTranslater::OnNoticeGuildCreate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildCreate() Exception Break");
        DNF_LOG_SCOPE_LINE(0x385, "./log/Except", "CPacketTranslater::OnNoticeGuildCreate() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildDismiss(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildDismiss : 0 == m_pclApp")
    Packet_Monitor_Notice_Guild_Dismiss* pb = (Packet_Monitor_Notice_Guild_Dismiss*)pkt;
    DNF_LOG_SCOPE_LINE(0x3a9,"./log/Web", "Packet_Monitor_Notice_Guild_Dismiss: guildkey : %d\n",
        pb->m_guildKey);
    Packet_No_Cache noCache;
    noCache.ma = 0;
    noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
    noCache.m12 = 1;
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
    noCache.ma = 0;
    noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
    noCache.m12 = 2;
    m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
    if (guild != 0)
    {
        guild->DismissGuildMemberAndNotice((int)m_pclApp->Get_ServerGroup() & 0xff);
        (&m_pclApp->m_guildManager)->GuildDismiss(guild);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildDismiss() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x3c9, "./log/Except", "CPacketTranslater::OnNoticeGuildDismiss() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildDismiss() Exception Break");
        DNF_LOG_SCOPE_LINE(0x3cf, "./log/Except", "CPacketTranslater::OnNoticeGuildDismiss() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildChatMsg(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildChatMsg : 0 == m_pclApp")
    Packet_Monitor_Guild_Chat* pb = (Packet_Monitor_Guild_Chat*)pkt;
    if (pb->m_guildKey != 0 && pb->m_charNo != 0 && pb->m_msgLen != 0)
    {
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo);
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        if (user != 0 && guild != 0)
        {
            guild->NoticeChatMsgToGuildMembers(pb->m_charNo, pb->m_msg,
                                               (int)(unsigned char)pb->m_msgLen,
                                               (const char*)user->GetCharName());
        }
    }
    else
    {
        throw CDNFException(
            "CPacketTranslater::OnNoticeGuildChatMsg : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x405, "./log/Except", "CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x40b, "./log/Except", "CPacketTranslater::OnNoticeGuildChatMsg() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeGuildChatMsgHyperLink(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnNoticeGuildChatMsgHyperLink : 0 == m_pclApp");
    Packet_Monitor_Guild_Chat_Hyper_Link* pb = (Packet_Monitor_Guild_Chat_Hyper_Link*)pkt;
    if (pb->m_guildKey != 0 && pb->m_charNo != 0 && pb->m_msgLen != 0)
    {
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* user = um->FindUser_CharNo(pb->m_charNo);
        if (user != 0)
        {
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
            if (guild != 0)
            {
                const char* name = user->GetCharName();
                guild->NoticeChatMsgToGuildMembersHyperLink(
                    pb->m_charNo, pb->m_msg, pb->m_msgLen,
                    pb->m_field_12, (hyperlink_item_info*)pb->m_items, name);
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
        DNF_LOG_SCOPE_LINE(0x430, "./log/Except", "CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break");
        DNF_LOG_SCOPE_LINE(0x436, "./log/Except", "CPacketTranslater::OnNoticeGuildChatMsgHyperLink() Exception Break\n");
    }
}

void CPacketTranslater::OnIncreaseGuildExp(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnIncreaseGuildExp : 0 == m_pclApp")
    Packet_Monitor_Increase_Guild_Exp* pb = (Packet_Monitor_Increase_Guild_Exp*)pkt;
    if (pb->m_guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnIncreaseGuildExp : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
    if (guild != 0)
    {
        unsigned int oldExp = guild->GetGuildExp();
        unsigned int addExp = pb->m_addExp;
        DNF_LOG_SCOPE_LINE(0x453,"./log/Guild",
            "GUILD EXP : char no(%d) guild key(%d), add exp(%d), guild exp(%d), book(%d)",
            pb->m_charNo, pb->m_guildKey, addExp, oldExp,
            (int)(char)pb->m_field_17);
        unsigned int max1 = (&m_pclApp->m_guildManager)->GetMaxGuildExp1();
        unsigned int max2 = (&m_pclApp->m_guildManager)->GetMaxGuildExp2();
        unsigned int level = guild->GetGuildLevel();
        int expLevel = (&m_pclApp->m_guildManager)->GetGuildLevelWithExp(oldExp);
        if (level == (unsigned int)expLevel)
        {
            if (pb->m_field_17 == 0)
            {
                unsigned int next = (&m_pclApp->m_guildManager)->GetGuildExpWithLevel(level + 1);
                unsigned int limit = next < max1 ? next : max1;
                guild->AddGuildExpUntilLimit(addExp, limit);
            }
            else if (pb->m_field_17 == 1 || pb->m_field_17 == 2)
            {
                guild->AddGuildExpUntilLimit(addExp, max2);
            }
            if (pb->m_field_16 != 0)
            {
                if (m_pclApp->Get_UserManager()->FindUser_CharNo(
                        pb->m_charNo) != 0)
                {
                    guild->SendGuildInfoToMembers(false);
                }
            }
        }
        else
        {
            CMyFileLog log2(__FUNCTION__, 0x462);
            log2("./log/Guild",
                 "OnIncreaseGuildExp : guild key(%d), curr guild exp(%d),lev(%d), next guild exp(%d), exp lev(%d)",
                 pb->m_guildKey, oldExp, level, oldExp + addExp, expLevel);
        }
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnIncreaseGuildExp() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x498, "./log/Except", "CPacketTranslater::OnIncreaseGuildExp() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnIncreaseGuildExp() Exception Break");
        DNF_LOG_SCOPE_LINE(0x49e, "./log/Except", "CPacketTranslater::OnIncreaseGuildExp() Exception Break\n");
    }
}

void CPacketTranslater::OnBuyGuildSkill(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnBuyGuildSkill : 0 == m_pclApp");
        CUser* user;
        CGuild* guild;
        Packet_Monitor_Buy_Guild_Skill* pb = (Packet_Monitor_Buy_Guild_Skill*)pkt;
        if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
        {
            throw CDNFException("CPacketTranslater::OnBuyGuildSkill : 0 == pclUser");
        }
        if (pb->m_guildKey != 0)
        {
            if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) != 0)
            {
                if (guild->IsGuildMaster(pb->m_charNo) || guild->IsSubGuildMaster(pb->m_charNo))
                {
                    if (guild->BuyGuildSkill(pb->m_skillId, pb->m_skillLevel,
                                             pb->m_slot, pb->m_cost))
                    {
                        char saveFlag = (char)guild->GetDBSaveFlag();
                        guild->SendGuildInfoToMembers(false);
                        guild->DBGuildSave(user->GetGameServer()->GetGroupNo(),
                                           m_pclApp->Get_ServerHandler(), 1);
                        if (saveFlag != 0)
                        {
                            guild->EnableDBSaveFlag();
                            guild->DBGuildSave(user->GetGameServer()->GetGroupNo(),
                                               m_pclApp->Get_ServerHandler(), 0);
                        }
                    }
                }
            }
        }
        else
        {
            throw CDNFException(
                "CPacketTranslater::OnBuyGuildSkill : packet->m_uCharID && packet->m_uGuildKey && packet->m_msgLen");
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnBuyGuildSkill() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x4f0, "./log/Except",
            "CPacketTranslater::OnBuyGuildSkill() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnBuyGuildSkill() Exception Break");
        DNF_LOG_SCOPE_LINE(0x4f6, "./log/Except",
            "CPacketTranslater::OnBuyGuildSkill() Exception Break\n");
    }
}

void CPacketTranslater::OnSetGuildMemberGradeFromWeb(PacketHeader* pkt)
{
    Packet_Monitor_Set_GuildMember_Grade_FromWeb* pb = (Packet_Monitor_Set_GuildMember_Grade_FromWeb*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnSetGuildMemberGradeFromWeb : 0 == m_pclApp");
    unsigned int guildKey = pb->m_guildKey;
    unsigned int masterCharNo = pb->m_masterCharNo;
    unsigned char newGrade = pb->m_newGrade;
    unsigned int targetCharNo = pb->m_targetCharNo;
    if (guildKey == 0)
    {
        DNF_LOG_SCOPE_LINE(0x515,"./log/GuildModify",
            "CPacketTranslater::OnSetGuildMemberGradeFromWeb : 0 == packet->m_uGuildKey(%d)",
            (unsigned int)(guildKey == 0));
        return;
    }
    CGuild* guild;
    if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
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
    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(targetCharNo);
    if (user != 0)
    {
        user->ChangeGuildMemberGrade(newGrade);
        Packet_Monitor_Notify_GuildMemberGrade notify;
        notify.m_channel = user->GetIdByChannel();
        notify.m_charNo = user->GetUniqCharNo();
        notify.m_grade = newGrade;
        user->SendToGameserver((char*)&notify, notify.packetSize);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x578, "./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x57d, "./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break\n");
    }
}

void CPacketTranslater::OnSetGuildMemberGrade(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnSetGuildMemberGrade : 0 == m_pclApp")
    Packet_Monitor_Set_Sub_Guild_Master* pb = (Packet_Monitor_Set_Sub_Guild_Master*)pkt;
    Packet_Monitor_Set_Sub_Guild_Master_Reply reply;
    CUser* caller;
    if ((caller = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x5f3,"./log/GuildModify", "CPacketTranslater::OnSetGuildMemberGrade : no pclRequester(%d)",
            pb->m_charNo);
        return;
    }
    reply.m_e = caller->GetIdByChannel();
    reply.m_12 = caller->GetUniqCharNo();
    reply.m_a = 0;
    reply.m_16 = pb->m_grade;
    memcpy(reply.m_name, pb->m_name, 0x1d);
    unsigned int guildKey = pb->m_guildKey;
    CGuild* guild = 0;
    if (guildKey == 0 ||
        (guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
    {
        reply.m_a = 100;
        caller->SendToGameserver((char*)&reply, 0x3a);
        return;
    }
    bool authorized = guild->IsGuildMaster(pb->m_charNo) == 1 ||
                      caller->IsSubGuildMaster();
    if (!authorized)
    {
        reply.m_a = 0x56;
    }
    if (pb->m_grade == 1)
    {
        reply.m_a = 0x58;
    }
    if (pb->m_grade == 2)
    {
        if (guild->GetCurSubGuildMasterCnt() < 5)
        {
            if (guild->IsGuildMaster(pb->m_charNo) != 1)
            {
                reply.m_a = 0x66;
            }
        }
        else
        {
            reply.m_a = 0x59;
        }
    }
    CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(
        pb->m_name);
    if (target == 0)
    {
        if (reply.m_a == 0)
        {
            Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade dbPkt;
            unsigned char group = caller->GetGameServer()->GetGroupNo();
            dbPkt.ma = group;
            dbPkt.mb = guildKey;
            dbPkt.m32 = pb->m_grade;
            dbPkt.m_f = pb->m_charNo;
            dbPkt.m_13 = caller->GetGuildMemDBInfo()->m_grade;
            memcpy(dbPkt.m_name, pb->m_name, 0x1d);
            m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
            return;
        }
    }
    else if (reply.m_a == 0)
    {
        if (guild->IsGuildMaster(target->GetUniqCharNo()) != 1)
        {
            if (target == caller)
            {
                reply.m_a = 0x55;
            }
            else
            {
                if (target->GetGuildMemDBInfo()->m_grade == (int)(char)pb->m_grade)
                {
                    reply.m_a = 0x65;
                }
                else
                {
                    if (pb->m_grade == 2)
                    {
                        if ((char)caller->GetGuildMemDBInfo()->m_grade == 1)
                        {
                            guild->SetSubGuildMaster(target->GetUniqCharNo(), true);
                            target->GetGuildMemDBInfo()->m_grade = 2;
                            guild->ChangeUnconnectedGuildMemberGrade(target->GetUniqCharNo(), 2);
                            guild->SendGuildInfoToMembers(false);
                        }
                        else
                        {
                            reply.m_a = 0x66;
                        }
                    }
                    else
                    {
                        if (guild->IsSubGuildMaster(target->GetUniqCharNo()))
                        {
                            if ((char)caller->GetGuildMemDBInfo()->m_grade == 1)
                            {
                                guild->SetSubGuildMaster(target->GetUniqCharNo(), false);
                                guild->SendGuildInfoToMembers(false);
                            }
                            else
                            {
                                reply.m_a = 0x66;
                            }
                        }
                    }
                    if (reply.m_a == 0)
                    {
                        target->ChangeGuildMemberGrade(pb->m_grade);
                        Packet_Monitor_Notify_GuildMemberGrade notify;
                        notify.m_grade = pb->m_grade;
                        notify.m_charNo = target->GetUniqCharNo();
                        notify.m_channel = target->GetIdByChannel();
                        target->SendToGameserver((char*)&notify, notify.packetSize);
                        if (target->GetGameServer() == 0)
                        {
                            reply.m_a = 3;
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
            reply.m_a = 0x57;
        }
    }
    caller->SendToGameserver((char*)&reply, 0x3a);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x680, "./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x685, "./log/Except", "CPacketTranslater::OnSetGuildMemberGrade() Exception Break\n");
    }
}

void CPacketTranslater::OnUpdateChangableCharInfo(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnUpdateChangableCharInfo : 0 == m_pclApp");
    Packet_Monitor_Char_Changable_Info* pb = (Packet_Monitor_Char_Changable_Info*)pkt;
    CUser* user = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid);
    if (user != 0)
    {
        user->SetUserChangableInfo(pb->m_field_f, (char)pb->m_field_11);
    }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x6a4, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x6aa, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break\n");
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
    CGuildWar* war = (&m_pclApp->m_guildManager)->GetGuildWar();
    if (war->IsGuildWarEventOn() == 1)
    {
        Packet_Notice_Guild_War_Point_Change* pb = (Packet_Notice_Guild_War_Point_Change*)pkt;
        if (war->IsGuildWarEnterableGuild(pb->m_guildKey) == 1)
        {
            war->AddGuildWarPoint(pb->m_guildKey, (int)(char)pb->m_point);
        }
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x705, "./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x70a, "./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break\n");
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
    CGuildWar* war = (&m_pclApp->m_guildManager)->GetGuildWar();
    if (war->IsGuildWarEventOn() == 1)
    {
        war->SetGuildWarEvent(false,
                              ((Packet_Notice_Guild_War_End*)pkt)->m_fieldA);
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
        dbPkt.m_group = group;
        war->GetGuildWarInfo((unsigned int*)dbPkt.m_fieldB,
                             (unsigned int*)dbPkt.m_field33,
                             (unsigned short*)dbPkt.m_field5b);
        m_pclApp->m_serverHandler->SendToDB(&dbPkt);
        Packet_Monitor_Event_End monPkt;
        monPkt.m_eventType = 9;
        OnEventEnd(&monPkt);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x76d, "./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x772, "./log/Except", "CPacketTranslater::OnNoticeGuildWarEnd Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildWarInfo(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildWarInfo : 0 == m_pclApp");
        CUser* user;
        CGuildWar* war = (&m_pclApp->m_guildManager)->GetGuildWar();
        Packet_Notice_Guild_War_Info* pb = (Packet_Notice_Guild_War_Info*)pkt;
        if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x78d,"./log/Except",
                "[USER] CPacketTranslater::OnRequestGuildWarInfo : pclUser == 0!\tchar id(%d)",
                pb->m_charNo);
        }
        else
        {
            war->GetGuildWarInfo((ST_Guild_War_Rank_Info*)pb->m_rankInfo);
            user->SendToGameserver((char*)pb, 0x15c);
        }
    }
    DNF_CATCH_LOG("./log/Except",
        "CPacketTranslater::OnRequestGuildWarInfo Exception Break", 0x796, 0x79b);
}

void CPacketTranslater::OnCharacterDelete(PacketHeader* pkt)
{
    char* pb = (char*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCharacterDelete : 0 == m_pclApp");
        unsigned int guildKey = ((Packet_Monitor_Charac_Delete*)pb)->m_guildKey;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x7bb,"./log/GuildModify",
                "CPacketTranslater::OnCharacterDelete : 0 == pclGuild, Char Key = %d (Maybe Requester was logout)",
                ((Packet_Monitor_Charac_Delete*)pb)->m_charNo);
        }
        else
        {
            ST_Notice_Guild_Secede notice;
            notice.m_guildKey = ((Packet_Monitor_Charac_Delete*)pb)->m_guildKey;
            notice.m_charNo = ((Packet_Monitor_Charac_Delete*)pb)->m_charNo;
            notice.m_secedeFlag = 1;
            memcpy(notice.m_guildName, guild->GetGuildName(), 0x16);
            (&m_pclApp->m_guildManager)->GuildSecede(
                ((Packet_Monitor_Charac_Delete*)pb)->m_guildKey, notice);
            guild->SendGuildInfoToMembers(false);
            (&m_pclApp->m_userManager)->DeleteBlackUserOnCharacDelete(
                ((Packet_Monitor_Charac_Delete*)pb)->m_charNo);
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except", "CPacketTranslater::OnCharacterDelete Exception Break", 0x7d2, 0x7d8)
}

void CPacketTranslater::OnCallGuildMembers(PacketHeader* pkt)
{
    Packet_Monitor_Call_Guild_Members* pb = (Packet_Monitor_Call_Guild_Members*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildMembers : 0 == m_pclApp")
        unsigned int key = pb->m_guildKey;
        if (key == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : packet->m_uGuildKey == 0");
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(
            pb->m_charNo);
        if (user == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : 0 == pclUser");
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
            pb->m_guildKey);
        if (guild == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildMembers : 0 == pclGuild");
        }
        guild->ReplyGuildMembers(user);
    }
    catch (std::exception& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        CMyFileLog log(__FUNCTION__, 0x812);
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
    Packet_Monitor_Call_Guild_Members* pb = (Packet_Monitor_Call_Guild_Members*)pkt;
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnCallGuildAllMembers : 0 == m_pclApp")
        if (pb->m_guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::Packet_Monitor_Call_Guild_All_Members : packet->m_uGuildKey == 0");
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(
            pb->m_charNo);
        if (user != 0)
        {
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
                pb->m_guildKey);
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
        DNF_LOG_SCOPE_LINE(0x851, "./log/Except", "CPacketTranslater::OnCallGuildAllMembers() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        DNF_LOG_SCOPE_LINE(0x857, "./log/Except", "CPacketTranslater::OnCallGuildAllMembers() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildAllMembers(PacketHeader* pkt)
{
    Packet_DB_Reply_Guild_All_Members* pb = (Packet_DB_Reply_Guild_All_Members*)pkt;
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDBReplyGuildAllMembers : 0 == m_pclApp");
        unsigned int guildKey = pb->m_guildId;
        if (guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyGuildAllMembers : packet->m_uGuildKey == 0");
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyGuildAllMembers : 0 == pclGuild ,the guild is out");
        }
        guild->LoadGuildAllMembersProxy(
            (STGuildMemberProxy*)&pb->m_members,
            pb->m_count,
            pb->m_field13);
        CUser* user = 0;
        if (pb->m_count == 2)
        {
            user = (&m_pclApp->m_userManager)->FindUser_CharNo(
                pb->m_characNo);
            if (user != 0)
            {
                guild->SetGuildDBFlag(0x10);
                guild->ReplyGuildAllMembers(user);
                DNF_LOG_SCOPE_LINE(0x87b,"./log/GuildModify", "OnDBReplyGuildAllMembers gKey(%d), cKey(%d)", guildKey,
                    pb->m_characNo);
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x886, "./log/Except", "CPacketTranslater::OnDBReplyGuildAllMembers() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        DNF_LOG_SCOPE_LINE(0x88c, "./log/Except", "CPacketTranslater::OnDBReplyGuildAllMembers() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyUnconnGuildMember(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDBReplyUnconnGuildMember : 0 == m_pclApp");
        Packet_DB_Reply_Unconn_Guild_Member* pb = (Packet_DB_Reply_Unconn_Guild_Member*)pkt;
        unsigned int guildKey = pb->m_guildId;
        if (guildKey == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnDBReplyUnconnGuildMember : packet->m_uGuildKey == 0");
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild != 0)
        {
            guild->LoadGuildOneMemberProxy(pb->m_proxy);
            return;
        }
        throw CDNFException(
            "CPacketTranslater::OnDBReplyUnconnGuildMember : 0 == pclGuild ,the guild is out");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCallGuildMembers() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x8b5, "./log/Except", "CPacketTranslater::OnDBReplyUnconnGuildMember() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCallGuildMembers() Exception Break");
        DNF_LOG_SCOPE_LINE(0x8bb, "./log/Except", "CPacketTranslater::OnDBReplyUnconnGuildMember() Exception Break\n");
    }
}void CPacketTranslater::OnCallGuildInvite(PacketHeader* pkt)
{
    Packet_Guild_Call_Guild_Invite* pb = (Packet_Guild_Call_Guild_Invite*)pkt;
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnMonitorSendGuildLetter : 0 == m_pclApp");
        Packet_Guild_Call_Guild_Invite_To_Caller callerPkt;
        if (pb->m_guildKey == 0)
        {
            throw CDNFException("CPacketTranslater::OnCallGuildInvite : packet->m_uGuildKey == 0");
        }
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* caller;
        if ((caller = um->FindUser_CharNo(pb->m_caller)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x8db,"./log/Except",
                "CPacketTranslater::OnCallGuildInvite : 0 == pclCaller, Char Key = %d",
                pb->m_caller);
            return;
        }
        callerPkt.m_a = pb->m_caller;
        callerPkt.m_e = caller->GetIdByChannel();
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(
                 pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x8e4,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : 0 == pclGuild, Guild Key = %d",
                pb->m_guildKey);
            callerPkt.m_12 = 0x22;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        if (guild->IsSetGuildDBFlag(4) != 1)
        {
            DNF_LOG_SCOPE_LINE(0x8eb,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
                pb->m_guildKey);
            callerPkt.m_12 = 0x22;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        if (!guild->IsGuildMaster(pb->m_caller) &&
            !guild->IsSubGuildMaster(pb->m_caller) &&
            (char)caller->GetGuildMemDBInfo()->m_grade != 0x03)
        {
            DNF_LOG_SCOPE_LINE(0x8f4,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
                pb->m_guildKey,
                pb->m_caller);
            callerPkt.m_12 = 0x24;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        if (300 < (guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff) + 1)
        {
            DNF_LOG_SCOPE_LINE(0x8fd,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : cnt(%d), Guild Key = %d",
                guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff,
                pb->m_guildKey);
            callerPkt.m_12 = 0x26;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        CUser* invited =
            um->FindUser_CharName(std::string(pb->m_name));
        if (invited == 0)
        {
            DNF_LOG_SCOPE_LINE(0x905,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : 0 == pclInvitedUser, Char Key = %d",
                pb->m_caller);
            callerPkt.m_12 = 3;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        if (caller->IsBlackUser(invited->GetUniqCharNo()) != 0 ||
            invited->IsBlackUser(caller->GetUniqCharNo()) != 0)
        {
            DNF_LOG_SCOPE_LINE(0x90d,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : 0 == pclInvitedUser, Char Key = %d",
                pb->m_caller);
            callerPkt.m_12 = 0x4d;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            return;
        }
        if (invited->GetGuildKey() == 0)
        {
            callerPkt.m_12 = 0;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
            invited->SetGuildInviteFact(guild->GetGuildKey(), caller->GetUniqCharNo(), 1);
            Packet_Guild_Call_Guild_Invite_To_Invited invitedPkt;
            invitedPkt.m_a = invited->GetUniqCharNo();
            invitedPkt.m_e = invited->GetIdByChannel();
            memcpy(invitedPkt.m_name, caller->GetCharName(), 0x1d);
            memcpy(invitedPkt.m_guildName, guild->GetGuildName(), 0x16);
            invited->SendToGameserver((char*)&invitedPkt, 0x47);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x916,"./log/GuildModify",
                "CPacketTranslater::OnCallGuildInvite : 0 != pclInvitedUser.GetGuildKey(), Char Key = %d",
                pb->m_caller);
            callerPkt.m_12 = 0x27;
            caller->SendToGameserver((char*)&callerPkt, 0x16);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x92d, "./log/Except", "CPacketTranslater::OnCallGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x932, "./log/Except", "CPacketTranslater::OnCallGuildInvite() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyGuildInvite(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnReplyGuildInvite : 0 == m_pclApp");
    unsigned int charNo = ((Packet_Guild_Reply_Guild_Invite_From_Invited*)pkt)->m_charNo;
    CUser* invited;
    if ((invited = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x94d,"./log/Except",
            "CPacketTranslater::OnReplyGuildInvite : 0 == pclInvitedUser, Char Key = %d", charNo);
        return;
    }
    Packet_Guild_Reply_Guild_Invite_To_Invited invitedPkt;
    invitedPkt.m_a = charNo;
    invitedPkt.m_e = invited->GetIdByChannel();
    int callerId = (int)invited->GetGuildInviteCallerId();
    int guildId = (int)invited->GetGuildInviteGuildId();
    if (callerId == 0 || guildId == 0)
    {
        invitedPkt.m_12 = 0x67;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        return;
    }
    CGuild* guild;
    if ((guild = (&m_pclApp->m_guildManager)->FindGuild((unsigned int)guildId)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x962,"./log/GuildModify",
            "CPacketTranslater::OnReplyGuildInvite : 0 == pclGuild, Guild Key = %d", guildId);
        invitedPkt.m_12 = 0x22;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        CUser* caller;
        if ((caller = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)callerId)) != 0)
        {
            Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
            callerPkt.m_a = callerId;
            callerPkt.m_e = caller->GetIdByChannel();
            callerPkt.m_12 = 0x22;
            caller->SendToGameserver((char*)&callerPkt, 0x34);
        }
        return;
    }
    if ((guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff) + 1 < 0x12d)
    {
        if (((Packet_Guild_Reply_Guild_Invite_From_Invited*)pkt)->m_accept == 1)
        {
            Packet_DBMW_Save_Guild_Join joinPkt;
            unsigned int dbid = invited->GetDBID();
            joinPkt.m_groupGuildId = ((unsigned int)invited->GetGameServer()->GetGroupNo() << 24) |
                                     ((unsigned int)guildId & 0xffffff);
            joinPkt.m_guildIdHi = (unsigned char)((unsigned int)guildId >> 24);
            joinPkt.m_dbidLo = (unsigned char)(dbid & 0xff);
            joinPkt.m_dbidMid = (unsigned short)((dbid >> 8) & 0xffff);
            joinPkt.m_dbidHi = (unsigned char)(dbid >> 24);
            joinPkt.m_callerIdLo = (unsigned char)((unsigned int)callerId & 0xff);
            joinPkt.m_callerIdMid = (unsigned short)(((unsigned int)callerId >> 8) & 0xffff);
            joinPkt.m_callerIdHi = (unsigned char)((unsigned int)callerId >> 24);
            joinPkt.m_uniqCharNo = invited->GetUniqCharNo();
            memcpy(joinPkt.m_name, invited->GetCharName(), 0x1d);
            joinPkt.m_job = invited->GetJob();
            joinPkt.m_growth = invited->GetGrowthType();
            joinPkt.m_level = invited->GetLevel();
            joinPkt.m_sex = invited->GetSex();
            memcpy(joinPkt.m_ssn, invited->GetSsn(), 2);
            m_pclApp->Get_ServerHandler()->GetDBServer()->SendToServer((char*)&joinPkt, 0x40);
        }
        else
        {
            CUser* caller;
            if ((caller = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)callerId)) != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
                callerPkt.m_a = callerId;
                callerPkt.m_e = caller->GetIdByChannel();
                callerPkt.m_12 = (unsigned int)((Packet_Guild_Reply_Guild_Invite_From_Invited*)pkt)->m_accept;
                memcpy(callerPkt.m_rest, invited->GetCharName(), 0x1d);
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
        invitedPkt.m_12 = 0x26;
        invited->SendToGameserver((char*)&invitedPkt, 0x16);
        invited->SetGuildInviteFact(0, 0, 0xff);
        CUser* caller;
        if ((caller = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)callerId)) != 0)
        {
            Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
            callerPkt.m_a = callerId;
            callerPkt.m_e = caller->GetIdByChannel();
            callerPkt.m_12 = 0x26;
            caller->SendToGameserver((char*)&callerPkt, 0x34);
        }
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x9b8, "./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x9bd, "./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWGuildJoin(PacketHeader* pkt)
{
    Packet_DBMW_Guild_Join* pb = (Packet_DBMW_Guild_Join*)pkt;
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnReplyGuildInvite : 0 == m_pclApp");
        CUser* joinUser;
        if ((joinUser = (&m_pclApp->m_userManager)->FindUser_CharNo(
                 pb->m_joiner)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x9d8, "./log/GuildModify", "CPacketTranslater::OnDBMWGuildJoin() 0 == pclJoinUser");
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
            pb->m_guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x9dc, "./log/GuildModify", "CPacketTranslater::OnDBMWGuildJoin() 0 == pclGuild");
        }
        if (pb->m_result == 0)
        {
            GuildJoin(guild, joinUser, pb->m_caller);
        }
        else
        {
            CUser* caller;
            if ((caller = (&m_pclApp->m_userManager)->FindUser_CharNo(
                     pb->m_caller)) != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Caller callerPkt;
                callerPkt.m_a = pb->m_caller;
                callerPkt.m_e = caller->GetIdByChannel();
                callerPkt.m_12 = pb->m_result;
                caller->SendToGameserver((char*)&callerPkt, 0x34);
            }
            if (joinUser != 0)
            {
                Packet_Guild_Reply_Guild_Invite_To_Invited invitedPkt;
                invitedPkt.m_a = pb->m_joiner;
                invitedPkt.m_e = joinUser->GetIdByChannel();
                invitedPkt.m_12 = pb->m_result;
                joinUser->SendToGameserver((char*)&invitedPkt, 0x16);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xa2c, "./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xa31, "./log/Except", "CPacketTranslater::OnReplyGuildInvite() Exception Break\n");
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
            // ORIG：ctor 后无 memset；+8=GetUniqCharNo、+0x41=dbid、+0x45=0，
            // +4=GetDBID 在两次 memcpy 之后最后写入
            info.m_guildKey = guild->GetGuildKey();
            info.m_charNo = user->GetUniqCharNo();
            info.m_field_4b = dbid;
            info.m_field_4f = 0;
            memcpy(info.m_charName, user->GetCharName(), 0x1d);
            memcpy(info.m_guildName, guild->GetGuildName(), 0x16);
            info.m_dbid = user->GetDBID();
            guild->NoticeEnterToGuildMember((char*)&info);
        }
        user->MakeGameServerSendUserInfoPacket(guild->GetGuildKey());
        guild->SendGuildInfoToMembers(false);
    }
    return;
}

void CPacketTranslater::GuildJoin(CGuild* guild, STGuildJoinInfo* joinInfo, unsigned int dbid)
{
    if (guild != 0)
    {
        if (guild->IsSetGuildDBFlag(4) != 0 || guild->IsSetGuildDBFlag(0x10) != 0)
        {
            STGuildMemberProxy proxy;
            memset(&proxy, 0, sizeof(proxy));
            // ORIG（ORIG 反汇编铁证，GuildJoin+0x12f 起）：
            //   +0x27=0、+0x23=pInfo[0x33](growType)、+0x22=pInfo[0x32](lev)、
            //   +0x26=pInfo[0x35](sex)、memcpy(+4,pInfo+0x14,0x1d)、
            //   +0=pInfo[0x10](characNo)、+0x28=0
            proxy.m_grade = 0;
            proxy.m_growType = ((STGuildJoinInfo*)joinInfo)->m_growType;
            proxy.m_job = ((STGuildJoinInfo*)joinInfo)->m_lev;
            proxy.m_sex = ((STGuildJoinInfo*)joinInfo)->m_sex;
            memcpy(proxy.m_name, ((STGuildJoinInfo*)joinInfo)->m_characName, 0x1d);
            proxy.m_no = ((STGuildJoinInfo*)joinInfo)->m_characNo;
            proxy.m_lastPlayTime = 0;
            if (guild->LoadGuildOneMemberProxy(proxy) != 1)
            {
                guild->IncTotalCnt_Of_GuildDBInfo();
            }
            DNF_LOG_SCOPE_LINE(0xa71,"./log/GuildModify", "GUILD JOIN guild(%s) char(%s)",
                guild->GetGuildName(), (char*)joinInfo + 0x14);
        }
        ST_Notice_Guild_Enter info;
        // ORIG：ctor 后无 memset；+8=pInfo[0x10](characNo)、+0x41=dbid、+0x45=0，
        // +4=pInfo[8](m_id) 在两次 memcpy 之后最后写入
        info.m_guildKey = guild->GetGuildKey();
        info.m_charNo = ((STGuildJoinInfo*)joinInfo)->m_characNo;
        info.m_field_4b = dbid;
        info.m_field_4f = 0;
        memcpy(info.m_charName, ((STGuildJoinInfo*)joinInfo)->m_characName, 0x1d);
        memcpy(info.m_guildName, guild->GetGuildName(), 0x16);
        info.m_dbid = ((STGuildJoinInfo*)joinInfo)->m_id;
        guild->NoticeEnterToGuildMember((char*)&info);
        guild->SendGuildInfoToMembers(false);
    }
    return;
}

void CPacketTranslater::OnMonitorSendGuildLetter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnMonitorSendGuildLetter : 0 == m_pclApp");
    char* pb = (char*)pkt;
    Packet_Monitor_Reply_Guild_Mail reply;
    unsigned int charNo = ((Packet_Monitor_Send_Guild_Mail*)pb)->m_charNo;
    unsigned int guildId = ((Packet_Monitor_Send_Guild_Mail*)pb)->m_guildId;
    const char* msg = ((Packet_Monitor_Send_Guild_Mail*)pb)->m_msg;
    {
        DNF_LOG_SCOPE_LINE(0xa98, "./log/GuildLetter", "charNo(%d),guildId(%d),msg(%s)", charNo, guildId, msg);
    }
    if (guildId == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnMonitorSendGuildLetter : packet->m_uGuildKey == 0");
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xaa0,"./log/GuildModify",
            "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclUser, Char Key = %d", charNo);
        return;
    }
    reply.m_charNo = charNo;
    reply.m_channel = user->GetIdByChannel();
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildId);
    if (guild != 0)
    {
        if (!(guild->IsGuildMaster(charNo) || guild->IsSubGuildMaster(charNo)))
        {
            DNF_LOG_SCOPE_LINE(0xab0,"./log/GuildModify",
                "CPacketTranslater::OnMonitorSendGuildLetter : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
                guildId, charNo);
            reply.m_result = 0x24;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
        else
        {
            if (user->GetGameServer() == 0)
            {
                DNF_LOG_SCOPE_LINE(0xab8,"./log/GuildModify",
                    "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclUser->GetGameServer(), g(%d), c(%d)",
                    guildId, charNo);
                reply.m_result = 1;
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
            else
            {
                SendPacketGuildMail(user->GetGameServer()->GetGroupNo(), charNo, guildId, "", msg, 0xffffffff);
            }
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xaa8,"./log/GuildModify",
            "CPacketTranslater::OnMonitorSendGuildLetter : 0 == pclGuild, Guild Key = %d", guildId);
        reply.m_result = 0x22;
        user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xac2, "./log/Except", "CPacketTranslater::OnMonitorSendGuildLetter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xac7, "./log/Except", "CPacketTranslater::OnMonitorSendGuildLetter() Exception Break\n");
    }
}

void CPacketTranslater::SendPacketGuildMail(unsigned char group, unsigned int charNo,
                                            unsigned int guildKey, const char* title,
                                            const char* content,
                                            unsigned int param)
{
    Packet_DBMW_Send_Guild_Mail pkt;
    char gap[0x1];
    pkt.m_group = group;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    int contentLen = strlen(content);
    if (contentLen <= 0xff)
    {
        memcpy(pkt.m_content, content, contentLen);
    }
    else
    {
        memcpy(pkt.m_content, content, 0xff);
    }
    int titleLen = strlen(title);
    if (titleLen <= 0x10)
    {
        memcpy(pkt.m_title, title, titleLen);
    }
    else
    {
        memcpy(pkt.m_title, title, 0x10);
    }
    pkt.m_param = param;
    m_pclApp->m_serverHandler->SendToDB(&pkt);
}

void CPacketTranslater::OnDBMWReplySendGuildLetter(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnDBMWReplySendGuildLetter : 0 == m_pclApp");
    char* pb = (char*)pkt;
    Packet_Monitor_Reply_Guild_Mail reply;
    if (((Packet_DBMW_Reply_Send_Guild_Letter*)pb)->m_guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnDBMWReplySendGuildLetter : packet->m_uGuildKey == 0");
    }
    unsigned int charNo = ((Packet_DBMW_Reply_Send_Guild_Letter*)pb)->m_charNo;
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb0e,"./log/GuildModify",
            "CPacketTranslater::OnDBMWReplySendGuildLetter : 0 == pclUser, Char Key = %d", charNo);
    }
    else
    {
        reply.m_charNo = charNo;
        reply.m_channel = user->GetIdByChannel();
        reply.m_result = ((Packet_DBMW_Reply_Send_Guild_Letter*)pb)->m_result;
        user->SendToGameserver((char*)&reply, 0x13);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xb19, "./log/Except", "CPacketTranslater::OnDBMWReplySendGuildLetter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb1e, "./log/Except", "CPacketTranslater::OnDBMWReplySendGuildLetter() Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildSecede(PacketHeader* pkt)
{
    Packet_Guild_Request_Guild_Secede* pb = (Packet_Guild_Request_Guild_Secede*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildSecede : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Secede resp;
    unsigned int guildKey = pb->m_guildKey;
    unsigned int requesterCharNo = pb->m_requesterCharNo;
    if (guildKey == 0)
    {
        throw CDNFException("CPacketTranslater::OnCallGuildInvite : packet->m_uGuildKey == 0");
    }
    CUser* requester;
    if ((requester = (&m_pclApp->m_userManager)->FindUser_CharNo(requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb3d,"./log/Except",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclRequester, Char Key = %d",
            requesterCharNo);
        return;
    }
    resp.m_requester = requesterCharNo;
    resp.m_channel = requester->GetIdByChannel();
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
    if (guild != 0)
    {
        if (guild->IsSetGuildDBFlag(4) != 1)
        {
            DNF_LOG_SCOPE_LINE(0xb4e,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildSecede : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
                guildKey);
            resp.m_secedeType = 0x22;
            requester->SendToGameserver((char*)&resp, 0x52);
            return;
        }
        Packet_DB_Request_Guild_Secede dbPkt;
        dbPkt.m_guildKey = guildKey;
        dbPkt.m_charNo = requesterCharNo;
        size_t nameLen = pb->m_nameLen;
        dbPkt.m_nameLen = nameLen;
        memcpy(dbPkt.m_name, pb->m_name, nameLen);
        dbPkt.m_masterId = guild->GetMasterId();
        if (pb->m_nameLen == 0)
        {
            dbPkt.m_flag = 1;
            const char* name = requester->GetCharName();
            memcpy(dbPkt.m_name, name, strlen(name));
        }
        else
        {
            if (!guild->IsGuildMaster(requesterCharNo) &&
                !guild->IsSubGuildMaster(requesterCharNo))
            {
                DNF_LOG_SCOPE_LINE(0xb60,"./log/GuildModify",
                    "CPacketTranslater::OnRequestGuildSecede : IsGuildMaster or IsSubGuildMaster, g(%d), c(%d)",
                    guildKey, requesterCharNo);
            resp.m_secedeType = 0x24;
                requester->SendToGameserver((char*)&resp, 0x52);
                return;
            }
            dbPkt.m_flag = 2;
        }
        m_pclApp->m_serverHandler->SendToDB(&dbPkt);
        return;
    }
    DNF_LOG_SCOPE_LINE(0xb46,"./log/GuildModify",
        "CPacketTranslater::OnRequestGuildSecede : 0 == pclGuild, Guild Key = %d", guildKey);
    resp.m_secedeType = 0x22;
    requester->SendToGameserver((char*)&resp, 0x52);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xb7e, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb83, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildSecede(PacketHeader* pkt)
{
    Packet_DB_Reply_Guild_Secede* pb = (Packet_DB_Reply_Guild_Secede*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildSecede : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Secede resp;
    CUser* requester;
    if ((requester = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb9e,"./log/Except",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclCaller, Char Key = %d",
            pb->m_requesterCharNo);
        return;
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xba3,"./log/GuildModify",
            "CPacketTranslater::OnRequestGuildSecede : 0 == pclGuild, Char Key = %d (Maybe Requester was logout)",
            pb->m_requesterCharNo);
        return;
    }
    resp.m_guildKey = pb->m_guildKey;
    resp.m_totalCnt = guild->GetTotalCnt_Of_GuildDBInfo();
    resp.m_requester = pb->m_requesterCharNo;
    resp.m_channel = requester->GetIdByChannel();
    resp.m_secedeFlag = pb->m_secedeFlag;
    memcpy(resp.m_name, pb->m_name,
           pb->m_nameLen);
    memcpy(resp.m_guildName, guild->GetGuildName(), 0x16);
    if (pb->m_secedeType == 0 || pb->m_secedeType == 1)
    {
        if (pb->m_requesterCharNo != pb->m_targetCharNo)
        {
            CUser* target;
            if ((target = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_targetCharNo)) == 0)
            {
                Packet_No_Cache noCache;
                noCache.ma = pb->m_dbid;
                noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
                noCache.m12 = 1;
                m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&noCache);
                noCache.ma = pb->m_dbid;
                noCache.me = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
                noCache.m12 = 2;
                m_pclApp->Get_ServerHandler()->SendTcpGameServerFirst(&noCache);
                Packet_DBMW_Query_Msg query;
                query.m_queryId = 0x4f00;
                query.m_handleIdx = 2;
                sprintf(query.m_data,
                        "upDate charac_info set guild_secede = 1 where charac_no = %u",
                        pb->m_targetCharNo);
                m_pclApp->Get_ServerHandler()->SendToDB(&query);
            }
            else
            {
                Packet_Guild_Exp_Book_Delete expDel;
                expDel.m_channel = target->GetIdByChannel();
                expDel.m_charNo = target->GetUniqCharNo();
                expDel.m16 = (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
                expDel.m_group = target->GetDBID();
                target->SendTcpGameserver((PacketHeader*)&expDel);
            }
        }
        ST_Notice_Guild_Secede notice;
        notice.m_guildKey = pb->m_guildKey;
        notice.m_charNo = pb->m_targetCharNo;
        notice.m_secedeFlag = (unsigned short)(signed char)pb->m_secedeFlag;
        memcpy(notice.m_charName, pb->m_name, 0x1d);
        memcpy(notice.m_guildName, guild->GetGuildName(), 0x16);
        guild->NoticeSecedeToGuildMember((char*)&notice);
        (&m_pclApp->m_guildManager)->GuildSecede(pb->m_guildKey, notice);
        guild->SendGuildInfoToMembers(false);
        if (guild != 0 && pb->m_secedeType == 1)
        {
            if (guild->GetTotalCnt_Of_GuildDBInfo() != 0)
            {
                DNF_LOG_SCOPE_LINE(0xbf3,"./log/GuildModify",
                    "CPacketTranslater::OnDBReplyGuildSecede : Guild(%d) Deleted, rest member count(%d)",
                    guild->GetGuildKey(), guild->GetTotalCnt_Of_GuildDBInfo() & 0xffff);
            }
            (&m_pclApp->m_guildManager)->GuildDismiss(guild);
        }
        resp.m_secedeType = 0;
        requester->SendToGameserver((char*)&resp, 0x52);
    }
    else
    {
        resp.m_secedeType = (unsigned int)pb->m_secedeType;
        requester->SendToGameserver((char*)&resp, 0x52);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xbff, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xc04, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnRequestGuildMasterDelegate(PacketHeader* pkt)
{
    Packet_Guild_Request_Guild_Master_Delegate* pb = (Packet_Guild_Request_Guild_Master_Delegate*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildMasterDelegate : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Master_Delegate resp;
    unsigned int guildKey = pb->m_guildKey;
    unsigned int requesterCharNo = pb->m_requesterCharNo;
    if (guildKey != 0)
    {
        CUser* requester;
        if ((requester = (&m_pclApp->m_userManager)->FindUser_CharNo(requesterCharNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc23,"./log/Except",
                "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclRequester, Char Key = %d",
                requesterCharNo);
            return;
        }
        resp.m_a = requesterCharNo;
        resp.m_e = requester->GetIdByChannel();
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc2c,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclGuild, Guild Key = %d",
                guildKey);
            resp.m_12 = 0x22;
            requester->SendToGameserver((char*)&resp, 0x16);
            return;
        }
        if (guild->IsSetGuildDBFlag(4) == 1)
        {
            if (guild->IsGuildMaster(requesterCharNo))
            {
                CUser* delegatee = (&m_pclApp->m_userManager)->FindUser_CharName(
                    std::string(pb->m_name));
                if (delegatee != 0 && !guild->IsSubGuildMaster(delegatee->GetUniqCharNo()))
                {
                    DNF_LOG_SCOPE_LINE(0xc49,"./log/GuildModify",
                        "CPacketTranslater::OnRequestGuildMasterDelegate : Delegatee is Not Sub Guild Master(%d)(%d)",
                        guildKey, delegatee->GetUniqCharNo());
                resp.m_12 = 0x56;
                    requester->SendToGameserver((char*)&resp, 0x16);
                    return;
                }
                if (requester->GetGameServer() != 0)
                {
                    Packet_DB_Request_Guild_Master_Delegate dbPkt;
                    dbPkt.m_guildKey = guildKey;
                    dbPkt.m_charNo = requesterCharNo;
                    dbPkt.m_group = requester->GetGameServer()->GetGroupNo();
                    memcpy(dbPkt.m_name, pb->m_name, 0x1d);
                    m_pclApp->m_serverHandler->SendToDB(&dbPkt);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0xc3d,"./log/GuildModify",
                    "CPacketTranslater::OnRequestGuildMasterDelegate : Requester is Not Guild Master(%d)(%d)",
                    guildKey, requesterCharNo);
                resp.m_12 = 0x24;
                requester->SendToGameserver((char*)&resp, 0x16);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xc34,"./log/GuildModify",
                "CPacketTranslater::OnRequestGuildMasterDelegate : !( m_eGuildDBFlag & GUILD_DB_LOAD_STATE ), Guild Key = %d",
                guildKey);
            resp.m_12 = 0x22;
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
        DNF_LOG_SCOPE_LINE(0xc5d, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xc62, "./log/Except", "CPacketTranslater::OnRequestGuildSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildMasterDelegate(PacketHeader* pkt)
{
    Packet_DB_Reply_Guild_Master_Delegate* pb = (Packet_DB_Reply_Guild_Master_Delegate*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnRequestGuildMasterDelegate : 0 == m_pclApp");
    Packet_Guild_Reply_Guild_Master_Delegate resp;
    unsigned int guildKey = pb->m_guildKey;
    unsigned int requesterCharNo = pb->m_requesterCharNo;
    unsigned int delegateeCharNo = pb->m_delegateeCharNo;
    int result = pb->m_result;
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnDBReplyGuildMasterDelegate : packet->m_uGuildKey == 0");
    }
    CUser* requester;
    if ((requester = (&m_pclApp->m_userManager)->FindUser_CharNo(requesterCharNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xc82,"./log/GuildModify",
            "CPacketTranslater::OnRequestGuildMasterDelegate : 0 == pclRequester, Char Key = %d",
            requesterCharNo);
        return;
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
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
            guild->NoticeGuildMasterDelegateToMembers(
                pb->m_name);
        }
        if (requester != 0)
        {
            resp.m_a = requesterCharNo;
            resp.m_e = requester->GetIdByChannel();
            resp.m_12 = result;
            requester->SendToGameserver((char*)&resp, 0x16);
        }
        CUser* delegatee;
        if ((delegatee = (&m_pclApp->m_userManager)->FindUser_CharNo(delegateeCharNo)) != 0)
        {
            Packet_Notice_Has_Been_Guild_Master notice;
            notice.m_charNo = delegateeCharNo;
            notice.m_channel = delegatee->GetIdByChannel();
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
        DNF_LOG_SCOPE_LINE(0xccc, "./log/Except", "CPacketTranslater::OnDBReplyGuildMasterDelegate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xcd1, "./log/Except", "CPacketTranslater::OnDBReplyGuildMasterDelegate() Exception Break\n");
    }
}

void CPacketTranslater::OnGuildMasterDelegateFromWeb(PacketHeader* pkt)
{
    Packet_Guild_Master_Delegate_FromWeb* pb = (Packet_Guild_Master_Delegate_FromWeb*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnGuildMasterDelegateFromWeb : 0 == m_pclApp");
    unsigned int guildKey = pb->m_guildKey;
    unsigned int requesterCharNo = pb->m_requesterCharNo;
    unsigned int delegateeCharNo = pb->m_delegateeCharNo;
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnGuildMasterDelegateFromWeb : packet->m_uGuildKey == 0");
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
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
            if ((requester = (&m_pclApp->m_userManager)->FindUser_CharNo(requesterCharNo)) == 0)
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
                guild->NoticeGuildMasterDelegateToMembers(
                    pb->m_msg);
                CUser* delegatee;
                if ((delegatee = (&m_pclApp->m_userManager)->FindUser_CharNo(delegateeCharNo)) != 0)
                {
                    Packet_Notice_Has_Been_Guild_Master notice;
                    notice.m_charNo = delegateeCharNo;
                    notice.m_channel = delegatee->GetIdByChannel();
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
        DNF_LOG_SCOPE_LINE(0xd39, "./log/Except", "CPacketTranslater::OnGuildMasterDelegateFromWeb() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xd3e, "./log/Except", "CPacketTranslater::OnGuildMasterDelegateFromWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnCheckGuildMemberConnectionFromWeb(PacketHeader* pkt)
{
    Packet_Check_Guild_Member_Connection_From_Web* pb = (Packet_Check_Guild_Member_Connection_From_Web*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCheckGuildMemberConnectionFromWeb : 0 == m_pclApp");
    unsigned int guildKey = pb->m_guildKey;
    unsigned short port = pkt->reversed1;
    unsigned int ip = pkt->m_connNo;
    if (guildKey == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb : packet->m_uGuildKey == 0");
    }
    {
        Packet_Answer_Guild_Member_Connection_From_Web resp;
        resp.m_guildKey = guildKey;
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild == 0 || guild->IsSetGuildDBFlag(4) != 1)
        {
            unsigned short size = (unsigned short)(0 * 5 + 0x10);
            resp.packetSize = size;
            if (m_pclApp->Get_UdpHandler()->SendToClient((char*)&resp, (int)size, port, 0, ip) != 1)
            {
                throw CDNFException(strerror(errno));
            }
        }
        else
        {
            short count = (short)guild->ReplyGuildMembersToWeb(
                (STGuildMemberWebConnInfo*)((char*)&resp + 0x10));
            unsigned short size = (unsigned short)(count * 5 + 0x10);
            resp.packetSize = size;
            if (m_pclApp->Get_UdpHandler()->SendToClient((char*)&resp, (int)size, port, 0, ip) != 1)
            {
                throw CDNFException(strerror(errno));
            }
        }
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xd6e, "./log/Except", "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xd73, "./log/Except", "CPacketTranslater::OnCheckGuildMemberConnectionFromWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildLevelUp(PacketHeader* pkt)
{
    Packet_Monitor_Call_Guild_Level_Up* pb = (Packet_Monitor_Call_Guild_Level_Up*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildLevelUp : 0 == m_pclApp")
    if (pb->m_guildKey == 0)
    {
        throw CDNFException("CPacketTranslater::OnCallGuildLevelUp : packet->m_uGuildKey == 0");
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(
             pb->m_charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd90, "./log/Except",
            "CPacketTranslater::OnCallGuildLevelUp : 0 == pclUser, Char Key = %d",
            pb->m_charNo);
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
        pb->m_guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd94, "./log/Except",
            "CPacketTranslater::OnCallGuildLevelUp : 0 == pclGuild, Guild Key = %d",
            pb->m_guildKey);
        return;
    }
    if (pb->m_fund > guild->GetGuildFund())
    {
        DNF_LOG_SCOPE_LINE(0xd9a, "./log/GuildModify",
            "CPacketTranslater::OnCallGuildLevelUp(%d : %d) Guild Fund Shortage",
            pb->m_guildKey,
            pb->m_charNo);
        return;
    }
    guild->SubGuildFund(pb->m_fund);
    unsigned char ret = (unsigned char)guild->GuildLevelUp(m_pclApp->Get_ServerHandler(), user);
    if (ret != 0)
    {
        DNF_LOG_SCOPE_LINE(0xda3, "./log/GuildModify",
            "CPacketTranslater::OnCallGuildLevelUp(%d : %d) ERR(%d)",
            pb->m_guildKey,
            pb->m_charNo, (int)ret);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xda7, "./log/Except", "CPacketTranslater::OnCallGuildLevelUp() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xdac, "./log/Except", "CPacketTranslater::OnCallGuildLevelUp() Exception Break\n");
    }
}

void CPacketTranslater::OnCallGuildInfo(PacketHeader* pkt)
{
    Packet_Monitor_Call_Guild_Info* pb = (Packet_Monitor_Call_Guild_Info*)pkt;
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnCallGuildInfo : 0 == m_pclApp")
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* user;
        if ((user = um->FindUser_CharNo(pb->m_charNo)) == 0)
        {
            CMyFileLog log(__FUNCTION__, 0xdc5);
            log("./log/Except",
                "CPacketTranslater::OnCallGuildInfo() pclUser = 0, Char Key : %d\n",
                pb->m_charNo);
            return;
        }
        Packet_Monitor_Call_Guild_Info_Reply replyPkt;
        replyPkt.ma = pb->m_fieldA;
        replyPkt.me = pb->m_charNo;
        replyPkt.m12 = pb->m_count;
        for (int i = 0; i < (unsigned char)pb->m_count; i++)
        {
            if (pb->m_guildKeys[i] == 0)
            {
                CMyFileLog log(__FUNCTION__, 0xdd3);
                log("./log/Guild",
                    "CPacketTranslater::OnCallGuildInfo : packet->m_uGuildKey[%d] == 0", i);
                continue;
            }
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKeys[i]);
            if (guild == 0)
            {
                continue;
            }
            replyPkt.m_infos[i].m_field0 = pb->m_guildKeys[i];
            replyPkt.m_infos[i].m_field4 = guild->GetGuildLevel();
            replyPkt.m_infos[i].m_field5 = guild->GetGuildRank();
            memcpy(replyPkt.m_infos[i].m_data, guild->GetGuildName(), 0x16);
        }
        unsigned short totalLen =
            (unsigned short)((unsigned char)pb->m_count * 0x1d + 0x13);
        replyPkt.packetSize = totalLen;
        user->SendToGameserver((char*)&replyPkt, totalLen);
    }
    catch (std::exception& e)
    {
        CMyFileLog log(__FUNCTION__, 0xde9);
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
    Packet_Notice_Guild_Mail_Arrive* pb = (Packet_Notice_Guild_Mail_Arrive*)pkt;
    for (int i = 0; i < (int)(unsigned int)(unsigned char)pb->m_count; i++)
    {
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKeys[i]);
        if (guild != 0)
        {
            guild->SendToGuildForMail();
        }
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xe10, "./log/Except", "CPacketTranslater::OnNoticeGuildMailArrive() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xe15, "./log/Except", "CPacketTranslater::OnNoticeGuildMailArrive() Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorManagerConnectOK(PacketHeader* pkt)
{
    (void)pkt;
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnMonitorManagerConnectOK : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SetManagerConnectFlag(true);
        DNF_LOG_SCOPE_LINE(0xe2d, "./log/Manager", "Manager Server Connect Success");
        puts("** Manager Server Connect Success **");
    }
    DNF_CATCH_LOG("./log/Except",
        "CPacketTranslater::OnMonitorManagerConnectOK() Exception Break", 0xe32, 0xe37);
}

void CPacketTranslater::OnEventStart(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        PacketHeader* pb = pkt;
        DNF_LOG_SCOPE_LINE(0xe4c,"./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n",
            ((Packet_Monitor_Event_Start*)pb)->m_eventType,
            (unsigned int)((Packet_Monitor_Event_Start*)pb)->m_eventParam1,
            (unsigned int)((Packet_Monitor_Event_Start*)pb)->m_eventParam2);
        m_pclApp->m_serverHandler->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventStart() \277\271\277\334 \271\337\273\375 : %s\n", ((std::exception&)e).what());
        DNF_LOG_SCOPE_LINE(0xe53, "./log/Except", "CPacketTranslater::OnEventStart() \277\271\277\334 \271\337\273\375 : %s\n", ((std::exception&)e).what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventStart() \277\271\277\334 \271\337\273\375");
        DNF_LOG_SCOPE_LINE(0xe59, "./log/Except", "CPacketTranslater::OnEventStart() \277\271\277\334 \271\337\273\375\n");
    }
}

void CPacketTranslater::OnEventEnd(PacketHeader* pkt)
{
    try
    {
    THROW_IF_NO_APP("CPacketTranslater::OnEventEnd : 0 == m_pclApp");
    PacketHeader* pb = pkt;
    DNF_LOG_SCOPE_LINE(0xe6b, "./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
        ((Packet_Monitor_Event_Start*)pb)->m_eventType);
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() \277\271\277\334 \271\337\273\375 : %s\n", ((std::exception&)e).what());
        DNF_LOG_SCOPE_LINE(0xe72, "./log/Except", "CPacketTranslater::OnEventEnd() \277\271\277\334 \271\337\273\375 : %s\n", ((std::exception&)e).what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() \277\271\277\334 \271\337\273\375");
        DNF_LOG_SCOPE_LINE(0xe78, "./log/Except", "CPacketTranslater::OnEventEnd() \277\271\277\334 \271\337\273\375\n");
    }
}

void CPacketTranslater::OnRegisterToBlackList(PacketHeader* pkt)
{
    try
    {
    Packet_Register_To_BlackList* pb = (Packet_Register_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xe8f, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == m_pclApp");
        return;
    }
    Packet_DBMW_Register_To_BlackList dbPkt;
    dbPkt.m_dbid = pb->m_dbid;
    memcpy(dbPkt.m_name, pb->m_name, 0x1d);
    CUser* requester = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid);
    if (requester == 0)
    {
        DNF_LOG_SCOPE_LINE(0xe9f, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == pclUser");
        return;
    }
    char* targetName = pb->m_name;
    if (strcmp(requester->GetCharName(), targetName) != 0 && requester->GetBlackListSize() < 10)
    {
        CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(targetName);
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
        DNF_LOG_SCOPE_LINE(0xed1, "./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xed6, "./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDeleteToBlackList(PacketHeader* pkt)
{
    try
    {
    Packet_Delete_To_BlackList* pb = (Packet_Delete_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xeeb, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == m_pclApp");
        return;
    }
    Packet_DMBW_Delete_To_BlackList dbPkt;
    dbPkt.m_dbid = pb->m_dbid;
    memcpy(dbPkt.m_name, pb->m_name, 0x1d);
    CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(pb->m_name);
    if (target != 0)
    {
        CUser* requester;
        if ((requester = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid)) == 0)
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
        DNF_LOG_SCOPE_LINE(0xf16, "./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf1b, "./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnRequestBlackList(PacketHeader* pkt)
{
    try
    {
    Packet_Request_BlackList* pb = (Packet_Request_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf2f, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == m_pclApp");
        return;
    }
    Packet_Request_Result_BlackList reply;
    reply.m_charNo = pb->m_idByChannel;
    reply.m_count = 0;
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf39, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == pclUser");
        return;
    }
    user->GetBlackList(reply.m_count, reply.m_items);
    user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xf45, "./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf4a, "./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWResisterToBlackList(PacketHeader* pkt)
{
    try
    {
    Packet_DBMW_Register_To_BlackList* pb = (Packet_DBMW_Register_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf5e,"./log/BlackList",
            "CPacketTranslater::OnDBMWResisterToBlackList : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xf65,"./log/BlackList",
            "CPacketTranslater::OnDBMWResisterToBlackList : 0 == pclUser");
        return;
    }
    Packet_Register_To_BlackList_RESULT reply;
    reply.m_channel = user->GetIdByChannel();
    memcpy(reply.m_name, pb->m_name, 0x1d);
    if (pb->m2c == (unsigned int)-1)
    {
        reply.m_result = 3;
        user->SendToGameserver((char*)&reply, reply.packetSize);
        return;
    }
    if (user->IsBlackUser(pb->m2c) == 0)
    {
        if (user->GetBlackListSize() < 10)
        {
            user->RegisterToBlackList(pb->m2c, pb->m_name);
            reply.m_result = 1;
            reply.m2c = pb->m2c;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
        else
        {
            reply.m_result = 4;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
    }
    else
    {
        reply.m_result = 2;
        user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xf8d, "./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf92, "./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWDeleteToBlackList(PacketHeader* pkt)
{
    try
    {
    Packet_DMBW_Delete_To_BlackList* pb = (Packet_DMBW_Delete_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xfa6,"./log/BlackList",
            "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser(pb->m_dbid)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xfad,"./log/BlackList",
            "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == pclUser");
        return;
    }
    Packet_Delete_To_BlackList_Result reply;
    reply.m_channel = user->GetIdByChannel();
    memcpy(reply.m_name, pb->m_name, 0x1d);
    if (pb->m2c == (unsigned int)-1)
    {
        reply.m_result = 3;
        user->SendToGameserver((char*)&reply, reply.packetSize);
        return;
    }
    if (user->DeleteToBlackList(pb->m2c) != 1)
    {
        reply.m_result = 2;
        user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    reply.m_result = 1;
    reply.m2c = pb->m2c;
    user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xfca, "./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xfcf, "./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::RequestBlackListToDBMW(unsigned int charNo)
{
    try
    {
        Packet_DBMW_Request_BlackList pkt;
        pkt.m_charNo = charNo;
        pkt.m_type = 0xcb;
        m_pclApp->m_serverHandler->SendToDB(&pkt);
    }
    catch (CDNFException& e)
    {
        CMyFileLog(__FUNCTION__, 0xfe7)("./log/Except",
            "CPacketTranslater::RequestBlackListToDBMW Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 0xfec)("./log/Except",
            "CPacketTranslater::RequestBlackListToDBMW Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWResponseBlackListOnLogin(PacketHeader* pkt)
{
    try
    {
    Packet_DBMW_Reponse_BlackList* pb = (Packet_DBMW_Reponse_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1000,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser(pb->m_mid)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1007,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == pclUser");
        return;
    }
    int i = 0;
    while (pb->m_blackList[i].m_occurTime != 0)
    {
        if (user->IsBlackUser((unsigned int)pb->m_blackList[i].m_field0) != 1)
        {
            user->RegisterToBlackList((unsigned int)pb->m_blackList[i].m_field0,
                                      pb->m_blackList[i].m_name,
                                      (unsigned int)pb->m_blackList[i].m_occurTime);
        }
        i++;
    }
    user->SetBlackListDBFlag(4);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x101b, "./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1020, "./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade(PacketHeader* pkt)
{
    Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade* pb =
        (Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1036,"./log/GuildModify",
                "CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade : 0 == m_pclApp");
            return;
        }
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x103c,"./log/GuildModify",
                "CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade : 0 == pclGuild(%d)",
                pb->m_guildId);
            return;
        }
        Packet_Monitor_Set_Sub_Guild_Master_Reply reply;
        int result = 0;
        if (pb->m_result == 0xff)
        {
            result = 2;
        }
        else if (pb->m_result == 0xfe)
        {
            result = 0x66;
        }
        else if (pb->m_grade == 1)
        {
            result = 0x58;
        }
        else if (pb->m_result == pb->m_grade)
        {
            result = 0x65;
        }
        else
        {
            if (pb->m_result == 2)
            {
                if (guild->GetCurSubGuildMasterCnt() < 5)
                {
                    guild->SetSubGuildMaster(pb->m_newGrade, true);
                    guild->SendGuildInfoToMembers(false);
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x105b,"./log/GuildModify", "CRITICAL ERR sub guild master cnt over(%d)",
                        (unsigned int)guild->GetCurSubGuildMasterCnt() & 0xff);
                    result = 0x59;
                }
            }
            else if (pb->m_grade == 2)
            {
                guild->SetSubGuildMaster(pb->m_newGrade, false);
                guild->SendGuildInfoToMembers(false);
            }
            if (result == 0)
            {
                guild->ChangeUnconnectedGuildMemberGrade(pb->m_newGrade, pb->m_result);
            }
        }
        {
            DNF_LOG_SCOPE_LINE(0x1073,"./log/GuildModify",
                "Change Guild(%d) Grade(%d) Prev Grade(%d) result(%d) unconnected charNo(%d) RequestNo(%d)",
                pb->m_guildId,
                (unsigned int)pb->m_result,
                (unsigned int)pb->m_grade,
                result,
                pb->m_newGrade,
                pb->m_characNo);
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_characNo);
        if (user != 0)
        {
            reply.m_a = result;
            reply.m_e = user->GetIdByChannel();
            reply.m_12 = user->GetUniqCharNo();
            reply.m_16 = pb->m_result;
            memcpy(reply.m_name, pb->m_name, 0x1d);
            user->SendToGameserver((char*)&reply, 0x3a);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade Exception Break", 0x1084, 0x1089);
}

void CPacketTranslater::OnNotifyMessageToGuild(PacketHeader* pkt)
{
    CGuild* guild;
    Packet_Web_Notify_Message_To_Guild* pb = (Packet_Web_Notify_Message_To_Guild*)pkt;
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x109d, "./log/Except", "CPacketTranslater::OnNotifyMessageToGuild : 0 == m_pclApp");
        return;
    }
    if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
    {
        return;
    }
    if (pb->m_mode != 1)
    {
        if (pb->m_mode != 2)
        {
            return;
        }
        Packet_Web_Notify_Message_To_Guild notify;
        memcpy(&notify, pb, 0x74);
        notify.m_mode = m_pclApp->Get_ServerGroup();
        m_pclApp->m_serverHandler->SendToDB(&notify);
    }
    guild->SetGuildMessage(pb->m_payload);
    guild->NotifyMessageToGuildMember();
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnNotifyMessageToGuild Exception Break", 0x10bf, 0x10c4);
}

void CPacketTranslater::OnRequestGuildCreate(PacketHeader* pkt)
{
    try
    {
    Packet_Request_Guild_Create* pb = (Packet_Request_Guild_Create*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x10d8, "./log/GuildModify", "CPacketTranslater::OnRequestGuildCreate : 0 == m_pclApp");
        return;
    }
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x10df, "./log/GuildModify", "CPacketTranslater::OnRequestGuildCreate : 0 == pclUser");
        return;
    }
    Packet_Reply_Guild_Create reply;
    reply.m_charNo = pb->m_charNo;
    reply.m_channel = user->GetIdByChannel();
    if (user->GetGuildKey() != 0)
    {
        reply.m_result = 0x20;
        user->SendToGameserver((char*)&reply, reply.packetSize);
        return;
    }
    if (user->GetGameServer() == 0)
    {
        reply.m_result = 1;
        user->SendToGameserver((char*)&reply, reply.packetSize);
        return;
    }
    Packet_DBMW_Request_Guild_Create dbPkt;
    dbPkt.m_group = user->GetGameServer()->GetGroupNo();
    dbPkt.m_dbid = user->GetDBID();
    dbPkt.m_charNo = user->GetUniqCharNo();
    memcpy(dbPkt.m_name, user->GetCharName(), 0x1d);
    dbPkt.m_job = user->GetJob();
    dbPkt.m_growth = user->GetGrowthType();
    dbPkt.m_level = user->GetLevel();
    dbPkt.m_sex = user->GetSex();
    memcpy(dbPkt.m_ssn, user->GetSsn(), 2);
    memcpy(dbPkt.m_guildName, pb->m_guildName, 0x16);
    memcpy(dbPkt.m_extra, pb->m_extra, 0xc);
    m_pclApp->Get_ServerHandler()->GetDBServer()->SendToServer((char*)&dbPkt, 0x5c);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1110, "./log/Except", "CPacketTranslater::OnRequestGuildCreate Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1115, "./log/Except", "CPacketTranslater::OnRequestGuildCreate Exception Break\n");
    }
}

void CPacketTranslater::OnDBReplyGuildCreate(PacketHeader* pkt)
{
    Packet_DBMW_Reply_Guild_Create* pbinfo = (Packet_DBMW_Reply_Guild_Create*)pkt;
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x112c, "./log/GuildModify", "CPacketTranslater::OnDBReplyGuildCreate : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = pbinfo->m_characNo;
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1133,"./log/GuildModify",
            "CPacketTranslater::OnDBReplyGuildCreate : 0 == pclRequestUser");
        return;
    }
    Packet_Notice_GuildName_On_Guild_Create notice;
    Packet_Reply_Guild_Create reply;
    unsigned int guildKey = pbinfo->m_guildId;
    notice.m_charNo = charNo;
    reply.m_charNo = charNo;
    unsigned int channel = user->GetIdByChannel();
    notice.m_channel = channel;
    reply.m_channel = channel;
    int result = pbinfo->m_result;
    reply.m_result = result;
    memcpy(reply.m_name, pbinfo->m_name, 0x16);
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
        notice.m_guildKey = guildKey;
        memcpy((char*)&notice + 0x16, pbinfo->m_name, 0x16);
        notice.m_group = m_pclApp->Get_ServerGroup();
        m_pclApp->m_serverHandler->SendAllTcpGameServer(&notice);
        (&m_pclApp->m_guildManager)->AttendGuild(guildKey, charNo);
        guild->QueryTodayGuildMember(m_pclApp->Get_ServerHandler());
    }
    user->SendTcpGameserver(&reply);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x116d, "./log/Except", "CPacketTranslater::OnDBReplyGuildCreate Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1172, "./log/Except", "CPacketTranslater::OnDBReplyGuildCreate Exception Break\n");
    }
}

void CPacketTranslater::OnPowerWarStartInfo(PacketHeader* pkt)
{
    Packet_Request_Power_War_Start_Info* pbinfo =
        (Packet_Request_Power_War_Start_Info*)pkt;
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
            start.m_eventType = 0x1e;
            m_pclApp->Get_ServerHandler()->SendToGameServer((unsigned char)pbinfo->m_field,
                                                            &start);
        }
        DNF_LOG_SCOPE_LINE(0x119a,"./log/Power", "OnPowerWarStartInfo(%d) - power war %s ", pbinfo->m_field,
            pm->IsPowerWarOn() ? "START!" : "END!");
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
            if (((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA != 1 &&
                ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA != 2)
            {
                if (((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x11c9,"./log/Power",
                        "ChangePowerWarPoint Invalid Power Side Income(side:%d, 0:%d, 1:%d, 2:%d, 3:%d, pp:%d)",
                        (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA,
                        ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldB,
                        ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldF,
                        ((Packet_Guild_Change_Power_War_Point*)pb)->m_field13,
                        ((Packet_Guild_Change_Power_War_Point*)pb)->m_field17,
                        (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_field2B);
                    return;
                }
                DNF_LOG_SCOPE_LINE(0x11ce,"./log/Power",
                    "ChangePowerWarPoint Invalid Power Side Income(side:%d, 0:%d, 1:%d, 2:%d, 3:%d, pp:%d)",
                    (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA,
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldB,
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldF,
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_field13,
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_field17,
                    (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_field2B);
                if (((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA == 3)
                {
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA = 1;
                }
                else
                {
                    ((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA = 2;
                }
            }
            int winScore = (int)(short)pm->IncPowerScore(
                (ENUM_POWER_SIDE_TYPE)((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA,
                ((Packet_Guild_Change_Power_War_Point*)pb)->m_field2B);
            ENUM_POWER_SIDE_TYPE loseSide;
            if (((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA == 1)
            {
                loseSide = (ENUM_POWER_SIDE_TYPE)2;
            }
            else
            {
                loseSide = (ENUM_POWER_SIDE_TYPE)1;
            }
            int loseScore = (int)(short)pm->IncPowerScore(
                loseSide, ((Packet_Guild_Change_Power_War_Point*)pb)->m_field2C);
            {
                DNF_LOG_SCOPE_LINE(0x11e1,"./log/Power", "win side(%d), win pp(%d, %d), lose pp(%d, %d)",
                    (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA,
                    (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_field2B,
                    winScore,
                    (unsigned int)((Packet_Guild_Change_Power_War_Point*)pb)->m_field2C, loseScore);
            }
            pm->UpdatePowerWarInfo(1,
                                   (ENUM_POWER_SIDE_TYPE)((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldA,
                                   ((Packet_Guild_Change_Power_War_Point*)pb)->m_field2B,
                                   &((Packet_Guild_Change_Power_War_Point*)pb)->m_fieldB);
            pm->UpdatePowerWarInfo(0, loseSide,
                                   ((Packet_Guild_Change_Power_War_Point*)pb)->m_field2C,
                                   &((Packet_Guild_Change_Power_War_Point*)pb)->m_field1B);
            std::vector<unsigned int> users;
            users.reserve(8);
            ((Packet_Guild_Change_Power_War_Point*)pkt)->GetUserList(users);
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
        Packet_Request_Join_Power* req = (Packet_Request_Join_Power*)pkt;
        Packet_Answer_Join_Power reply;
        reply.m_a = 0;
        reply.m_12 = req->m_charNo;
        reply.m_16 = req->m_field12;
        if (m_pclApp != 0)
        {
            unsigned int charNo = req->m_charNo;
            CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x123d, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == pclRequestUser");
                return;
            }
            reply.m_e = user->GetIdByChannel();
            unsigned int guildKey = req->m_guildKey;
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
            if (guildKey == 0 || guild == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1244, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == pclGuild");
                reply.m_a = 100;
                user->SendToGameserver((char*)&reply, reply.packetSize);
                return;
            }
            if (guild->IsSubGuildMaster(charNo) == 1 || guild->IsGuildMaster(charNo) == 1)
            {
                unsigned char side = req->m_field12 == 1 ? 3 : 4;
                guild->SetPowerSide(side);
                guild->IncPowerJoinCount();
                CServerInterface* gs = user->GetGameServer();
                guild->DBGuildSave(gs->GetGroupNo(), m_pclApp->Get_ServerHandler(), 0);
                reply.m_16 = side;
                user->SendToGameserver((char*)&reply, reply.packetSize);
                guild->SendGuildInfoToMembers(false);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x124f,"./log/Power",
                    "CPacketTranslater::OnPacketJoinPower : %d is not guild master or sub master(g:%d)",
                    charNo, guildKey);
                reply.m_a = 0x56;
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1237, "./log/Power", "CPacketTranslater::OnPacketJoinPower : 0 == m_pclApp");
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1291, "./log/Except", "CPacketTranslater::OnPacketJoinPower Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1296, "./log/Except", "CPacketTranslater::OnPacketJoinPower Exception Break\n");
    }
}

void CPacketTranslater::OnPacketSecedePower(PacketHeader* pkt)
{
    Packet_Request_Secede_Power* pb = (Packet_Request_Secede_Power*)pkt;
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12a9, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == m_pclApp");
        return;
    }
    Packet_Answer_Secede_Power reply;
    reply.m_a = 0;
    unsigned int charNo = pb->m_charNo;
    reply.m_12 = charNo;
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12b5, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == pclRequestUser");
        return;
    }
    reply.m_e = user->GetIdByChannel();
    unsigned int guildKey = pb->m_guildKey;
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
    if (guildKey == 0 || guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x12bc, "./log/Power", "CPacketTranslater::OnPacketSecedePower : 0 == pclGuild");
        reply.m_a = 100;
        user->SendToGameserver((char*)&reply, reply.packetSize);
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
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x12db,"./log/Power",
                    "CPacketTranslater::OnPacketSecedePower : %d power war on(g:%d)", charNo,
                    guildKey);
                reply.m_a = 0x7f;
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x12d1,"./log/Power",
                "CPacketTranslater::OnPacketSecedePower : %d is not guild master or sub master(g:%d)",
                charNo, guildKey);
            reply.m_a = 0x56;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
        return;
    }
    DNF_LOG_SCOPE_LINE(0x12c8,"./log/Power",
        "CPacketTranslater::OnPacketSecedePower : %d did not join power side(g:%d)", charNo,
        guildKey);
    reply.m_a = 0x82;
    user->SendToGameserver((char*)&reply, reply.packetSize);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x12ff, "./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1304, "./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break\n");
    }
}

void CPacketTranslater::OnSetPowerWarCfg(PacketHeader* pkt)
{
    Packet_Notice_Power_War_CFG* pb = (Packet_Notice_Power_War_CFG*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1317, "./log/Power", "CPacketTranslater::OnSetPowerWarCfg : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        pm->SetPowerWarEndKillPoint(pb->m_endKillPoint);
        pm->SetPowerWarRewardInfo(pb->m_basic, pb->m_first, pb->m_decrease,
                                  pb->m_ranking);
        DNF_LOG_SCOPE_LINE(0x1321,"./log/Power",
            "CPacketTranslater::OnSetPowerWarCfg EndKillPoint:%d, BasicPowerWarPoint:%d, FirstGradeBonusPowerWarPoint:%d, BonusDecreasePoint:%d, RankingBonusGrade:%d",
            (unsigned int)pb->m_endKillPoint, pb->m_basic, pb->m_first,
            pb->m_decrease, pb->m_ranking);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break", 0x132b, 0x1330);
}

void CPacketTranslater::OnLoadFromDBOnGuildBooting(PacketHeader* pkt)
{
    Packet_DB_Reply_On_Guild_Booting* pb = (Packet_DB_Reply_On_Guild_Booting*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1343,"./log/Power",
            "CPacketTranslater::OnLoadFromDBOnGuildBooting : 0 == m_pclApp");
        return;
    }
    try
    {
        CPowerManager* pm = m_pclApp->GetPowerManager();
        pm->SetPowerInfo(pb->m_12, pb->m_a, pb->m_e);
        pm->SendPowerWarInfo();
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPacketSecedePower Exception Break", 0x134f, 0x1354);
}

void CPacketTranslater::OnGMPowerWarStart(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x13f9, "./log/Power", "CPacketTranslater::OnGMPowerWarStart : 0 == m_pclApp");
        return;
    }
    try
    {
        CServerHandler* handler;
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (pm->IsPowerWarOn())
        {
            return;
        }
        pm->StartPowerWarEvent();
        Packet_Monitor_Event_Start start;
        start.m_eventType = 0x1e;
        if ((handler = m_pclApp->Get_ServerHandler()) == 0)
        {
            throw CDNFException(
                "CGuildManager::OnGMPowerWarStart() pclServerHandler == NULL\n");
        }
        handler->SendAllTcpGameServer(&start);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGMPowerWarStart Exception Break", 0x140e, 0x1413);
}

void CPacketTranslater::OnGMPowerWarEnd(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1426, "./log/Power", "CPacketTranslater::OnGMPowerWarEnd : 0 == m_pclApp");
        return;
    }
    try
    {
        CServerHandler* handler;
        CPowerManager* pm = m_pclApp->GetPowerManager();
        if (!pm->IsPowerWarOn())
        {
            return;
        }
        pm->SendPowerWarScore();
        pm->EndPowerWarEvent();
        Packet_Monitor_Event_End end;
        end.m_eventType = 0x1e;
        if ((handler = m_pclApp->Get_ServerHandler()) == 0)
        {
            throw CDNFException(
                "CGuildManager::OnGMPowerWarStart() pclServerHandler == NULL\n");
        }
        handler->SendAllTcpGameServer(&end);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGMPowerWarEnd Exception Break", 0x143d, 0x1442);
}

void CPacketTranslater::OnInnerPacketLogin(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1455, "./log/Except", "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
        return;
    }
    Packet_InnerPakcet_Login* pb = (Packet_InnerPakcet_Login*)pkt;
    try
    {
        if (m_pclApp->Get_ServerHandler()->GetTcpDBServer()->GetSock() == pb->m_connNo)
        {
            m_pclApp->Get_ServerHandler()->GetTcpDBServer()->Connected();
        }
        else
        {
            CTcpGameServer* tgs = m_pclApp->Get_ServerHandler()->CreateTcpGameServer(pb->m_connNo);
            if (tgs != 0)
            {
                char* buf = tgs->makePacketHeader(8000, 0xc);
                if (buf != 0)
                {
                    buf[10] = 1;
                    buf[11] = m_pclApp->Get_ServerGroup();
                    tgs->SendToGameServer(buf);
                    char* buf2 = tgs->makePacketHeader(0x3ea, 0xb);
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
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x149e, "./log/Except", "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
        return;
    }
    try
    {
        Packet_InnerPakcet_Logout* pb = (Packet_InnerPakcet_Logout*)pkt;
        if (m_pclApp->Get_ServerHandler()->GetTcpDBServer()->GetSock() == pb->m_connNo)
        {
            m_pclApp->Get_ServerHandler()->GetTcpDBServer()->DisConnected();
        }
        else
        {
            CTcpGameServer* tgs = m_pclApp->FindTcpGameServer(pb->m_connNo);
            m_pclApp->OnTcpGameServerDown(tgs);
            unsigned char ch = tgs->GetChannelNo();
            if (ch != 0)
            {
                m_pclApp->Get_ServerHandler()->UnregistGameServer((unsigned int)ch);
            }
            m_pclApp->Get_ServerHandler()->DeleteTcpGameServer(pb->m_connNo);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break", 0x14be, 0x14c3);
}

void CPacketTranslater::OnChangeGuildName(PacketHeader* pkt)
{
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14dc, "./log/Except", "CPacketTranslater::OnChangeGuildName : 0 == m_pclApp");
        return;
    }
    Packet_Reply_Change_Guild_Name reply;
    Packet_Change_Guild_Name* pb = (Packet_Change_Guild_Name*)pkt;
    reply.m_charNo = pb->m_charNo;
    memcpy(reply.m_name, pb->m_name, 0x16);
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(
             pb->m_charNo)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14e7, "./log/GuildModify", "CPacketTranslater::OnChangeGuildName : 0 == pclRequestUser");
        return;
    }
    reply.m_12 = user->GetIdByChannel();
    CGuild* guild = 0;
    if (pb->m_guildKey == 0 ||
        (guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x14ef, "./log/GuildModify", "CPacketTranslater::OnChangeGuildName : 0 == pclGuild");
        reply.m_result = 100;
        user->SendTcpGameserver(&reply);
        return;
    }
    if (guild->IsGuildMaster(pb->m_charNo) == 1)
    {
        guild->ChangeGuildName(pb->m_name, 1);
        guild->DBGuildSave(user->GetGameServer()->GetGroupNo(),
                           m_pclApp->Get_ServerHandler(), 0);
        guild->SendGuildNameChangeToMembers();
        reply.m_guildKey = pb->m_guildKey;
        reply.m_result = 0;
        user->SendTcpGameserver(&reply);
        char mailContent[0x100];
        memset(mailContent, 0, sizeof(mailContent));
        sprintf(mailContent, LETTER_TEXT + 0xff,
                pb->m_name);
        SendPacketGuildMail(user->GetGameServer()->GetGroupNo(),
                            user->GetUniqCharNo(), pb->m_guildKey, LETTER_TEXT,
                            mailContent, 0xffffffff);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x14f7,"./log/GuildModify",
            "CPacketTranslater::OnChangeGuildName : %d is not guild master or sub master(g:%d)",
            pb->m_charNo,
            pb->m_guildKey);
        reply.m_result = 0x56;
        user->SendTcpGameserver(&reply);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1527, "./log/Except", "CPacketTranslater::OnChangeGuildName Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x152c, "./log/Except", "CPacketTranslater::OnChangeGuildName Exception Break\n");
    }
}

void CPacketTranslater::OnChangeCharName(PacketHeader* pkt)
{
    Packet_Change_Char_Name* pb = (Packet_Change_Char_Name*)pkt;
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1544, "./log/Except", "CPacketTranslater::OnChangeGuildName : 0 == m_pclApp");
        return;
    }
    if (pb->m_field12 == 0)
    {
        DNF_LOG_SCOPE_LINE(0x154c, "./log/ServerUnify", "CPacketTranslater::OnChangeCharName : packet->m_uGuildKey");
        return;
    }
    Packet_UnChangable_GuildInfo_Save save;
    save.m_guildKey = pb->m_field12;
    save.m_charNo = pb->m_charNo;
    memcpy(save.m_name, pb->m_name, 0x1d);
    m_pclApp->Get_ServerHandler()->SendToDB(&save);
    STTodayGuildMember* today =
        m_pclApp->Get_GuildManager()->GetTodayMember(
            pb->m_field12);
    if (today != 0 && *(int*)today == (int)pb->m_charNo)
    {
        memset((char*)today + 4, 0, 0x1e);
        strncpy((char*)today + 4, pb->m_name, 0x1d);
    }
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
        pb->m_field12);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1563, "./log/ServerUnify", "CPacketTranslater::OnChangeCharName : 0 == pclGuild");
        return;
    }
    char deadBuf[0x1e];
    memset(deadBuf, 0, 0x1e);
    if (guild->ChangeGuildMemberCharName(pb->m_charNo,
                                         pb->m_name) == 1)
    {
        CUser* user;
        if ((user = (&m_pclApp->m_userManager)->FindUser(
                 pb->m_idByChannel)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1572,"./log/ServerUnify",
                "CPacketTranslater::OnChangeGuildName : 0 == pclRequestUser");
        }
        else
        {
            user->ChangeCharName(pb->m_name);
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
        DNF_LOG_SCOPE_LINE(0x157f, "./log/Except", "CPacketTranslater::OnChangeCharName Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1584, "./log/Except", "CPacketTranslater::OnChangeCharName Exception Break\n");
    }
}

void CPacketTranslater::OnChangeGuildNameFromWeb(PacketHeader* pkt)
{
    Packet_Change_Guild_Name_From_Web* pb = (Packet_Change_Guild_Name_From_Web*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x159d,"./log/Except",
                "CPacketTranslater::OnChangeGuildNameFromWeb : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x15a5,"./log/GuildModify",
                "CPacketTranslater::OnChangeGuildNameFromWeb : 0 == pclGuild");
            return;
        }
        unsigned int charNo = pb->m_charNo;
        if (guild->IsSubGuildMaster(charNo) == 1 || guild->IsGuildMaster(charNo) == 1)
        {
            guild->ChangeGuildName(pb->m_name, 0);
            guild->SendGuildInfoToMembers(false);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x15ab,"./log/GuildModify",
                "CPacketTranslater::OnChangeGuildNameFromWeb : %d is not guild master or sub master(g:%d)",
                charNo, guildKey);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnChangeGuildNameFromWeb Exception Break", 0x15b7, 0x15bc);
}

void CPacketTranslater::OnInCreaseDecreasePowerWarPoint(PacketHeader* pkt)
{
    Packet_Guild_Increase_Decrease_Power_War_Point* pb = (Packet_Guild_Increase_Decrease_Power_War_Point*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x15d3,"./log/Except",
                "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x15db,"./log/Power",
                "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : 0 == pclGuild");
            return;
        }
        int point = pb->m_point;
        if (point == 0)
        {
            DNF_LOG_SCOPE_LINE(0x15e2,"./log/Power",
                "CPacketTranslater::OnInCreaseDecreasePowerWarPoint : Invalid Power War Point Reached(characNo:%d, PowerWarPoint:%d)",
            pb->m_charNo, point);
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
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnInCreaseDecreasePowerWarPoint Exception Break", 0x15f1, 0x15f6);
}

void CPacketTranslater::OnCreateGuildAgit(PacketHeader* pkt)
{
    CGuild* guild = 0;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1608, "./log/Except", "CPacketTranslater::OnCreateGuildAgit : 0 == m_pclApp");
            return;
        }
        Packet_Guild_Create_Guild_Agit* pb = (Packet_Guild_Create_Guild_Agit*)pkt;
        if (pb->m_guildKey != 0)
        {
            guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        }
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1610, "./log/GuildAgit", "CPacketTranslater::OnCreateGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(pb->m_charNo))
        {
            if (!guild->IsExistGuildAgit())
            {
                if (pb->m_fund > guild->GetGuildFund())
                {
                    DNF_LOG_SCOPE_LINE(0x1623,"./log/GuildAgit",
                        "CPacketTranslater::OnCreateGuildAgit : %d guild fund shortage",
                        pb->m_guildKey);
                }
                else
                {
                    guild->CreateGuildAgit(m_pclApp->Get_ServerHandler(), pb->m_guildKey,
                                           pb->m_charNo, pb->m_field12, pb->m_fund);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x161c,"./log/GuildAgit",
                    "CPacketTranslater::OnCreateGuildAgit : %d guild already have guild agit",
                    pb->m_guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1616,"./log/GuildAgit",
                "CPacketTranslater::OnCreateGuildAgit : %d is not guild master(g:%d)",
                pb->m_charNo, pb->m_guildKey);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnCreateGuildAgit Exception Break", 0x1630, 0x1635)
}

void CPacketTranslater::OnDeleteGuildAgit(PacketHeader* pkt)
{
    CGuild* guild = 0;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1647, "./log/Except", "CPacketTranslater::OnDeleteGuildAgit : 0 == m_pclApp");
            return;
        }
        Packet_Guild_Delete_Guild_Agit* pb = (Packet_Guild_Delete_Guild_Agit*)pkt;
        if (pb->m_guildKey != 0)
        {
            guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        }
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x164f, "./log/GuildAgit", "CPacketTranslater::OnDeleteGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(pb->m_charNo))
        {
            if (guild->IsExistGuildAgit())
            {
                if (guild->GetGuildCargo()->IsEmpty())
                {
                    guild->DeleteGuildAgit(m_pclApp->Get_ServerHandler(), pb->m_guildKey,
                                           pb->m_charNo);
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x1663,"./log/GuildAgit",
                        "CPacketTranslater::OnDeleteGuildAgit : %d guild cargo is not empty",
                        pb->m_guildKey);
                    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo);
                    if (user == 0)
                    {
                        DNF_LOG_SCOPE_LINE(0x166b,"./log/GuildCargo",
                            "CPacketTranslater::OnDeleteGuildAgit : 0 == pclUser(%d)",
                            pb->m_charNo);
                    }
                    else
                    {
                        Packet_Channel_Delete_Guild_Agit reply;
                        reply.ma = user->GetIdByChannel();
                        reply.me = user->GetUniqCharNo();
                        reply.m12 = user->GetUniqCharNo();
                        reply.m16 = 0xd4;
                        user->SendToGameserver((char*)&reply, 0x17);
                    }
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x165b,"./log/GuildAgit",
                    "CPacketTranslater::OnDeleteGuildAgit : %d guild do not have guild agit",
                    pb->m_guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1655,"./log/GuildAgit",
                "CPacketTranslater::OnDeleteGuildAgit : %d is not guild master(g:%d)",
                pb->m_charNo, pb->m_guildKey);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDeleteGuildAgit Exception Break", 0x167d, 0x1682);
}

void CPacketTranslater::OnDBCreateGuildAgit(PacketHeader* pkt)
{
    Packet_DB_Create_Guild_Agit_Reply* pb = (Packet_DB_Create_Guild_Agit_Reply*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1696, "./log/Except", "CPacketTranslater::OnDBCreateGuildAgit : 0 == m_pclApp");
            return;
        }
        if (pb->m_field12 != 0)
        {
            DNF_LOG_SCOPE_LINE(0x16a0,"./log/GuildAgit",
                "CPacketTranslater::OnDBCreateGuildAgit / Failure! Create Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
                pb->m_guildKey, pb->m_charNo, pb->m_field12);
            return;
        }
        {
            unsigned int guildKey = pb->m_guildKey;
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
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
                cargoInfo.m_capacity = 8;
                guild->GetGuildCargo()->SetGuildCargoDBInfo(cargoInfo);
                guild->NotifyCreateGuildAgitToGuildMember(
                    pb->m_charNo);
                guild->SendGuildAgitInfoToMembers();
                guild->SendGuildInfoToMembers(false);
            }
        }
        return;
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBCreateGuildAgit Exception Break", 0x16c4, 0x16c9);
}

void CPacketTranslater::OnDBDeleteGuildAgit(PacketHeader* pkt)
{
    Packet_DB_Delete_Guild_Agit_Reply* pb = (Packet_DB_Delete_Guild_Agit_Reply*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x16dd, "./log/Except", "CPacketTranslater::OnDBDeleteGuildAgit : 0 == m_pclApp");
            return;
        }
        if (pb->m_field12 != 0)
        {
            DNF_LOG_SCOPE_LINE(0x16e6,"./log/GuildAgit",
                "CPacketTranslater::OnDBDeleteGuildAgit / Failure! Delete Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
                pb->m_guildKey, pb->m_charNo, pb->m_field12);
            return;
        }
        {
            unsigned int guildKey = pb->m_guildKey;
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
            if (guildKey == 0 || guild == 0)
            {
                DNF_LOG_SCOPE_LINE(0x16ef, "./log/GuildAgit", "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
            }
            else
            {
                guild->SetGuildAgitFlag(false);
                guild->NotifyDeleteGuildAgitToGuildMember(
                    pb->m_charNo);
                guild->SendGuildInfoToMembers(false);
            }
        }
        return;
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBDeleteGuildAgit Exception Break", 0x16fe, 0x1703);
}

void CPacketTranslater::OnPowerWarProcessInfo(PacketHeader* pkt)
{
    int uVar1 = 0;
    int uVar2 = 0;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1717, "./log/Except", "CPacketTranslater::OnPowerWarProcessInfo : 0 == m_pclApp");
        return;
    }
    try
    {
        char* pb = (char*)pkt;
        CPowerManager* pm = m_pclApp->GetPowerManager();
        pm->SendPowerWarProcessInfo(((Packet_Guild_Power_War_Process_Info*)pb)->m_a);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnPowerWarProcessInfo Exception Break", 0x1722, 0x1727);
}

void CPacketTranslater::OnDBLoadGuildAgit(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x173a);
            log("./log/Except", "CPacketTranslater::OnDBLoadGuildAgit : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = ((Packet_Guild_Load_Guild_Agit*)pkt)->m_guildId;
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guildKey == 0 || guild == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x1743);
            log("./log/GuildAgit", "CPacketTranslater::OnDBCreateGuildAgit : 0 == pclGuild");
        }
        else
        {
            guild->SetGuildAgitInfo(((Packet_Guild_Load_Guild_Agit*)pkt)->m_info);
            guild->SendGuildAgitInfoToMembers();
        }
    }
    catch (std::exception& e)
    {
        CMyFileLog log(__FUNCTION__, 0x174f);
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
    CGuild* guild = 0;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1767, "./log/Except", "CPacketTranslater::OnUpgradeGuildAgit : 0 == m_pclApp");
            return;
        }
        Packet_Guild_Upgrade_Guild_Agit* pb = (Packet_Guild_Upgrade_Guild_Agit*)pkt;
        if (pb->m_guildKey != 0)
        {
            guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        }
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(6000, "./log/GuildAgit", "CPacketTranslater::OnCreateGuildAgit : 0 == pclGuild");
        }
        else if (guild->IsGuildMaster(pb->m_charNo))
        {
            if (guild->IsExistGuildAgit())
            {
                if (pb->m_fund > guild->GetGuildFund())
                {
                    DNF_LOG_SCOPE_LINE(0x1783,"./log/GuildAgit",
                        "CPacketTranslater::OnCreateGuildAgit : %d guild fund shortage",
                        pb->m_guildKey);
                }
                else
                {
                    guild->UpgradeGuildAgit(m_pclApp->Get_ServerHandler(), pb->m_guildKey,
                                            pb->m_charNo, pb->m_field12, pb->m_fund);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x177c,"./log/GuildAgit",
                    "CPacketTranslater::OnDeleteGuildAgit : %d guild already have guild agit",
                    pb->m_guildKey);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1776,"./log/GuildAgit",
                "CPacketTranslater::OnCreateGuildAgit : %d is not guild master(g:%d)",
                pb->m_charNo, pb->m_guildKey);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnUpgradeGuildAgit Exception Break", 0x1790, 0x1795);
}

void CPacketTranslater::OnDBUpgradeGuildAgit(PacketHeader* pkt)
{
    Packet_DB_Upgrade_Guild_Agit_Reply* pb = (Packet_DB_Upgrade_Guild_Agit_Reply*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x17a8, "./log/Except", "CPacketTranslater::OnDBUpgradeGuildAgit : 0 == m_pclApp");
            return;
        }
        if (pb->m_field12 != 0)
        {
            DNF_LOG_SCOPE_LINE(0x17b2,"./log/GuildAgit",
                "CPacketTranslater::OnDBUpgradeGuildAgit / Failure! Upgrade Guild Agit(Guild No : %d, Master No : %d, Reason : %d)",
                pb->m_guildKey, pb->m_charNo, pb->m_field12);
            return;
        }
        {
            unsigned int guildKey = pb->m_guildKey;
            CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
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
        }
        return;
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBUpgradeGuildAgit Exception Break", 0x17c5, 0x17ca);
}

void CPacketTranslater::OnGuildExpLimit(PacketHeader* pkt)
{
    (void)pkt;
}

void CPacketTranslater::OnWriteGuildMemberMemo(PacketHeader* pkt)
{
    Packet_Guild_Write_Guild_Member_Memo* pb = (Packet_Guild_Write_Guild_Member_Memo*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x17eb,"./log/GuildMember",
                "CPacketTranslater::OnWriteGuildMemberMemo : 0 == m_pclApp");
            return;
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(
            pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x17f5, "./log/GuildMember", "CPacketTranslater::OnWriteGuildMemberMemo : 0 == pclUser");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x17fc, "./log/GuildMember", "CPacketTranslater::OnWriteGuildMemberMemo : 0 == pclGuild");
            return;
        }
        Packet_DB_Write_Guild_Member_Memo dbPkt;
        dbPkt.m_charNo = pb->m_charNo;
        dbPkt.m_guildKey = guildKey;
        size_t len = strlen(pb->m_memo);
        memcpy(dbPkt.m_memo, pb->m_memo, len < 0x15 ? len : 0x14);
        m_pclApp->m_serverHandler->SendToDB(&dbPkt);
        guild->WriteGuildMemberMemo(user, pb->m_memo);
        guild->NotifyMemoToGuildMember(user, pb->m_memo);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnWriteGuildMemberMemo Exception Break", 0x1818, 0x181d);
}

void CPacketTranslater::OnLoadGuildCargo(PacketHeader* pkt)
{
    CGuild* guild;
    Packet_Guild_Load_Guild_Cargo* pb = (Packet_Guild_Load_Guild_Cargo*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1831, "./log/GuildCargo", "CPacketTranslater::OnLoadGuildCargo : 0 == m_pclApp");
        }
        else
        {
            if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1838, "./log/GuildCargo", "CPacketTranslater::OnLoadGuildCargo : 0 == pclGuild");
            }
            else
            {
                guild->GetGuildCargo()->SetGuildCargoDBInfo(pb->m_cargo);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1849, "./log/Except", "CPacketTranslater::OnLoadGuildCargo Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x184e, "./log/Except", "CPacketTranslater::OnLoadGuildCargo Exception Break\n");
    }
}

void CPacketTranslater::OnLoadGuildCargoHistory(PacketHeader* pkt)
{
    try
    {
        CGuild* guild;
        Packet_Guild_Load_Guild_Cargo_History* pb = (Packet_Guild_Load_Guild_Cargo_History*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1861,"./log/GuildCargo",
                "CPacketTranslater::OnLoadGuildCargoHistory : 0 == m_pclApp");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1868,"./log/GuildCargo",
                "CPacketTranslater::OnLoadGuildCargoHistory : 0 == pclGuild");
            return;
        }
        guild->GetGuildCargo()->SetGuildCargoHistory(pb->m_count, &pb->m_logs);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1875, "./log/Except",
            "CPacketTranslater::OnLoadGuildCargoHistory Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x187a, "./log/Except",
            "CPacketTranslater::OnLoadGuildCargoHistory Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargo(PacketHeader* pkt)
{
    try
    {
        Packet_Guild_Cargo_Requst* pb = (Packet_Guild_Cargo_Requst*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x188c, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == m_pclApp");
            return;
        }
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* user;
        if ((user = um->FindUser_CharNo(pb->m_charNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1896, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == pclUser");
            return;
        }
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x189e, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo : 0 == pclGuild");
            return;
        }
        Packet_Guild_Cargo_Response reply;
        reply.m_a = user->GetIdByChannel();
        reply.m_b = pb->m_charNo;
        if (guild->GetGuildCargo()->IsLoadComplete() == 1)
        {
            unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
            if (grade == 3 || grade == 1 || grade == 2)
            {
                if ((&m_pclApp->m_guildManager)->IsCargoLock())
                {
                    reply.m_flag = 0xcc;
                    DNF_LOG_SCOPE_LINE(0x18c2, "./log/GuildCargo", "CPacketTranslater::OnGuildCargo GUILD CARGO LOCKED!");
                }
                else
                {
                    reply.m_flag = 0xc1;
                    memcpy(&reply.m_cargo, guild->GetGuildCargo()->GetGuildCargoDBInfo(), 0x18dc);
                }
                user->SendTcpGameserver(&reply);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x18b7,"./log/GuildCargo",
                    "CPacketTranslater::OnGuildCargo : Access Deny(%d,%d,%d)", pb->m_charNo, pb->m_guildKey,
                    (unsigned int)grade);
                reply.m_flag = 0x24;
                user->SendTcpGameserver(&reply);
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x18aa,"./log/GuildCargo", "CPacketTranslater::OnGuildCargo : Guild(%d,%d) Not Loaded",
                pb->m_charNo, pb->m_guildKey);
            reply.m_flag = 0xc3;
            user->SendTcpGameserver(&reply);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x18d5, "./log/Except", "CPacketTranslater::OnGuildCargo Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x18da, "./log/Except", "CPacketTranslater::OnGuildCargo Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoHistory(PacketHeader* pkt)
{
    Packet_Guild_Cargo_Requst* pb = (Packet_Guild_Cargo_Requst*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x18ec, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == m_pclApp");
            return;
        }
        unsigned int charNo = pb->m_charNo;
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x18f6, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == pclUser");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x18fe, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoHistory : 0 == pclGuild");
            return;
        }
        if ((&m_pclApp->m_guildManager)->IsCargoLock() == 0)
        {
            unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
            if (grade == 3 || grade == 1 || grade == 2)
            {
                Packet_Guild_Cargo_History_Response reply;
                reply.ma = user->GetIdByChannel();
                reply.me = charNo;
                guild->GetGuildCargo()->GetHistory(reply.m_log);
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
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGuildCargoHistory Exception Break", 0x191e, 0x1923);
}

void CPacketTranslater::OnGuildCargoCheckPushItem(PacketHeader* pkt)
{
    Packet_Guild_Check_Guild_Cargo_Push_Item* pb = (Packet_Guild_Check_Guild_Cargo_Push_Item*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1935,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == m_pclApp");
            return;
        }
        Packet_Channel_Check_Guild_Cargo_Push_Item reply;
        unsigned int charNo = pb->m_charNo;
        reply.me = charNo;
        reply.m13 = pb->m_field12;
        reply.m17 = pb->m_field16;
        reply.m1b = pb->m_field1a;
        reply.m23 = pb->m_field22;
        reply.m1f = pb->m_field1e;
        reply.m25 = pb->m_field24;
        reply.m24 = pb->m_field23;
        unsigned int guildKey = pb->m_guildKey;
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x194a,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == pclUser");
            return;
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1952,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : 0 == pclGuild");
            return;
        }
        reply.ma = user->GetIdByChannel();
        if ((&m_pclApp->m_guildManager)->IsCargoLock() != 0)
        {
            DNF_LOG_SCOPE_LINE(0x195c,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem GUILD CARGO LOCKED!(%d,%d)", charNo,
                guildKey);
            reply.m12 = 0xcc;
            user->SendTcpGameserver(&reply);
            return;
        }
        if (guild->IsExistGuildAgit() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x1967,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoPushItem : (%d,%d) No GuildAgit", charNo, guildKey);
            reply.m12 = 0xb5;
            user->SendTcpGameserver(&reply);
            return;
        }
        if (guild->GetGuildCargo()->IsLoadComplete() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x1971,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoPushItem : Guild(%d,%d) Not Loaded", charNo,
                guildKey);
            reply.m12 = 0xc3;
            user->SendTcpGameserver(&reply);
            return;
        }
        unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
        if (grade == 3 || grade == 1 || grade == 2)
        {
            int result = guild->GetGuildCargo()->CheckInsertItem(
                pb->m_field16,
                pb->m_field1a,
                pb->m_field12,
                pb->m_field22,
                pb->m_field1e);
            reply.m12 = (unsigned char)result;
            user->SendTcpGameserver(&reply);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x197f,"./log/GuildCargo",
                "CPacketTranslater::OnGuildCargoCheckPushItem : Access Deny(%d,%d,%d)", charNo,
                guildKey, (unsigned int)grade);
            reply.m12 = 0x24;
            user->SendTcpGameserver(&reply);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1994, "./log/Except", "CPacketTranslater::OnGuildCargoCheckPushItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1999, "./log/Except", "CPacketTranslater::OnGuildCargoCheckPushItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoPushItem(PacketHeader* pkt)
{
    try
    {
        Packet_Guild_Guild_Cargo_Push_Item* pb = (Packet_Guild_Guild_Cargo_Push_Item*)pkt;
        Packet_Channel_Guild_Cargo_Push_Item resp;
        resp.m_b = pb->m_charNo;
        resp.m_d = pb->m_slot;
        resp.m_item = *(DnfItemInfo*)pb->m_item;
        resp.m_e = pb->m_fsn;
        resp.m_flag = pb->m_fst;
        char* itemDesc = (char*)CGuildCargo::PrintDnfItemInfo(*(DnfItemInfo*)pb->m_item);
        CMyFileLog log0(__FUNCTION__, 0x19b3);
        log0("./log/GuildCargo", "PUSH ITEM(g:%d,cn:%d,sn:%d,sl:%d,fsn:%d,fst:%d,it:%d,%s)",
             pb->m_guildKey, pb->m_charNo, pb->m_slot,
             pb->m_count, (unsigned int)pb->m_fsn, (unsigned int)pb->m_fst,
             (unsigned int)pb->m_itemType, itemDesc);
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19c3, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == m_pclApp");
            return;
        }
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* user = um->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19cd, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == pclUser");
            return;
        }
        resp.m_a = user->GetIdByChannel();
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x19d7, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : 0 == pclGuild");
            resp.m_c = 100;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        if ((&m_pclApp->m_guildManager)->IsCargoLock())
        {
            DNF_LOG_SCOPE_LINE(0x19e2,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem GUILD CARGO LOCKED!(%d,%d)",
                pb->m_charNo, pb->m_guildKey);
            resp.m_c = 0xcc;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        if (guild->IsExistGuildAgit() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x19ed,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : (%d,%d) No GuildAgit",
                pb->m_charNo, pb->m_guildKey);
            resp.m_c = 0xb5;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        if (guild->GetGuildCargo()->IsLoadComplete() != 1)
        {
            DNF_LOG_SCOPE_LINE(0x19f7,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : Guild(%d,%d) Not Loaded",
                pb->m_charNo, pb->m_guildKey);
            resp.m_c = 0xc3;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
        if (grade != 3 && grade != 1 && grade != 2)
        {
            DNF_LOG_SCOPE_LINE(0x1a05,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPushItem : Access Deny(%d,%d,%d)",
                pb->m_charNo, pb->m_guildKey, (unsigned int)grade);
            resp.m_c = 0x24;
            user->SendTcpGameserver((PacketHeader*)&resp);
            return;
        }
        int slot = pb->m_slot;
        int result = guild->GetGuildCargo()->InsertItem(*(DnfItemInfo*)pb->m_item, slot, pb->m_count, pb->m_itemType, (int)pb->m_charNo);
        resp.m_c = (unsigned char)result;
        resp.m_d = slot;
        if (result == 0xc1)
        {
            guild->GetGuildCargo()->InsertHistory((ENUM_GUILD_CARGO_BEHAVIOR)1, (int)pb->m_charNo, user->GetCharName(),
                                 ((DnfItemInfo*)pb->m_item)->m_itemId, ((DnfItemInfo*)pb->m_item)->m_addInfo, &((DnfItemInfo*)pb->m_item)->m_opt);
            CServerHandler* handler = m_pclApp->m_serverHandler;
            guild->GetGuildCargo()->SendHistoryToDBMW(handler, (ENUM_GUILD_CARGO_BEHAVIOR)1, (int)pb->m_charNo,
                                     user->GetCharName(), slot, 0, *(DnfItemInfo*)pb->m_item);
            guild->GetGuildCargo()->SendGuildCargoToDBMW(handler, (int)pb->m_charNo);
            guild->GetGuildCargo()->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)1);
        }
        user->SendTcpGameserver((PacketHeader*)&resp);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a48, "./log/Except", "CPacketTranslater::OnGuildCargoPushItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a4d, "./log/Except", "CPacketTranslater::OnGuidCargoPushItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoPopItem(PacketHeader* pkt)
{
    Packet_Guild_Guild_Cargo_Pop_Item* pb = (Packet_Guild_Guild_Cargo_Pop_Item*)pkt;
    try
    {
    CMyFileLog log0(__FUNCTION__, 0x1a65);
    log0("./log/GuildCargo", "POP ITEM(g:%d,cn:%d,sn:%d,id:%d,cnt:%d,it:%d,uit:%d)",
         pb->m_guildKey, pb->m_charNo, pb->m_slot, pb->m_id, pb->m_count, (unsigned int)pb->m_itemType, pb->m_uit);
    Packet_Channel_Guild_Cargo_Pop_Item resp;
    resp.m_b = pb->m_charNo;
    resp.m_c = pb->m_uit;
    resp.m_d = pb->m_slot;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a74, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == m_pclApp");
        return;
    }
    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a7e, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == pclUser");
        return;
    }
    resp.m_a = user->GetIdByChannel();
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1a88, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : 0 == pclGuild");
        resp.m_flag = 100;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    if ((&m_pclApp->m_guildManager)->IsCargoLock())
    {
        DNF_LOG_SCOPE_LINE(0x1a92,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem GUILD CARGO LOCKED!(%d,%d)",
            pb->m_charNo, pb->m_guildKey);
        resp.m_flag = 0xcc;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    if (guild->IsExistGuildAgit() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1a9d,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : (%d,%d) No GuildAgit",
            pb->m_charNo, pb->m_guildKey);
        resp.m_flag = 0xb5;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    if (guild->GetGuildCargo()->IsLoadComplete() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1aa7,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : Guild(%d,%d) Not Loaded",
            pb->m_charNo, pb->m_guildKey);
        resp.m_flag = 0xc3;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
    if (grade != 3 && grade != 1 && grade != 2)
    {
        DNF_LOG_SCOPE_LINE(0x1ab5,"./log/GuildCargo", "CPacketTranslater::OnGuildCargoPopItem : Access Deny(%d,%d,%d)",
            pb->m_charNo, pb->m_guildKey, (unsigned int)grade);
        resp.m_flag = 0x24;
        user->SendTcpGameserver((PacketHeader*)&resp);
        return;
    }
    DnfItemInfo poppedItem;
    int result = guild->GetGuildCargo()->DeleteItem(poppedItem, pb->m_slot, pb->m_count, pb->m_itemType, pb->m_id, (int)pb->m_charNo);
    resp.m_flag = (unsigned char)result;
    if (result == 0xc1)
    {
        *(unsigned int*)((char*)&resp.m_item + 0x00) = *(unsigned int*)((char*)&poppedItem + 0x00);
        *(unsigned int*)((char*)&resp.m_item + 0x04) = *(unsigned int*)((char*)&poppedItem + 0x04);
        *(unsigned int*)((char*)&resp.m_item + 0x08) = *(unsigned int*)((char*)&poppedItem + 0x08);
        *(unsigned int*)((char*)&resp.m_item + 0x0c) = *(unsigned int*)((char*)&poppedItem + 0x0c);
        *(unsigned int*)((char*)&resp.m_item + 0x10) = *(unsigned int*)((char*)&poppedItem + 0x10);
        *(unsigned int*)((char*)&resp.m_item + 0x14) = *(unsigned int*)((char*)&poppedItem + 0x14);
        *(unsigned int*)((char*)&resp.m_item + 0x18) = *(unsigned int*)((char*)&poppedItem + 0x18);
        *(unsigned int*)((char*)&resp.m_item + 0x1c) = *(unsigned int*)((char*)&poppedItem + 0x1c);
        *(unsigned int*)((char*)&resp.m_item + 0x20) = *(unsigned int*)((char*)&poppedItem + 0x20);
        *(unsigned int*)((char*)&resp.m_item + 0x24) = *(unsigned int*)((char*)&poppedItem + 0x24);
        *(unsigned int*)((char*)&resp.m_item + 0x28) = *(unsigned int*)((char*)&poppedItem + 0x28);
        *(unsigned int*)((char*)&resp.m_item + 0x2c) = *(unsigned int*)((char*)&poppedItem + 0x2c);
        *(unsigned char*)((char*)&resp.m_item + 0x34) = *(unsigned char*)((char*)&poppedItem + 0x34);
        guild->GetGuildCargo()->InsertHistory((ENUM_GUILD_CARGO_BEHAVIOR)2, (int)pb->m_charNo, user->GetCharName(),
                             pb->m_id, pb->m_count, (RandomOption*)((char*)&poppedItem + 0x1d));
        CServerHandler* handler = m_pclApp->m_serverHandler;
        guild->GetGuildCargo()->SendHistoryToDBMW(handler, (ENUM_GUILD_CARGO_BEHAVIOR)2, (int)pb->m_charNo,
                                 user->GetCharName(), pb->m_slot, 0, poppedItem);
        guild->GetGuildCargo()->SendGuildCargoToDBMW(handler, (int)pb->m_charNo);
        guild->GetGuildCargo()->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)2);
    }
    user->SendTcpGameserver((PacketHeader*)&resp);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1af8, "./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1afd, "./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoMoveItem(PacketHeader* pkt)
{
    Packet_Guild_Guild_Cargo_Move_Item* pb = (Packet_Guild_Guild_Cargo_Move_Item*)pkt;
    try
    {
    Packet_Channel_Guild_Cargo_Move_Item reply;
    unsigned int charNo = pb->m_charNo;
    unsigned int fromSlot = pb->m_fromSlot;
    unsigned int toSlot = pb->m_toSlot;
    {
        DNF_LOG_SCOPE_LINE(0x1b15,"./log/GuildCargo", "MOVE ITEM(g:%d,cn:%d,fsn:%d,dsn:%d",
            pb->m_guildKey, charNo, fromSlot, toSlot);
    }
    reply.me = charNo;
    reply.m13 = fromSlot;
    reply.m17 = toSlot;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b21,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : 0 == m_pclApp");
        return;
    }
    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b2b, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoMoveItem : 0 == pclUser");
        return;
    }
    reply.ma = user->GetIdByChannel();
    CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(
        pb->m_guildKey);
    if (guild == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b35, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoMoveItem : 0 == pclGuild");
        reply.m12 = 100;
        user->SendTcpGameserver(&reply);
        return;
    }
    if ((&m_pclApp->m_guildManager)->IsCargoLock() != 0)
    {
        DNF_LOG_SCOPE_LINE(0x1b40,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem GUILD CARGO LOCKED!(%d,%d)",
            pb->m_charNo,
            pb->m_guildKey);
        reply.m12 = 0xcc;
        user->SendTcpGameserver(&reply);
        return;
    }
    if (guild->IsExistGuildAgit() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1b4b,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : (%d,%d) No GuildAgit",
            pb->m_charNo,
            pb->m_guildKey);
        reply.m12 = 0xb5;
        user->SendTcpGameserver(&reply);
        return;
    }
    CGuildCargo* cargo = guild->GetGuildCargo();
    if (cargo->IsLoadComplete() != 1)
    {
        DNF_LOG_SCOPE_LINE(0x1b55,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : Guild(%d,%d) Not Loaded",
            pb->m_charNo,
            pb->m_guildKey);
        reply.m12 = 0xc3;
        user->SendTcpGameserver(&reply);
        return;
    }
    unsigned char grade = user->GetGuildMemDBInfo()->m_grade;
    if (grade == 3 || grade == 1 || grade == 2)
    {
        DnfItemInfo fromItem;
        DnfItemInfo toItem;
        int result = cargo->MoveItem(fromItem, toItem,
                                     pb->m_fromSlot,
                                     pb->m_count,
                                     pb->m_toSlot,
                                     pb->m_type,
                                     pb->m_charNo);
        reply.m12 = (unsigned char)result;
        if (result == 0xc1)
        {
            cargo->SendGuildCargoToDBMW(m_pclApp->m_serverHandler,
                                        pb->m_charNo);
            cargo->PrintCargo((ENUM_GUILD_CARGO_BEHAVIOR)3);
        }
        user->SendTcpGameserver(&reply);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x1b63,"./log/GuildCargo",
            "CPacketTranslater::OnGuildCargoMoveItem : Access Deny(%d,%d,%d)",
            pb->m_charNo,
            pb->m_guildKey, (unsigned int)grade);
        reply.m12 = 0x24;
        user->SendTcpGameserver(&reply);
    }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1bb6, "./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1bbb, "./log/Except", "CPacketTranslater::OnGuildCargoPopItem Exception Break\n");
    }
}

void CPacketTranslater::OnGuildCargoUpgrade(PacketHeader* pkt)
{
    Packet_Guild_Guild_Cargo_Upgrade* pb = (Packet_Guild_Guild_Cargo_Upgrade*)pkt;
    try
    {
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1bd1, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == m_pclApp");
        return;
    }
    unsigned int charNo = pb->m_charNo;
    CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
    if (user == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1bdb, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == pclUser");
        return;
    }
    unsigned int guildKey = pb->m_guildKey;
    CGuild* guild;
    if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1be2, "./log/GuildCargo", "CPacketTranslater::OnGuildCargoUpgrade : 0 == pclGuild");
        return;
    }
    if (guild->IsGuildMaster(charNo) == 1)
    {
        if (guild->IsExistGuildAgit() == 1)
        {
            int curCapacity = guild->GetGuildCargo()->GetCapacity();
            if (pb->m_field12 == curCapacity)
            {
                unsigned int reqFund = pb->m_field1a;
                unsigned int guildFund = guild->GetGuildFund();
                if (guildFund < reqFund)
                {
                    DNF_LOG_SCOPE_LINE(0x1c00,"./log/GuildCargo",
                        "CPacketTranslater::OnGuildCargoUpgrade guild fund shortage(c:%d,n:%d, gcapa:%d) Error! Guild(%d), User(%d), ReqFund(%d), CurrFund(%d)",
                        pb->m_field12,
                        pb->m_field16, curCapacity,
                        guildKey, charNo, reqFund, guildFund);
                }
                else
                {
                    guild->SubGuildFund(reqFund);
                    guild->SendGuildInfoToMembers(true);
                    guild->GetGuildCargo()->SetCapacity(
                        pb->m_field16);
                    CTcpDBServer* tcpDb = m_pclApp->Get_ServerHandler()->GetTcpDBServer();
                    char* buf = tcpDb->makePacketHeader(0x714, 0x16);
                    *(unsigned int*)(buf + 0xa) = guildKey;
                    *(unsigned int*)(buf + 0xe) = charNo;
                    *(unsigned int*)(buf + 0x12) =
                        pb->m_field16;
                    tcpDb->SendToServer(buf);
                    Packet_Channel_Guild_Cargo_Upgrade reply;
                    reply.m_channel = user->GetIdByChannel();
                    reply.m_charNo = charNo;
                    reply.m_result = 0xc1;
                    user->SendToGameserver((char*)&reply, 0x13);
                    guild->GetGuildCargo()->SendGuildCargo(user);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x1bf7,"./log/GuildCargo",
                    "CPacketTranslater::OnGuildCargoUpgrade Capacity(c:%d,n:%d, gcapa:%d) Error! Guild(%d), User(%d)",
                    pb->m_field12,
                    pb->m_field16, curCapacity,
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
        DNF_LOG_SCOPE_LINE(0x1c34, "./log/Except", "CPacketTranslater::OnGuildApplyOriginalPowerSide Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1c39, "./log/Except", "CPacketTranslater::OnGuildApplyOriginalPowerSide Exception Break\n");
    }
}

void CPacketTranslater::OnGuildRequestGuildBoardOpen(PacketHeader* pkt)
{
    Packet_Guild_Request_Guild_Board_Open* pb = (Packet_Guild_Request_Guild_Board_Open*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1c4a,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == m_pclApp");
            return;
        }
        CUserManager* um = &m_pclApp->m_userManager;
        CUser* user = um->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1c54,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == pclUser");
            return;
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey);
        if (guild == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1c5b,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardOpen : 0 == pclGuild");
            return;
        }
        if (guild->GetGuildBoard()->isWebGuildBoardAction() == 0 &&
            pb->m_boardType == 0 &&
            guild->GetGuildBoard()->getGuildBoardDBLoadState() != 0)
        {
            guild->GetGuildBoard()->sendGuildBoardData(
                pb->m_guildKey,
                pb->m_charNo,
                0x232a, user);
            return;
        }
        if (guild->GetGuildBoard()->getGuildBoardDBLoadState() == 0)
        {
            Packet_DB_Load_Request_Guild_Board_Open dbPkt;
            dbPkt.m_guildKey = pb->m_guildKey;
            dbPkt.m_charNo = pb->m_charNo;
            m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
            guild->GetGuildBoard()->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)1);
            return;
        }
        if (guild->GetGuildBoard()->isWebGuildBoardAction() == 0 &&
            pb->m_boardType == 0)
        {
            return;
        }
        guild->GetGuildBoard()->setWebGuildBoardAction(false);
        if (guild->GetGuildBoard()->getGuildBoardDBLoadState() == 2 &&
            guild->GetGuildBoard()->isGuildBoardDBAccess() != 0)
        {
            Packet_DB_Load_Request_Guild_Board_Open dbPkt;
            dbPkt.m_guildKey = pb->m_guildKey;
            dbPkt.m_charNo = pb->m_charNo;
            m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
            guild->GetGuildBoard()->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)1);
        }
        else
        {
            guild->GetGuildBoard()->sendGuildBoardData(
                pb->m_guildKey,
                pb->m_charNo,
                0x232a, user);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGuildRequestGuildBoardOpen Exception Break", 0x1c89, 0x1c8e);
}

void CPacketTranslater::OnDBLoadReplyGuildBoardOpen(PacketHeader* pkt)
{
    try
    {
        Packet_DB_Load_Reply_Guild_Board_Open* reply =
            (Packet_DB_Load_Reply_Guild_Board_Open*)pkt;
        if (m_pclApp == 0)
        {
            CMyFileLog log1(__FUNCTION__, 0x1c9d);
            log1("./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == m_pclApp");
            return;
        }
        CUserManager* userMgr = &m_pclApp->m_userManager;
        CUser* user = userMgr->FindUser_CharNo(reply->m_charNo);
        if (user == 0)
        {
            CMyFileLog log2(__FUNCTION__, 0x1ca7);
            log2("./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == pclUser");
            return;
        }
        CGuild* guild = (&m_pclApp->m_guildManager)->FindGuild(reply->m_guildId);
        if (guild == 0)
        {
            CMyFileLog log3(__FUNCTION__, 0x1cae);
            log3("./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardOpen : 0 == pclGuild");
            return;
        }
        guild->GetGuildBoard()->setGuildBoardData(
            reply->m_guildId, reply->m_charNo, guild,
            (int)(char)reply->m_boardCount,
            &reply->m_boards);
        if (reply->m_count != 0)
        {
            guild->GetGuildBoard()->sendGuildBoardData(reply->m_guildId, reply->m_charNo, 0x232a, user);
            guild->GetGuildBoard()->setGuildBoardDBLoadState((ENUM_DB_LOAD_STATE)2);
            guild->GetGuildBoard()->setGuildBoardDBAccess();
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBLoadReplyGuildBoardOpen Exception Break", 0x1cbf, 0x1cc4);
}

void CPacketTranslater::OnGuildRequestGuildBoardWrite(PacketHeader* pkt)
{
    try
    {
        Packet_Guild_Request_Guild_Board_Write* pb =
            (Packet_Guild_Request_Guild_Board_Write*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1cd3,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == m_pclApp");
            return;
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1cdd,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == pclUser");
            return;
        }
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(
                 pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1ce4,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardWrite : 0 == pclGuild");
            return;
        }
        Packet_DB_Load_Request_Guild_Board_Write dbPkt;
        dbPkt.m_b = pb->m_guildKey;
        dbPkt.m_c = pb->m_boardNo;
        dbPkt.m_d = pb->m_charNo;
        *(unsigned char*)((char*)&dbPkt + 0x9b) = pb->m_field16;
        *(unsigned char*)((char*)&dbPkt + 0x9c) = pb->m_field17;
        memcpy((char*)&dbPkt + 0x9e, user->GetCharName(), 0x1e);
        memcpy(&dbPkt.m_info, pb->m_info, 0x78);
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGuildRequestGuildBoardWrite Exception Break", 0x1cf8, 0x1cfd);
}

void CPacketTranslater::OnDBLoadReplyGuildBoardWrite(PacketHeader* pkt)
{
    try
    {
        CUser* user;
        CGuild* guild;
        Packet_DB_Load_Reply_Guild_Board_Write* pb = (Packet_DB_Load_Reply_Guild_Board_Write*)pkt;
        CUserManager* um;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d0c,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == m_pclApp");
            return;
        }
        um = &m_pclApp->m_userManager;
        user = um->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d16,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == pclUser");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d1d,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : 0 == pclGuild");
            return;
        }
        if (pb->m_result == 0)
        {
            guild->GetGuildBoard()->setGuildBoardData(
                pb->m_guildId, pb->m_charNo, guild, 1,
                &pb->m_board);
            if (pb->m_board.m_writerCharNo != 0)
            {
                guild->GetGuildBoard()->sendGuildBoardData(pb->m_guildId, pb->m_charNo, 0x232e, user);
            }
        }
        else
        {
            Packet_Guild_Reply_Guild_Board reply;
            reply.m_c = 0x232e;
            reply.m_a = pb->m_result;
            reply.m_f = user->GetIdByChannel();
            reply.m_13 = user->GetUniqCharNo();
            user->SendTcpGameserver(&reply);
            DNF_LOG_SCOPE_LINE(0x1d37,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardWrite : Write Fail!!");
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBLoadReplyGuildBoardWrite Exception Break", 0x1d3e, 0x1d43);
}

void CPacketTranslater::OnGuildRequestGuildBoardDelete(PacketHeader* pkt)
{
    try
    {
        CUser* user;
        CGuild* guild;
        Packet_Guild_Request_Guild_Board_Delete* pb = (Packet_Guild_Request_Guild_Board_Delete*)pkt;
        CUserManager* um;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d52,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == m_pclApp");
            return;
        }
        um = &m_pclApp->m_userManager;
        user = um->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d5c,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == pclUser");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(
                 pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d63,"./log/GuildBoard",
                "CPacketTranslater::OnGuildRequestGuildBoardDelete : 0 == pclGuild");
            return;
        }
        Packet_DB_Load_Request_Guild_Board_Delete dbPkt;
        dbPkt.m_b = pb->m_boardNo;
        dbPkt.m_c = pb->m_guildKey;
        dbPkt.m_d = pb->m_charNo;
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGuildRequestGuildBoardDelete Exception Break", 0x1d71, 0x1d76);
}

void CPacketTranslater::OnDBLoadReplyGuildBoardDelete(PacketHeader* pkt)
{
    try
    {
        CUser* user;
        CGuild* guild;
        Packet_DB_Load_Reply_Guild_Board_Delete* pb = (Packet_DB_Load_Reply_Guild_Board_Delete*)pkt;
        CUserManager* um;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d84,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == m_pclApp");
            return;
        }
        um = &m_pclApp->m_userManager;
        user = um->FindUser_CharNo(pb->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d8e,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == pclUser");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1d95,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : 0 == pclGuild");
            return;
        }
        if (pb->m_result == 0)
        {
            guild->GetGuildBoard()->deleteGuildBoardData(
                pb->m_boardNo, pb->m_guildId,
                pb->m_charNo);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1d9e,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyGuildBoardDelete : Delete Fail!!");
        }
        Packet_Guild_Reply_Guild_Board_Delete reply;
        reply.m_result = pb->m_result;
        reply.m_channel = user->GetIdByChannel();
        reply.m_charNo = user->GetUniqCharNo();
        user->SendTcpGameserver(&reply);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBLoadReplyGuildBoardDelete Exception Break", 0x1daa, 0x1daf);
}

void CPacketTranslater::OnWebGuildBoardWrite(PacketHeader* pkt)
{
    try
    {
        Packet_DB_Load_Request_Web_Guild_Board_Write* pb =
            (Packet_DB_Load_Request_Web_Guild_Board_Write*)pkt;
        unsigned int guildKey = pb->m_guildKey;
        unsigned int charNo = pb->m_charNo;
        unsigned int no = pb->m_no;
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
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1dc9, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardWrite : 0 == pclGuild");
            return;
        }
        Packet_DB_Load_Request_Web_Guild_Board_Write dbPkt;
        dbPkt.m_guildKey = guildKey;
        dbPkt.m_charNo = charNo;
        dbPkt.m_no = no;
        m_pclApp->Get_ServerHandler()->SendToDB(&dbPkt);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnWebGuildBoardWrite Exception Break", 0x1dd7, 0x1ddc);
}

void CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite(PacketHeader* pkt)
{
    try
    {
        CGuild* guild;
        Packet_DB_Load_Reply_Web_Guild_Board_Write* pb = (Packet_DB_Load_Reply_Web_Guild_Board_Write*)pkt;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1ded,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite : 0 == m_pclApp");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1df4,"./log/GuildBoard",
                "CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite : 0 == pclGuild");
            return;
        }
        if (pb->m_result == 0)
        {
            guild->GetGuildBoard()->setGuildBoardData(
                                                      pb->m_guildId,
                                                      pb->m_charNo,
                                                      guild, 1,
                                                      &pb->m_board);
            guild->GetGuildBoard()->setWebGuildBoardAction(true);
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite Exception Break", 0x1e01, 0x1e06);
}

void CPacketTranslater::OnWebGuildBoardDelete(PacketHeader* pkt)
{
    try
    {
        CGuild* guild;
        Packet_DB_Load_Request_Web_Guild_Board_Write* pb = (Packet_DB_Load_Request_Web_Guild_Board_Write*)pkt;
        {
            DNF_LOG_SCOPE_LINE(0x1e15,"./log/GuildBoard",
                "CPacketTranslater::OnWebGuildBoardDelete Receive Data: GuildKey : %u, CharacID : %u, NO : %u",
                pb->m_guildKey, pb->m_charNo, pb->m_no);
        }
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e19, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardDelete : 0 == m_pclApp");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e20, "./log/GuildBoard", "CPacketTranslater::OnWebGuildBoardDelete : 0 == pclGuild");
            return;
        }
        guild->GetGuildBoard()->deleteGuildBoardData(pb->m_no, pb->m_guildKey, pb->m_charNo);
        guild->GetGuildBoard()->setWebGuildBoardAction(true);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1e29, "./log/Except", "CPacketTranslater::OnWebGuildBoardDelete Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1e2e, "./log/Except", "CPacketTranslater::OnWebGuildBoardDelete Exception Break\n");
    }
}

void CPacketTranslater::OnGuildApplyOriginalPowerSide(PacketHeader* pkt)
{
    Packet_Guild_Apply_Origial_Power_Side* pb = (Packet_Guild_Apply_Origial_Power_Side*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e44,"./log/Power",
                "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == m_pclApp");
            return;
        }
        unsigned int charNo = pb->m_charNo;
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e4e, "./log/Power", "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == pclUser");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1e55, "./log/Power", "CPacketTranslater::OnGuildApplyOriginalPowerSide : 0 == pclGuild");
            return;
        }
        unsigned char side = pb->m_field12;
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
        reply.ma = user->GetIdByChannel();
        reply.me = charNo;
        reply.m12 = guildKey;
        reply.m16 = newSide;
        user->SendToGameserver((char*)&reply, 0x17);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnGuildApplyOriginalPowerSide Exception Break", 0x1e7d, 0x1e82);
}

void CPacketTranslater::OnNotifyNewGroupMail(PacketHeader* pkt)
{
    Packet_Notify_New_Group_Mail* pb = (Packet_Notify_New_Group_Mail*)pkt;
    CUserManager* um = &m_pclApp->m_userManager;
    CUser* user = 0;
    Packet_Monitor_Notify_New_Mail mail;
    try
    {
        for (int i = 0; i < (int)pb->m_count; i++)
        {
            user = um->FindUser_CharNo(pb->m_characNos[i]);
            if (user != 0)
            {
                mail.ma = pb->m_characNos[i];
                mail.me = user->GetIdByChannel();
                user->SendToGameserver((char*)&mail, 0x12);
            }
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnNotifyNewGroupMail() Exception Break",
                  0x1e9f, 0x1ea4);
}

void CPacketTranslater::OnGameServerRegist(PacketHeader* pkt)
{
    Packet_Game_Server_Regist* pb = (Packet_Game_Server_Regist*)pkt;
    stServerInfo* info = new stServerInfo;
    info->m_group = pb->m_group;
    info->m_field1 = pb->m_channel;
    info->m_field2 = pb->m_type;
    info->m_port = pb->m_port;
    strncpy(info->m_name, pb->m_name, 0x10);
    CTcpGameServer* tgs = m_pclApp->Get_ServerHandler()->GetTcpGameServer(pb->m_connNo);
    if (tgs == 0)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x1ec6,"./log/GameServer", "Get Packet - OnGameServerRegist from Channel:%d",
        (unsigned int)(unsigned char)info->m_field1);
    if (tgs->GetChannelNo() == 0)
    {
    }
    else
    {
        return;
    }
    char* reply = tgs->makePacketHeader(0x1f42, 0xc);
    char* dummy = 0;
    if (reply != 0)
    {
        dummy = reply;
        if (!m_pclApp->Get_ServerHandler()->RegistGameServer(info))
        {
            reply[0xb] = 1;
            DNF_LOG_SCOPE_LINE(0x1ede,"./log/GameServer",
                "Game server regist failed. Channel: %d is already exist.",
                (unsigned int)(unsigned char)info->m_field1);
        }
        else
        {
            tgs->SetChannelNo(pb->m_channel);
            CGameServer* gs = m_pclApp->Get_ServerHandler()->GetGameServer(info->m_field1);
            gs->SetSocket(pb->m_connNo);
            reply[0xb] = 0;
            DNF_LOG_SCOPE_LINE(0x1eeb,"./log/GameServer", "Game server regist success. Channel: %d",
                (unsigned int)(unsigned char)info->m_field1);
        }
        reply[10] = 1;
        tgs->SendToGameServer(reply);
    }
}

void CPacketTranslater::OnAddGuildFund(PacketHeader* pkt)
{
    Packet_Add_Guild_Fund* pb = (Packet_Add_Guild_Fund*)pkt;
    CGuild* guild = 0;
    CUser* user = 0;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f03, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == m_pclApp");
        return;
    }
    try
    {
        if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f09, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f0f, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pGuild");
            return;
        }
        if (pb->m_mode == 1)
        {
            int result = guild->IsAddableGuildFund(pb->m_fund);
            if (result == 0)
            {
                guild->AddGuildFund(pb->m_fund);
                guild->GetGuildBoard()->sendMessageToDBMW_GuildFund(m_pclApp->Get_ServerHandler(),
                                                                    (int)pb->m_fund, user);
            }
            Packet_Add_Guild_Fund_Result reply;
            reply.mf = user->GetIdByChannel();
            reply.mb = pb->m_charNo;
            reply.ma = (unsigned char)result;
            user->SendToGameserver((char*)&reply, 0x17);
            guild->SendGuildInfoToManagers();
        }
        else if (pb->m_mode == 2)
        {
            guild->GetGuildDBInfo()->m_info.m_guildFund = pb->m_fund;
            guild->SendGuildInfoToMemberOnly(user);
        }
        else
        {
            guild->AddGuildFund(pb->m_fund);
        }
    }
    catch (std::exception& e)
    {
        DNF_LOG_SCOPE_LINE(0x1f36, "./log/Except",
            "CPacketTranslater::OnAddGuildFund Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1f3b, "./log/Except",
            "CPacketTranslater::OnAddGuildFund Exception Break\n");
    }
}

void CPacketTranslater::OnRefreshGuildInfo(PacketHeader* pkt)
{
    try
    {
        PacketHeader* p = pkt;
        CGuild* guild = 0;
        CUser* user = 0;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f4b, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == m_pclApp");
            return;
        }
        if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(
                 ((Packet_Refresh_Guild_Info*)p)->m_charNo)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f51, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
            return;
        }
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(
            ((Packet_Refresh_Guild_Info*)p)->m_guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f57, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pGuild");
            return;
        }
        guild->SendGuildInfoToMemberOnly(user);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnAddGuildFund Exception Break", 0x1f5f, 0x1f64);
}

void CPacketTranslater::OnReplyTodayGuildMember(PacketHeader* pkt)
{
    Packet_Reply_Today_Guild_Member* pb = (Packet_Reply_Today_Guild_Member*)pkt;
    CGuild* guild = 0;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1f75, "./log/Guild", "CPacketTranslater::OnReplyTodayGuildMember : 0 == m_pclApp");
        return;
    }
    try
    {
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildId)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1f7b, "./log/Guild", "CPacketTranslater::OnReplyTodayGuildMember : 0 == pGuild");
            return;
        }
        m_pclApp->Get_GuildManager()->InsertTodayMember(pb->m_guildId, pb->m_member);
        guild->SetTodayGuildMember(pb->m_member);
        guild->NotifyAllTodayGuildMember();
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnReplyTodayGuildMember Exception Break", 0x1f8a, 0x1f8f);
}

void CPacketTranslater::OnApproveJoinGuild(PacketHeader* pkt)
{
    Packet_Approve_Join_Guild* pb = (Packet_Approve_Join_Guild*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1fa0, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) != 0)
        {
            unsigned int charNo = pb->m_charNo;
            if (guild->IsGuildMaster(charNo) == 1 || guild->IsSubGuildMaster(charNo) == 1)
            {
                Packet_DB_Request_Approve_Join_Guild dbPkt;
                dbPkt.m_guildKey = guildKey;
                dbPkt.m_charNo = charNo;
                dbPkt.m_no = pb->m_no;
                dbPkt.m_group = m_pclApp->Get_ServerGroup();
                m_pclApp->m_serverHandler->SendToDB(&dbPkt);
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x1fad, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild authority error");
            }
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1fa6, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == pGuild");
        }
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnApproveJoinGuild Exception Break", 0x1fbb, 0x1fc0);
}

void CPacketTranslater::OnDBResponseApproveJoinGuild(PacketHeader* pkt)
{
    Packet_DB_Response_Approve_Join_Guild* pb = (Packet_DB_Response_Approve_Join_Guild*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1fd1, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = pb->m_guildId;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1fd7, "./log/Guild", "CPacketTranslater::OnApproveJoinGuild : 0 == pGuild");
            return;
        }
        unsigned int requesterNo = pb->m_characNo;
        CUser* requester = (&m_pclApp->m_userManager)->FindUser_CharNo(requesterNo);
        if (requester == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1fdd, "./log/Guild", "CPacketTranslater::OnAddGuildFund : 0 == pUser");
            return;
        }
        unsigned int joinerNo = pb->m_id;
        CUser* joiner = (&m_pclApp->m_userManager)->FindUser_CharNo(joinerNo);
        Packet_Approve_Join_Guild_Result reply;
        reply.ma = guildKey;
        reply.me = requester->GetUniqCharNo();
        reply.m12 = requester->GetIdByChannel();
        reply.m16 = pb->m_characNo;
        if (guildKey == 0)
        {
            if (joiner == 0)
            {
                GuildJoin(guild, (STGuildJoinInfo*)&pb->m_joinInfo,
                          joinerNo);
            }
            else
            {
                GuildJoin(guild, joiner, joinerNo);
            }
        }
        requester->SendToGameserver((char*)&reply, 0x1a);
    }
    DNF_CATCH_LOG("./log/Except", "CPacketTranslater::OnApproveJoinGuild Exception Break", 0x1ff5, 0x1ffa);
}

void CPacketTranslater::OnGuildAttendanceInfo(PacketHeader* pkt)
{
    Packet_Guild_Attendance_Info* pb = (Packet_Guild_Attendance_Info*)pkt;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x200e, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == m_pclApp");
            return;
        }
        unsigned int guildKey = pb->m_guildKey;
        CGuild* guild;
        if ((guild = (&m_pclApp->m_guildManager)->FindGuild(guildKey)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0x2014, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == pGuild");
            return;
        }
        unsigned int charNo = pb->m_charNo;
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0x201a, "./log/Guild", "CPacketTranslater::OnGuildAttendanceInfo : 0 == pUser");
            return;
        }
        Packet_Guild_Attendance_Info_Reply reply;
        reply.m_a = user->GetUniqCharNo();
        reply.m_b = user->GetIdByChannel();
        reply.m_c = pb->m_field12;
        (&m_pclApp->m_guildManager)->GetAttendanceInfo(
            guildKey, reply.m_info);
        user->SendToGameserver((char*)&reply, 0x2f);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x2029, "./log/Except", "CPacketTranslater::OnApproveJoinGuild Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x202e, "./log/Except", "CPacketTranslater::OnApproveJoinGuild Exception Break\n");
    }
}

void CPacketTranslater::OnGuildDebug(PacketHeader* pkt)
{
    try
    {
        Packet_Guild_Debug* pb = (Packet_Guild_Debug*)pkt;
        CUser* user = 0;
        CGuild* guild = 0;
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x2040, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == m_pclApp");
        }
        else
        {
            if ((guild = (&m_pclApp->m_guildManager)->FindGuild(pb->m_guildKey)) == 0)
            {
                DNF_LOG_SCOPE_LINE(0x2046, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == pGuild");
            }
            else
            {
                if ((user = (&m_pclApp->m_userManager)->FindUser_CharNo(pb->m_charNo)) == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x204c, "./log/Guild", "CPacketTranslater::OnGuildDebug : 0 == pUser");
                }
                else
                {
                    switch (pb->m_mode)
                    {
                    case 0:
                        m_pclApp->Get_GuildManager()->RefreshAttendanceInfo(true);
                        break;
                    case 1:
                        m_pclApp->Get_GuildManager()->RefreshTodayMember(true);
                        break;
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x2061, "./log/Except",
            "CPacketTranslater::OnGuildDebug Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2066, "./log/Except",
            "CPacketTranslater::OnGuildDebug Exception Break\n");
    }
}

void CPacketTranslater::OnRenew_GM_List(PacketHeader* pkt)
{
    Packet_Sync_GM_List* pb = (Packet_Sync_GM_List*)pkt;
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && pb != 0)
    {
        if (pb->m_field_a == 0)
        {
            gm->clearGmList();
        }
        for (int i = 0; i < (char)pb->m_field_b; i++)
        {
            gm->AppendGM_Sys(pb->m_gmIds[i], (char)pb->m_gmNames[i]);
        }
    }
}

Packet_Tcp_Server_Connect::Packet_Tcp_Server_Connect()
    : PacketHeader(0x3f8, 0xb)
{
    (void)0;
}

Packet_Monitor_UDP_User_Getout::Packet_Monitor_UDP_User_Getout()
    : PacketHeader(0x3ee, 0xe)
{
    ma = 0;
}

Packet_Monitor_Notice_Guild_Create_ToUser::Packet_Monitor_Notice_Guild_Create_ToUser()
    : PacketHeader(0x40c, 0x2d), m_member(0xffffffff)
{
}

Packet_Monitor_Call_Guild_Info_Reply::Packet_Monitor_Call_Guild_Info_Reply()
    : PacketHeader(0x40d, 0xad6), ma(0), me(0), m12(0)
{
}

Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade::
    Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade()
    : PacketHeader(0x42b, 0x33)
{
    ma = 0;
    mb = 0;
    m32 = 255;
    memset(&m_name, 0, 0x1e);
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
    memset(m_memo, 0, 0x15);
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
    memset(m_payload, 0, 0x65);
}

Packet_Notice_DB_Guild_War_End::Packet_Notice_DB_Guild_War_End()
    : PacketHeader(0x412, 0x6f)
{
    memset(m_fieldB, 0, 0x28);
    memset(m_field33, 0, 0x28);
    memset(m_field5b, 0, 0x14);
}

Packet_Channel_Delete_Guild_Agit::Packet_Channel_Delete_Guild_Agit()
    : PacketHeader(0x6de, 0x17)
{
    ma = 4294967295;
    me = 0;
    m12 = 0;
    m16 = 0;
}

Packet_Guild_Apply_Origial_Power_Side_Reply::Packet_Guild_Apply_Origial_Power_Side_Reply()
    : PacketHeader(0x6b2, 0x17)
{
    ma = 0;
    me = 0;
    m12 = 0;
    m16 = 0;
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
    memset(m_name, 0, 0x1e);
}

Packet_Delete_To_BlackList_Result::Packet_Delete_To_BlackList_Result()
    : PacketHeader(0x5dd, 0x31)
{
    memset(m_name, 0, 0x1e);
}

Packet_Request_Result_BlackList::Packet_Request_Result_BlackList()
    : PacketHeader(0x5e0, 0x19f)
{
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
    memset(m_name, 0, 0x1e);
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
    memset(m_name, 0, 0x1e);
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
    memset(m_title, 0, 0x11);
    memset(m_content, 0, 0x100);
}

Packet_Guild_Call_Guild_Invite_To_Caller::Packet_Guild_Call_Guild_Invite_To_Caller()
    : PacketHeader(0x434, 0x16)
{
    (void)0;
}

Packet_Guild_Call_Guild_Invite_To_Invited::Packet_Guild_Call_Guild_Invite_To_Invited()
    : PacketHeader(0x435, 0x47)
{
    memset(m_name, 0, 0x1e);
    memset(m_guildName, 0, 0x17);
}

Packet_Guild_Reply_Guild_Invite_To_Invited::Packet_Guild_Reply_Guild_Invite_To_Invited()
    : PacketHeader(0x436, 0x16)
{
    (void)0;
}

Packet_Guild_Reply_Guild_Invite_To_Caller::Packet_Guild_Reply_Guild_Invite_To_Caller()
    : PacketHeader(0x437, 0x34)
{
    memset(m_rest, 0, 0x1e);
}

Packet_DBMW_Save_Guild_Join::Packet_DBMW_Save_Guild_Join()
    : PacketHeader(0x438, 0x40)
{
    memset(m_name, 0, 0x1e);
    memset(m_ssn, 0, 3);
}

Packet_Guild_Reply_Guild_Secede::Packet_Guild_Reply_Guild_Secede()
    : PacketHeader(0x439, 0x52)
{
    memset(m_name, 0, 0x1e);
    memset(m_guildName, 0, 0x17);
}

Packet_DB_Request_Guild_Secede::Packet_DB_Request_Guild_Secede()
    : PacketHeader(0x439, 0x39)
{
    memset(m_name, 0, 0x1e);
}

Packet_Guild_Reply_Guild_Master_Delegate::Packet_Guild_Reply_Guild_Master_Delegate()
    : PacketHeader(0x43b, 0x16)
{
    (void)0;
}

Packet_DB_Request_Guild_Master_Delegate::Packet_DB_Request_Guild_Master_Delegate()
    : PacketHeader(0x43b, 0x31)
{
    memset(m_name, 0, 0x1e);
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
    memset(m_name, 0, 0x17);
}

Packet_UnChangable_GuildInfo_Save::Packet_UnChangable_GuildInfo_Save()
    : PacketHeader(0x452, 0x30)
{
    m_guildKey = 0;
    m_charNo = 0;
    memset(m_name, 0, 0x1e);
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
    memset(&m_members, 0, 0x5dc);
}

Packet_Reply_Guild_Create::Packet_Reply_Guild_Create()
    : PacketHeader(0x43f, 0x2d)
{
    memset(m_name, 0, 0x17);
}

Packet_DBMW_Request_Guild_Create::Packet_DBMW_Request_Guild_Create()
    : PacketHeader(0x43f, 0x5c)
{
    memset(m_name, 0, 0x17);
    memset(m_ssn, 0, 3);
    memset(m_extra, 0, 0xd);
    memset(m_guildName, 0, 0x17);
}

Packet_Notice_GuildName_On_Guild_Create::Packet_Notice_GuildName_On_Guild_Create()
    : PacketHeader(0x445, 0x2e)
{
    m_charNo = 0;
    m_channel = 4294967295;
    m_guildKey = 0;
    m_group = 255;
    memset(pad0x16, 0, 0x17);
}

void Packet_Guild_Change_Power_War_Point::GetUserList(std::vector<unsigned int>& list)
{
    unsigned int* users = &m_fieldB;
    for (int i = 0; i < 8; ++i)
    {
        if (users[i] != 0)
        {
            list.push_back(users[i]);
        }
    }
}

Packet_Guild_Cargo_Response::Packet_Guild_Cargo_Response()
    : PacketHeader(0x70a, 0x18ef), m_a(0xffffffff), m_b(0), m_flag(0)
{
    memset(&m_cargo, 0, sizeof(m_cargo));
}

Packet_Guild_Cargo_History_Response::Packet_Guild_Cargo_History_Response()
    : PacketHeader(0x70b, 0x972), ma(0xffffffff), me(0)
{
    memset(m_log, 0, 0x960);
}

Packet_Channel_Check_Guild_Cargo_Push_Item::Packet_Channel_Check_Guild_Cargo_Push_Item()
    : PacketHeader(0x70c, 0x27)
{
    ma = 4294967295;
    me = 0;
    m12 = 0;
    m13 = 0;
    m17 = 0;
    m1b = 0;
    m1f = 0;
    m23 = 0;
    m24 = 0;
    m25 = 0;
}

Packet_Channel_Guild_Cargo_Push_Item::Packet_Channel_Guild_Cargo_Push_Item()
    : PacketHeader(0x70d, 0x4f), m_a(0xffffffff), m_b(0), m_c(0), m_d(0)
{
    m_flag = 0;
    m_e = 0;
    memset(&m_item, 0, 0x35);
}

Packet_Channel_Guild_Cargo_Pop_Item::Packet_Channel_Guild_Cargo_Pop_Item()
    : PacketHeader(0x70e, 0x50), m_a(0xffffffff), m_b(0), m_c(0)
{
    m_flag = 0;
    m_d = 0;
    memset(&m_item, 0, 0x35);
}

Packet_Channel_Guild_Cargo_Move_Item::Packet_Channel_Guild_Cargo_Move_Item()
    : PacketHeader(0x70f, 0x1b)
{
    ma = 4294967295;
    me = 0;
    m12 = 0;
    m13 = 0;
    m17 = 0;
}

Packet_Channel_Guild_Cargo_Upgrade::Packet_Channel_Guild_Cargo_Upgrade()
    : PacketHeader(0x713, 0x13)
{
    m_channel = 4294967295;
    m_charNo = 0;
    m_result = 0;
}

Packet_DBMW_Query_Msg::Packet_DBMW_Query_Msg()
    : PacketHeader(0x177d, 0x1013)
{
    m_queryId = 0;
    m_handleIdx = 0;
    int i = 0;
    for (i = 0; i <= 0x1000; i++) { m_data[i] = 0x0; }
}

Packet_Guild_Exp_Book_Delete::Packet_Guild_Exp_Book_Delete()
    : PacketHeader(0x1b6c, 0x1a)
{
    m_channel = 4294967295;
    m_charNo = 0;
    m16 = 0;
}

Packet_No_Cache::Packet_No_Cache()
    : PacketHeader(0x1b6d, 0x16)
{
    ma = 0;
    me = 0;
    m12 = 0;
}

Packet_Add_Guild_Fund_Result::Packet_Add_Guild_Fund_Result()
    : PacketHeader(0x1bbd, 0x17)
{
    ma = 0;
    mb = 0;
    mf = 0;
    m13 = 0;
}

Packet_Approve_Join_Guild_Result::Packet_Approve_Join_Guild_Result()
    : PacketHeader(0x1bc3, 0x1a)
{
    ma = 0;
    me = 0;
    m12 = 0;
    m16 = 0;
}
