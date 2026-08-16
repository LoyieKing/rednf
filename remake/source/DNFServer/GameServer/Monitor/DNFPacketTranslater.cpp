// df_monitor_r — DNFPacketTranslater（从 MonitorTypes/App/Table 拆分）
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
#include <new>
#include <string>

#include "DNFPacketTranslater.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFMember.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Reply_UserInfo.h"
#include "Packet_Monitor_UDP_Double_Connect.h"
#include "Packet_Monitor_Charac_Delete.h"
#include "Packet_Monitor_User_Repel_ByCharName.h"
#include "Packet_Monitor_Find_Factory_Hub_User.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_Request_Charac_Tower_Update_Ranking.h"
#include "Packet_Find_Charac_Name_UseUID.h"
#include "Packet_Relay_Server_User_Check.h"
#include "Packet_Check_Overlapped_Accusation.h"
#include "Packet_Disable_User_OneToOneChat_Police.h"
#include "Packet_No_Cache.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Monitor_Char_Info.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_UDP_Logout_Complete.h"
#include "Packet_Register_To_BlackList.h"
#include "Packet_Delete_To_BlackList.h"
#include "Packet_Request_BlackList.h"
#include "Packet_Monitor_Add_Buddy.h"
#include "Packet_Monitor_Del_Buddy.h"
#include "Packet_Register_GM_MID.h"
#include "Packet_Change_Char_Name.h"
#include "Packet_Web_Notice_Single.h"
#include "Packet_Monitor_Request_Member_Enter.h"
#include "Packet_Monitor_Member_Enter_Reply.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_Monitor_Call_Member_List.h"
#include "Packet_Monitor_Member_Chat.h"
#include "Packet_Monitor_Member_Pay_Tax.h"
#include "Packet_Monitor_Other_Channel_Chat.h"
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"
#include "Packet_Monitor_Notice_Message.h"
#include "Packet_Forbid_Chat_By_Monitor.h"
#include "Packet_Exchange_Server_Info.h"
#include "Packet_Monitor_Notice_Charac_Live_On_Ten_Min.h"
#include "Packet_Monitor_Notify_New_Mail.h"
#include "Packet_ChannelType.h"
#include "Packet_Monitor_Member_Chat_Hyper_Link.h"
#include "Packet_Web_Prohibit_User_Connect.h"
#include "Packet_Game_Monitor_GM_Village_Attacked.h"
#include "Packet_Item_Limit_Edition_Buyable_Query.h"
#include "Packet_CollectItems.h"
#include "Packet_CollectItemsGm.h"
#include "Packet_Broadcast_Msg.h"
#include "Packet_Monitor_Request_Charac_Info.h"
#include "Packet_VillageAttackedGMCommand.h"
#include "Packet_VillageMonsterFightResult.h"
#include "DNFManagerServer.h"
#include "DNFMemberConfig.h"
#include "DNFUdpHandler.h"
#include "VillageAttackedManager.h"
#include "Arad_BirthdayEvent.h"
#include "Arad_MomijiEvent.h"
#include "BuddyRegisterManager.h"
#include "DNFAppConfig.h"
#include "DNFApplication.h"
#include "DNFBuddy.h"

// 与 ORIG 逐指令对齐用的本地报文布局（handler 直改 pkt 字节时的成员形态）
struct __attribute__((packed)) MonitorMegaPhonePkt
{
    char m_pad0[2];
    unsigned short m_sz;   // +0x2
    char m_pad1[6];
    signed char m_sg;      // +0xa
};

struct __attribute__((packed)) MonitorCollectGmPkt
{
    char m_pad0[2];
    unsigned short m_sz;   // +0x2
    char m_pad1[6];
    unsigned char m_sg;    // +0xa
};

struct __attribute__((packed)) MonitorMailPkt
{
    char m_pad0[10];
    unsigned int m_charNo;      // +0xa
    int m_idByChannel;          // +0xe
};

// 裸指针偏移 → 结构体成员访问（消除 `add $off; mov` 与 ORIG `mov off(%reg)` 的寻址差异）
struct __attribute__((packed)) MonitorBlackListRecord
{
    unsigned int m_charNo;      // +0x0
    char m_name[0x20];          // +0x4
    unsigned int m_uniqCharNo;  // +0x24
};

struct __attribute__((packed)) MonitorResponseBlackListPkt
{
    char m_base[0xe];
    MonitorBlackListRecord m_items[1];  // +0xe
};

struct __attribute__((packed)) MonitorBuddyInfoRec
{
    char m_name[0x1e];        // +0x0
    short m_lev;              // +0x1e
    char m_job;               // +0x20
    char m_growType;          // +0x21
    unsigned int m_characNo;  // +0x22
    char m_sex;               // +0x26
};

struct __attribute__((packed)) MonitorBuddyListPkt
{
    char m_base[0xf];
    MonitorBuddyInfoRec m_items[1];  // +0xf
};

struct __attribute__((packed)) MonitorTowerFullRankRec
{
    char m_pad0[0x17];          // +0x0
    unsigned short m_field17;   // +0x17
    unsigned short m_field19;   // +0x19
    char m_field1b;             // +0x1b
    char m_memberInfo[0x49];    // +0x1c .. 0x64
};

struct __attribute__((packed)) MonitorItemLimitEditionRec
{
    unsigned int m_ipgno;       // +0x0
    char m_rest[0x44];          // +0x4 .. 0x47
};

struct __attribute__((packed)) MonitorItemLimitEditionListPkt
{
    char m_base[0xf];
    MonitorItemLimitEditionRec m_items[1];  // +0xf
};

struct __attribute__((packed)) MonitorItemSellEndPkt
{
    char m_base[0x12];
    unsigned int m_items[1];    // +0x12
};

struct __attribute__((packed)) MonitorVillageFightPkt
{
    char m_base[10];
    unsigned int m_keys[8];     // +0xa
};

struct __attribute__((packed)) MonitorEmergencyPatchPkt
{
    char m_base[10];
    char m_channels[0x32];      // +0xa
};

struct __attribute__((packed)) MonitorCollectItemsState
{
    unsigned int m_total;       // +0x0
    unsigned int m_current;     // +0x4
    long m_time;                // +0x8
};

#include "DNFChannelWaitingUser.h"
#include "DNFGameServer.h"
#include "DNFManagerServer.h"
#include "DNFMember.h"
#include "DNFMemberManager.h"
#include "DNFProhibitUser.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFTableBase.h"
#include "DNFTowerRank.h"
#include "DNFUser.h"
#include "DNFUserManager.h"
#include "EventActionManager.h"
#include "GMAccounts.h"
#include "IPCounter.h"
#include "ItemLimitEditionManager.h"
#include "LimitNpcBuyItem.h"
#include "MemoryCashManager.h"
#include "OnTimeEventManager.h"

CApplication* CPacketTranslater::m_pclApp = 0;

void CPacketTranslater::attach(CApplication* app) { m_pclApp = app; }

void CPacketTranslater::SendRequestMemberEnterResult(CUser* user, unsigned char result,
                                                     const char* name)
{
    Packet_Monitor_Request_Member_Enter_To_Requester pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_result = result;
    memcpy(pkt.m_name, name, 0x1d);
    user->SendTcpGameserver(&pkt);
}

void CPacketTranslater::SendNoticeMemberEnterPacketOk(CUser* user, CUser* other, unsigned char a,
                                                      unsigned char b, unsigned char c,
                                                      unsigned char d, unsigned char e)
{
    Packet_Monitor_Notice_Member_Enter_Ok pkt;
    pkt.m_result = a;
    pkt.m_flag = b;
    pkt.m_flag2 = c;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_level = d;
    memcpy(pkt.m_name, other->GetCharName(), 0x1d);
    pkt.m_expLevel = e;
    if (b == 1)
    {
        pkt.m_extraCharNo = other->GetUniqCharNo();
    }
    user->SendTcpGameserver(&pkt);
}

void CPacketTranslater::SendNoticeMemberEnterPacketReply(CUser* user, CUser* other,
                                                         unsigned char a, unsigned char b,
                                                         unsigned char c, unsigned char d,
                                                         unsigned char e)
{
    Packet_Monitor_Member_Enter_Reply_ToResponser pkt;
    if (a == 2)
    {
        pkt.m_flag = a + 1;
    }
    else
    {
        pkt.m_flag = b;
    }
    pkt.m_result = a;
    pkt.m_flag2 = c;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_level = d;
    memcpy(pkt.m_name, other->GetCharName(), 0x1d);
    pkt.m_expLevel = e;
    if (b == 1)
    {
        pkt.m_extraCharNo = other->GetUniqCharNo();
    }
    user->SendTcpGameserver(&pkt);
}

void CPacketTranslater::SendRequestMemberDeleteResult(CUser* user, unsigned char result,
                                                      const char* name)
{
    Packet_Monitor_Member_Secede pkt;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_type = result;
    memcpy(pkt.m_name, name, 0x1d);
    user->SendTcpGameserver(&pkt);
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            Packet_Monitor_UDP_Login* login = (Packet_Monitor_UDP_Login*)pkt;
            unsigned int channel = (unsigned int)(unsigned char)login->m_channel;
            CTcpGameServer* tcpGs =
                (CTcpGameServer*)m_pclApp->FindTcpGameServer(login->m_connNo);
            if (tcpGs != 0)
            {
                CServerInterface* gs =
                    (CServerInterface*)m_pclApp->FindGameServer((int)channel);
                if (gs == 0)
                {
                    char* dbid = NumberToString(login->m_dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x129,"./log/Channel", "Not Found M_ID(%s) Channel No(%d)", dbid,
                        (unsigned int)(unsigned char)login->m_channel);
                }
                else
                {
                    CLoginLogoutStatistics* stats =
                        (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                    if (stats != 0)
                    {
                        stats->CountNumOfLoginout((ENUM_LOGIN_LOGOUT)0);
                    }
                    Packet_Monitor_UDP_Login* out =
                        (Packet_Monitor_UDP_Login*)tcpGs->makePacketHeader(1000, 0x33);
                    if (out != 0)
                    {
                        memcpy(&out->m_channelId, &login->m_channelId, 0x29);
                    }
                    CUserManager* userMgr = &m_pclApp->m_userManager;
                    CUser* user = userMgr->FindUser(login->m_dbid);
                    if (user == 0)
                    {
                        if (userMgr->FindProhibitUser(login->m_dbid) == 0)
                        {
                            user = userMgr->CreateUser(
                                login->m_dbid, 0, "",
                                login->m_channelId, (CGameServer*)gs);
                            user->SetSex(login->m_sex);
                            DNF_LOG_SCOPE_LINE(0x198,"./log/User", "OnLogin - SetSex : %d",
                                (unsigned int)(unsigned char)login->m_sex);
                            user->SetSsn(login->m_ssn);
                            user->SetTcpGameServer(tcpGs);
                            out->m_sex = 1;
                            char* dbid = NumberToString(login->m_dbid, 0);
                            CMyFileLog log2(__FUNCTION__, 0x1a5);
                            log2("./log/User",
                                 "Current user count : %d\tConnected User DB ID : %s\n",
                                 userMgr->Size(), dbid);
                            CLoginLogoutStatistics* stats2 =
                                (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                            if (stats2 != 0)
                            {
                                stats2->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)0,
                                                              (int)userMgr->Size());
                            }
                            CMemoryCashManager* cash =
                                (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                            if (cash->QueryCashMemoryBlackList(user) == 1)
                            {
                                user->SendBlackList();
                            }
                            else
                            {
                                RequestBlackListToDBMW(login->m_dbid);
                            }
                        }
                        else
                        {
                            if (out != 0)
                            {
                                out->m_sex = 3;
                            }
                            unsigned int ch = gs->GetChannelNo();
                            char* dbid = NumberToString(login->m_dbid, 0);
                            DNF_LOG_SCOPE_LINE(400,"./log/User",
                                "PROHIBIT USER CONNECTED : m_ID(%s)\tChannel(%d)\n", dbid,
                                ch & 0xff);
                        }
                    }
                    else
                    {
                        CTcpGameServer* oldGs =
                            (CTcpGameServer*)user->GetTcpGameServer();
                        if (oldGs != 0)
                        {
                            Packet_Monitor_UDP_Double_Connect* oldBuf =
                                (Packet_Monitor_UDP_Double_Connect*)oldGs->makePacketHeader(0x3ee, 0xe);
                            if (oldBuf != 0)
                            {
                                oldBuf->m_dbid = login->m_dbid;
                                oldGs->SendToGameServer((char*)oldBuf);
                            }
                        }
                        unsigned int ch = gs->GetChannelNo();
                        char* dbid = NumberToString(login->m_dbid, 1);
                        char* oldDbid = NumberToString(user->GetDBID(), 0);
                        DNF_LOG_SCOPE_LINE(0x17a,"./log/User",
                            "DOUBLE CONNECTED : Already User DB ID(%s)\tCurrent Connect User "
                            "DB ID(%s)\tChannel(%d)\n",
                            oldDbid, dbid, ch & 0xff);
                        if (out != 0)
                        {
                            out->m_sex = 2;
                        }
                    }
                    if (out != 0)
                    {
                        out->m_cache4 = 0;
                        out->m_cache0 = 0;
                    }
                    exchange_server::CACHE_CHARACTER_TYPE cacheType;
                    if (exchange_server::GetInstanceCacheCharacterMgr()->GetCacheCharacter(
                            login->m_dbid, &cacheType) != 0 &&
                        out != 0)
                    {
                        out->m_cache4 = (unsigned int)cacheType.m_field4;
                        out->m_cache0 = (unsigned int)cacheType.m_field0;
                    }
                    if (tcpGs != 0 && out != 0)
                    {
                        tcpGs->SendToGameServer((char*)out);
                    }
                }
            }
        }
        catch (CDNFException& e)
        {
            DNF_LOG_SCOPE_LINE(0x1ff, "./log/Except", "CPacketTranslater::OnLogin() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            DNF_LOG_SCOPE_LINE(0x204, "./log/Except", "CPacketTranslater::OnLogin() Exception Break\n");
        }
    }
}

void CPacketTranslater::OnLogout(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        return;
    }
    try
    {
        Packet_Monitor_UDP_Logout* logout = (Packet_Monitor_UDP_Logout*)pkt;
        if (((CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics()) != 0 && logout->m_foc == 0)
        {
            ((CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics())->CountNumOfLoginout((ENUM_LOGIN_LOGOUT)6);
        }
        CUser* user = (&m_pclApp->m_userManager)->FindUser(logout->m_dbid);
        if (user == 0)
        {
            char* dbid = NumberToString(logout->m_dbid, 0);
            DNF_LOG_SCOPE_LINE(0x22d,"./log/User", "LOGOUT ERR : User DB ID(%s), F.O.C(%d), Ch(%d)", dbid,
                (unsigned int)(unsigned char)logout->m_foc,
                (unsigned int)(unsigned char)logout->m_channel);
            return;
        }
        char* name = user->GetCharName();
        unsigned int memberKey = user->GetMemberKey();
        unsigned int charNo = user->GetUniqCharNo();
        char* dbid = NumberToString(logout->m_dbid, 0);
        DNF_LOG_SCOPE_LINE(0x230,"./log/User",
            "LOGOUT : User DB ID(%s), Char No(%d), Member K(%d) , name(%s), F.O.C(%d), Ch(%d)",
            dbid, charNo, memberKey, name,
            (unsigned int)(unsigned char)logout->m_foc,
            (unsigned int)(unsigned char)logout->m_channel);
        unsigned int memberKey2 = logout->m_memberKey;
        CMemoryCashManager* cash = (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
        CMember* member = (m_pclApp->Get_MemberManager())->FindMember(logout->m_memberKey);
        bool f1 = false;
        bool f2 = false;
        cash->InsertCashMemorySetCharacterObject(user, member, f1, f2);
        if (memberKey2 != 0)
        {
            (m_pclApp->Get_MemberManager())->MemberMemLogout(memberKey2, user, !f2);
        }
        if (user->GetUniqCharNo() != 0)
        {
            if ((&m_pclApp->m_userManager)->DeleteUser_CharNo(user->GetUniqCharNo()) == 1)
            {
                user->GetUniqCharNo();
                m_pclApp->Remove_GM_id(user->GetUniqCharNo());
                unsigned int charNos[32];
                int n = user->GetBuddysCharNo(charNos);
                CBuddyRegisterManager* buddyReg =
                    &m_pclApp->m_buddyMgr;
                for (int i = 0; i < n; i++)
                {
                    (&m_pclApp->m_buddyMgr)->delBuddyRegister(charNos[i], user->GetUniqCharNo());
                }
                std::vector<unsigned int> vec;
                (&m_pclApp->m_buddyMgr)->findBuddyRegister(user->GetUniqCharNo(), vec);
                for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end();
                     ++it)
                {
                    CUser* other = (&m_pclApp->m_userManager)->FindUser_CharNo(*it);
                    if (other != 0)
                    {
                        other->SendNoticeBuddyInOut(
                            ((CServerInterface*)user->GetGameServer())->GetChannelNo(),
                            user->GetUniqCharNo(), user->GetCharName(),
                            (unsigned char)(user->IsBlackUser(*it) != 0), 0, 0);
                    }
                }
                exchange_server::CACHE_CHARACTER_TYPE cacheType;
                cacheType.m_field0 = logout->m_cache0;
                cacheType.m_field4 = (int)(unsigned char)logout->m_channel;
                char* dbid2 = NumberToString(logout->m_dbid, 0);
                CMyFileLog log2(__FUNCTION__, 0x290);
                log2("./log/ExchangeServer", "CacheCharacter() (%s,%d,%d)\n", dbid2,
                     cacheType.m_field0, cacheType.m_field4);
                if (logout->m_cacheFlag != 0)
                {
                    exchange_server::GetInstanceCacheCharacterMgr()->CacheCharacter(
                        logout->m_dbid, &cacheType);
                }
                CLoginLogoutStatistics* stats2 =
                    (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                if (stats2 != 0)
                {
                    stats2->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)4,
                                                  (int)(&m_pclApp->m_userManager)->GetSizeOfCharnoUsers());
                }
            }
            (&m_pclApp->m_userManager)->DeleteUser_CharName(user->GetCharName());
            user->ResetCharInfo(!f1);
        }
        if (logout->m_foc != 0)
        {
            user->SetUserPosState(2);
            return;
        }
        CMemoryCashManager* cash2 = (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
        if (cash2->SetUserObject(user) == 1)
        {
            user->ResetBlackList(0);
        }
        else
        {
            user->ResetBlackList(1);
        }
        if (user->GetGameServer() != 0 &&
            ((CServerInterface*)user->GetGameServer())->GetChannelNo() ==
                (unsigned char)logout->m_channel)
        {
            if ((&m_pclApp->m_userManager)->DeleteUser(user) != 1)
            {
                char* dbid3 = NumberToString(logout->m_dbid, 0);
                CMyFileLog log3(__FUNCTION__, 0x2c5);
                log3("./log/User",
                     "[NO USER] Disconnected User DB ID : %s, Char No : %d , char name:%s\n",
                     dbid3, user->GetUniqCharNo(), user->GetCharName());
            }
        }
        else if (user->GetGameServer() != 0)
        {
            unsigned char alreadyCh =
                ((CServerInterface*)user->GetGameServer())->GetChannelNo();
            unsigned char logoutCh = (unsigned char)logout->m_channel;
            char* dbid4 = NumberToString(logout->m_dbid, 0);
            CMyFileLog log4(__FUNCTION__, 0x2cc);
            log4("./log/User",
                 "[LOGOUT SESSION MISMATCH] User DB ID : %s, Char No : %d , Already Ch(%d), "
                 "Logout Ch(%d)",
                 dbid4, user->GetUniqCharNo(), (unsigned int)alreadyCh,
                 (unsigned int)logoutCh);
        }
        CLoginLogoutStatistics* stats3 =
            (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
        if (stats3 != 0)
        {
            stats3->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)0, (int)(&m_pclApp->m_userManager)->Size());
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x2eb, "./log/Except", "CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnLogout() Exception Break");
        DNF_LOG_SCOPE_LINE(0x2f1, "./log/Except", "CPacketTranslater::OnLogout() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyUserInfo(PacketHeader* pkt)
{
    try
    {
        Packet_Monitor_UDP_Reply_UserInfo* reply = (Packet_Monitor_UDP_Reply_UserInfo*)pkt;
        DNF_LOG_SCOPE_LINE(0x361,"./log/Reboot", "[GAME SERVER] Channel No : %d\n",
            (unsigned int)(unsigned char)reply->m_channel);
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
        }
        CGameServer* gs =
            (CGameServer*)m_pclApp->FindGameServer(
                (int)(unsigned char)reply->m_channel);
        if (gs == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
        }
        CTcpGameServer* tcpGs =
            (CTcpGameServer*)m_pclApp->FindTcpGameServer(reply->m_connNo);
        if (tcpGs != 0)
        {
            CUserManager* userMgr = &m_pclApp->m_userManager;
            for (int i = 0;
                 i < (int)(unsigned int)(unsigned char)reply->m_count; i++)
            {
                ST_MonitorReplyUserInfo* entry = &reply->m_items[i];
                if (entry->m_dbid != 0)
                {
                    CUser* user = userMgr->FindUser(entry->m_dbid);
                    if (user == 0)
                    {
                        user = userMgr->CreateUser(
                            entry->m_dbid, entry->m_charNo,
                            entry->m_name, entry->m_channel, gs);
                        user->SetUserInfo_CharNo(
                            entry->m_job, entry->m_grade,
                            entry->m_level, entry->m_charNo,
                            entry->m_name);
                        user->SetSex(entry->m_sex);
                        user->SetSsn(entry->m_ssn);
                        user->SetTcpGameServer(tcpGs);
                        CMemoryCashManager* cash =
                            (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                        if (cash->QueryCashMemoryBlackList(user) != 1)
                        {
                            RequestBlackListToDBMW(entry->m_dbid);
                        }
                        if (entry->m_memberKey != 0)
                        {
                            CMemoryCashManager* cash2 =
                                (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                            if (cash2->QueryCashMemoryMember(user) != 1)
                            {
                                (&m_pclApp->m_memberManager)
                                    ->MemerMemLogin(entry->m_memberKey, user);
                            }
                        }
                        CMemoryCashManager* cash3 =
                            (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                        if (cash3->QueryCashMemoryBuddyInfo(user) != 1)
                        {
                            user->QueryBuddyInfo(
                                m_pclApp->m_serverHandler2);
                        }
                    }
                    else
                    {
                        char* dbid = NumberToString(entry->m_dbid, 0);
                        CMyFileLog log2("OnReplyUserInfo", 0x37a);
                        log2("./log/Except", "CPacketTranslater::OnReplyUserInfo() : %s\n", dbid);
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x3ac, "./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyUserInfo() Exception Break");
        DNF_LOG_SCOPE_LINE(0x3b2, "./log/Except", "CPacketTranslater::OnReplyUserInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnHeartBeat(PacketHeader* pkt)
{
    Packet_Monitor_UDP_HeartBeat* p = (Packet_Monitor_UDP_HeartBeat*)pkt;
    if (m_pclApp != 0)
    {
        CServerHandler* handler = m_pclApp->m_serverHandler2;
        if (handler != 0)
        {
            try
            {
                unsigned char channel = p->m_channel;
                if (channel == 0xc8)
                {
                    handler->ResetDBHeartBeat();
                    if (!handler->IsConnectedDBServer())
                    {
                        handler->SetDBConnectFlag(true);
                        handler->SendDBMWConnectionCheck();
                        DNF_LOG_SCOPE_LINE(0x318, "./log/DBHeartBeat", "DB Server Connection Complete!");
                    }
                }
                else if (channel != 0 && channel <= 0xbe)
                {
                    handler->ResetHeartBeat(channel);
                    if (!handler->IsConnectedGameServer(channel))
                    {
                        handler->SetConnectFlag(channel, true);
                        Packet_Tcp_Server_Connect pkt2;
                        pkt2.m_channel = 0xc9;
                        CServerInterface* gs;
                        if ((gs = handler->GetGameServer((unsigned int)channel)) != 0)
                        {
                            gs->SendToServer((char*)&pkt2, 0xb);
                        }
                        else
                        {
                            DNF_LOG_SCOPE_LINE(0x337,"./log/Except",
                                "CPacketTranslater::OnHeartBeat() => Channel Index : %d\n",
                                (unsigned int)channel);
                        }
                    }
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x341,"./log/Except", "[ERROR - HEART BEAT] Channel Index(%d) Over.",
                        (unsigned int)p->m_channel);
                }
            }
            catch (CDNFException& e)
            {
                printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
                DNF_LOG_SCOPE_LINE(0x348, "./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
            }
            catch (...)
            {
                puts("CPacketTranslater::OnHeartBeat() Exception Break");
                DNF_LOG_SCOPE_LINE(0x34e, "./log/Except", "CPacketTranslater::OnHeartBeat() Exception Break\n");
            }
        }
    }
}

void CPacketTranslater::OnCharLogin(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            Packet_Monitor_Char_Info* info = (Packet_Monitor_Char_Info*)pkt;
            CUserManager* userMgr = &m_pclApp->m_userManager;
            CUser* user = userMgr->FindUser(info->m_dbid);
            if (user == 0)
            {
                char* dbid = NumberToString(info->m_dbid, 0);
                DNF_LOG_SCOPE_LINE(0x457,"./log/User",
                    "[CHAR_LOGIN_ERR]\tDB ID : %s\tChar Key : %d\tGuild Key : %d\tJob : "
                    "%d\tname : %s\n",
                    dbid, info->m_charNo,
                    info->m_guildKey,
                    (int)info->m_job, info->m_name);
            }
            else
            {
                user->SetUserInfo_CharNo(
                    info->m_job, info->m_grade,
                    info->m_level, info->m_charNo,
                    info->m_name);
                char* dbid = NumberToString(info->m_dbid, 0);
                DNF_LOG_SCOPE_LINE(0x3cf,"./log/User",
                    "[CHAR_LOGIN]\tDB ID(%s)\tChar Key(%d)\tGuild K(%d)\tMember K(%d)\tJob(%d)"
                    "\tname(%s)\tCh No(%d)\treturn_user(%d)\n",
                    dbid, info->m_charNo,
                    info->m_guildKey,
                    info->m_memberKey,
                    (int)info->m_job, info->m_name,
                    (unsigned int)(unsigned char)info->m_channel,
                    (int)info->m_returnUser);
                user->SetUserPosState(3);
                CMemoryCashManager* cash =
                    (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                if (cash->QueryCashMemoryBuddyInfo(user) != 1)
                {
                    user->QueryBuddyInfo(
                        m_pclApp->m_serverHandler2);
                }
                std::vector<unsigned int> vec;
                (&m_pclApp->m_buddyMgr)
                    ->findBuddyRegister(user->GetUniqCharNo(), vec);
                for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end();
                     ++it)
                {
                    CUser* other = userMgr->FindUser_CharNo(*it);
                    if (other != 0)
                    {
                        other->SendNoticeBuddyInOut(
                            ((CServerInterface*)user->GetGameServer())->GetChannelNo(),
                            user->GetUniqCharNo(), user->GetCharName(),
                            (unsigned char)(user->IsBlackUser(*it) != 0), 1,
                            info->m_returnUser);
                    }
                }
                if ((int)info->m_memberKey != 0)
                {
                    CMemoryCashManager* cash2 =
                        (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                    if (cash2->QueryCashMemoryMember(user) != 1)
                    {
                        (&m_pclApp->m_memberManager)
                            ->MemerMemLogin(info->m_memberKey, user);
                    }
                }
                if (userMgr->InsertUser_CharName(info->m_name, user) != 1)
                {
                    char* dbid = NumberToString(info->m_dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x3fd,"./log/Except",
                        "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!", dbid,
                        info->m_name);
                }
                if (userMgr->InsertUser_CharNo(info->m_charNo, user) != 1)
                {
                    DNF_LOG_SCOPE_LINE(0x401,"./log/Except",
                        "Insert Fail!\tChar ID : %d\t\xc4\xb3\xb8\xaf\xc5\xcd \xc0\xcc\xb8\xa7:%s\n",
                        info->m_charNo, info->m_name);
                }
                ((CMemoryCashManager*)m_pclApp->Get_MemoryCashManager())
                    ->DeleteCashObjecct(user->GetDBID());
                stTowerRankElement_t elements[5];
                Packet_Request_Charac_Tower_Ranking rankPkt;
                rankPkt.m_idByChannel = user->GetIdByChannel();
                rankPkt.m_uniqCharNo = info->m_charNo;
                char hasData = 0;
                for (int t = 0; t < 4; t++)
                {
                    unsigned int cnt =
                        ((CTowerRank*)m_pclApp->getTowerRank())
                            ->getRankData((unsigned int)(t + 1), info->m_name, 5,
                                          elements);
                    for (unsigned int i = 0; i < cnt; i++)
                    {
                        rankPkt.m_scores[(unsigned int)(unsigned char)elements[i].m_job][t] =
                            (unsigned int)elements[i].m_score;
                        hasData = 1;
                    }
                }
                if (hasData != 0)
                {
                    user->SendToGameserver(
                        (char*)&rankPkt, rankPkt.packetSize);
                }
                time_t now = time(0);
                tm* pt = localtime(&now);
                Packet_Send_Time_Sync_For_Login syncPkt;
                syncPkt.m_dbid = user->GetDBID();
                syncPkt.m_idByChannel = user->GetIdByChannel();
                syncPkt.m_hour = (unsigned short)pt->tm_hour;
                syncPkt.m_min = (unsigned short)pt->tm_min;
                user->SendTcpGameserver(&syncPkt);
                CLoginLogoutStatistics* stats =
                    (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                if (stats != 0)
                {
                    stats->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)4,
                                                 (int)userMgr->GetSizeOfCharnoUsers());
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnCharLogin() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x463, "./log/Except", "CPacketTranslater::OnCharLogin() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CPacketTranslater::OnCharLogin() Exception Break");
            DNF_LOG_SCOPE_LINE(0x469, "./log/Except", "CPacketTranslater::OnCharLogin() Exception Break\n");
        }
    }
}

void CPacketTranslater::OnNoticeOtherChannelChatMsg(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeBuddyChatMsg : 0 == m_pclApp");
    }
    CUserManager* userMgr = &m_pclApp->m_userManager;
    Packet_Monitor_Other_Channel_Chat* chat =
        (Packet_Monitor_Other_Channel_Chat*)pkt;
    if (chat->what_0x1b != 0 &&
        (unsigned char)chat->what_0x1b < 0x1e)
    {
        CUser* target = userMgr->FindUser_CharName(chat->buddy_n_user_id_what);
        chat->what_0x17 =
            target != 0 ? (int)target->GetUniqCharNo() : (int)0xffffffff;
    }
    if (chat->what_0x13 == 0 ||
        chat->what_0x17 == 0 ||
        chat->chatLength == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb46,"./log/Except",
            "CPacketTranslater::OnNoticeOtherChannelChatMsg, sender(%d), receiver(%d), "
            "msglen(%d)",
            chat->what_0x13, chat->what_0x17,
            (unsigned int)(unsigned char)chat->chatLength);
        throw CDNFException(
            "CPacketTranslater::OnNoticeOtherChannelChatMsg : packet->m_uSenderCharID &&  "
            "packet->m_uRecverCharID && packet->m_msgLen");
    }
    Packet_Monitor_Other_Channel_Chat_ToUser reply;
    reply.m_senderCharId = (unsigned int)chat->what_0x0a;
    CUser* sender = userMgr->FindUser_CharNo((unsigned int)chat->what_0x13);
    if (sender == 0)
    {
        return;
    }
    CUser* receiver = userMgr->FindUser_CharNo((unsigned int)chat->what_0x17);
    if (receiver == 0)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.m_type = 1;
        reply.packetSize = 0x37;
        sender->SendToGameserver((char*)&reply, 0x37);
        return;
    }
    bool blocked = false;
    if (receiver->IsBlackUser(sender->GetUniqCharNo()) != 0 &&
        m_pclApp->isGM_regFromChannel(sender->GetUniqCharNo()) != 1)
    {
        blocked = true;
    }
    if (!blocked && sender->IsBlackUser(receiver->GetUniqCharNo()) != 0 &&
        m_pclApp->isGM_regFromChannel(receiver->GetUniqCharNo()) != 1)
    {
        blocked = true;
    }
    if (blocked)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        reply.m_type = 2;
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.packetSize = 0x37;
        sender->SendToGameserver((char*)&reply, 0x37);
        return;
    }
    if (m_pclApp->isGM_regFromChannel(sender->GetUniqCharNo()) != 0)
    {
        m_pclApp->AddChattableUserWithGM(sender->GetUniqCharNo(), receiver->GetUniqCharNo());
    }
    bool gmBlocked = false;
    if (m_pclApp->isGM_regFromChannel(receiver->GetUniqCharNo()) != 0)
    {
        if (m_pclApp->isAbleUserChatWithGM(receiver->GetUniqCharNo(),
                                           sender->GetUniqCharNo()) != 1)
        {
            gmBlocked = true;
        }
    }
    if (gmBlocked)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        reply.m_type = 3;
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.packetSize = 0x37;
        sender->SendToGameserver((char*)&reply, 0x37);
    }
    else
    {
        memcpy(reply.m_name, sender->GetCharName(), 0x1d);
        reply.m_idByChannel = receiver->GetIdByChannel();
        reply.m_uniqCharNo = receiver->GetUniqCharNo();
        reply.m_msgLen = chat->chatLength;
        memcpy(reply.m_msg, chat->chatContent,
               (unsigned int)(unsigned char)chat->chatLength);
        reply.packetSize =
            (unsigned short)((unsigned char)chat->chatLength + 0x37);
        receiver->SendToGameserver((char*)&reply, reply.packetSize);
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xb4c, "./log/Except", "Exception Break : %s", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb51, "./log/Except", "Exception Break");
    }
}

void CPacketTranslater::OnCeraUpdate(PacketHeader* pkt)
{try
{

    Packet_Cera_Update* pkt2 = (Packet_Cera_Update*)pkt;
    CUser* user =
        (&m_pclApp->m_userManager)->FindUser(pkt2->m_dbid);
    if (user != 0)
    {
        DNF_LOG_SCOPE_LINE(0x47f, "./log/User", "Cera Payed User , DB ID : %s\n",
            NumberToString(pkt2->m_dbid, 0));
        if (user->GetGameServer() != 0)
        {
            ((CServerInterface*)user->GetGameServer())->SendToServer((char*)pkt2, 0xe);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x48a, "./log/Except",
                "CPacketTranslater::OnCeraUpdate : pUser->GetGameServer() == 0");
        }
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCeraUpdate() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x490, "./log/Except", "CPacketTranslater::OnCeraUpdate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCeraUpdate() Exception Break");
        DNF_LOG_SCOPE_LINE(0x496, "./log/Except", "CPacketTranslater::OnCeraUpdate() Exception Break\n");
    }
}

void CPacketTranslater::OnEventItemUpdate(PacketHeader* pkt)
{
    try
    {
        Packet_Event_Item_Update* item = (Packet_Event_Item_Update*)pkt;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(item->m_dbid);
        CServerInterface* gs = 0;
        if (user == 0)
        {
            for (int i = 0; i < 0xff; i++)
            {
                gs = (CServerInterface*)m_pclApp->FindGameServer(i);
                if (gs != 0 && gs->IsConnected() != 0)
                {
                    break;
                }
            }
            if (gs == 0)
            {
                char* dbid = NumberToString(item->m_dbid, 0);
                DNF_LOG_SCOPE_LINE(0x4b0,"./log/User",
                    "Fail: Event User DB ID : %s [EventType: %d][CharacNo: %d][ItemID: "
                    "%d][Stack: %d]\n",
                    dbid, item->m_eventType,
                    item->m_charNo,
                    item->m_itemId,
                    item->m_stack);
                return;
            }
        }
        else
        {
            gs = (CServerInterface*)user->GetGameServer();
        }
        unsigned char channel = gs->GetChannelNo();
        char* dbid = NumberToString(item->m_dbid, 0);
        DNF_LOG_SCOPE_LINE(0x4b7,"./log/User",
            "Event User DB ID : %s [EventType: %d][CharacNo: %d][ItemID: %d][Stack: "
            "%d][TableID: %d][Channel No: %d]\n",
            dbid, item->m_eventType,
            item->m_charNo, item->m_itemId,
            item->m_stack, item->m_tableId,
            (unsigned int)channel);
        if (gs != 0)
        {
            gs->SendToServer((char*)pkt, 0x22);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCoinUpdate() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x4c3, "./log/Except", "CPacketTranslater::OnCoinUpdate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCoinUpdate() Exception Break");
        DNF_LOG_SCOPE_LINE(0x4c9, "./log/Except", "CPacketTranslater::OnCoinUpdate() Exception Break\n");
    }
}

void CPacketTranslater::OnReplyQueryMember(PacketHeader* pkt)
{try
{
    Packet_DB_Reply_Query_Member* p = (Packet_DB_Reply_Query_Member*)pkt;


    if (p->m_success == 1)
    {
        CServerHandler* handler = m_pclApp->m_serverHandler2;
        if (handler == 0) goto end;
        if (!(&m_pclApp->m_memberManager)
                ->LoadMember(p->m_memberId,
                             p->m_info,
                             p->m_maxCreateTime,
                             p->m_maxDeleteTime, handler))
        {
            DNF_LOG_SCOPE_LINE(0x4e5,"./log/MemberMember",
                "CHECK MEMBER ID: CPacketTranslater::OnReplyQueryMember()\t"
                "m_clMemberManager.LoadMember()\tmember id(%d)",
                p->m_memberId);
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x4eb,"./log/Except",
            "[DB ERROR]CPacketTranslater::OnReplyQueryMember() packet->bSuccess : %d\n",
            (unsigned int)(unsigned char)p->m_success);
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnReplyQueryMember() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x4f1, "./log/Except", "CPacketTranslater::OnReplyQueryMember() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyQueryMember() Exception Break");
        DNF_LOG_SCOPE_LINE(0x4f7, "./log/Except", "CPacketTranslater::OnReplyQueryMember() Exception Break\n");
    }
end:
    ;
}

void CPacketTranslater::OnRequestMemberEnter(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        return;
    }
    CUserManager* userMgr = &m_pclApp->m_userManager;
    CMemberManager* memberMgr = &m_pclApp->m_memberManager;
    Packet_Monitor_Request_Member_Enter* req =
        (Packet_Monitor_Request_Member_Enter*)pkt;
    CUser* requester = userMgr->FindUser_CharNo(req->m_charNo);
    if (requester == 0)
    {
        return;
    }
    CUser* target = userMgr->FindUser_CharName(req->m_name);
    if (target == 0)
    {
        SendRequestMemberEnterResult(requester, '1', req->m_name);
        return;
    }
    if (memberMgr->IsAlreadyMemberMember(req->m_charNo,
                                         target->GetUniqCharNo()) != 0)
    {
        SendRequestMemberEnterResult(requester, '2', req->m_name);
        DNF_LOG_SCOPE_LINE(0x599,"./log/MemberModify", "Err Already Member : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    if (requester->IsAbleToRegisterMember() != 1 || target->IsAbleToRegisterMember() != 1)
    {
        SendRequestMemberEnterResult(requester, '7', req->m_name);
        DNF_LOG_SCOPE_LINE(0x5a2,"./log/MemberModify",
            "Err Member Register Restrict : requester(%d:%d) responser(%d:%d)",
            requester->GetUniqCharNo(), requester->IsAbleToRegisterMember(),
            target->GetUniqCharNo(), target->IsAbleToRegisterMember());
        return;
    }
    if (requester->IsBlackUser(target->GetUniqCharNo()) != 0)
    {
        SendRequestMemberEnterResult(requester, '6', req->m_name);
        DNF_LOG_SCOPE_LINE(0x5a9,"./log/MemberModify", "Err Member Register Black : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && gm->isGM(target->GetDBID()) != 0)
    {
        SendRequestMemberEnterResult(requester, 'Z', req->m_name);
        DNF_LOG_SCOPE_LINE(0x5b4,"./log/MemberModify", "Err Member Register GM : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    CMember* requesterMember = memberMgr->FindMember(requester->GetUniqCharNo());
    CMember* targetMember = memberMgr->FindMember(target->GetUniqCharNo());
    if (target->GetMemberEnterCallerId() != 0)
    {
        SendRequestMemberEnterResult(requester, ')', req->m_name);
        return;
    }
    bool pending = false;
    if (requester->GetMemberEnterCallerId() != 0 &&
        requester->GetMemberEnterCallerId() == target->GetUniqCharNo())
    {
        pending = true;
    }
    if (pending)
    {
        SendRequestMemberEnterResult(requester, ')', req->m_name);
    }
    else
    {
        int err = memberMgr->CheckMemberEnter(requester, requesterMember, target, targetMember);
        if (err == 0)
        {
            if (target->RecordCallMemberEnter(req->m_charNo, 1) == 1)
            {
                SendRequestMemberEnterResult(requester, 0, req->m_name);
                Packet_Monitor_Request_Member_Enter_To_Responser rpkt;
                rpkt.m_idByChannel = target->GetIdByChannel();
                rpkt.m_uniqCharNo = target->GetUniqCharNo();
                rpkt.m_level = (unsigned short)requester->GetLevel();
                memcpy(rpkt.m_name, requester->GetCharName(), 0x1d);
                target->SendTcpGameserver(&rpkt);
            }
            else
            {
                SendRequestMemberEnterResult(requester, ')', req->m_name);
            }
        }
        else
        {
            SendRequestMemberEnterResult(requester, (unsigned char)err, req->m_name);
            DNF_LOG_SCOPE_LINE(0x5d0, "./log/MemberModify",
                "Err(%d) Member Register : requester(%d) responser(%d)", err,
                requester->GetUniqCharNo(), target->GetUniqCharNo());
        }
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnRequestMemberEnter() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x5ee, "./log/Except", "CPacketTranslater::OnRequestMemberEnter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnRequestMemberEnter() Exception Break");
        DNF_LOG_SCOPE_LINE(0x5f4, "./log/Except", "CPacketTranslater::OnRequestMemberEnter() Exception Break\n");
    }
}

void CPacketTranslater::OnMemberEnterReply(PacketHeader* pkt)
{try
{


    if (m_pclApp != 0)
    {
        Packet_Monitor_Member_Enter_Reply* reply =
            (Packet_Monitor_Member_Enter_Reply*)pkt;
        CUser* requester = (&m_pclApp->m_userManager)->FindUser_CharNo(reply->m_charNo);
        if (requester != 0)
        {
            CUser* responser = (&m_pclApp->m_userManager)->FindUser_CharNo(requester->GetMemberEnterCallerId());
            if (responser != 0)
            {
                if (responser->IsAbleToRegisterMember() != 1 ||
                    requester->IsAbleToRegisterMember() != 1)
                {
                    SendRequestMemberEnterResult(responser, '7', responser->GetCharName());
                    DNF_LOG_SCOPE_LINE(0x621,"./log/MemberModify",
                        "Err Member Register Restrict : requester(%d:%d) responser(%d:%d)",
                        responser->GetUniqCharNo(), responser->IsAbleToRegisterMember(),
                        requester->GetUniqCharNo(), requester->IsAbleToRegisterMember());
                }
                else if (requester->CheckPrevCallMemberEnter() == 1)
                {
                    unsigned char code = reply->m_code;
                    if (code == 2)
                    {
                        SendNoticeMemberEnterPacketOk(responser, requester, 2, 0, 0, 0, 0);
                        SendNoticeMemberEnterPacketReply(requester, responser, 2, 0, 0, 0, 0);
                        DNF_LOG_SCOPE_LINE(0x63a,"./log/MemberModify",
                            "Char id(%d) Reject And Reset char id(%d)",
                            requester->GetUniqCharNo(), responser->GetUniqCharNo());
                        requester->ResetRequestMemberEnter();
                    }
                    else if (code == 3)
                    {
                        SendNoticeMemberEnterPacketOk(responser, requester, 3, 0, 0, 0, 0);
                        SendNoticeMemberEnterPacketReply(requester, responser, 3, 0, 0, 0, 0);
                        DNF_LOG_SCOPE_LINE(0x647,"./log/MemberModify",
                            "Char id(%d) Reject And Reset char id(%d)",
                            requester->GetUniqCharNo(), responser->GetUniqCharNo());
                        requester->ResetRequestMemberEnter();
                    }
                    else if (code == 4)
                    {
                        SendNoticeMemberEnterPacketOk(responser, requester, 4, 0, 0, 0, 0);
                        SendNoticeMemberEnterPacketReply(requester, responser, 4, 0, 0, 0, 0);
                        DNF_LOG_SCOPE_LINE(0x654,"./log/MemberModify",
                            "Char id(%d) Reject And Reset char id(%d)",
                            requester->GetUniqCharNo(), responser->GetUniqCharNo());
                        requester->ResetRequestMemberEnter();
                    }
                    else
                    {
                        CMember* responserMember =
                            (&m_pclApp->m_memberManager)->FindMember(responser->GetUniqCharNo());
                        CMember* requesterMember =
                            (&m_pclApp->m_memberManager)->FindMember(requester->GetUniqCharNo());
                        if (requester->GetMemberEnterCallerId() == 0)
                        {
                            SendRequestMemberEnterResult(requester, '0',
                                                         responser->GetCharName());
                            DNF_LOG_SCOPE_LINE(0x662,"./log/MemberModify",
                                "Err : %d not received request from %d",
                                requester->GetUniqCharNo(), responser->GetUniqCharNo());
                        }
                        else
                        {
                            int err = (&m_pclApp->m_memberManager)->CheckMemberEnter(
                                responser, responserMember, requester, requesterMember);
                            if (err == 0)
                            {
                                CServerHandler* handler =
                                    m_pclApp->m_serverHandler2;
                                if (handler != 0)
                                {
                                    if (responserMember == 0)
                                    {
                                        responserMember =
                                            (&m_pclApp->m_memberManager)->CreateMemberInJoin(responser);
                                    }
                                    if (requesterMember == 0)
                                    {
                                        requesterMember =
                                            (&m_pclApp->m_memberManager)->CreateMemberInJoin(requester);
                                    }
                                    short rl = responser->GetLevel();
                                    if ((&m_pclApp->m_memberManager)->RegisterMember(responserMember, rl, requester,
                                                                  true) == 1)
                                    {
                                        short ql = requester->GetLevel();
                                        if ((&m_pclApp->m_memberManager)->RegisterMember(requesterMember, ql,
                                                                      responser, true) == 1)
                                        {
                                            (&m_pclApp->m_memberManager)->SendToDBMemberUpdateCharInfo(
                                                handler, responser->GetUniqCharNo(), 1);
                                            (&m_pclApp->m_memberManager)->SendToDBMemberUpdateCharInfo(
                                                handler, requester->GetUniqCharNo(), 1);
                                            (&m_pclApp->m_memberManager)->SaveMemberOnConnect(
                                                handler, responser, requester, 1);
                                            requester->ResetRequestMemberEnter();
                                            responser->SetMemberRegisterFlag(false);
                                            requester->SetMemberRegisterFlag(false);
                                            DNF_LOG_SCOPE_LINE(0x69b,"./log/MemberModify",
                                                "pclResponserUser Char id(%d)(%d) success and reset, pclRequestUser char id(%d)(%d)!",
                                                requester->GetUniqCharNo(),
                                                requester->IsAbleToRegisterMember(),
                                                responser->GetUniqCharNo(),
                                                responser->IsAbleToRegisterMember());
                                            short l1 = requester->GetLevel();
                                            short l2 = responser->GetLevel();
                                            if (l2 < l1)
                                            {
                                                unsigned char el =
                                                    requester->GetUpperMemberExpLevel();
                                                unsigned char lv =
                                                    (unsigned char)requester->GetLevel();
                                                SendNoticeMemberEnterPacketOk(
                                                    responser, requester, 1, 1, 1, lv, el);
                                                unsigned char lv2 =
                                                    (unsigned char)responser->GetLevel();
                                                SendNoticeMemberEnterPacketReply(
                                                    requester, responser, 1, 2, 0, lv2, 0);
                                            }
                                            else
                                            {
                                                short l3 = requester->GetLevel();
                                                short l4 = responser->GetLevel();
                                                if (l3 < l4)
                                                {
                                                    unsigned char lv3 =
                                                        (unsigned char)requester->GetLevel();
                                                    SendNoticeMemberEnterPacketOk(
                                                        responser, requester, 1, 2, 0, lv3, 0);
                                                    unsigned char el2 =
                                                        responser->GetUpperMemberExpLevel();
                                                    unsigned char lv4 =
                                                        (unsigned char)responser->GetLevel();
                                                    SendNoticeMemberEnterPacketReply(
                                                        requester, responser, 1, 1, 1, lv4, el2);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            short l5 = requester->GetLevel();
                                            short l6 = responser->GetLevel();
                                            DNF_LOG_SCOPE_LINE(0x688,"./log/MemberModify",
                                                "CPacketTranslater::OnMemberEnterReply  :  RegisterMember return false , Responser Char id(%d), Responser Member id(%d), Caller Level(%d), Responser Level(%d)!",
                                                requester->GetUniqCharNo(),
                                                requesterMember->GetMemberKey(), (int)l6,
                                                (int)l5);
                                        }
                                    }
                                    else
                                    {
                                        short l7 = requester->GetLevel();
                                        short l8 = responser->GetLevel();
                                        DNF_LOG_SCOPE_LINE(0x681,"./log/MemberModify",
                                            "CPacketTranslater::OnMemberEnterReply  :  RegisterMember return false , Caller Char id(%d), Caller Member id(%d), Caller Level(%d), Responser Level(%d)!",
                                            responser->GetUniqCharNo(),
                                            responser->GetMemberKey(), (int)l8, (int)l7);
                                    }
                                }
                            }
                            else
                            {
                                SendRequestMemberEnterResult(requester, (unsigned char)err,
                                                             responser->GetCharName());
                                DNF_LOG_SCOPE_LINE(0x66a,"./log/MemberModify", "Err(%d) : %d Fail And Reset %d", err,
                                    requester->GetUniqCharNo(), responser->GetUniqCharNo());
                                requester->ResetRequestMemberEnter();
                            }
                        }
                    }
                }
                else
                {
                    SendRequestMemberEnterResult(requester, '/', responser->GetCharName());
                    DNF_LOG_SCOPE_LINE(0x62e,"./log/MemberModify", "Char id(%d) Reset char id(%d)",
                        requester->GetUniqCharNo(), responser->GetUniqCharNo());
                    requester->ResetRequestMemberEnter();
                }
            }
        }
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnRequestMemberEnter() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x6be, "./log/Except", "CPacketTranslater::OnRequestMemberEnter() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnRequestMemberEnter() Exception Break");
        DNF_LOG_SCOPE_LINE(0x6c4, "./log/Except", "CPacketTranslater::OnRequestMemberEnter() Exception Break\n");
    }
}

void CPacketTranslater::OnMemberSecede(PacketHeader* pkt)
{try
{


    if (m_pclApp != 0)
    {
        CUserManager* userMgr = &m_pclApp->m_userManager;
        CMemberManager* memberMgr = &m_pclApp->m_memberManager;
        Packet_Monitor_Member_Secede* secede = (Packet_Monitor_Member_Secede*)pkt;
        unsigned int secederCharNo = secede->m_uniqCharNo;
        CUser* seceder = userMgr->FindUser_CharNo(secederCharNo);
        if (seceder != 0)
        {
            CMember* member = memberMgr->FindMember(secederCharNo);
            if (member == 0)
            {
                SendRequestMemberDeleteResult(seceder, '1', secede->m_name);
            }
            else
            {
                CServerHandler* handler = m_pclApp->m_serverHandler2;
                if (handler != 0)
                {
                    CUser* target = userMgr->FindUser_CharName(secede->m_name);
                    unsigned char result = 0;
                    unsigned int targetKey = 0;
                    if (target == 0)
                    {
                        result = (unsigned char)member->DeleteMemberByName(secede->m_name,
                                                                          targetKey);
                        if (result == 3)
                        {
                            return;
                        }
                        if (memberMgr->CheckEmptyMember(member, seceder) != 0)
                        {
                            memberMgr->SendToDBMemberUpdateCharInfo(handler, secederCharNo, 0);
                        }
                        memberMgr->SaveMemberOnUnConnect(handler, secederCharNo, targetKey,
                                                         (unsigned int)result, 2);
                    }
                    else
                    {
                        targetKey = target->GetUniqCharNo();
                        CMember* targetMember = memberMgr->FindMember(targetKey);
                        if (targetKey == secederCharNo)
                        {
                            return;
                        }
                        result = 2;
                        if (member->GetUpperMember_CharId() == (int)targetKey)
                        {
                            member->DeleteUpperMember(targetKey, true);
                            if (targetMember != 0)
                            {
                                targetMember->DeleteLowerMember(secederCharNo, false);
                            }
                            result = 1;
                        }
                        else
                        {
                            member->DeleteLowerMember(targetKey, true);
                            if (targetMember != 0)
                            {
                                targetMember->DeleteUpperMember(secederCharNo, false);
                            }
                        }
                        memberMgr->SaveMemberOnConnect(handler, seceder, target, 2);
                        if (memberMgr->CheckEmptyMember(member, seceder) != 0)
                        {
                            memberMgr->SendToDBMemberUpdateCharInfo(handler, secederCharNo, 0);
                        }
                        if (memberMgr->CheckEmptyMember(targetMember, target) != 0)
                        {
                            memberMgr->SendToDBMemberUpdateCharInfo(handler, targetKey, 0);
                        }
                        Packet_Monitor_Member_Secede_To_Seceder spkt;
                        spkt.m_idByChannel = target->GetIdByChannel();
                        spkt.m_uniqCharNo = targetKey;
                        spkt.m_type = 1;
                        if (result == 1)
                        {
                            spkt.m_type = 2;
                        }
                        memcpy(spkt.m_name, seceder->GetCharName(), 0x1d);
                        target->SendTcpGameserver(&spkt);
                    }
                    SendRequestMemberDeleteResult(seceder, result, secede->m_name);
                    seceder->SetMemberRegisterFlag(false);
                }
            }
        }
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnMemberSecede() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x750, "./log/Except", "CPacketTranslater::OnMemberSecede() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnMemberSecede() Exception Break");
        DNF_LOG_SCOPE_LINE(0x756, "./log/Except", "CPacketTranslater::OnMemberSecede() Exception Break\n");
    }
}

void CPacketTranslater::OnCallMemberList(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            CUserManager* userMgr = &m_pclApp->m_userManager;
            CMemberManager* memberMgr = &m_pclApp->m_memberManager;
            CUser* user = userMgr->FindUser_CharNo(((Packet_Monitor_Call_Member_List*)pkt)->m_charNo);
            if (user != 0)
            {
                CMember* member = memberMgr->FindMember(user->GetUniqCharNo());
                if (member != 0)
                {
                    Packet_Monitor_Call_Member_List_ToUser rpkt;
                    rpkt.m_idByChannel = user->GetIdByChannel();
                    rpkt.m_uniqCharNo = user->GetUniqCharNo();
                    STMemberDBInfo* db = (STMemberDBInfo*)member->GetMemberDBInfo();
                    CUser* upperUser = userMgr->FindUser_CharNo(db->m_member.m_charNo);
                    if (upperUser == 0)
                    {
                        rpkt.m_memberList.m_info.m_channelNo = 0xff;
                    }
                    else if (upperUser->GetGameServer() == 0)
                    {
                        rpkt.m_memberList.m_info.m_channelNo = 0xff;
                    }
                    else
                    {
                        if (upperUser->IsBlackUser(user->GetUniqCharNo()) != 0)
                        {
                            rpkt.m_memberList.m_info.m_blackFlag = 1;
                        }
                        rpkt.m_memberList.m_info.m_channelNo =
                            ((CServerInterface*)upperUser->GetGameServer())->GetChannelNo();
                    }
                    rpkt.m_memberList.m_info.m_flag = db->m_member.m_flag4;
                    memcpy(rpkt.m_memberList.m_info.m_name, db->m_member.m_name, 0x1d);
                    rpkt.m_memberList.m_info.m_exp = db->m_member.m_exp;
                    unsigned int upperExp = rpkt.m_memberList.m_info.m_exp;
                    unsigned int upperExpNext = rpkt.m_memberList.m_info.m_expNext;
                    unsigned char upperExpLevel = rpkt.m_memberList.m_info.m_level;
                    memberMgr->GetMemberExpNextLevelNeedExpLevel(
                        upperExp, upperExpNext, upperExpLevel);
                    rpkt.m_memberList.m_info.m_exp = upperExp;
                    rpkt.m_memberList.m_info.m_expNext = upperExpNext;
                    rpkt.m_memberList.m_info.m_level = upperExpLevel;
                    rpkt.m_memberList.m_count = db->m_count27;
                    for (int i = 0; i < (int)(unsigned int)db->m_count27; i++)
                    {
                        ST_MemberInfo* entry = &rpkt.m_memberList.m_members[i];
                        unsigned int lowerCharNo = db->m_lowers[i].m_charNo;
                        CUser* lowerUser = userMgr->FindUser_CharNo(lowerCharNo);
                        if (lowerUser == 0)
                        {
                            entry->m_channelNo = 0xff;
                        }
                        else if (lowerUser->GetGameServer() == 0)
                        {
                            entry->m_channelNo = 0xff;
                        }
                        else
                        {
                            if (lowerUser->IsBlackUser(user->GetUniqCharNo()) != 0)
                            {
                                entry->m_blackFlag = 1;
                            }
                            entry->m_channelNo =
                                ((CServerInterface*)lowerUser->GetGameServer())->GetChannelNo();
                        }
                        entry->m_flag = db->m_lowers[i].m_flag4;
                        memcpy(entry->m_name, db->m_lowers[i].m_name, 0x1d);
                        entry->m_exp = db->m_lowers[i].m_exp;
                        unsigned int exp = entry->m_exp;
                        unsigned int expNext = entry->m_expNext;
                        unsigned char expLevel = entry->m_level;
                        memberMgr->GetMemberExpNextLevelNeedExpLevel(
                            exp, expNext, expLevel);
                        entry->m_exp = exp;
                        entry->m_expNext = expNext;
                        entry->m_level = expLevel;
                    }
                    int size = (int)(unsigned int)db->m_count27 * 0x2a + 0x3d;
                    user->SendToGameserver((char*)&rpkt, size);
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnCallMemberList() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x7cd, "./log/Except",
                "CPacketTranslater::OnCallMemberList() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CPacketTranslater::OnCallMemberList() Exception Break");
            DNF_LOG_SCOPE_LINE(0x7d3, "./log/Except", "CPacketTranslater::OnCallMemberList() Exception Break\n");
        }
    }
}

void CPacketTranslater::OnNoticeMemberChatMsg(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeMemberChatMsg : 0 == m_pclApp");
    }
    Packet_Monitor_Member_Chat* chat = (Packet_Monitor_Member_Chat*)pkt;
    if (chat->m_memberId != 0 && chat->m_msgLen != 0)
    {
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser_CharNo(
                chat->m_memberId);
        if (user != 0)
        {
            CMember* member =
                (&m_pclApp->m_memberManager)->FindMember(
                    chat->m_memberId);
            if (member != 0)
            {
                member->NoticeChatMsgToMemberMembers(
                    chat->m_msg, (unsigned int)(unsigned char)chat->m_msgLen,
                    user);
            }
        }
        return;
    }
    throw CDNFException(
        "CPacketTranslater::OnNoticeMemberChatMsg : packet->m_uMemberID && packet->m_msgLen");


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x822, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x828, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break\n");
    }
}

void CPacketTranslater::OnPayTaxToUpper(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnPayTaxToUpper : 0 == m_pclApp");
        }
        CMemberManager* memberMgr = &m_pclApp->m_memberManager;
        CUserManager* userMgr = &m_pclApp->m_userManager;
        Packet_Monitor_Member_Pay_Tax* tax = (Packet_Monitor_Member_Pay_Tax*)pkt;
        CMember* member = memberMgr->FindMember(tax->m_charNo);
        if (member == 0)
        {
            DNF_LOG_SCOPE_LINE(0x842,"./log/Except",
                "[MEMBER] CPacketTranslater::OnPayTaxToUpper : pclMember == 0!\tchar id(%d)\t"
                "money(%d)\tfatigue(%d)",
                tax->m_charNo, tax->m_money,
                tax->m_fatigue);
        }
        else
        {
            CUser* upperUser = userMgr->FindUser_CharNo(member->GetUpperMember_CharId());
            if (upperUser == 0)
            {
                DNF_LOG_SCOPE_LINE(0x849,"./log/Member",
                    "CPacketTranslater::OnPayTaxToUpper : pclUpperUser == 0!, Maybe, upper "
                    "Member is not connect!\tupper char id(%d)",
                    member->GetUpperMember_CharId());
            }
            else
            {
                CUser* payUser = userMgr->FindUser_CharNo(tax->m_charNo);
                if (payUser == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x84f,"./log/Except",
                        "[MEMBER] CPacketTranslater::OnPayTaxToUpper : pclPayUser == 0!\tchar "
                        "id(%d)\tmoney(%d)\tfatigue(%d)",
                        tax->m_charNo, tax->m_money,
                        tax->m_fatigue);
                }
                else
                {
                    int lowerCnt = upperUser->GetConnLowerMemberCnt();
                    short level = upperUser->GetLevel();
                    unsigned int limit =
                        m_pclApp->Get_MemberManager()->GetLowerMemberEnterLimit(
                            (unsigned int)level);
                    float rate = (float)(0.01 * (double)payUser->GetUpperMemberExpLevel()) +
                                 (float)(0.05 * ((double)lowerCnt / (double)limit));
                    int moneyTax =
                        (int)((double)tax->m_money * (double)rate);
                    float rate2 = (float)(0.01 * (double)payUser->GetUpperMemberExpLevel()) +
                                  (float)(0.05 * ((double)lowerCnt / (double)limit));
                    int fatigueTax =
                        (int)((double)tax->m_fatigue * (double)rate2);
                    if (moneyTax != 0 || fatigueTax != 0)
                    {
                        Packet_Monitor_Member_Pay_Tax_ToUpper reply;
                        reply.m_idByChannel = upperUser->GetIdByChannel();
                        reply.m_uniqCharNo = upperUser->GetUniqCharNo();
                        reply.m_money = moneyTax;
                        reply.m_fatigue = fatigueTax;
                        memcpy(reply.m_name, payUser->GetCharName(), 0x1d);
                        upperUser->SendToGameserver((char*)&reply, 0x38);
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnPayTaxToUpper() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x880,"./log/Except", "CPacketTranslater::OnPayTaxToUpper() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnPayTaxToUpper() Exception Break");
        DNF_LOG_SCOPE_LINE(0x886,"./log/Except", "CPacketTranslater::OnPayTaxToUpper() Exception Break\n");
    }
}

void CPacketTranslater::OnUpdateChangableCharInfo(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnUpdateChangableCharInfo : 0 == m_pclApp");
    }
    CUser* user;
    Packet_Monitor_Char_Changable_Info* p = (Packet_Monitor_Char_Changable_Info*)pkt;
    if ((user = (&m_pclApp->m_userManager)
                    ->FindUser(p->m_dbid)) != 0)
    {
        user->SetUserChangableInfo(p->m_level, p->m_flag);
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x7f2, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeMemberChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x7f8, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsg() Exception Break\n");
    }
}

void CPacketTranslater::OnLogoutComplete(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnLogoutComplete : 0 == m_pclApp");
    }
    CUserManager* userMgr = &m_pclApp->m_userManager;
    Packet_Monitor_UDP_Logout_Complete* complete =
        (Packet_Monitor_UDP_Logout_Complete*)pkt;
    char ok = userMgr->DeleteProhibitUser(complete->m_dbid,
                                          (char)complete->m_channel);
    if (ok == 1)
    {
        char* dbid = NumberToString(complete->m_dbid, 0);
        DNF_LOG_SCOPE_LINE(0x8a5,"./log/User", "[LOGOUT COMPLETE] m_id : %s\tchannel No : %d\n", dbid,
            (unsigned int)(unsigned char)complete->m_channel);
    }
    else
    {
        char* dbid = NumberToString(complete->m_dbid, 0);
        DNF_LOG_SCOPE_LINE(0x8a1,"./log/User",
            "[DELETE_ERR_] CPacketTranslater::OnLogoutComplete m_id : %s\tChannel No : %d\n",
            dbid, (unsigned int)(unsigned char)complete->m_channel);
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnLogoutComplete() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x8aa, "./log/Except", "CPacketTranslater::OnLogoutComplete() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnLogoutComplete() Exception Break");
        DNF_LOG_SCOPE_LINE(0x8b0, "./log/Except", "CPacketTranslater::OnLogoutComplete() Exception Break\n");
    }
}

void CPacketTranslater::OnUserRepel(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnUserRepel : 0 == m_pclApp");
    }
    CUser* user;
    Packet_Monitor_User_Repel* pkt2 = (Packet_Monitor_User_Repel*)pkt;
    DNF_LOG_SCOPE_LINE(0x954,"./log/Web", "CPacketTranslater::OnUserRepel m_id(%s) , charNo(%d)\n",
        NumberToString(pkt2->m_idByChannel, 0), pkt2->m_charNo);
    CUserManager* userMgr = &m_pclApp->m_userManager;
    if ((user = userMgr->FindUser(pkt2->m_idByChannel)) == 0) goto onend;
    if (pkt2->m_charNo != 0)
    {
        if ((user = userMgr->FindUser_CharNo(pkt2->m_charNo)) == 0) goto onend;
    }
    pkt2->m_idByChannel = user->GetIdByChannel();
    user->SendToGameserver((char*)pkt2, 0x12);
onend:
    ;


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnUserRepel Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x969, "./log/Except", "CPacketTranslater::OnUserRepel Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnUserRepel Exception Break");
        DNF_LOG_SCOPE_LINE(0x96f, "./log/Except", "CPacketTranslater::OnUserRepel Exception Break\n");
    }
}

void CPacketTranslater::OnCharacterDelete(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnCharacterDelete : 0 == m_pclApp");
    }
    CServerHandler* handler = m_pclApp->m_serverHandler2;
    if (handler != 0)
    {
        Packet_Monitor_Charac_Delete* del = (Packet_Monitor_Charac_Delete*)pkt;
        (&m_pclApp->m_userManager)
            ->DeleteBlackUserOnCharacDelete(del->m_charNo);
        Packet_DB_Member_Delete_As_Charac_Delete dbPkt;
        dbPkt.m_charNo = del->m_charNo;
        handler->SendToDB(&dbPkt);
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnCharacterDelete Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x9d8, "./log/Except", "CPacketTranslater::OnCharacterDelete Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnCharacterDelete Exception Break");
        DNF_LOG_SCOPE_LINE(0x9de, "./log/Except", "CPacketTranslater::OnCharacterDelete Exception Break\n");
    }
}

void CPacketTranslater::OnEventStart(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        }
        Packet_Monitor_Event_Start* rpkt = (Packet_Monitor_Event_Start*)pkt;
        DNF_LOG_SCOPE_LINE(0x9f4,"./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), "
            "eventParam2(%d)\n",
            (unsigned int)rpkt->m_eventCode,
            (unsigned int)rpkt->m_eventParam1,
            (unsigned int)rpkt->m_eventParam2);
        m_pclApp->m_eventActionMgr
            ->OnStartAction(rpkt);
        (m_pclApp->m_serverHandler2)
            ->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0xa3b, "./log/Except", "CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd");
        DNF_LOG_SCOPE_LINE(0xa41, "./log/Except", "CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd\n");
    }
}

void CPacketTranslater::OnEventEnd(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnEventEnd : 0 == m_pclApp");
        }
        Packet_Monitor_Event_End* rpkt = (Packet_Monitor_Event_End*)pkt;
        DNF_LOG_SCOPE_LINE(0xa66, "./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
            rpkt->m_eventCode);
        m_pclApp->m_eventActionMgr->OnEndAction(rpkt->m_eventCode);
        m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0xa73, "./log/Except", "CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd");
        DNF_LOG_SCOPE_LINE(0xa79, "./log/Except", "CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc\x20\xb9\xdf\xbb\xfd\n");
    }
}

void CPacketTranslater::OnNotifyNewMail(PacketHeader* pkt)
{
    try
    {
        Packet_Monitor_Notify_New_Mail* pkt2 = (Packet_Monitor_Notify_New_Mail*)pkt;
        CUser* user;
        CUserManager* mgr = &m_pclApp->m_userManager;
        if ((user = mgr->FindUser_CharNo(pkt2->m_charNo)) != 0)
        {
            pkt2->m_idByChannel = user->GetIdByChannel();
            user->SendToGameserver((char*)pkt2, 0x12);
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb66, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnWebQueryUserState(PacketHeader* pkt)
{try
{


    CUserManager* userMgr = &m_pclApp->m_userManager;
    Packet_Web_Query_User_State* q = (Packet_Web_Query_User_State*)pkt;
    if (userMgr->FindUser(q->m_dbid) == 0)
    {
        q->m_online = 0;
    }
    else
    {
        q->m_online = 1;
    }
    const char* state = q->m_online == 1 ? "true" : "false";
    char* dbid = NumberToString(q->m_dbid, 0);
    DNF_LOG_SCOPE_LINE(0xb78, "./log/User", "WebQueryUserState Result[m_id: %s] : [%s] : %d\n", dbid, state);
    unsigned int addr = q->m_connNo;
    unsigned short port = q->reversed1;
    if (m_pclApp->Get_UdpHandler()->SendToClient((char*)pkt, 0x13, port, (char*)0, addr) != 1)
    {
        throw CDNFException(strerror(errno));
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xb7f, "./log/Except", "%s Exception Break : %s\n", __FUNCTION__, e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb84, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnNoticeMessage(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeMessage : 0 == m_pclApp");
    }
    Packet_Monitor_Notice_Message* lpkt = (Packet_Monitor_Notice_Message*)pkt;
    DNF_LOG_SCOPE_LINE(0xf8e, "./log/GM_msg", "CPacketTranslater::OnNoticeMessage()%s\n", lpkt->m_text);
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)lpkt, 0x10b);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xf95, "./log/Except", "CPacketTranslater::OnNoticeMessage() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf9a, "./log/Except", "CPacketTranslater::OnNoticeMessage() Exception Break\n");
    }
}

void CPacketTranslater::OnRelayServerUserCheck(PacketHeader* pkt)
{try
{


    CUserManager* userMgr = &m_pclApp->m_userManager;
    CUser* user =
        userMgr->FindUser(((Packet_Relay_Server_User_Check*)pkt)->m_dbid);
    unsigned int channel =
        (unsigned int)(unsigned char)((Packet_Relay_Server_User_Check*)pkt)->m_channel;
    if (user != 0)
    {
        ((Packet_Relay_Server_User_Check*)pkt)->m_channel = 1;
    }
    else
    {
        ((Packet_Relay_Server_User_Check*)pkt)->m_channel = 0;
    }
    CServerInterface* gs =
        (CServerInterface*)m_pclApp->FindGameServer((int)channel);
    if (gs == 0)
    {
        throw CDNFException(strerror(errno));
    }
    gs->SendToServer((char*)pkt, 0xf);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xfc8, "./log/Except", "%s Exception Break : %s\n", __FUNCTION__, e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xfcd, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnForbidChat(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnForbidChat : 0 == m_pclApp");
    }
    Packet_Forbid_Chat_By_Monitor* pkt2 = (Packet_Forbid_Chat_By_Monitor*)pkt;
    DNF_LOG_SCOPE_LINE(0xfdd,"./log/GM_msg", "CPacketTranslater::OnForbidChat() %s for %d secs\n", pkt2->m_name,
        pkt2->m_secs);
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt2, 0x30);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xfe3, "./log/Except", "CPacketTranslater::OnForbidChat() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xfe8, "./log/Except", "CPacketTranslater::OnForbidChat() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeProhibitConnectUser(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException(
                "CPacketTranslater::OnNoticeProhibitConnectUser : 0 == m_pclApp");
        }
        CUserManager* userMgr = &m_pclApp->m_userManager;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        Packet_Web_Prohibit_User_Connect* pu = (Packet_Web_Prohibit_User_Connect*)pkt;
        unsigned int dbid = (unsigned int)pu->m_fieldA;
        exchange_server::CACHE_CHARACTER_TYPE cacheType;
        if (exchange_server::GetInstanceCacheCharacterMgr()->GetCacheCharacter(dbid,
                                                                               &cacheType) != 0)
        {
            char* s = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_LINE(0x8cc,"./log/ExchangeServer", "OnNoticeProhibitConnectUser() (%s,%d,%d)\n", s,
                cacheType.m_field0, cacheType.m_field4);
        }
        bool notPresent =
            userMgr->FindUser(dbid) == 0 && userMgr->FindProhibitUser(dbid) == 0;
        pu->m_field11 = notPresent ? 0 : 1;
        if (pu->m_fieldE == 0)
        {
            CDNFProhibitUser* p = userMgr->FindProhibitUser(dbid);
            if (p == 0)
            {
                p = new CDNFProhibitUser;
                p->SetUserConnectableTime(dbid, (short)pu->m_fieldF, -1, true);
                if (userMgr->InsertProhibitUser(dbid, p) != 1)
                {
                    char* s = NumberToString(dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x922,"./log/ProhibitUser",
                        "[INSERT_ERR] CPacketTranslater::OnNoticeProhibitConnectUser m_id : "
                        "%s, flag( %d ), time( %d ) \n",
                        s, (int)(char)pu->m_fieldE,
                        (int)pu->m_fieldF);
                    delete p;
                }
                char* s = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x926,"./log/ProhibitUser",
                    "[INSERT_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser "
                    "m_id : %s, flag( %d ), time( %d ) \n",
                    s, (int)(char)pu->m_fieldE,
                    (int)pu->m_fieldF);
            }
            else
            {
                if (p->GetChannelNo() == -1)
                {
                    pu->m_fieldE = 2;
                    pu->packetId = 0x4c9;
                    pu->m_field12 = (char)m_pclApp->Get_ServerGroup();
                    handler->GetTcpManagerServer()->SendTcpPacket(pkt);
                    char* s = NumberToString(dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x90a,"./log/ProhibitUser",
                        "[ALREADY_INSERT] CPacketTranslater::OnNoticeProhibitConnectUser m_id "
                        ": %s, flag( %d ), time( %d ) \n",
                        s, (int)(char)pu->m_fieldE,
                        (int)pu->m_fieldF);
                    return;
                }
                char* s = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x90e,"./log/ProhibitUser",
                    "[ALREADY_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser "
                    "m_id : %s, flag( %d ), time( %d ) \n",
                    s, (int)(char)pu->m_fieldE,
                    (int)pu->m_fieldF);
            }
            pu->packetId = 0x4c9;
            pu->m_field12 = (char)m_pclApp->Get_ServerGroup();
            handler->GetTcpManagerServer()->SendTcpPacket(pkt);
        }
        else
        {
            if (userMgr->DeleteProhibitUser(dbid, -1) != 1)
            {
                char* s = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x8ef,"./log/ProhibitUser",
                    "[DELETE_ERR] CPacketTranslater::OnNoticeProhibitConnectUser m_id : %s, "
                    "flag( %d ), time( %d ) \n",
                    s, (int)(char)pu->m_fieldE,
                    (int)pu->m_fieldF);
            }
            char* s = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_LINE(0x8f2,"./log/ProhibitUser",
                "[DELETE_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser m_id : "
                "%s, flag( %d ), time( %d ) \n",
                s, (int)(char)pu->m_fieldE,
                (int)pu->m_fieldF);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x939,"./log/Except",
            "CPacketTranslater::OnNoticeProhibitConnectUser Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x93e, "./log/Except",
            "CPacketTranslater::OnNoticeProhibitConnectUser Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorManagerConnectOK(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnMonitorManagerConnectOK : 0 == m_pclApp");
        }
        m_pclApp->m_serverHandler2->SetManagerConnectFlag(true);
        DNF_LOG_SCOPE_LINE(0xc19, "./log/Manager", "Manager Server Connect Success");
        puts("** Manager Server Connect Success **");
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xc1e,"./log/Except",
            "CPacketTranslater::OnMonitorManagerConnectOK() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xc23, "./log/Except",
            "CPacketTranslater::OnMonitorManagerConnectOK() Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorMegaPhoneMsg(PacketHeader* pkt)
{
    MonitorMegaPhonePkt* pkt2 = (MonitorMegaPhonePkt*)pkt;
    pkt2->m_sg = m_pclApp->Get_ServerGroup();
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, pkt2->m_sz);
}

void CPacketTranslater::OnRegisterGM_mid(PacketHeader* pkt)
{
    Packet_Register_GM_MID* p = (Packet_Register_GM_MID*)pkt;
    if (m_pclApp != 0)
    {
        m_pclApp->Add_GM_id(p->m_gmId);
    }
}

void CPacketTranslater::OnRegisterToBlackList(PacketHeader* pkt)
{try
{


    Packet_Register_To_BlackList* req = (Packet_Register_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xc41, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == m_pclApp");
    }
    else
    {
        Packet_DBMW_Register_To_BlackList dbPkt;
        Packet_Register_To_BlackList_RESULT result;
        dbPkt.m_charNo = req->m_dbid;
        memcpy(dbPkt.m_name, req->m_name, 0x1d);
        CUser* user = (&m_pclApp->m_userManager)->FindUser(req->m_dbid);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc50, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == pclUser");
        }
        else if (strcmp(user->GetCharName(), req->m_name) == 0)
        {
            result.m_result = 3;
            result.m_idByChannel = user->GetIdByChannel();
            result.m_charNo = 0xffffffff;
            memcpy(result.m_name, req->m_name, 0x1d);
            user->SendToGameserver((char*)&result, result.packetSize);
        }
        else
        {
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, req->m_name, 0x1d);
            if (user->GetBlackListSize() < 10)
            {
                CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(req->m_name);
                if (target != 0)
                {
                    result.m_charNo = target->GetUniqCharNo();
                    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
                    if (gm != 0 && gm->isGM(target->GetDBID()) != 0)
                    {
                        result.m_result = 5;
                        user->SendToGameserver((char*)&result,
                                               result.packetSize);
                        return;
                    }
                    if (user->RegisterToBlackList(target->GetUniqCharNo(),
                                                  req->m_name) != 1)
                    {
                        result.m_result = 2;
                        user->SendToGameserver((char*)&result,
                                               result.packetSize);
                        return;
                    }
                    dbPkt.m_charNo2 = target->GetUniqCharNo();
                    result.m_result = 1;
                    user->SendToGameserver((char*)&result,
                                           result.packetSize);
                }
                m_pclApp->m_serverHandler2->SendToDB(&dbPkt);
            }
            else
            {
                result.m_result = 4;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xc8c, "./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xc91, "./log/Except", "CPacketTranslater::OnRegisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDeleteToBlackList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xca6, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == m_pclApp");
        return;
    }
    Packet_DMBW_Delete_To_BlackList dbPkt;
    Packet_Delete_To_BlackList_Result result;
    dbPkt.m_charNo = ((Packet_Delete_To_BlackList*)pkt)->m_dbid;
    memcpy(dbPkt.m_name, ((Packet_Delete_To_BlackList*)pkt)->m_name, 0x1d);
    CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(
        ((Packet_Delete_To_BlackList*)pkt)->m_name);
    if (target != 0)
    {
        CUser* user = (&m_pclApp->m_userManager)->FindUser(
            ((Packet_Delete_To_BlackList*)pkt)->m_dbid);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xcb6, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == pclUser");
            return;
        }
        result.m_idByChannel = user->GetIdByChannel();
        result.m_charNo = target->GetUniqCharNo();
        memcpy(result.m_name, ((Packet_Delete_To_BlackList*)pkt)->m_name, 0x1d);
        if (user->DeleteToBlackList(target->GetUniqCharNo()) != 1)
        {
            result.m_result = 2;
            user->SendToGameserver((char*)&result, result.packetSize);
            return;
        }
        dbPkt.m_charNo2 = target->GetUniqCharNo();
        result.m_result = 1;
        user->SendToGameserver((char*)&result, result.packetSize);
    }
    m_pclApp->m_serverHandler2->SendToDB(&dbPkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xcd0, "./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xcd5, "./log/Except", "CPacketTranslater::OnDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnRequestBlackList(PacketHeader* pkt)
{try
{
    CUser* user;
    Packet_Request_BlackList* p = (Packet_Request_BlackList*)pkt;


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xce9, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == m_pclApp");
    }
    else
    {
        Packet_Request_Result_BlackList reply;
        reply.m_idByChannel = p->m_idByChannel;
        if ((user = (&m_pclApp->m_userManager)
                        ->FindUser(p->m_dbid)) == 0)
        {
            DNF_LOG_SCOPE_LINE(0xcf3, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == pclUser");
        }
        else
        {
            reply.m_count = 0;
            user->GetBlackList(reply.m_count, reply.m_blackList);
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xcff, "./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xd04, "./log/Except", "CPacketTranslater::OnRequestBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWResisterToBlackList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd18, "./log/BlackList", "CPacketTranslater::OnDBMWResisterToBlackList : 0 == m_pclApp");
    }
    else
    {
        Packet_DBMW_Register_To_BlackList* db = (Packet_DBMW_Register_To_BlackList*)pkt;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(db->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xd1f, "./log/BlackList", "CPacketTranslater::OnDBMWResisterToBlackList : 0 == pclUser");
        }
        else
        {
            Packet_Register_To_BlackList_RESULT result;
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, db->m_name, 0x1d);
            if (db->m_result == 0x5a)
            {
                result.m_result = 5;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
            else if ((int)db->m_charNo2 == -1)
            {
                result.m_result = 3;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
            else if (user->IsBlackUser(db->m_charNo2) != 0)
            {
                result.m_result = 2;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
            else if (user->GetBlackListSize() < 10)
            {
                user->RegisterToBlackList(db->m_charNo2, db->m_name);
                result.m_result = 1;
                result.m_charNo = db->m_charNo2;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
            else
            {
                result.m_result = 4;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xd50, "./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xd55, "./log/Except", "CPacketTranslater::OnDBMWResisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnDBMWDeleteToBlackList(PacketHeader* pkt)
{try
{


    Packet_DMBW_Delete_To_BlackList* db = (Packet_DMBW_Delete_To_BlackList*)pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd69, "./log/BlackList", "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == m_pclApp");
    }
    else
    {
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(db->m_charNo);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xd70, "./log/BlackList", "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == pclUser");
        }
        else
        {
            Packet_Delete_To_BlackList_Result result;
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, db->m_name, 0x1d);
            if ((int)db->m_charNo2 == -1)
            {
                result.m_result = 3;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
            else
            {
                if (user->DeleteToBlackList(db->m_charNo2) != 1)
                {
                    result.m_result = 2;
                    user->SendToGameserver((char*)&result, result.packetSize);
                }
                result.m_result = 1;
                result.m_charNo = db->m_charNo2;
                user->SendToGameserver((char*)&result, result.packetSize);
            }
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xd8d, "./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xd92, "./log/Except", "CPacketTranslater::OnDBMWDeleteToBlackList Exception Break\n");
    }
}

void CPacketTranslater::RequestBlackListToDBMW(unsigned int charNo)
{try
{


    Packet_DBMW_Request_BlackList pkt;
    pkt.m_charNo = charNo;
    pkt.m_flag = 0xc9;
    m_pclApp->m_serverHandler2->SendToDB(&pkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xdab, "./log/Except", "CPacketTranslater::RequestBlackListToDBMW Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xdb0, "./log/Except", "CPacketTranslater::RequestBlackListToDBMW Exception Break\n");
    }
}

void CPacketTranslater::SendColletItemsReward(unsigned int charNo, int itemId,
                                              const char* itemName, int nameLen,
                                              TimeGateRewardType::T type)
{
    CUser* user;
    if ((user = (&m_pclApp->m_userManager)->FindUser(charNo)) != 0)
    {
        Packet_CollectItemsReward pkt;
        pkt.m_idByChannel = user->GetIdByChannel();
        pkt.m_charNo = (unsigned int)itemId;
        pkt.m_type = (unsigned char)type;
        pkt.m_nameLen = (unsigned char)nameLen;
        strncpy(pkt.m_name, itemName, (unsigned int)nameLen);
        user->SendTcpGameserver(&pkt);
        Packet_CollectItemsRewardBroadcast bpkt;
        bpkt.m_charNo = (unsigned int)itemId;
        bpkt.m_type = (unsigned char)type;
        bpkt.m_nameLen = (unsigned char)nameLen;
        strncpy(bpkt.m_name, itemName, (unsigned int)nameLen);
        m_pclApp->m_serverHandler2->SendAllTcpGameServer(&bpkt);
    }
}

void CPacketTranslater::OnDBMWResponseBlackListOnLogin(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xdc4,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == m_pclApp");
    }
    else
    {
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(
                ((Packet_Register_To_BlackList*)pkt)->m_dbid);
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xdcb,"./log/BlackList",
                "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == pclUser");
        }
        else
        {
            MonitorResponseBlackListPkt* pb = (MonitorResponseBlackListPkt*)pkt;
            for (int i = 0; pb->m_items[i].m_uniqCharNo != 0; i++)
            {
                if (user->IsBlackUser(pb->m_items[i].m_charNo) != 1)
                {
                    user->RegisterToBlackList(
                        pb->m_items[i].m_charNo,
                        pb->m_items[i].m_name,
                        pb->m_items[i].m_uniqCharNo);
                }
            }
            user->SetBlackListDBFlag(4);
            user->SendBlackList();
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xde0, "./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xde5, "./log/Except", "CPacketTranslater::OnDBMWResponseBlackListOnLogin Exception Break\n");
    }
}

void CPacketTranslater::OnExchangeServerInfo(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0xe59,"./log/ExchangeServer",
                "CPacketTranslater::OnExchangeServerInfo : 0 == m_pclApp");
        }
        else
        {
            Packet_Exchange_Server_Info* reply = (Packet_Exchange_Server_Info*)pkt;
            unsigned int channel = reply->m_channel;
            bool result = false;
            GetInstanceExchangeServer()->SetExchageServer(
                reply->m_ip, reply->m_port, (int)reply->m_channel, result);
            if (!result)
            {
                reply->m_port =
                    GetInstanceExchangeServer()->GetExchangeServerPort();
                reply->m_ip =
                    GetInstanceExchangeServer()->GetExchangeServerIp();
                reply->m_channel =
                    GetInstanceExchangeServer()->GetExchangeServerChannelNo();
                m_pclApp->Get_ServerHandler()->SendToGameServer((unsigned char)channel, reply);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xe71,"./log/Except",
            "CPacketTranslater::OnExchangeServerInfo Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xe76, "./log/Except", "CPacketTranslater::OnExchangeServerInfo Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeCharLiveOnTenMin(PacketHeader* pkt)
{
    try
    {
        CUser* user = (&m_pclApp->m_userManager)->FindUser_CharNo(((Packet_Monitor_Notice_Charac_Live_On_Ten_Min*)pkt)->m_charNo);
        if (user != 0)
        {
            CMember* member = user->GetMember();
            if (member != 0 && m_pclApp->m_serverHandler2 != 0)
            {
                if ((&m_pclApp->m_memberManager)->FindMemberUser(
                        (unsigned int)member->GetUpperMember_CharId()) != 0)
                {
                    unsigned int maxExp =
                        (&m_pclApp->m_memberManager)->GetMemberExpTable()->GetMaxMemberExp();
                    unsigned int newExp =
                        (unsigned int)member->IncConnUpperMemberExp(maxExp);
                    if (newExp != 0)
                    {
                        unsigned int upperCharId =
                            (unsigned int)member->GetUpperMember_CharId();
                        CUser* upperUser = (&m_pclApp->m_userManager)->FindUser_CharNo(upperCharId);
                        if (upperUser != 0)
                        {
                            CMember* upperMember = upperUser->GetMember();
                            if (upperMember != 0)
                            {
                                unsigned int max2 =
                                    (&m_pclApp->m_memberManager)->GetMemberExpTable()->GetMaxMemberExp();
                                unsigned int memberKey =
                                    (unsigned int)member->GetMemberKey();
                                upperMember->IncConnLowerMemberExp(memberKey, max2);
                            }
                        }
                        unsigned int upperId =
                            (unsigned int)member->GetUpperMember_CharId();
                        unsigned int key = (unsigned int)member->GetMemberKey();
                        (&m_pclApp->m_memberManager)->SaveMemberExp(m_pclApp->m_serverHandler2, key, upperId, newExp);
                        if ((&m_pclApp->m_memberManager)->IsMemberExpLevelUp(newExp))
                        {
                            unsigned int u = (unsigned int)member->GetUpperMember_CharId();
                            (&m_pclApp->m_memberManager)->NoticeLevelUpToLowers(u, newExp);
                        }
                    }
                }
                int idx = 0;
                unsigned int lowerCount = member->GetLowerMemberCount();
                unsigned int* proxy = member->GetLowerMember_Proxy();
                while (lowerCount != 0)
                {
                    lowerCount--;
                    CMember* lowerMember = (&m_pclApp->m_memberManager)->FindMember(*proxy);
                    if (lowerMember != 0)
                    {
                        unsigned int maxE =
                            (&m_pclApp->m_memberManager)->GetMemberExpTable()->GetMaxMemberExp();
                        unsigned int lowerNew =
                            (unsigned int)lowerMember->IncConnUpperMemberExp(maxE);
                        if (lowerNew != 0)
                        {
                            unsigned int maxE2 =
                                (&m_pclApp->m_memberManager)->GetMemberExpTable()->GetMaxMemberExp();
                            member->IncConnLowerMemberExp(idx, *proxy, maxE2);
                            unsigned int lowerUpperId =
                                (unsigned int)lowerMember->GetUpperMember_CharId();
                            unsigned int lowerKey =
                                (unsigned int)lowerMember->GetMemberKey();
                            (&m_pclApp->m_memberManager)->SaveMemberExp(m_pclApp->m_serverHandler2, lowerKey, lowerUpperId,
                                                     lowerNew);
                            if ((&m_pclApp->m_memberManager)->IsMemberExpLevelUp(lowerNew))
                            {
                                unsigned int lu =
                                    (unsigned int)lowerMember->GetUpperMember_CharId();
                                (&m_pclApp->m_memberManager)->NoticeLevelUpToLowers(lu, lowerNew);
                            }
                        }
                    }
                    idx++;
                    proxy = (unsigned int*)((char*)proxy + 0x27);
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xf29,"./log/Except",
            "CPacketTranslater::OnNoticeCharLiveOn10Min Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf2e,"./log/Except",
            "CPacketTranslater::OnNoticeCharLiveOn10Min Exception Break\n");
    }
}

void CPacketTranslater::OnWebNoticeSingle(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0xf67, "./log/WebNotice", "CPacketTranslater::OnWebNoticeSingle : 0 == m_pclApp");
        }
        else
        {
            Packet_Web_Notice_Single* pkt2 = (Packet_Web_Notice_Single*)pkt;
            m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, pkt2->packetSize);
            DNF_LOG_SCOPE_LINE(0xf6f,"./log/WebNotice", "OnWebNoticeSingle : (%s,%d)\n", pkt2->m_text,
                (unsigned int)(unsigned char)pkt2->m_len);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xf73, "./log/Except", "CPacketTranslater::OnWebNoticeSingle Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xf78, "./log/Except", "CPacketTranslater::OnWebNoticeSingle Exception Break\n");
    }
}

void CPacketTranslater::OnAddBuddy(PacketHeader* pkt)
{
    CUser* user;
    Packet_Monitor_Add_Buddy* p = (Packet_Monitor_Add_Buddy*)pkt;
    CUserManager* userMgr;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0xfff, "./log/buddy", "CPacketTranslater::OnAddBuddy : 0 == m_pclApp");
        }
        else
        {
            userMgr = &m_pclApp->m_userManager;
            if ((user = userMgr->FindUser_CharNo(p->m_charNo)) != 0)
            {
                int r = user->AddBuddyDB(m_pclApp->Get_ServerHandler(), p->m_name);
                if (r != 0)
                {
                    Packet_Monitor_Add_Buddy_Reply reply;
                    reply.m_charNo = p->m_charNo;
                    reply.m_idByChannel = p->m_idByChannel;
                    reply.m_result = (unsigned char)r;
                    user->SendToGameserver((char*)&reply, reply.packetSize);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x1012, "./log/buddy", "CPacketTranslater::OnAddBuddy\t pclUser is NULL");
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1018, "./log/Except",
            "CPacketTranslater::OnRegisterToBlackList Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x101d, "./log/Except",
            "CPacketTranslater::OnRegisterToBlackList Exception Break\n");
    }
}

void CPacketTranslater::OnAddBuddyDBReply(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1032, "./log/buddy", "CPacketTranslater::OnAddBuddyDBReply : 0 == m_pclApp");
        }
        else
        {
            CUserManager* userMgr = &m_pclApp->m_userManager;
            Packet_DBMW_Add_Buddy_Reply* db = (Packet_DBMW_Add_Buddy_Reply*)pkt;
            CUser* user = userMgr->FindUser_CharNo(db->m_charNo);
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x105f, "./log/buddy", "CPacketTranslater::OnAddBuddyDBReply\tpclUser is NULL");
            }
            else
            {
                Packet_Monitor_Add_Buddy_Reply reply;
                reply.m_charNo = db->m_charNo;
                reply.m_idByChannel = user->GetIdByChannel();
                if (db->m_result == 0)
                {
                    user->AddBuddy(db->m_buddy);
                    memcpy(reply.m_name, db->m_buddy.m_name, 0x27);
                    CUser* other =
                        userMgr->FindUser_CharNo(db->m_buddy.m_characNo);
                    if (other == 0)
                    {
                        reply.m_channel = 0xff;
                    }
                    else if (other->GetGameServer() == 0)
                    {
                        reply.m_channel = 0xff;
                    }
                    else
                    {
                        reply.m_channel =
                            ((CServerInterface*)other->GetGameServer())->GetChannelNo();
                    }
                    reply.m_result = (unsigned char)db->m_result;
                    (&m_pclApp->m_buddyMgr)
                        ->addBuddyRegister(db->m_buddy.m_characNo,
                                           user->GetUniqCharNo());
                }
                else
                {
                    reply.m_result = (unsigned char)db->m_result;
                }
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1065, "./log/Except",
            "CPacketTranslater::OnAddBuddyDBReply Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x106a, "./log/Except",
            "CPacketTranslater::OnAddBuddyDBReply Exception Break\n");
    }
}

void CPacketTranslater::OnDelBuddy(PacketHeader* pkt)
{
    CUser* user;
    Packet_Monitor_Del_Buddy* p = (Packet_Monitor_Del_Buddy*)pkt;
    CUserManager* userMgr;
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x107e, "./log/buddy", "CPacketTranslater::OnDelBuddy : 0 == m_pclApp");
        }
        else
        {
            userMgr = &m_pclApp->m_userManager;
            if ((user = userMgr->FindUser_CharNo(p->m_charNo)) != 0)
            {
                int r = user->DelBuddyDB(m_pclApp->Get_ServerHandler(), p->m_name);
                if (r != 0)
                {
                    Packet_Monitor_Del_Buddy_Reply reply;
                    reply.m_charNo = p->m_charNo;
                    reply.m_idByChannel = p->m_idByChannel;
                    memcpy(reply.m_name, p->m_name, 0x1d);
                    reply.m_result = (unsigned char)r;
                    user->SendToGameserver((char*)&reply, reply.packetSize);
                }
            }
            else
            {
                DNF_LOG_SCOPE_LINE(0x1092, "./log/buddy", "CPacketTranslater::OnDelBuddy\t pclUser is NULL");
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1098, "./log/Except",
            "CPacketTranslater::OnDelBuddy Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x109d, "./log/Except",
            "CPacketTranslater::OnDelBuddy Exception Break\n");
    }
}

void CPacketTranslater::OnDelBuddyDBReply(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x10b1, "./log/buddy", "CPacketTranslater::OnDelBuddyDBReply : 0 == m_pclApp");
        }
        else
        {
            CUserManager* userMgr = &m_pclApp->m_userManager;
            Packet_DBMW_Del_Buddy_Reply* db = (Packet_DBMW_Del_Buddy_Reply*)pkt;
            CUser* user = userMgr->FindUser_CharNo(db->m_charNo);
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x10cf, "./log/buddy", "CPacketTranslater::OnDelBuddyDBReply\tpclUser is NULL");
            }
            else
            {
                Packet_Monitor_Del_Buddy_Reply reply;
                reply.m_charNo = db->m_charNo;
                reply.m_idByChannel = user->GetIdByChannel();
                if (db->m_result == 0)
                {
                    user->DelBuddy(db->m_name);
                    memcpy(reply.m_name, db->m_name, 0x1d);
                    reply.m_result = (unsigned char)db->m_result;
                    unsigned int uniq = user->GetUniqCharNo();
                    (&m_pclApp->m_buddyMgr)
                        ->delBuddyRegister(db->m_buddyCharNo, uniq);
                }
                else
                {
                    reply.m_result = 3;
                }
                user->SendToGameserver((char*)&reply, reply.packetSize);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x10d5, "./log/Except",
            "CPacketTranslater::OnDelBuddyDBReply Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x10da, "./log/Except",
            "CPacketTranslater::OnDelBuddyDBReply Exception Break\n");
    }
}

void CPacketTranslater::OnQueryBuddyInfoDBReply(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x10f0, "./log/buddy", "CPacketTranslater::OnQueryBuddyInfoDBReply : 0 == m_pclApp");
        }
        else
        {
            CUserManager* userMgr = &m_pclApp->m_userManager;
            Packet_DBMW_Query_Buddy_Info_Reply* pb =
                (Packet_DBMW_Query_Buddy_Info_Reply*)pkt;
            if (0x20 < pb->m_count)
            {
                pb->m_count = 0x20;
            }
            CUser* user = userMgr->FindUser_CharNo(pb->m_charNo);
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1112,"./log/buddy",
                    "CPacketTranslater::OnQueryBuddyInfoDBReply\t  pclUser is NULL");
            }
            else
            {
                for (int i = 0; i < (int)(unsigned int)pb->m_count; i++)
                {
                    user->AddBuddy(pb->m_items[i]);
                    (&m_pclApp->m_buddyMgr)
                        ->addBuddyRegister(pb->m_items[i].m_characNo,
                                           user->GetUniqCharNo());
                }
                if (pb->m_count != 0)
                {
                    userMgr->SendConnectedBuddysList(user);
                }
                user->SetBuddyDBFlag(4);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1118,"./log/Except",
            "CPacketTranslater::OnQueryBuddyInfoDBReply Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x111d, "./log/Except",
            "CPacketTranslater::OnQueryBuddyInfoDBReply Exception Break\n");
    }
}

void CPacketTranslater::OnWebChangeUserHandicap(PacketHeader* pkt)
{
    PacketHeader* pktLocal = pkt;
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1127, "./log/hack", "CPacketTranslater::OnWebChangeUserHandicap : 0 == m_pclApp");
    }
    else
    {
        CUser* user = 0;
        CUserManager* mgr = &m_pclApp->m_userManager;
        user = mgr->FindUser(((Packet_Change_User_Handicap*)pktLocal)->m_charNo);
        if (user != 0)
        {
            Packet_Change_User_Handicap reply;
            reply.m_charNo = ((Packet_Change_User_Handicap*)pktLocal)->m_charNo;
            reply.m_fieldE = ((Packet_Change_User_Handicap*)pktLocal)->m_fieldE;
            reply.m_field12 = ((Packet_Change_User_Handicap*)pktLocal)->m_field12;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
    }
}

void CPacketTranslater::OnGMRequestMid(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x113c, "./log/Except", "CPacketTranslater::OnGMRequestMid : 0 == m_pclApp");
    }
    else
    {
        Packet_GM_Request_Mid* p = (Packet_GM_Request_Mid*)pkt;
        CUser* user = 0;
        CUser* target = 0;
        CUserManager* userMgr = &m_pclApp->m_userManager;
        user = userMgr->FindUser(p->m_dbid);
        if (user == 0) goto end;
        Packet_GM_Request_Mid reply;
        reply.m_gmId = p->m_gmId;
        memcpy(reply.m_name, p->m_name, 0x1d);
        target = userMgr->FindUser_CharName(p->m_name);
        if (target == 0)
        {
            reply.m_dbid = 0xffffffff;
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
        else
        {
            reply.m_charNo = target->GetUniqCharNo();
            reply.m_dbid = target->GetDBID();
            user->SendToGameserver((char*)&reply, reply.packetSize);
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1161, "./log/Except", "CPacketTranslater::OnQueryBuddyInfoDBReply Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1166, "./log/Except", "CPacketTranslater::OnQueryBuddyInfoDBReply Exception Break\n");
    }
end:
    ;
}

void CPacketTranslater::OnUserRepelByCharName(PacketHeader* pkt)
{try
{


    CUserManager* userMgr = 0;
    Packet_Monitor_User_Repel_ByCharName* repel = 0;
    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnUserRepel : 0 == m_pclApp");
    }
    userMgr = &m_pclApp->m_userManager;
    repel = (Packet_Monitor_User_Repel_ByCharName*)pkt;
    CUser* user = userMgr->FindUser(repel->m_dbid);
    if (user != 0)
    {
        Packet_Monitor_User_Repel reply;
        CUser* target = userMgr->FindUser_CharName(repel->m_name);
        if (target != 0)
        {
            reply.m_idByChannel = target->GetIdByChannel();
            target->SendToGameserver((char*)&reply, 0x12);
        }
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnUserRepel Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x9a2, "./log/Except", "CPacketTranslater::OnUserRepel Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnUserRepel Exception Break");
        DNF_LOG_SCOPE_LINE(0x9a8, "./log/Except", "CPacketTranslater::OnUserRepel Exception Break\n");
    }
}

void CPacketTranslater::onReplyLoadTowerFullRank(PacketHeader* pkt)
{
    try
    {
        Packet_Reply_Load_Tower_Full_Rank* rank =
            (Packet_Reply_Load_Tower_Full_Rank*)pkt;
        if (rank->m_reset != 0)
        {
            ((CTowerRank*)m_pclApp->getTowerRank())->reset();
        }
        DNF_LOG_SCOPE_LINE(0x1172,"./log/DeathTower", "%d/%d\n", rank->m_count,
            rank->m_total);
        for (unsigned int i = 0; i < rank->m_count; i++)
        {
            MonitorTowerFullRankRec* rec = (MonitorTowerFullRankRec*)pkt + i;
            ((CTowerRank*)m_pclApp->getTowerRank())->registRank(
                (unsigned int)(unsigned char)rec->m_field1b,
                (unsigned int)rec->m_field17,
                (unsigned int)rec->m_field19,
                (const stDeathTowerRecordMemberInfo_t*)rec->m_memberInfo);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x117e,"./log/Except",
            "CPacketTranslater::onReplyLoadTowerFullRank Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1183, "./log/Except",
            "CPacketTranslater::onReplyLoadTowerFullRank Exception Break\n");
    }
}

void CPacketTranslater::onRequestCharacTowerUpdateRank(PacketHeader* pkt)
{try
{


    Packet_Request_Charac_Tower_Update_Ranking* req =
        (Packet_Request_Charac_Tower_Update_Ranking*)pkt;
    CUser* user =
        (&m_pclApp->m_userManager)->FindUser_CharNo(req->m_charNo);
    if (user != 0)
    {
        unsigned int a = req->m_score;
        unsigned int b = req->m_job;
        char* name = user->GetCharName();
        unsigned int c = req->m_floor;
        CTowerRank* tower = (CTowerRank*)m_pclApp->getTowerRank();
        tower->registCharacRank(c, name, b, a);
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1197, "./log/Except", "CPacketTranslater::onRequestCharacTowerUpdateRank Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x119c, "./log/Except", "CPacketTranslater::onRequestCharacTowerUpdateRank Exception Break\n");
    }
}

void CPacketTranslater::onRequestReloadTowerRanker(PacketHeader* pkt)
{
    register CServerHandler* handler = m_pclApp->m_serverHandler2;
    try
    {
        ((CTowerRank*)m_pclApp->getTowerRank())->processReloadRanking(handler, true, 5);
        m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 10);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x11aa, "./log/Except", "CPacketTranslater::onRequestReloadTowerRanker Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x11af, "./log/Except", "CPacketTranslater::onRequestReloadTowerRanker Exception Break\n");
    }
}

void CPacketTranslater::onWebReqReloadAutoPunishRule(PacketHeader* pkt)
{try
{


    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 0xb);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x11bc, "./log/Except", "CPacketTranslater::onWebReqReloadAutoPunishRule Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x11c1, "./log/Except", "CPacketTranslater::onWebReqReloadAutoPunishRule Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogin(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x11d2, "./log/Except", "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
        }
        else
        {
            CServerHandler* handler = m_pclApp->Get_ServerHandler();
            if (handler->GetTcpDBServer()->GetSock() == (int)pkt->m_connNo)
            {
                handler = m_pclApp->Get_ServerHandler();
                handler->GetTcpDBServer()->Connected();
            }
            else
            {
                handler = m_pclApp->Get_ServerHandler();
                if (handler->GetTcpManagerServer()->GetSock() == (int)pkt->m_connNo)
                {
                    unsigned char group = m_pclApp->Get_ServerGroup();
                    handler = m_pclApp->Get_ServerHandler();
                    handler->GetTcpManagerServer()->Connected(group);
                }
                else
                {
                    unsigned int sock = pkt->m_connNo;
                    handler = m_pclApp->Get_ServerHandler();
                    CTcpGameServer* tcp = handler->CreateTcpGameServer(sock);
                    if (tcp != 0)
                    {
                        char* buf = tcp->makePacketHeader(8000, 0xc);
                        if (buf != 0)
                        {
                            buf[10] = 0;
                            char* out = buf;
                            out[0xb] = (char)m_pclApp->Get_ServerGroup();
                            tcp->SendToGameServer(out);
                            char* out2 = tcp->makePacketHeader(0x3ea, 0xb);
                            char* out3 = 0;
                            if (out2 != 0)
                            {
                                out3 = out2;
                            }
                            out3[10] = -0x37;
                            tcp->SendToGameServer(out3);
                            void* net = m_pclApp->Get_TcpNetSystem();
                            DNF_LOG_SCOPE_LINE(0x123e, "./log/Tcp", "OnInnerPacketLogin : Network system (%x)", net);
                            char* out4 = tcp->makePacketHeader(0x1004, 0x7ef);
                            if (out4 != 0)
                            {
                                out4[10] = 1;
                                char* out5 = out4;
                                m_pclApp->getItemLimitEditionMgr()
                                    ->makeItemLimitEditionSellStartPacket(
                                        *(Packet_Item_Limit_Edition_Sell_Start*)out5);
                                tcp->SendToGameServer(out5);
                                tcp->makePacketHeader(0x1b6a, 0x12);
                            }
                        }
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x126d,"./log/Except", "CPacketTranslater::OnInnerPacketLogin Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1272, "./log/Except", "CPacketTranslater::OnInnerPacketLogin Exception Break\n");
    }
}

void CPacketTranslater::OnInnerPacketLogout(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1282, "./log/Except", "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
        }
        else
        {
            CServerHandler* handler = m_pclApp->Get_ServerHandler();
            if (handler->GetTcpDBServer()->GetSock() == (int)pkt->m_connNo)
            {
                handler = m_pclApp->Get_ServerHandler();
                handler->GetTcpDBServer()->DisConnected();
            }
            else
            {
                handler = m_pclApp->Get_ServerHandler();
                if (handler->GetTcpManagerServer()->GetSock() == (int)pkt->m_connNo)
                {
                    handler = m_pclApp->Get_ServerHandler();
                    handler->GetTcpManagerServer()->DisConnected();
                }
                else
                {
                    CTcpGameServer* tcp = (CTcpGameServer*)m_pclApp->FindTcpGameServer(
                        pkt->m_connNo);
                    m_pclApp->OnTcpGameServerDown(tcp);
                    unsigned char channel = tcp->GetChannelNo();
                    if (channel != 0)
                    {
                        handler = m_pclApp->Get_ServerHandler();
                        handler->UnregistGameServer((unsigned int)channel);
                    }
                    handler = m_pclApp->Get_ServerHandler();
                    handler->DeleteTcpGameServer(pkt->m_connNo);
                    void* net = m_pclApp->Get_TcpNetSystem();
                    DNF_LOG_SCOPE_LINE(0x12af, "./log/Tcp", "OnInnerPacketLogout : Network system (%x)", net);
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x12b3,"./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x12b8, "./log/Except", "CPacketTranslater::OnInnerPacketLogout Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeSlang(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeSlang : 0 == m_pclApp");
    }
    PacketHeader* lpkt = pkt;
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)lpkt, 0x3d);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x12d1, "./log/Except", "CPacketTranslater::OnNoticeSlang() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x12d6, "./log/Except", "CPacketTranslater::OnNoticeSlang() Exception Break\n");
    }
}

void CPacketTranslater::onLoadCleanPadPoint(PacketHeader* pkt)
{try
{


    DNF_LOG_SCOPE_LINE(0x12e0, "./log/Cleanpad", "CleanPad Point");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 10);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x12e5, "./log/Except", "CPacketTranslater::onLoadCleanPadPoint Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x12ea, "./log/Except", "CPacketTranslater::onLoadCleanPadPoint Exception Break\n");
    }
}

void CPacketTranslater::onLoadBlackIPMonitor(PacketHeader* pkt) {}

void CPacketTranslater::onLoadBlackIPMonitorPartLoad(PacketHeader* pkt)
{try
{


    DNF_LOG_SCOPE_LINE(0x1307, "./log/BlackIP", "BlackIP Monitor Part Load");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 10);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x130c, "./log/Except", "CPacketTranslater::onLoadBlackIPMonitorPartLoad Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1311, "./log/Except", "CPacketTranslater::onLoadBlackIPMonitorPartLoad Exception Break\n");
    }
}

void CPacketTranslater::onLoadBlackIPMonitorDeleteIP(PacketHeader* pkt)
{try
{


    DNF_LOG_SCOPE_LINE(0x131a, "./log/BlackIP", "BlackIP Monitor Delete IP");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 0x19e);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1320, "./log/Except", "CPacketTranslater::onLoadBlackIPMonitorDeleteIP Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1325, "./log/Except", "CPacketTranslater::onLoadBlackIPMonitorDeleteIP Exception Break\n");
    }
}

void CPacketTranslater::OnChangeCharName(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x133d, "./log/Except", "CPacketTranslater::OnChangeGuildName : 0 == m_pclApp");
        }
        else
        {
            Packet_Change_Char_Name* rpkt = (Packet_Change_Char_Name*)pkt;
            Packet_DBMW_Change_Char_Name pkt2;
            pkt2.m_type = (char)m_pclApp->Get_ServerGroup();
            pkt2.m_charNo = rpkt->m_charNo;
            memcpy(pkt2.m_name, rpkt->m_name, 0x1d);
            m_pclApp->Get_ServerHandler()->SendToDB(&pkt2);
            CUserManager* userMgr = m_pclApp->Get_UserManager();
            userMgr->ChangeBlackListCharName(rpkt->m_charNo, rpkt->m_name);
            std::vector<unsigned int> vec;
            (&m_pclApp->m_buddyMgr)
                ->findBuddyRegister(rpkt->m_charNo, vec);
            for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end(); ++it)
            {
                CUser* user = userMgr->FindUser_CharNo(*it);
                if (user != 0)
                {
                    std::string name(rpkt->m_name);
                    user->SetBuddyCharName((int)rpkt->m_charNo, name);
                }
            }
            std::string name2(rpkt->m_name);
            ((CMemoryCashManager*)m_pclApp->Get_MemoryCashManager())->InsertUpdatedCharacName(
                rpkt->m_charNo, name2);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1363,"./log/Except", "CPacketTranslater::OnChangeCharName Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1368, "./log/Except", "CPacketTranslater::OnChangeCharName Exception Break\n");
    }
}

void CPacketTranslater::OnNotifyAuctionMail(PacketHeader* pkt)
{
    try
    {
        MonitorMailPkt* pkt2 = (MonitorMailPkt*)pkt;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser_CharNo(
                pkt2->m_charNo);
        if (user != 0)
        {
            pkt2->m_idByChannel = user->GetIdByChannel();
            user->SendToGameserver((char*)pkt2, 0x26);
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x137e, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnPvPChannelInfo(PacketHeader* pkt)
{
    try
    {
        CUserManager* userMgr = &m_pclApp->m_userManager;
        Packet_PvPChannelInfo* info = (Packet_PvPChannelInfo*)pkt;
        CUser* user = userMgr->FindUser_CharNo(info->m_charNo);
        if (user != 0)
        {
            Packet_PvPChannelUserCount pkt2;
            pkt2.m_charNo = info->m_charNo;
            pkt2.m_fieldE = info->m_fieldE;
            pkt2.m_field12 = info->m_field12;
            unsigned int sg = m_pclApp->Get_ServerGroup();
            (void)sg;
            CServerHandler* handler = m_pclApp->m_serverHandler2;
            int count = handler->SendAllTcpGameServer(
                &pkt2, (int)(unsigned char)info->m_channelCount);
            user->ResetChannelUserCount(count);
            if (count == 0 || (int)info->m_field12 == 0)
            {
                Packet_PvPChannelInfo reply;
                reply.m_charNo = info->m_charNo;
                reply.m_fieldE = info->m_fieldE;
                reply.m_field12 = info->m_field12;
                reply.m_count = 0;
                reply.packetSize =
                    (unsigned short)((reply.m_count << 4) + 0x18);
                user->SendTcpGameserver(&reply);
            }
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x13aa, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnPvPChannelUserCount(PacketHeader* pkt)
{
    try
    {
        CUserManager* userMgr = &m_pclApp->m_userManager;
        Packet_PvPChannelUserCount* cnt = (Packet_PvPChannelUserCount*)pkt;
        CUser* user = userMgr->FindUser_CharNo(cnt->m_charNo);
        if (user != 0)
        {
            user->SetChannelUserCount(cnt->m_countA, cnt->m_countB,
                                      cnt->m_countC, cnt->m_countD);
            if (user->IsCompleteChannelUserCount())
            {
                Packet_PvPChannelInfo reply;
                reply.m_charNo = cnt->m_charNo;
                reply.m_fieldE = cnt->m_fieldE;
                reply.m_field12 = cnt->m_field12;
                reply.m_count = 0xff;
                user->GetChannelUserCount((STPvPChannelInfo*)reply.m_channels, reply.m_count);
                reply.packetSize =
                    (unsigned short)((unsigned int)reply.m_count * 0x10 + 0x18);
                user->SendTcpGameserver(&reply);
            }
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x13d3, "./log/Except", "%s Exception Break\n", __FUNCTION__);
    }
}

void CPacketTranslater::OnChannelType(PacketHeader* pkt)
{
    CGameServer* gs;
    CTcpGameServer* tcpGs;
    Packet_ChannelType* p = (Packet_ChannelType*)pkt;
    if (m_pclApp != 0)
    {
        try
        {
            if ((gs = (CGameServer*)m_pclApp->FindGameServer((int)p->m_channel)) == 0)
            {
                throw CDNFException("CPacketTranslater::OnChannelType : pclGameServer == 0");
            }
            if ((tcpGs = (CTcpGameServer*)m_pclApp->FindTcpGameServer(p->m_connNo)) == 0)
            {
                return;
            }
            tcpGs->SetChannelType(p->m_type);
        }
        catch (...)
        {
            DNF_LOG_SCOPE_LINE(0x13f4, "./log/Except", "%s Exception Break\n", __FUNCTION__);
        }
    }
}

void CPacketTranslater::OnServerMessageInfo(PacketHeader* pkt)
{try
{


    PacketHeader* lpkt = pkt;
    DNF_LOG_SCOPE_LINE(0x1402, "./log/ServerEvent", "Packet_Monitor_Server_Message_Info");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)lpkt, 0x5f);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1408, "./log/Except", "CPacketTranslater::OnServerMessageInfo Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x140d, "./log/Except", "CPacketTranslater::OnServerMessageInfo Exception Break\n");
    }
}

void CPacketTranslater::OnRequestReloadPowerWarRanker(PacketHeader* pkt)
{try
{


    PacketHeader* pkt2 = pkt;
    DNF_LOG_SCOPE_LINE(0x1418, "./log/ServerEvent", "Packet_Request_Reload_Power_War_Ranker");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt2, 10);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x141e, "./log/Except", "CPacketTranslater::OnRequestReloadPowerWarRanker Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1423, "./log/Except", "CPacketTranslater::OnRequestReloadPowerWarRanker Exception Break\n");
    }
}

void CPacketTranslater::onLoadPunishUserReq(PacketHeader* pkt)
{try
{


    DNF_LOG_SCOPE_LINE(0x142d, "./log/Secu", "Punish User Request");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 0x4bd);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1432, "./log/Except", "CPacketTranslater::onLoadPunishUserReq Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1437, "./log/Except", "CPacketTranslater::onLoadPunishUserReq Exception Break\n");
    }
}

void CPacketTranslater::onIPCounterControl(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::onIPCounterControl : 0 == m_pclApp");
    }
    Packet_IPCounter_Control* lpkt = (Packet_IPCounter_Control*)pkt;
    DNF_LOG_SCOPE_LINE(0x1448,"./log/Secu", "IPCounterControl - type : %d, value : %d ",
        (unsigned int)(unsigned char)lpkt->m_type,
        (unsigned int)(unsigned char)lpkt->m_value);
    ((CIPCounter*)m_pclApp->getIPCounter())->setOption(lpkt->m_type, lpkt->m_value);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1452, "./log/Except", "CPacketTranslater::onIPCounterControl Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1457, "./log/Except", "CPacketTranslater::onIPCounterControl Exception Break\n");
    }
}

void CPacketTranslater::onItemLimitEditionLoadDataReq(PacketHeader* pkt)
{
    try
    {
        Packet_Item_Limit_Edition_Load_Data_Req* req =
            (Packet_Item_Limit_Edition_Load_Data_Req*)pkt;
        if (req->m_fullLoad == 0)
        {
            unsigned int grp = req->m_serverType;
            if (grp != ((unsigned int)m_pclApp->Get_ServerGroup() & 0xff))
            {
                unsigned int g = req->m_serverType;
                DNF_LOG_SCOPE_LINE(0x146d, "./log/ItemLimitEdition", "(Ignore another server msg: %d)", g);
                return;
            }
        }
        else
        {
            req->m_serverType =
                (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
        }
        (m_pclApp->m_serverHandler2)->SendToDB(pkt);
        DNF_LOG_SCOPE_LINE(0x1474,"./log/ItemLimitEdition",
            "(FullLoad: %d, ServerType:%d, LoadTargetNum: %d, IPGNO: "
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
            (unsigned int)(unsigned char)req->m_fullLoad,
            req->m_serverType, req->m_loadTargetNum,
            req->m_ipgNo[0], req->m_ipgNo[1],
            req->m_ipgNo[2], req->m_ipgNo[3],
            req->m_ipgNo[4], req->m_ipgNo[5],
            req->m_ipgNo[6], req->m_ipgNo[7],
            req->m_ipgNo[8], req->m_ipgNo[9],
            req->m_ipgNo[10], req->m_ipgNo[11],
            req->m_ipgNo[12], req->m_ipgNo[13],
            req->m_ipgNo[14], req->m_ipgNo[15],
            req->m_ipgNo[16], req->m_ipgNo[17],
            req->m_ipgNo[18], req->m_ipgNo[19],
            req->m_ipgNo[20], req->m_ipgNo[21],
            req->m_ipgNo[22], req->m_ipgNo[23]);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1494,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1499,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break\n");
    }
}

void CPacketTranslater::onItemLimitEditionLoadDataRpy(PacketHeader* pkt)
{
    try
    {
        Packet_Item_Limit_Edition_Load_Data_Rpy* rpy =
            (Packet_Item_Limit_Edition_Load_Data_Rpy*)pkt;
        if ((int)rpy->m_count != 0)
        {
            if (rpy->m_fullLoad != 0)
            {
                m_pclApp->getItemLimitEditionMgr()->clear();
            }
            for (unsigned int i = 0; i < rpy->m_count; i++)
            {
                unsigned int ipgno = rpy->m_items[i].m_ipgno;
                if (ipgno > 799999 && ipgno < 1000000)
                {
                    m_pclApp->getItemLimitEditionMgr()->registItem(
                        *(stItemLimitEditionItemInfo_t*)&rpy->m_items[i]);
                }
            }
            DNF_LOG_SCOPE_LINE(0x14c2,"./log/ItemLimitEdition",
                "(FullLoad: %d, LoadTargetNum: %d, IPGNO: "
                "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                (unsigned int)(unsigned char)rpy->m_fullLoad,
                rpy->m_count, rpy->m_items[0].m_ipgno,
                rpy->m_items[1].m_ipgno, rpy->m_items[2].m_ipgno,
                rpy->m_items[3].m_ipgno, rpy->m_items[4].m_ipgno,
                rpy->m_items[5].m_ipgno, rpy->m_items[6].m_ipgno,
                rpy->m_items[7].m_ipgno, rpy->m_items[8].m_ipgno,
                rpy->m_items[9].m_ipgno, rpy->m_items[10].m_ipgno,
                rpy->m_items[11].m_ipgno, rpy->m_items[12].m_ipgno,
                rpy->m_items[13].m_ipgno, rpy->m_items[14].m_ipgno,
                rpy->m_items[15].m_ipgno, rpy->m_items[16].m_ipgno,
                rpy->m_items[17].m_ipgno, rpy->m_items[18].m_ipgno,
                rpy->m_items[19].m_ipgno, rpy->m_items[20].m_ipgno,
                rpy->m_items[21].m_ipgno, rpy->m_items[22].m_ipgno,
                rpy->m_items[23].m_ipgno);
            if (m_pclApp->getItemLimitEditionMgr()->isEmpty() != 1)
            {
                Packet_Item_Limit_Edition_Sell_Start pkt2;
                pkt2.m_fullLoad = rpy->m_fullLoad;
                if (rpy->m_fullLoad == 0)
                {
                    for (unsigned int i = 0; i < rpy->m_count; i++)
                    {
                        CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()
                                                      ->getItemInfo(
                                                          rpy->m_items[i].m_ipgno);
                        if (item != 0)
                        {
                            item->makeItemInfo(
                                *(stItemLimitEditionItemInfo_t*)&pkt2.m_items[i]);
                            pkt2.m_count = pkt2.m_count + 1;
                        }
                    }
                }
                else
                {
                    m_pclApp->getItemLimitEditionMgr()
                        ->makeItemLimitEditionSellStartPacket(pkt2);
                }
                m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&pkt2);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x14f6,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x14fb,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break\n");
    }
}

void CPacketTranslater::onItemLimitEditionSellEnd(PacketHeader* pkt)
{
    try
    {
        Packet_Item_Limit_Edition_Sell_end* end =
            (Packet_Item_Limit_Edition_Sell_end*)pkt;
        unsigned int stype = end->m_serverType;
        if (stype == ((unsigned int)m_pclApp->Get_ServerGroup() & 0xff))
        {
            for (unsigned int i = 0; i < end->m_sellEndNum; i++)
            {
                CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()->getItemInfo(
                    end->m_ipgNo[i]);
                if (item != 0)
                {
                    m_pclApp->getItemLimitEditionMgr()->removeItem(
                        end->m_ipgNo[i]);
                }
            }
            DNF_LOG_SCOPE_LINE(0x1519,"./log/ItemLimitEdition",
                "(ServerType: %d, SellEndNum: %d, IPGNO: "
                "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                end->m_serverType, end->m_sellEndNum,
                end->m_ipgNo[0], end->m_ipgNo[1],
                end->m_ipgNo[2], end->m_ipgNo[3],
                end->m_ipgNo[4], end->m_ipgNo[5],
                end->m_ipgNo[6], end->m_ipgNo[7],
                end->m_ipgNo[8], end->m_ipgNo[9],
                end->m_ipgNo[10], end->m_ipgNo[11],
                end->m_ipgNo[12], end->m_ipgNo[13],
                end->m_ipgNo[14], end->m_ipgNo[15],
                end->m_ipgNo[16], end->m_ipgNo[17],
                end->m_ipgNo[18], end->m_ipgNo[19],
                end->m_ipgNo[20], end->m_ipgNo[21],
                end->m_ipgNo[22], end->m_ipgNo[23]);
            m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
        }
        else
        {
            unsigned int v = end->m_serverType;
            DNF_LOG_SCOPE_LINE(0x150a, "./log/ItemLimitEdition", "(Ignore another server msg: %d)", v);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x153a,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x153f,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break\n");
    }
}

void CPacketTranslater::onItemLimitEditionBuyableRequest(PacketHeader* pkt)
{
    try
    {
        Packet_Item_Limit_Edition_Buyable_Query* q =
            (Packet_Item_Limit_Edition_Buyable_Query*)pkt;
        CTcpGameServer* tcp = (CTcpGameServer*)m_pclApp->FindTcpGameServer(
            q->m_connNo);
        if (tcp != 0)
        {
            time_t now = time(0);
            char* buf = tcp->makePacketHeader(0x100a, 0x1e1);
            if (buf != 0)
            {
                CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()->getItemInfo(
                    q->m_ipgno);
                bool expired = (item == 0 || (int)item->getSellEndTime() < (int)now);
                if (expired)
                {
                    *(unsigned int*)(buf + 0xa) = q->m_seq;
                    buf[0x16] = 1;
                    tcp->SendToGameServer(buf);
                }
                else
                {
                    *(unsigned int*)(buf + 0xa) = q->m_seq;
                    *(unsigned int*)(buf + 0xe) = q->m_ipgno;
                    if (item->isSellComplete() == 0)
                    {
                        unsigned int num = q->m_ipgno;
                        m_pclApp->getItemLimitEditionMgr()->updateItem(
                            num, item->getSellNum() + 1);
                        buf[0x16] = 0;
                    }
                    else
                    {
                        buf[0x16] = 2;
                    }
                    *(unsigned int*)(buf + 0x12) = item->getSellNum();
                    memcpy(buf + 0x17, q->m_payload, 0x1ca);
                    tcp->SendToGameServer(buf);
                    if (item->isSellComplete())
                    {
                        m_pclApp->getItemLimitEditionMgr()->processScheduledJob(m_pclApp,
                                                                                true);
                    }
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1599,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x159e,"./log/Except",
            "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorFindFactoryHubUser(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnMonitorFindFactoryHubUser : 0 == m_pclApp");
    }
    CUserManager* userMgr = &m_pclApp->m_userManager;
    Packet_Monitor_Find_Factory_Hub_User* find =
        (Packet_Monitor_Find_Factory_Hub_User*)pkt;
    int targetCharNo = 0;
    if (find->m_nameLen != 0 &&
        (unsigned char)find->m_nameLen < 0x1e)
    {
        CUser* target = userMgr->FindUser_CharName(find->m_name);
        targetCharNo = target != 0 ? (int)target->GetUniqCharNo() : -1;
    }
    if (targetCharNo != 0)
    {
        CUser* userA = userMgr->FindUser_CharNo(find->m_charNo);
        if (userA != 0)
        {
            Packet_Notice_Find_Factory_Hub_User reply;
            CUser* userB = userMgr->FindUser_CharNo((unsigned int)targetCharNo);
            if (userB == 0)
            {
                reply.m_idByChannel = userA->GetIdByChannel();
                reply.m_found = 0;
                reply.m_nameLen = find->m_nameLen;
                strncpy(reply.m_name, find->m_name,
                        (unsigned int)(unsigned char)find->m_nameLen);
                reply.m_field2e = find->m_field2e;
                reply.m_field30 = find->m_field30;
                reply.packetSize = 0x34;
                userA->SendToGameserver((char*)&reply, 0x34);
            }
            else
            {
                reply.m_idByChannel = userB->GetIdByChannel();
                reply.m_found = 1;
                reply.m_nameLen = 0;
                reply.m_field2e = find->m_field2e;
                reply.m_field30 = find->m_field30;
                reply.packetSize = 0x34;
                userB->SendToGameserver((char*)&reply, 0x34);
            }
        }
        return;
    }
    throw CDNFException("CPacketTranslater::OnMonitorFindFactoryHubUser");


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1629, "./log/Except", "CPacketTranslater::OnMonitorFindFactoryHubUser() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x162f, "./log/Except", "CPacketTranslater::OnMonitorFindFactoryHubUser() Exception Break\n");
    }
}

void CPacketTranslater::OnSetCleanPadPoint(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnSetCleanPadPoint : 0 == m_pclApp");
        }
        int charNo = 0;
        Packet_Request_Set_CleanPad_Point* req = (Packet_Request_Set_CleanPad_Point*)pkt;
        CUser* user = (&m_pclApp->m_userManager)->FindUser(req->m_dbid);
        if (user != 0)
        {
            charNo = (int)user->GetUniqCharNo();
        }
        else
        {
            charNo = -1;
        }
        if (charNo == 0)
        {
            throw CDNFException("CPacketTranslater::OnSetCleanPadPoint");
        }
        CUser* target = 0;
        Packet_Set_CleanPad_Point reply;
        target = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)charNo);
        if (target != 0)
        {
            reply.m_idByChannel = target->GetIdByChannel();
            reply.m_point = req->m_point;
            reply.packetSize = 0x10;
            target->SendToGameserver((char*)&reply, reply.packetSize);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1660, "./log/Except",
            "CPacketTranslater::OnSetCleanPadPoint() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1666, "./log/Except",
            "CPacketTranslater::OnSetCleanPadPoint() Exception Break\n");
    }
}

void CPacketTranslater::OnResponseIPCounterList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnResponseIPCounterList : 0 == m_pclApp");
    }
    Packet_IP_Counter_List* pkt2 = (Packet_IP_Counter_List*)pkt;
    DNF_LOG_SCOPE_LINE(0x16c8,"./log/Secu", "[IP Counter] DataStats : %d, DataSize : %d ",
        (unsigned int)(unsigned char)pkt2->m_dataStats,
        (unsigned int)(unsigned char)pkt2->m_dataSize);
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt2, pkt2->packetSize);


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnResponseIPCounterList() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x16d1, "./log/Except", "CPacketTranslater::OnResponseIPCounterList() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnResponseIPCounterList() Exception Break");
        DNF_LOG_SCOPE_LINE(0x16d7, "./log/Except", "CPacketTranslater::OnResponseIPCounterList() Exception Break\n");
    }
}

void CPacketTranslater::OnResponseFullIPCounterList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnResponseFullIPCounterList : 0 == m_pclApp");
    }
    Packet_IP_Counter_List* pkt2 = (Packet_IP_Counter_List*)pkt;
    DNF_LOG_SCOPE_LINE(0x16e8,"./log/Secu", "[D_IP Counter] DataStats : %d, DataSize : %d ",
        (unsigned int)(unsigned char)pkt2->m_dataStats,
        (unsigned int)(unsigned char)pkt2->m_dataSize);
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt2, pkt2->packetSize);


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnResponseFullIPCounterList() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x16f1, "./log/Except", "CPacketTranslater::OnResponseFullIPCounterList() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnResponseFullIPCounterList() Exception Break");
        DNF_LOG_SCOPE_LINE(0x16f7, "./log/Except", "CPacketTranslater::OnResponseFullIPCounterList() Exception Break\n");
    }
}

void CPacketTranslater::OnTakeScreenShot(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnTakeScreenShot : 0 == m_pclApp");
        }
        Packet_Monitor_Take_Screen_Shot shotCopy = *(Packet_Monitor_Take_Screen_Shot*)pkt;
        CServerHandler* handler = m_pclApp->m_serverHandler2;
        if (shotCopy.m_channel == 0)
        {
            handler->SendAllToGameServer((char*)&shotCopy, shotCopy.packetSize);
        }
        else
        {
            handler->SendToGameServer(shotCopy.m_channel, (PacketHeader*)&shotCopy);
        }
        DNF_LOG_SCOPE_LINE(0x1710,"./log/ScreenShot", "Recv TakeScreenShot Command! channel(%d) time(%d)",
            (unsigned int)(unsigned char)shotCopy.m_channel,
            shotCopy.m_time);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnTakeScreenShot() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1717, "./log/Except",
            "CPacketTranslater::OnResponseIPCounterList() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnTakeScreenShot() Exception Break");
        DNF_LOG_SCOPE_LINE(0x171d, "./log/Except",
            "CPacketTranslater::OnResponseIPCounterList() Exception Break\n");
    }
}

void CPacketTranslater::OnVillageMonsterFightResult(PacketHeader* pkt)
{try
{


    unsigned int users[4] = {0, 0, 0, 0};
    CUserManager* userMgr = &m_pclApp->m_userManager;
    for (int i = 0; i < 4; i++)
    {
        unsigned int key = ((MonitorVillageFightPkt*)pkt)->m_keys[i + 4];
        if (key != 0)
        {
            users[i] = (unsigned int)userMgr->FindUser_CharNo(key);
        }
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1736, "./log/Except", "CPacketTranslater::OnVillageMonsterFightResult() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x173b, "./log/Except", "CPacketTranslater::OnVillageMonsterFightResult() Exception Break\n");
    }
}

void CPacketTranslater::OnVillageAttackedGMCommand(PacketHeader* pkt)
{
    try
    {
        Packet_VillageAttackedGMCommand* pkt2 =
            (Packet_VillageAttackedGMCommand*)pkt;
        CUserManager* mgr = &m_pclApp->m_userManager;
        CUser* user = 0;
        user = mgr->FindUser_CharNo(pkt2->m_charNo);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1764,"./log/Except",
            "CPacketTranslater::OnVillageAttackedGMCommand() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1769, "./log/Except",
            "CPacketTranslater::OnVillageAttackedGMCommand() Exception Break\n");
    }
}

void CPacketTranslater::OnVillageAttackedRank(PacketHeader* pkt)
{
    PacketHeader* p = pkt;
    CUserManager* x = &m_pclApp->m_userManager;
    int n = 0;
    char buf[0x10];
}

void CPacketTranslater::OnMonitorFullLevelBroadCast(PacketHeader* pkt)
{
    PacketHeader* pkt2 = pkt;
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, pkt2->packetSize);
}

void CPacketTranslater::OnSetARSInfo(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnSetARSInfo : 0 == m_pclApp");
    }
    PacketHeader* pkt2 = pkt;
    DNF_LOG_SCOPE_LINE(0x183d, "./log/Secu", "[ARS_INFO] DBMW -> Monitor -> GameSvr");
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, 0x4bf);


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnSetARSInfo() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1844, "./log/Except", "CPacketTranslater::OnSetARSInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnSetARSInfo() Exception Break");
        DNF_LOG_SCOPE_LINE(0x184a, "./log/Except", "CPacketTranslater::OnSetARSInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnWebRequestARSInfo(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnWebRequestARSInfo : 0 == m_pclApp");
    }
    Packet_Web_Request_ARS_Info* p = (Packet_Web_Request_ARS_Info*)pkt;
    CServerHandler* handler = m_pclApp->m_serverHandler2;
    if (handler != 0)
    {
        DNF_LOG_SCOPE_LINE(0x181d, "./log/Secu", "[ARS_INFO] Web -> Monitor -> DBMW");
        handler->SendDBMWRequestARSInfo(p->m_flag);
    }


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnWebRequestARSInfo() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1824, "./log/Except", "CPacketTranslater::OnWebRequestARSInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnWebRequestARSInfo() Exception Break");
        DNF_LOG_SCOPE_LINE(0x182a, "./log/Except", "CPacketTranslater::OnWebRequestARSInfo() Exception Break\n");
    }
}

void CPacketTranslater::OnCheckOverlappedAccusation(PacketHeader* pkt)
{
    Packet_Check_Overlapped_Accusation* acc =
        (Packet_Check_Overlapped_Accusation*)pkt;
    CTcpGameServer* tcpGs =
        (CTcpGameServer*)m_pclApp->FindTcpGameServer(acc->m_connNo);
    if (tcpGs != 0)
    {
        acc->m_result = m_pclApp->AddAccusationCharac(
            std::string(acc->m_name1),
            std::string(acc->m_name2),
            acc->m_type,
            acc->m_result);
        char* buf = tcpGs->makePacketHeader(0x1b66, 0x15f);
        if (buf != 0)
        {
            char* out = buf;
            memcpy(out, acc, 0x15f);
            tcpGs->SendToGameServer(out);
        }
    }
}

void CPacketTranslater::OnGameServerRegist(PacketHeader* pkt)
{
    Packet_Game_Server_Regist* regist = (Packet_Game_Server_Regist*)pkt;
    stServerInfo* info = new stServerInfo;
    info->m_field0 = regist->m_group;
    info->m_group = regist->m_channel;
    info->m_type = regist->m_type;
    info->m_port = regist->m_port;
    strncpy(info->m_name, regist->m_name, 0x10);
    unsigned int sock = regist->m_connNo;
    CTcpGameServer* tcp =
        (CTcpGameServer*)m_pclApp->Get_ServerHandler()->GetTcpGameServer(sock);
    if (tcp != 0)
    {
        DNF_LOG_SCOPE_LINE(0x190c,"./log/GameServer", "Get Packet - OnGameServerRegist from Channel:%d",
            (unsigned int)(unsigned char)info->m_group);
        if (tcp->GetChannelNo() == 0)
        {
            char* buf = tcp->makePacketHeader(0x1f42, 0xc);
            char* out = 0;
            if (buf != 0)
            {
                out = buf;
            }
            if (m_pclApp->Get_ServerHandler()->RegistGameServer(info) == 1)
            {
                tcp->SetChannelNo(regist->m_channel);
                CGameServer* gs =
                    m_pclApp->Get_ServerHandler()->GetGameServer(
                        (unsigned int)(unsigned char)info->m_group);
                gs->SetSocket(regist->m_connNo);
                out[0xb] = 0;
                CMyFileLog log2(__FUNCTION__, 0x1930);
                log2("./log/GameServer", "Game server regist success. Channel: %d",
                     (unsigned int)(unsigned char)info->m_group);
            }
            else
            {
                out[0xb] = 1;
                CMyFileLog log2(__FUNCTION__, 0x1923);
                log2("./log/GameServer",
                     "Game server regist failed. Channel: %d is already exist.",
                     (unsigned int)(unsigned char)info->m_group);
            }
            out[0xa] = 0;
            tcp->SendToGameServer(out);
        }
        char* seed = tcp->makePacketHeader(0x27f8, 0xe);
        if (seed != 0)
        {
            *(unsigned int*)(seed + 10) = m_pclApp->getMiniCraneSeed();
            tcp->SendToGameServer(seed);
        }
    }
}

void CPacketTranslater::OnNoCache(PacketHeader* pkt)
{
    Packet_No_Cache* p = (Packet_No_Cache*)pkt;
    if (p->m_dbid == 0)
    {
        exchange_server::GetInstanceCacheCharacterMgr()->Reset();
    }
    else
    {
        exchange_server::CACHE_CHARACTER_TYPE type;
        if (exchange_server::GetInstanceCacheCharacterMgr()->GetCacheCharacter(
                p->m_dbid, &type))
        {
            register char* s;
            register int f0;
            register int f4;
            f4 = type.m_field4;
            f0 = type.m_field0;
            s = NumberToString(p->m_dbid, 0);
            DNF_LOG_SCOPE_LINE(0x1970,"./log/ExchangeServer", "OnNoCache() (%s,%d,%d)\n", s, f0, f4);
        }
    }
}

void CPacketTranslater::OnDisableUserOneToOneChat_GM(PacketHeader* pkt)
{
    Packet_Disable_User_OneToOneChat_Police* pktLocal =
        (Packet_Disable_User_OneToOneChat_Police*)pkt;
    if (m_pclApp != 0)
    {
        if (m_pclApp->isGM_regFromChannel(pktLocal->m_gmId))
        {
            CUser* target =
                (&m_pclApp->m_userManager)->FindUser_CharName(
                    pktLocal->m_name);
            if (target != 0)
            {
                m_pclApp->DisableChatUserWithGM(
                    pktLocal->m_gmId, target->GetUniqCharNo());
            }
        }
    }
}

void CPacketTranslater::OnFindCharacName_useUID(PacketHeader* pkt)
{
    CTcpGameServer* tcpGs =
        (CTcpGameServer*)m_pclApp->FindTcpGameServer(pkt->m_connNo);
    if (tcpGs != 0)
    {
        Packet_Find_Charac_Name_UseUID* req = (Packet_Find_Charac_Name_UseUID*)pkt;
        char* buf = tcpGs->makePacketHeader(0x1f45, 0x34);
        Packet_Find_Charac_Name_UseUID* pb =
            (Packet_Find_Charac_Name_UseUID*)buf;
        pb->m_dbid = req->m_dbid;
        pb->m_charNo = req->m_charNo;
        pb->m_len = 0;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser_CharNo(req->m_charNo);
        if (user != 0)
        {
            pb->m_len = strlen(user->GetCharName());
            if (0x1d < pb->m_len)
            {
                pb->m_len = 0;
            }
            if (pb->m_len != 0)
            {
                strncpy(pb->m_name, user->GetCharName(), pb->m_len);
            }
        }
        tcpGs->SendToGameServer((char*)pb);
    }
}

void CPacketTranslater::OnRenew_GM_List(PacketHeader* pkt)
{
    struct RenewGMListPkt
    {
        char pad_a[10];
        char m_flag;
        char m_count;
        unsigned int m_ids[20];
        char m_levels[0x14];
    } __attribute__((packed));
    RenewGMListPkt* p = (RenewGMListPkt*)pkt;
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && p != 0)
    {
        if (p->m_flag == 0)
        {
            gm->clearGmList();
        }
        for (int i = 0; i < (int)(char)p->m_count; i++)
        {
            gm->AppendGM_Sys(p->m_ids[i], p->m_levels[i]);
        }
        m_pclApp->m_serverHandler2->SendToDB((PacketHeader*)p);
    }
}

void CPacketTranslater::OnLoadPeriodicMessage(PacketHeader* pkt)
{try
{


    PacketHeader* pkt2 = pkt;
    m_pclApp->m_serverHandler2->SendToDB(pkt);
    DNF_LOG_SCOPE_LINE(0x19e0, "./log/PeriodicMessage", "Web Request is Arrived and Send Request DBMW");


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x19e4, "./log/Except", "CPacketTranslater::OnLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x19e9, "./log/Except", "CPacketTranslater::OnLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnResultLoadPeriodicMessage(PacketHeader* pkt)
{
    try
    {
        struct ResultPeriodicInfo
        {
            char pad[0xa];
            char msg[0x200];
            int start;
            int end;
        } __attribute__((packed));
        ResultPeriodicInfo* info = (ResultPeriodicInfo*)pkt;
        DNF_LOG_SCOPE_LINE(0x19f7,"./log/PeriodicMessage",
            "DB Load Message : Message(%s), start_hour(%d), end_hour(%d)", info->msg,
            info->start, info->end);
        ((CPeriodicMessageMgr*)m_pclApp->GetPeriodicMessageManager())
            ->SetMessageData(info->msg, info->start, info->end);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a02,"./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a07, "./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnRegisterEventIdx(PacketHeader* pkt)
{try
{

    Packet_Register_Event_Idx* pkt2 = (Packet_Register_Event_Idx*)pkt;
    DNF_LOG_SCOPE_LINE(0x1a15,"./log/OnTimeEvent", "OnRegisterEventIdx:result =%d, Eventidx =%d",
        (unsigned int)(unsigned char)pkt2->m_result, pkt2->m_eventIdx);
    if (pkt2->m_result != 0)
    {
        m_pclApp->m_onTimeEventMgr->SetEventIdx(pkt2->m_eventIdx);
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a1e, "./log/Except", "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a23, "./log/Except", "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnRegisterEventUserIdx(PacketHeader* pkt)
{
    try
    {
        Packet_Register_Event_User_Idx* pkt2 =
            (Packet_Register_Event_User_Idx*)pkt;
        if (m_pclApp != 0)
        {
            DNF_LOG_SCOPE_LINE(0x1a30,"./log/OnTimeEvent",
                "OnRegisterEventUserIdx:id = %u , rcv_idx = %u, cur_idx = %d, errortype = %d",
                pkt2->m_dbid, pkt2->m_idx,
                m_pclApp->m_onTimeEventMgr->GetEvent_Idx(),
                (unsigned short)pkt2->m_errortype);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x1a35,"./log/OnTimeEvent",
                "OnRegisterEventUserIdx:id = %u , idx = %u, errortype = %d",
                pkt2->m_dbid, pkt2->m_idx,
                (unsigned int)(unsigned short)pkt2->m_errortype);
        }
        if (pkt2->m_errortype == 0 || pkt2->m_errortype == 3)
        {
            CUser* user = m_pclApp->Get_UserManager()->FindUser(pkt2->m_dbid);
            if (user != 0)
            {
                user->SetEvent_idx(pkt2->m_idx);
                if (pkt2->m_errortype == 3)
                {
                    user->Event_idx_modify_state();
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a4a,"./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a4f, "./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnRegisterEventItem(PacketHeader* pkt)
{
    try
    {
        Packet_Register_Event_Item* rpkt = (Packet_Register_Event_Item*)pkt;
        unsigned int errortype = (unsigned int)(unsigned short)rpkt->m_errortype;
        unsigned int cnt = rpkt->m_cnt;
        unsigned int idx = rpkt->m_idx;
        DNF_LOG_SCOPE_LINE(0x1a5a,"./log/OnTimeEvent", "OnRegisterEventItem:idx = %u , cnt = %u, errortype = %d",
            idx, cnt, errortype);
        if (rpkt->m_errortype == 0)
        {
            if ((int)rpkt->m_idx == 0 || (int)rpkt->m_cnt == 0)
            {
                CMyFileLog log2(__FUNCTION__, 0x1a66);
                log2("./log/OnTimeEvent", "wrong item data", idx, cnt, errortype);
            }
            else if (m_pclApp != 0)
            {
                COnTimeEventManager* mgr = m_pclApp->m_onTimeEventMgr;
                mgr->SetEventItem(rpkt->m_idx, rpkt->m_cnt);
                mgr->StartEvent();
            }
        }
        else
        {
            CMyFileLog log3(__FUNCTION__, 0x1a60);
            log3("./log/OnTimeEvent", "db error not item", idx, cnt, errortype);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a73,"./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a78,"./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnResultRegisterEventIdx(PacketHeader* pkt)
{
    try
    {
        Packet_Result_Register_Event_Idx* pkt2 =
            (Packet_Result_Register_Event_Idx*)pkt;
        if (m_pclApp != 0)
        {
            DNF_LOG_SCOPE_LINE(0x1a85,"./log/OnTimeEvent",
                "OnResultRegisterEventIdx:event_idx(%d) , cur_idx(%d)",
                pkt2->m_eventIdx, m_pclApp->m_onTimeEventMgr->GetEvent_Idx());
            if (m_pclApp->m_onTimeEventMgr->GetEvent_Idx() < pkt2->m_eventIdx)
            {
                m_pclApp->m_onTimeEventMgr->SetEventIdx(pkt2->m_eventIdx);
            }
            m_pclApp->m_onTimeEventMgr->SendContinueTimeToGS();
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a92,"./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a97, "./log/Except",
            "CPacketTranslater::OnResultLoadPeriodicMessage Exception Break\n");
    }
}

void CPacketTranslater::OnGameMonitorGMVillageAttacked(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnGameMonitorGMVillageAttacked : 0 == m_pclApp");
    }
    Packet_Game_Monitor_GM_Village_Attacked* va =
        (Packet_Game_Monitor_GM_Village_Attacked*)pkt;
    if (va->m_flag != 0)
    {
        village_attacked::SetGMConfig(va->m_a, va->m_b, va->m_c);
    }
    else
    {
        village_attacked::SetRealConfig();
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1bbd, "./log/Except", "CPacketTranslater::OnGameMonitorGMVillageAttacked Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1bc2, "./log/Except", "CPacketTranslater::OnGameMonitorGMVillageAttacked Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorPunishCancel(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnMonitorPunishCancel : 0 == m_pclApp");
        }
        Packet_Punish_Cancel* in = (Packet_Punish_Cancel*)pkt;
        Packet_Punish_Cancel reply;
        int charNo = 0;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(in->m_idByChannel);
        if (user == 0)
        {
            charNo = -1;
        }
        else
        {
            charNo = (int)user->GetUniqCharNo();
        }
        if (charNo != 0)
        {
            CUser* target = (&m_pclApp->m_userManager)
                                ->FindUser_CharNo((unsigned int)charNo);
            if (target != 0)
            {
                reply.m_idByChannel = target->GetIdByChannel();
                reply.m_type = in->m_type;
                reply.m_param = in->m_param;
                reply.packetSize = 0x12;
                target->SendToGameserver((char*)&reply, reply.packetSize);
            }
            return;
        }
        throw CDNFException("CPacketTranslater::OnMonitorPunishCancel");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnMonitorPunishCancel() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1bf5,"./log/Except",
            "CPacketTranslater::OnMonitorPunishCancel() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnMonitorPunishCancel() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1bfb, "./log/Except",
            "CPacketTranslater::OnMonitorPunishCancel() Exception Break\n");
    }
}

void CPacketTranslater::OnBroadcastMsg(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1c0c, "./log/WebNotice", "CPacketTranslater::OnBroadcastMsg : 0 == m_pclApp");
        }
        else
        {
            Packet_Broadcast_Msg* pkt2 = (Packet_Broadcast_Msg*)pkt;
            m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, pkt2->packetSize);
            DNF_LOG_SCOPE_LINE(0x1c14,"./log/WebNotice", "OnBroadcastMsg : (%s,%d)\n", pkt2->m_text,
                (unsigned int)pkt2->m_len);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1c18, "./log/Except",
            "CPacketTranslater::OnBroadcastMsg Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1c1d, "./log/Except",
            "CPacketTranslater::OnBroadcastMsg Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorSecuServiceConnWeb(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnMonitorSecuServiceConnWeb : 0 == m_pclApp");
        }
        Packet_SecuService_Connect_Web* in = (Packet_SecuService_Connect_Web*)pkt;
        int charNo = 0;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(in->m_idByChannel);
        if (user == 0)
        {
            charNo = -1;
        }
        else
        {
            charNo = (int)user->GetUniqCharNo();
        }
        if (charNo != 0)
        {
            CUser* target = (&m_pclApp->m_userManager)
                                ->FindUser_CharNo((unsigned int)charNo);
            if (target != 0)
            {
                Packet_SecuService_Connect_Web reply;
                memcpy(&reply, pkt, 0x15);
                reply.m_idByChannel = target->GetIdByChannel();
                target->SendToGameserver((char*)&reply, reply.packetSize);
            }
            return;
        }
        throw CDNFException("CPacketTranslater::OnMonitorSecuServiceConnWeb");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break : %s\n",
               e.what());
        DNF_LOG_SCOPE_LINE(0x1c4f,"./log/Except",
            "CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1c55, "./log/Except",
            "CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnResetTODAPCInfo(PacketHeader* pkt)
{
    char buf[0x20];
}

void CPacketTranslater::OnNoticeMemberChatMsgHyperLink(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeMemberChatMsgHyperLink : 0 == m_pclApp");
    }
    Packet_Monitor_Member_Chat_Hyper_Link* chat =
        (Packet_Monitor_Member_Chat_Hyper_Link*)pkt;
    if (chat->m_memberId != 0 && chat->m_msgLen != 0)
    {
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser_CharNo(chat->m_memberId);
        if (user != 0)
        {
            CMember* member =
                (&m_pclApp->m_memberManager)->FindMember(chat->m_memberId);
            if (member != 0)
            {
                member->NoticeChatMsgToMemberMembersHyperLink(
                    chat->m_msg,
                    (unsigned int)(unsigned char)chat->m_msgLen,
                    chat->m_itemCount,
                    (hyperlink_item_info*)chat->m_items, user);
            }
        }
        return;
    }
    throw CDNFException(
        "CPacketTranslater::OnNoticeMemberChatMsgHyperLink : packet->m_uMemberID && "
        "packet->m_msgLen");


    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeMemberChatMsgHyperLink() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1cb7, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsgHyperLink() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeMemberChatMsgHyperLink() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1cbd, "./log/Except", "CPacketTranslater::OnNoticeMemberChatMsgHyperLink() Exception Break\n");
    }
}

void CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException(
            "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink : 0 == m_pclApp");
    }
    Packet_Monitor_Other_Channel_Chat_Hyper_Link* chat =
        (Packet_Monitor_Other_Channel_Chat_Hyper_Link*)pkt;
    if (chat->what_0x1b != 0 &&
        (unsigned char)chat->what_0x1b < 0x1e)
    {
        CUser* target = (&m_pclApp->m_userManager)->FindUser_CharName(chat->buddy_n_user_id_what);
        chat->what_0x17 =
            target != 0 ? (int)target->GetUniqCharNo() : (int)0xffffffff;
    }
    if (chat->what_0x13 == 0 ||
        chat->what_0x17 == 0 ||
        chat->what_0x173 == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d71,"./log/Except",
            "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink, sender(%d), "
            "receiver(%d), msglen(%d)",
            chat->what_0x13, chat->what_0x17,
            (unsigned int)(unsigned char)chat->what_0x173);
        throw CDNFException(
            "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink : packet->m_uSenderCharID "
            "&&  packet->m_uRecverCharID && packet->m_msgLen");
    }
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link reply;
    reply.m_senderCharId = (unsigned int)chat->what_0x0a;
    CUser* sender = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)chat->what_0x13);
    if (sender == 0)
    {
        return;
    }
    CUser* receiver = (&m_pclApp->m_userManager)->FindUser_CharNo((unsigned int)chat->what_0x17);
    if (receiver == 0)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.m_type = 1;
        reply.m_itemCount = chat->what_0x3a;
        for (int i = 0; i < (int)(unsigned int)(unsigned char)chat->what_0x3a; i++)
        {
            memcpy(reply.m_items[i], chat->what_0x3b[i], 0x68);
        }
        reply.packetSize = 0x170;
        sender->SendToGameserver((char*)&reply, 0x170);
        return;
    }
    bool blocked = false;
    if (receiver->IsBlackUser(sender->GetUniqCharNo()) != 0 &&
        m_pclApp->isGM_regFromChannel(sender->GetUniqCharNo()) != 1)
    {
        blocked = true;
    }
    if (!blocked && sender->IsBlackUser(receiver->GetUniqCharNo()) != 0 &&
        m_pclApp->isGM_regFromChannel(receiver->GetUniqCharNo()) != 1)
    {
        blocked = true;
    }
    if (blocked)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        reply.m_type = 2;
        reply.m_itemCount = chat->what_0x3a;
        for (int i = 0; i < (int)(unsigned int)(unsigned char)chat->what_0x3a; i++)
        {
            memcpy(reply.m_items[i], chat->what_0x3b[i], 0x68);
        }
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.packetSize = 0x170;
        sender->SendToGameserver((char*)&reply, 0x170);
        return;
    }
    if (m_pclApp->isGM_regFromChannel(sender->GetUniqCharNo()) != 0)
    {
        m_pclApp->AddChattableUserWithGM(sender->GetUniqCharNo(), receiver->GetUniqCharNo());
    }
    bool gmBlocked = false;
    if (m_pclApp->isGM_regFromChannel(receiver->GetUniqCharNo()) != 0)
    {
        if (m_pclApp->isAbleUserChatWithGM(receiver->GetUniqCharNo(),
                                           sender->GetUniqCharNo()) != 1)
        {
            gmBlocked = true;
        }
    }
    if (gmBlocked)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        reply.m_type = 3;
        reply.m_itemCount = chat->what_0x3a;
        for (int i = 0; i < (int)(unsigned int)(unsigned char)chat->what_0x3a; i++)
        {
            memcpy(reply.m_items[i], chat->what_0x3b[i], 0x68);
        }
        memcpy(reply.m_name, chat->buddy_n_user_id_what, 0x1d);
        reply.packetSize = 0x170;
        sender->SendToGameserver((char*)&reply, 0x170);
    }
    else
    {
        memcpy(reply.m_name, sender->GetCharName(), 0x1d);
        reply.m_idByChannel = receiver->GetIdByChannel();
        reply.m_uniqCharNo = receiver->GetUniqCharNo();
        reply.m_itemCount = chat->what_0x3a;
        for (int i = 0; i < (int)(unsigned int)(unsigned char)chat->what_0x3a; i++)
        {
            memcpy(reply.m_items[i], chat->what_0x3b[i], 0x68);
        }
        reply.m_msgLen = chat->what_0x173;
        memcpy(reply.m_msg, chat->what_0x174,
               (unsigned int)(unsigned char)chat->what_0x173);
        reply.packetSize =
            (unsigned short)((unsigned char)chat->what_0x173 + 0x170);
        receiver->SendToGameserver((char*)&reply, reply.packetSize);
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1d77, "./log/Except", "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1d7c, "./log/Except", "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink() Exception Break\n");
    }
}

void CPacketTranslater::OnMonitorMegaPhoneMsgHyperLink(PacketHeader* pkt)
{
    MonitorMegaPhonePkt* local_pkt = (MonitorMegaPhonePkt*)pkt;
    local_pkt->m_sg = m_pclApp->Get_ServerGroup();
    m_pclApp->m_serverHandler2->SendAllToGameServer((char*)pkt, local_pkt->m_sz);
}

void CPacketTranslater::onSocialEventRewardItemRequest(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::onSocialEventRewardItemRequest");
    }
    char* local_pkt = (char*)pkt;
    m_pclApp->m_serverHandler2->SendToDB((PacketHeader*)local_pkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1db8, "./log/Except", "CPacketTranslater::onSocialEventRewardItemRequest Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1dbd, "./log/Except", "CPacketTranslater::onSocialEventRewardItemRequest Exception Break\n");
    }
}

void CPacketTranslater::onSocialEventRewardItemResponse(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::onSocialEventRewardItemResponse");
        }
        Packet_Social_Event_Reward_Item_Response* rpkt =
            (Packet_Social_Event_Reward_Item_Response*)pkt;
        LimitNpcBuyItemManager* mgr = m_pclApp->getLimitNpcBuyItemManager();
        mgr->registItemClear();
        unsigned int i = 0;
        while (i < rpkt->m_count && i < 0x1e)
        {
            NpcBuyLimitItem* item = (NpcBuyLimitItem*)&rpkt->m_items[i];
            mgr = m_pclApp->getLimitNpcBuyItemManager();
            mgr->registItem(*item);
            unsigned int c = item->m_sellCount;
            unsigned int b = item->m_maxCount;
            unsigned int a = item->m_itemId;
            DNF_LOG_SCOPE_LINE(0x1dd0,"./log/NpcBuyLimitItem", "Load-> itemId: %d, maxCount: %d, sellCount: %d)",
                a, b, c);
            i++;
        }
        (m_pclApp->m_serverHandler2)
            ->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1dd8, "./log/Except",
            "CPacketTranslater::onSocialEventRewardItemResponse Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1ddd,"./log/Except",
            "CPacketTranslater::onSocialEventRewardItemResponse Exception Break\n");
    }
}

void CPacketTranslater::onSocialEventRewardItemInfo(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::onSocialEventRewardItemInfo");
        }
        LimitNpcBuyItemInfo* rpkt = (LimitNpcBuyItemInfo*)pkt;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(rpkt->m_dbid);
        if (user == 0)
        {
            unsigned int cn = rpkt->m_charNo;
            char* s = NumberToString(rpkt->m_dbid, 0);
            DNF_LOG_SCOPE_LINE(0x1dee,"./log/Except",
                "CPacketTranslater::onSocialEventRewardItemInfo(), buyUser(%s), "
                "characNo(%u)",
                s, cn);
            throw CDNFException(
                "CPacketTranslater::onSocialEventRewardItemInfo : Not Exist User characNo");
        }
        int result = m_pclApp->getLimitNpcBuyItemManager()->sellNpcLimitBuyItem(rpkt);
        if (result < 1)
        {
            user->SendTcpGameserver(pkt);
        }
        else
        {
            rpkt->m_count = 0;
            rpkt->m_errorNo = (unsigned int)result;
            user->SendTcpGameserver(pkt);
            unsigned int buyCount = rpkt->m_count;
            unsigned int itemId = rpkt->m_itemId;
            unsigned int errorNo = rpkt->m_errorNo;
            unsigned int charNo = rpkt->m_charNo;
            DNF_LOG_SCOPE_LINE(0x1dfc,"./log/NpcBuyLimitItem",
                "don\'t sell-> characNo: %u, errorNo: %u, itemId: %u, buyCount: %u)",
                charNo, errorNo, itemId, buyCount);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1e06, "./log/Except",
            "CPacketTranslater::onSocialEventRewardItemInfo() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1e0b,"./log/Except",
            "CPacketTranslater::onSocialEventRewardItemInfo() Exception Break\n");
    }
}

void CPacketTranslater::onSocialEventRewardItemInfoAll(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::onSocialEventRewardItemInfoAll");
        }
        LimitNpcBuyItemInfoAll* rpkt = (LimitNpcBuyItemInfoAll*)pkt;
        CUser* user =
            (&m_pclApp->m_userManager)->FindUser(rpkt->m_dbid);
        if (user == 0)
        {
            unsigned int cn = rpkt->m_charNo;
            char* s = NumberToString(rpkt->m_dbid, 0);
            DNF_LOG_SCOPE_LINE(0x1e1c,"./log/Except",
                "CPacketTranslater::onSocialEventRewardItemInfoAll(), buyUser(%s), "
                "characNo(%u)",
                s, cn);
            throw CDNFException(
                "CPacketTranslater::onSocialEventRewardItemInfoAll : Not Exist User");
        }
        m_pclApp->getLimitNpcBuyItemManager()->getNpcLimitBuyItemInfoAll(rpkt);
        user->SendTcpGameserver(pkt);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1e28,"./log/Except",
            "CPacketTranslater::onSocialEventRewardItemInfoAll() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1e2d,"./log/Except",
            "CPacketTranslater::onSocialEventRewardItemInfoAll() Exception Break\n");
    }
}

void CPacketTranslater::onSocialEventRewardItemUpdate(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::onSocialEventRewardItemUpdate");
        }
        LimitNpcBuyItemUpdate* rpkt = (LimitNpcBuyItemUpdate*)pkt;
        LimitNpcBuyItemChangeInfo change;
        if ((int)rpkt->m_errorNo == 0)
        {
            unsigned int itemId = rpkt->m_itemId;
            LimitNpcBuyItemManager* mgr = m_pclApp->getLimitNpcBuyItemManager();
            mgr->getNpcLimitBuyItemCount(itemId, change);
            (m_pclApp->m_serverHandler2)
                ->SendAllTcpGameServer(&change);
            (m_pclApp->m_serverHandler2)->SendToDB(pkt);
            unsigned int buyCount = rpkt->m_cancelCount;
            unsigned int itemId2 = rpkt->m_itemId;
            unsigned int charNo = rpkt->m_charNo;
            DNF_LOG_SCOPE_LINE(0x1e46,"./log/NpcBuyLimitItem",
                "DB Update-> characNo: %u, itemId: %u, buyCount: %u)", charNo, itemId2,
                buyCount);
        }
        else
        {
            m_pclApp->getLimitNpcBuyItemManager()->undoNpcLimitBuyItem(
                (LimitNpcBuyItemUpdate*)pkt);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1e4b, "./log/Except",
            "CPacketTranslater::onSocialEventRewardItemUpdate Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1e50,"./log/Except",
            "CPacketTranslater::onSocialEventRewardItemUpdate Exception Break\n");
    }
}

void CPacketTranslater::onRequestCharacInfoByCharacName(PacketHeader* pkt)
{
    CUser* requester;
    CUser* target;
    if (m_pclApp != 0)
    {
        CUserManager* userMgr = &m_pclApp->m_userManager;
        Packet_Monitor_Request_Charac_Info* rpkt =
            (Packet_Monitor_Request_Charac_Info*)pkt;
        try
        {
            requester = userMgr->FindUser_CharNo(rpkt->m_requesterNo);
            if (requester != 0)
            {
                Packet_Monitor_Reply_Charac_Info reply;
                reply.m_result = 0;
                reply.m_idByChannel = requester->GetIdByChannel();
                strncpy(reply.m_name, rpkt->m_name, 0x1d);
                target = userMgr->FindUser_CharName(rpkt->m_name);
                bool notfound = (target == 0);
                if (notfound)
                {
                    reply.m_channel = 0;
                }
                else
                {
                    CServerInterface* gs = (CServerInterface*)target->GetGameServer();
                    reply.m_channel = gs->GetChannelNo();
                    reply.m_targetIdByChannel = target->GetIdByChannel();
                    reply.m_level = target->GetLevel();
                    reply.m_job = target->GetJob();
                    reply.m_growthType = target->GetGrowthType();
                }
                requester->SendToGameserver((char*)&reply, 0x39);
            }
        }
        catch (CDNFException& e)
        {
            printf(
                "CPacketTranslater::onRequestCharacInfoByCharacName() Exception Break : %s\n",
                e.what());
            DNF_LOG_SCOPE_LINE(0x1e82,"./log/Except",
                "CPacketTranslater::onRequestCharacInfoByCharacName() Exception Break : %s\n",
                e.what());
        }
    }
}

void CPacketTranslater::OnWebNoticeInGameAD(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("m_pclApp == 0");
    }
    Packet_Web_Notice_InGame_Advertisement reply;
    m_pclApp->m_serverHandler2->SendAllTcpGameServer(&reply);
    DNF_LOG_SCOPE_LINE(0x1f84,"./log/Web", "OnWebNoticeInGameAD() packet_id(%d)\n",
        (unsigned int)*(unsigned short*)pkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1f88, "./log/Except", "CPacketTranslater::OnWebNoticeInGameAD Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1f8d, "./log/Except", "CPacketTranslater::OnWebNoticeInGameAD Exception Break\n");
    }
}

void CPacketTranslater::onCollectItems(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::onCollectItems");
        }
        Packet_CollectItems* req = (Packet_CollectItems*)pkt;
        unsigned int cur =
            ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current;
        if (cur < ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_total)
        {
            if (((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current == 0)
            {
                SendColletItemsReward(req->m_charNo, req->m_idByChannel, req->m_name,
                                      (int)req->m_nameLen, TimeGateRewardType::TYPE_0);
            }
            else
            {
                unsigned int cur2 =
                    ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current;
                if (cur2 + req->m_add >=
                    ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_total)
                {
                    SendColletItemsReward(req->m_charNo, req->m_idByChannel, req->m_name,
                                          (int)req->m_nameLen, TimeGateRewardType::TYPE_2);
                    ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_time =
                        (long)time(0);
                }
                else
                {
                    unsigned int cur3 =
                        ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current;
                    unsigned int rest =
                        ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current;
                    if ((cur3 - rest % 0x14) + 0x14 <=
                        ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current +
                            req->m_add)
                    {
                        SendColletItemsReward(req->m_charNo, req->m_idByChannel, req->m_name,
                                              (int)req->m_nameLen, TimeGateRewardType::TYPE_1);
                    }
                }
            }
            ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current =
                ((MonitorCollectItemsState*)m_pclApp->getCollectItems())->m_current +
                req->m_add;
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1fcb, "./log/Except",
            "CPacketTranslater::onCollectItems Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1fd0, "./log/Except",
            "CPacketTranslater::onCollectItems Exception Break\n");
    }
}

void CPacketTranslater::onCollectItemsResult(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::onCollectItemsResult");
    }
    struct STCollectItemsData
    {
        unsigned int m_uniqCharNo;
        unsigned int m_money;
        unsigned int m_etc;
        unsigned char m_end;
    };
    Packet_CollectItemsResult* p = (Packet_CollectItemsResult*)pkt;
    ((STCollectItemsData*)m_pclApp->getCollectItems())->m_money = p->m_fieldE;
    ((STCollectItemsData*)m_pclApp->getCollectItems())->m_uniqCharNo = p->m_fieldA;
    ((STCollectItemsData*)m_pclApp->getCollectItems())->m_etc = p->m_field12;
    ((STCollectItemsData*)m_pclApp->getCollectItems())->m_end = 0;


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1fe6, "./log/Except", "CPacketTranslater::onCollectItemsResult Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1feb, "./log/Except", "CPacketTranslater::onCollectItemsResult Exception Break\n");
    }
}

void CPacketTranslater::onCollectItemsGm(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::onCollectItemsGm");
    }
    MonitorCollectGmPkt* local_pkt = (MonitorCollectGmPkt*)pkt;
    local_pkt->m_sg = m_pclApp->Get_ServerGroup();
    m_pclApp->m_serverHandler2->SendToDB((PacketHeader*)local_pkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1ffe, "./log/Except", "CPacketTranslater::onCollectItemsGm Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2003, "./log/Except", "CPacketTranslater::onCollectItemsGm Exception Break\n");
    }
}

void CPacketTranslater::OnPcRoomPlayTimeReward(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnPcRoomPlayTimeReward");
    }
    char* rpkt = (char*)pkt;
    m_pclApp->m_serverHandler2->SendToDB((PacketHeader*)rpkt);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x2017, "./log/Except", "CPacketTranslater::OnPcRoomPlayTimeReward Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x201c, "./log/Except", "CPacketTranslater::OnPcRoomPlayTimeReward Exception Break\n");
    }
}

void CPacketTranslater::OnWebEmergencyPatchMessage(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnWebEmergencyPatchMessage");
        }
        Packet_Web_Emergency_Patch_Message* rpkt =
            (Packet_Web_Emergency_Patch_Message*)pkt;
        for (int i = 0; i < 0x32; i++)
        {
            if (rpkt->m_channels[i] != 0)
            {
                unsigned char ch = (unsigned char)rpkt->m_channels[i];
                CTcpGameServer* tcp =
                    m_pclApp->Get_ServerHandler()->GetTcpGameServerByCh(ch);
                if (tcp != 0)
                {
                    char* buf = tcp->makePacketHeader(0x27f2, 0x10a);
                    if (buf != 0)
                    {
                        *(char*)(buf + 10) = (char)rpkt->m_len;
                        char* out = buf;
                        memset(buf + 0xb, 0, 0xff);
                        memcpy(out + 0xb, rpkt->m_text, (unsigned int)rpkt->m_len);
                        tcp->SendToGameServer(out);
                    }
                }
            }
        }
        unsigned short pid = *(unsigned short*)pkt;
        DNF_LOG_SCOPE_LINE(0x204a,"./log/Web", "CPacketTranslater::OnWebEmergencyPatchMessage packet_id(%d)",
            (unsigned int)pid);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x204e,"./log/Except",
            "CPacketTranslater::OnWebEmergencyPatchMessage Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2053, "./log/Except", "CPacketTranslater::OnWebEmergencyPatchMessage Exception Break\n");
    }
}

void CPacketTranslater::OnUpdateMiniCraneSeed(PacketHeader* pkt)
{
    try
    {
        Packet_MiniCraneSeed* local_pkt = (Packet_MiniCraneSeed*)pkt;
        m_pclApp->SetMiniCraneRandomSeed();
        local_pkt->m_seed = (unsigned int)m_pclApp->getMiniCraneSeed();
        if (local_pkt == 0)
        {
            throw CDNFException("CPacketTranslater::OnUpdateMiniCraneSeed, packet is null");
        }
        if (m_pclApp != 0 && m_pclApp->m_serverHandler2 != 0)
        {
            m_pclApp->m_serverHandler2->SendAllToGameServer((char*)local_pkt, local_pkt->packetSize);
            return;
        }
        throw CDNFException("CPacketTranslater::OnUpdateMiniCraneSeed m_pclApp or m_pclServerHandler is null");
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x222b,"./log/Except",
            "CPacketTranslater::OnUpdateMiniCraneSeed Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2230, "./log/Except",
            "CPakcetTranslater::OnUpdateMiniCraneSeed Exception Break\n");
    }
}

void CPacketTranslater::onStartGameEventFromServer(PacketHeader* pkt)
{try
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x22e2, "./log/AradOnly", "[Server Event] m_pclApp is null.");
        throw 0x22e3;
    }
    Packet_StartGameEventFromServer* p = (Packet_StartGameEventFromServer*)pkt;
    if (p == 0)
    {
        DNF_LOG_SCOPE_LINE(0x22e9, "./log/AradOnly", "[Server Event] Packet_StartGameEventFromServer is null.");
        throw 0x22ea;
    }
    Packet_Monitor_Event_Start epkt;
    epkt.m_eventCode = p->m_eventCode;
    epkt.m_eventParam = p->m_eventParam;
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&epkt);
    DNF_LOG_SCOPE_LINE(0x22f2,"./log/AradOnly", "[Server Event] start event. (event:%d, param:%d,%d)",
        p->m_eventCode, (unsigned int)p->m_eventParam1,
        (unsigned int)p->m_eventParam2);


    }
    catch (int line)
    {
        DNF_LOG_SCOPE_LINE(0x22f9, "./log/AradOnly", "[6ThBirthday] error onStartGameEventFromServer line. (line:%u)", line);
    }
}

void CPacketTranslater::onEndGameEventFromServer(PacketHeader* pkt)
{try
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x2304, "./log/AradOnly", "[Server Event] m_pclApp is null.");
        throw 0x2305;
    }
    Packet_StopGameEventFromServer* p = (Packet_StopGameEventFromServer*)pkt;
    if (p == 0)
    {
        DNF_LOG_SCOPE_LINE(0x230b, "./log/AradOnly", "[Server Event] Packet_StopGameEventFromServer is null.");
        throw 0x230c;
    }
    Packet_Monitor_Event_End epkt;
    epkt.m_eventCode = p->m_eventCode;
    m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(&epkt);
    DNF_LOG_SCOPE_LINE(0x2312,"./log/AradOnly", "[Server Event] end event. (event:%d)",
        p->m_eventCode);


    }
    catch (int line)
    {
        DNF_LOG_SCOPE_LINE(0x2316, "./log/AradOnly", "[6ThBirthday] error onEndGameEventFromServer line. (line:%u)", line);
    }
}

void CPacketTranslater::onReloadCountryCode(PacketHeader* pkt)
{try
{


    m_pclApp->m_serverHandler2->SendAllTcpGameServer(pkt);
    DNF_LOG_SCOPE_LINE(0x2344, "./log/Web", "CPacketTranslater::onReloadCountryCode()\n");


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x2348, "./log/Except", "CPacketTranslater::onReloadCountryCode Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x234d, "./log/Except", "CPacketTranslater::onReloadCountryCode Exception Break\n");
    }
}

void CPacketTranslater::onReloadSecurityRestrictPolicy(PacketHeader* pkt)
{try
{


    m_pclApp->m_serverHandler2->SendAllTcpGameServer(pkt);
    DNF_LOG_SCOPE_LINE(0x2359, "./log/Web", "CPacketTranslater::onReloadSecurityRestrictPolicy()\n");

    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x235d, "./log/Except", "CPacketTranslater::onReloadSecurityRestrictPolicy Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x2362, "./log/Except", "CPacketTranslater::onReloadSecurityRestrictPolicy Exception Break\n");
    }
}

Packet_Item_Limit_Edition_Load_Data_Req::Packet_Item_Limit_Edition_Load_Data_Req()
    : PacketHeader(0x1007, 0x83)
{
    m_fullLoad = 0;
    m_loadTargetNum = 0;
}

Packet_Item_Limit_Edition_Sell_end::Packet_Item_Limit_Edition_Sell_end()
    : PacketHeader(0x1005, 0x82)
{
    m_sellEndNum = 0;
    m_serverType = 0;
}

Packet_Item_Limit_Edition_Update::Packet_Item_Limit_Edition_Update()
    : PacketHeader(0x1006, 0x10e)
{
    m_serverGroup = 0;
    m_count = 0;
}

Packet_Monitor_Event_Start::Packet_Monitor_Event_Start() : PacketHeader(0x44f, 0x12) {}

Packet_Monitor_Event_End::Packet_Monitor_Event_End() : PacketHeader(0x450, 0xe) {}

Packet_Monitor_Take_Screen_Shot::Packet_Monitor_Take_Screen_Shot()
    : PacketHeader(0x9d3, 0xf)
{
    m_channel = 0;
    m_time = 0;
}

Packet_DBMW_Query_Msg::Packet_DBMW_Query_Msg() : PacketHeader(0x177d, 0x1013)
{
    m_queryId = 0;
    m_handleIdx = 0;
    int i = 0;
    for (i = 0; i <= 0x1000; i++)
    {
        m_sql[i] = 0;
    }
}

Packet_VillageAttackedRewardServer::Packet_VillageAttackedRewardServer()
    : PacketHeader(0x177a, 0xe)
{
    m_rewardType = 0;
}

Packet_VillageAttackedEnd::Packet_VillageAttackedEnd() : PacketHeader(0x1774, 0x16)
{
    m_dungeonRemain = 0;
    m_huntingPoint = 0;
    m_maxHuntingPoint = 0;
}

Packet_VillageAttackedUpdate::Packet_VillageAttackedUpdate() : PacketHeader(0x1777, 0x16)
{
    m_remainTime = 0;
    m_huntingPoint = 0;
    m_maxHuntingPoint = 0;
}

Packet_VillageAttackedScore::Packet_VillageAttackedScore() : PacketHeader(0x1778, 0x26)
{
    m_idByChannel = 0;
    m_uniqCharNo = 0;
    m_remainTime = 0;
    m_huntingPoint = 0;
    m_maxHuntingPoint = 0;
    m_cur = 0;
    m_max = 0;
}

Packet_VillageAttackedReward::Packet_VillageAttackedReward() : PacketHeader(0x1775, 0x1a)
{
    m_idByChannel = 0;
    m_uniqCharNo = 0;
    m_rewardType = 0;
}

Packet_DBMW_Add_Buddy::Packet_DBMW_Add_Buddy() : PacketHeader(0x673, 0x2c)
{
    memset(m_charName, 0, 0x1e);
}

Packet_DBMW_Del_Buddy::Packet_DBMW_Del_Buddy() : PacketHeader(0x675, 0x30)
{
}

Packet_Monitor_Notice_Member_Member_Login_out::
    Packet_Monitor_Notice_Member_Member_Login_out()
    : PacketHeader(0x4b6, 0x38)
{
    m_flag = 0;
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_channelNo = 0xff;
    m_type = 2;
    m_uniqCharNo2 = 0;
    memset(m_charName, 0, 0x1e);
}

Packet_Monitor_Member_Chat_ToUser_Hyper_Link::
    Packet_Monitor_Member_Chat_ToUser_Hyper_Link()
    : PacketHeader(0x271a, 0x26a)
{
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_itemCount = 0;
    m_msgLen = 0;
    memset(m_charName, 0, 0x1e);
    memset(m_msg, 0, 0x100);
    memset(m_items, 0, 0x138);
}

Packet_Monitor_Member_Chat_ToUser::Packet_Monitor_Member_Chat_ToUser()
    : PacketHeader(0x4bf, 0x131)
{
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_msgLen = 0;
    memset(m_charName, 0, 0x1e);
    memset(m_msg, 0, 0x100);
}

Packet_Monitor_Notice_Delete_Member_Id::Packet_Monitor_Notice_Delete_Member_Id()
    : PacketHeader(0x4bd, 0x12)
{
}

Packet_Monitor_SAVE_Member_Update_Char_Info::
    Packet_Monitor_SAVE_Member_Update_Char_Info()
    : PacketHeader(0x4b5, 0xf)
{
}

Packet_Monitor_SAVE_Member::Packet_Monitor_SAVE_Member() : PacketHeader(0x4b4, 0x14)
{
}

Packet_Monitor_Request_Member_Enter_To_Requester::
    Packet_Monitor_Request_Member_Enter_To_Requester()
    : PacketHeader(0x4b7, 0x31)
{
    memset(m_name, 0, 0x1e);
}

Packet_Monitor_Request_Member_Enter_To_Responser::
    Packet_Monitor_Request_Member_Enter_To_Responser()
    : PacketHeader(0x4b8, 0x32)
{
}

Packet_Monitor_Member_Enter_Reply_ToResponser::
    Packet_Monitor_Member_Enter_Reply_ToResponser()
    : PacketHeader(0x4b9, 0x39)
{
    m_extraCharNo = 0;
    memset(m_name, 0, 0x1e);
}

Packet_Monitor_Notice_Member_Enter_Ok::Packet_Monitor_Notice_Member_Enter_Ok()
    : PacketHeader(0x4ba, 0x39)
{
    m_extraCharNo = 0;
    memset(m_name, 0, 0x1e);
}

Packet_Monitor_Member_Secede_To_Seceder::Packet_Monitor_Member_Secede_To_Seceder()
    : PacketHeader(0x4bc, 0x31)
{
    memset(m_name, 0, 0x1e);
}

Packet_Monitor_Member_Pay_Tax_ToUpper::Packet_Monitor_Member_Pay_Tax_ToUpper()
    : PacketHeader(0x4c0, 0x38)
{
    memset(m_name, 0, 0x1e);
}

Packet_Notice_Find_Factory_Hub_User::Packet_Notice_Find_Factory_Hub_User()
    : PacketHeader(0x100f, 0x34)
{
    m_idByChannel = 0;
    m_nameLen = 0;
    m_found = 0;
    m_field2e = 0;
    m_field30 = 0;
    memset(m_name, 0, 0x1e);
}

Packet_DBMW_Register_To_BlackList::Packet_DBMW_Register_To_BlackList()
    : PacketHeader(0x5de, 0x34)
{
    m_charNo2 = 0xffffffff;
    memset(m_name, 0, 0x1e);
}

Packet_Register_To_BlackList_RESULT::Packet_Register_To_BlackList_RESULT()
    : PacketHeader(0x5dc, 0x31)
{
    memset(m_name, 0, 0x1e);
}

Packet_DMBW_Delete_To_BlackList::Packet_DMBW_Delete_To_BlackList()
    : PacketHeader(0x5df, 0x30)
{
    m_charNo2 = 0xffffffff;
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

Packet_Web_Notice_InGame_Advertisement::Packet_Web_Notice_InGame_Advertisement()
    : PacketHeader(0x27e2, 10)
{
}

Packet_Monitor_Notice_Buddy_In_Out::Packet_Monitor_Notice_Buddy_In_Out()
    : PacketHeader(0x3ef, 0x34)
{
    m_charNo = 0;
    m_idByChannel = 0xffffffff;
    m_channel = 0xff;
    m_flag3 = 0;
    memset(m_name, 0, 0x1e);
}

Packet_DBMW_Request_BlackList::Packet_DBMW_Request_BlackList()
    : PacketHeader(0x5e1, 0xf)
{
}

Packet_DB_Member_Delete_As_Charac_Delete::Packet_DB_Member_Delete_As_Charac_Delete()
    : PacketHeader(0x4c3, 0xe)
{
}

Packet_DBMW_Query_Buddy_Info::Packet_DBMW_Query_Buddy_Info()
    : PacketHeader(0x676, 0xe)
{
}

Packet_Request_Charac_Tower_Ranking::Packet_Request_Charac_Tower_Ranking()
    : PacketHeader(0x4cb, 0x62)
{
    memset(m_scores, 0, 0x50);
}

Packet_Send_Time_Sync_For_Login::Packet_Send_Time_Sync_For_Login()
    : PacketHeader(0x1f4c, 0x16)
{
    m_hour = 0;
    m_min = 0;
}

Packet_Tcp_Server_Connect::Packet_Tcp_Server_Connect() : PacketHeader(0x3f8, 0xb) {}

Packet_Monitor_Add_Buddy_Reply::Packet_Monitor_Add_Buddy_Reply()
    : PacketHeader(0x672, 0x3b)
{
    memset(m_name, 0, 0x27);
}

Packet_Monitor_Del_Buddy_Reply::Packet_Monitor_Del_Buddy_Reply()
    : PacketHeader(0x674, 0x31)
{
    memset(m_name, 0, 0x1e);
}

Packet_DBMW_Connection_Check::Packet_DBMW_Connection_Check()
    : PacketHeader(0x413, 0xb)
{
}

Packet_Monitor_Notice_Black_List::Packet_Monitor_Notice_Black_List()
    : PacketHeader(0x5e2, 0x3b)
{
    memset(m_charNos, 0xff, 0x28);
}

Packet_CollectItemsReward::Packet_CollectItemsReward() : PacketHeader(0x27e8, 0x32)
{
    m_idByChannel = 0;
    m_charNo = 0;
    m_type = 0;
    m_nameLen = 0;
    memset(m_name, 0, 0x1e);
}

Packet_CollectItemsRewardBroadcast::Packet_CollectItemsRewardBroadcast()
    : PacketHeader(0x27e9, 0x32)
{
    m_fieldA = 0;  // +10 语义未知（SendColletItemsReward 未赋值），保留原名
    m_charNo = 0;
    m_type = 0;
    m_nameLen = 0;
    memset(m_name, 0, 0x1e);
}

Packet_MTG_OntimeEvent_RewardStart::Packet_MTG_OntimeEvent_RewardStart()
    : PacketHeader(0x17c0, 0x1a)
{
    m_eventIdx = 0;
    m_remainSec = 0;
    m_itemIndex = 0;
    m_itemCount = 0;
}

Packet_MTG_OntimeEvent_RewardEnd::Packet_MTG_OntimeEvent_RewardEnd()
    : PacketHeader(0x17c2, 0xa)
{
}

Packet_Arad_DeleteEffect::Packet_Arad_DeleteEffect(int group, int code)
    : PacketHeader(0x27fa, 0x12)
{
    m_group = group;
    m_code = code;
}

Packet_DB_InsertMail::Packet_DB_InsertMail() : PacketHeader(0x177c, 0x133)
{
    m_characNo = 0;
    m_fieldE = 0;
    m_field12 = 0;
    m_field16 = 0;
    m_delayHours = 0;
    int i = 0;
    for (i = 0; (unsigned)i <= 0x14; i++)
    {
        m_subject[i] = 0;
    }
    for (i = 0; i <= 0xff; i++)
    {
        m_content[i] = 0;
    }
}

Packet_DBMW_Statistic_Login_Logout::Packet_DBMW_Statistic_Login_Logout()
    : PacketHeader(0x17b8, 0x618),
      m_channelNo(0),
      m_occCount0(0),
      m_occCount4(0),
      m_loginCount(0),
      m_logoutCount(0)
{
    memset(m_entries, 0, sizeof(m_entries));
}

Packet_Monitor_Call_Member_List_ToUser::Packet_Monitor_Call_Member_List_ToUser()
    : PacketHeader(0x4be, 0x1e1), m_memberList()
{
}

Packet_Punish_Cancel::Packet_Punish_Cancel() : PacketHeader(0xb64, 0x12)
{
    m_idByChannel = 0;
    m_type = 0;
    m_param = 0;
}

Packet_Set_CleanPad_Point::Packet_Set_CleanPad_Point() : PacketHeader(0xb60, 0x10)
{
    m_idByChannel = 0;
    m_point = 0;
}

Packet_SecuService_Connect_Web::Packet_SecuService_Connect_Web()
    : PacketHeader(0xb65, 0x15)
{
    m_idByChannel = 0;
    m_fieldE = 0;  // 整包透传，语义未知，保留原名
    m_fieldF = 0;
    memset(m_payload, 0, 5);
}

Packet_Monitor_User_Repel::Packet_Monitor_User_Repel() : PacketHeader(0x4c1, 0x12) {}

Packet_Monitor_Other_Channel_Chat_ToUser::Packet_Monitor_Other_Channel_Chat_ToUser()
    : PacketHeader(0x3f3, 0x137)
{
    m_serverId = 0;
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_type = 0;
    m_msgLen = 0;
    memset(m_name, 0, 0x1e);
    memset(m_msg, 0, 0x100);
}

Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link::
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link()
    : PacketHeader(0x2719, 0x270)
{
    m_serverId = 0;
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_type = 0;
    m_itemCount = 0;
    m_msgLen = 0;
    memset(m_name, 0, 0x1e);
    memset(m_msg, 0, 0x100);
    memset(m_items, 0, 0x138);
}

Packet_Change_User_Handicap::Packet_Change_User_Handicap() : PacketHeader(0x3f7, 0x16) {}

Packet_Web_Request_ARS_Info::Packet_Web_Request_ARS_Info() : PacketHeader(0xb62, 0xb) {}

Packet_Arad_ApplyEffect::Packet_Arad_ApplyEffect(int group, int code, unsigned int time)
    : PacketHeader(0x27f9, 0x16)
{
    m_group = group;
    m_code = code;
    m_time = time;
}

Packet_Load_Periodic_Message::Packet_Load_Periodic_Message() : PacketHeader(0x1f48, 10) {}

Packet_CollectItemsUpdate::Packet_CollectItemsUpdate() : PacketHeader(0x27ea, 0x14)
{
    m_fieldA = 0;
    m_fieldE = 0;
    m_fieldF = 0;
    m_field13 = 0;
}

Packet_CollectItemsResult::Packet_CollectItemsResult() : PacketHeader(0x27e7, 0x16)
{
    m_fieldA = 0;
    m_fieldE = 0;
    m_field12 = 0;
}

Packet_Server_Queue_Load_Statistic::Packet_Server_Queue_Load_Statistic()
    : PacketHeader(0x9d2, 0xe)
{
    m_flag = 0;
    m_param = 0;
    m_value = 0;
}

Packet_InnerPakcet_Login::Packet_InnerPakcet_Login() : PacketHeader(0xfa0, 0xa) {}

Packet_InnerPakcet_Logout::Packet_InnerPakcet_Logout() : PacketHeader(0xfa1, 0xa) {}

Packet_Monitor_UDP_HeartBeat::Packet_Monitor_UDP_HeartBeat() : PacketHeader(0x3ec, 0xb)
{
    m_channel = 0xff;
}

Packet_MiniCraneSeed::Packet_MiniCraneSeed() : PacketHeader(0x27f8, 0xe)
{
    m_seed = 0;
}

Packet_DB_Query_Member::Packet_DB_Query_Member() : PacketHeader(0x4b2, 0xe)
{
    m_memberKey = 0;
}

Packet_DB_Query_Member_Member::Packet_DB_Query_Member_Member() : PacketHeader(0x4b0, 0xe)
{
    m_memberKey = 0;
}

Packet_Send_Time_Sync::Packet_Send_Time_Sync() : PacketHeader(0x1f4b, 0xe)
{
    m_hour = 0;
    m_min = 0;
}
