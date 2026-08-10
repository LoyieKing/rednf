// df_monitor_r — DNFPacketTranslater（从 MonitorTypes/App/Table 拆分）
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

#include "DNFPacketTranslater.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
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
    pkt.m_fieldA = a;
    pkt.m_fieldB = b;
    pkt.m_fieldC = c;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_fieldD = d;
    memcpy(pkt.m_name, other->GetCharName(), 0x1d);
    pkt.m_fieldE = e;
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
        pkt.m_fieldB = 3;
    }
    else
    {
        pkt.m_fieldB = b;
    }
    pkt.m_fieldA = a;
    pkt.m_fieldC = c;
    pkt.m_idByChannel = user->GetIdByChannel();
    pkt.m_uniqCharNo = user->GetUniqCharNo();
    pkt.m_fieldD = d;
    memcpy(pkt.m_name, other->GetCharName(), 0x1d);
    pkt.m_fieldE = e;
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
    *(unsigned int*)((char*)&pkt + 0xa) = user->GetIdByChannel();
    *(unsigned int*)((char*)&pkt + 0xe) = user->GetUniqCharNo();
    *(unsigned char*)((char*)&pkt + 0x12) = result;
    memcpy((char*)&pkt + 0x13, name, 0x1d);
    user->SendTcpGameserver(&pkt);
}

void CPacketTranslater::OnLogin(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            unsigned int channel =
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x12);
            CTcpGameServer* tcpGs =
                (CTcpGameServer*)m_pclApp->FindTcpGameServer(channel);
            if (tcpGs != 0)
            {
                CServerInterface* gs =
                    (CServerInterface*)m_pclApp->FindGameServer((int)channel);
                if (gs == 0)
                {
                    char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 0);
                    DNF_LOG_SCOPE_LINE(0x129,"./log/Channel", "Not Found M_ID(%s) Channel No(%d)", dbid,
                        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x12));
                }
                else
                {
                    CLoginLogoutStatistics* stats =
                        (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                    if (stats != 0)
                    {
                        stats->CountNumOfLoginout((ENUM_LOGIN_LOGOUT)0);
                    }
                    char* pktBuf = tcpGs->makePacketHeader(1000, 0x33);
                    char* outBuf = 0;
                    if (pktBuf != 0)
                    {
                        outBuf = pktBuf;
                        memcpy(pktBuf + 10, (char*)pkt + 0xa, 0x29);
                    }
                    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
                    CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xe));
                    if (user == 0)
                    {
                        if (userMgr->FindProhibitUser(
                                *(unsigned int*)((char*)pkt + 0xe)) == 0)
                        {
                            user = userMgr->CreateUser(
                                *(unsigned int*)((char*)pkt + 0xe), 0, "",
                                *(int*)((char*)pkt + 0xa), (CGameServer*)gs);
                            user->SetSex(*(unsigned char*)((char*)pkt + 0x23));
                            DNF_LOG_SCOPE_LINE(0x198,"./log/User", "OnLogin - SetSex : %d",
                                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x23));
                            user->SetSsn((char*)pkt + 0x2c);
                            user->SetTcpGameServer(tcpGs);
                            outBuf[0x23] = 1;
                            char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 0);
                            CMyFileLog log2("OnLogin", 0x1a5);
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
                                RequestBlackListToDBMW(
                                    *(unsigned int*)((char*)pkt + 0xe));
                            }
                        }
                        else
                        {
                            if (pktBuf != 0)
                            {
                                outBuf[0x23] = 3;
                            }
                            unsigned int ch = gs->GetChannelNo();
                            char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 0);
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
                            char* oldBuf = oldGs->makePacketHeader(0x3ee, 0xe);
                            if (oldBuf != 0)
                            {
                                *(unsigned int*)(oldBuf + 10) =
                                    *(unsigned int*)((char*)pkt + 0xe);
                                oldGs->SendToGameServer(oldBuf);
                            }
                        }
                        unsigned int ch = gs->GetChannelNo();
                        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 1);
                        char* oldDbid = NumberToString(user->GetDBID(), 0);
                        DNF_LOG_SCOPE_LINE(0x17a,"./log/User",
                            "DOUBLE CONNECTED : Already User DB ID(%s)\tCurrent Connect User "
                            "DB ID(%s)\tChannel(%d)\n",
                            oldDbid, dbid, ch & 0xff);
                        if (pktBuf != 0)
                        {
                            outBuf[0x23] = 2;
                        }
                    }
                    if (pktBuf != 0)
                    {
                        outBuf[0x28] = 0;
                        outBuf[0x29] = 0;
                        outBuf[0x2a] = 0;
                        outBuf[0x2b] = 0;
                        outBuf[0x24] = 0;
                        outBuf[0x25] = 0;
                        outBuf[0x26] = 0;
                        outBuf[0x27] = 0;
                    }
                    exchange_server::CACHE_CHARACTER_TYPE cacheType;
                    if (exchange_server::GetInstanceCacheCharacterMgr()->GetCacheCharacter(
                            *(unsigned int*)((char*)pkt + 0xe), &cacheType) != 0 &&
                        pktBuf != 0)
                    {
                        *(unsigned int*)(outBuf + 0x28) =
                            (unsigned int)cacheType.m_field4;
                        *(unsigned int*)(outBuf + 0x24) =
                            (unsigned int)cacheType.m_field0;
                    }
                    if (tcpGs != 0 && outBuf != 0)
                    {
                        tcpGs->SendToGameServer(outBuf);
                    }
                }
            }
        }
        catch (CDNFException& e)
        {
            DNF_LOG_SCOPE_LINE(0x1ff, "%s", "CPacketTranslater::OnLogin() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            DNF_LOG_SCOPE_LINE(0x204, "%s", "CPacketTranslater::OnLogin() Exception Break");
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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CMemberManager* memberMgr = m_pclApp->Get_MemberManager();
        CLoginLogoutStatistics* stats =
            (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
        if (stats != 0 && *(char*)((char*)pkt + 0x17) == 0)
        {
            stats->CountNumOfLoginout((ENUM_LOGIN_LOGOUT)6);
        }
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
            DNF_LOG_SCOPE_LINE(0x22d,"./log/User", "LOGOUT ERR : User DB ID(%s), F.O.C(%d), Ch(%d)", dbid,
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x17),
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
            return;
        }
        char* name = user->GetCharName();
        unsigned int memberKey = user->GetMemberKey();
        unsigned int charNo = user->GetUniqCharNo();
        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
        DNF_LOG_SCOPE_LINE(0x230,"./log/User",
            "LOGOUT : User DB ID(%s), Char No(%d), Member K(%d) , name(%s), F.O.C(%d), Ch(%d)",
            dbid, charNo, memberKey, name,
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x17),
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
        unsigned int memberKey2 = *(unsigned int*)((char*)pkt + 0x18);
        CMemoryCashManager* cash = (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
        CMember* member = memberMgr->FindMember(*(unsigned int*)((char*)pkt + 0xa));
        bool f1 = false;
        bool f2 = false;
        cash->InsertCashMemorySetCharacterObject(user, member, f1, f2);
        if (memberKey2 != 0)
        {
            memberMgr->MemberMemLogout(memberKey2, user, !f2);
        }
        if (user->GetUniqCharNo() != 0)
        {
            if (userMgr->DeleteUser_CharNo(user->GetUniqCharNo()) == 1)
            {
                user->GetUniqCharNo();
                m_pclApp->Remove_GM_id(user->GetUniqCharNo());
                unsigned int charNos[32];
                int n = user->GetBuddysCharNo(charNos);
                CBuddyRegisterManager* buddyReg =
                    (CBuddyRegisterManager*)((char*)m_pclApp + 0x300);
                for (int i = 0; i < n; i++)
                {
                    buddyReg->delBuddyRegister(charNos[i], user->GetUniqCharNo());
                }
                std::vector<unsigned int> vec;
                buddyReg->findBuddyRegister(user->GetUniqCharNo(), vec);
                for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end();
                     ++it)
                {
                    CUser* other = userMgr->FindUser_CharNo(*it);
                    if (other != 0)
                    {
                        other->SendNoticeBuddyInOut(
                            ((CServerInterface*)other->GetGameServer())->GetChannelNo(),
                            user->GetUniqCharNo(), user->GetCharName(),
                            (unsigned char)(user->IsBlackUser(*it) != 0), 0, 0);
                    }
                }
                exchange_server::CACHE_CHARACTER_TYPE cacheType;
                cacheType.m_field0 = *(int*)((char*)pkt + 0xf);
                cacheType.m_field4 = (int)(unsigned char)*(char*)((char*)pkt + 0xe);
                char* dbid2 = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                CMyFileLog log2("OnLogout", 0x290);
                log2("./log/ExchangeServer", "CacheCharacter() (%s,%d,%d)\n", dbid2,
                     cacheType.m_field0, cacheType.m_field4);
                if (*(char*)((char*)pkt + 0x3a) != 0)
                {
                    exchange_server::GetInstanceCacheCharacterMgr()->CacheCharacter(
                        *(unsigned int*)((char*)pkt + 0xa), &cacheType);
                }
                CLoginLogoutStatistics* stats2 =
                    (CLoginLogoutStatistics*)m_pclApp->GetLoginLogoutStatistics();
                if (stats2 != 0)
                {
                    stats2->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)4,
                                                  (int)userMgr->GetSizeOfCharnoUsers());
                }
            }
            userMgr->DeleteUser_CharName(user->GetCharName());
            user->ResetCharInfo(!f1);
        }
        if (*(char*)((char*)pkt + 0x17) != 0)
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
                (unsigned char)*(char*)((char*)pkt + 0xe))
        {
            if (userMgr->DeleteUser(user) != 1)
            {
                char* dbid3 = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                CMyFileLog log3("OnLogout", 0x2c5);
                log3("./log/User",
                     "[NO USER] Disconnected User DB ID : %s, Char No : %d , char name:%s\n",
                     dbid3, user->GetUniqCharNo(), user->GetCharName());
            }
        }
        else if (user->GetGameServer() != 0)
        {
            unsigned char alreadyCh =
                ((CServerInterface*)user->GetGameServer())->GetChannelNo();
            unsigned char logoutCh = (unsigned char)*(char*)((char*)pkt + 0xe);
            char* dbid4 = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
            CMyFileLog log4("OnLogout", 0x2cc);
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
            stats3->CountNumOfOccupations((ENUM_LOGIN_LOGOUT)0, (int)userMgr->Size());
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x2eb, "%s", "CPacketTranslater::OnLogout() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnLogout() Exception Break");
        DNF_LOG_SCOPE_LINE(0x2f1, "%s", "CPacketTranslater::OnLogout() Exception Break");
    }
}

void CPacketTranslater::OnReplyUserInfo(PacketHeader* pkt)
{
    try
    {
        DNF_LOG_SCOPE_LINE(0x361,"./log/Reboot", "[GAME SERVER] Channel No : %d\n",
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xb));
        if (m_pclApp == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : m_pclApp == 0");
        }
        CGameServer* gs =
            (CGameServer*)m_pclApp->FindGameServer(
                (int)(unsigned char)*(char*)((char*)pkt + 0xb));
        if (gs == 0)
        {
            throw CDNFException("CPacketTranslater::OnReplyUserInfo : pclGameServer == 0");
        }
        CTcpGameServer* tcpGs =
            (CTcpGameServer*)m_pclApp->FindTcpGameServer(
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xb));
        if (tcpGs != 0)
        {
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            for (int i = 0;
                 i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa); i++)
            {
                char* entry = (char*)pkt + i * 0x4e + 0xc;
                if (*(int*)entry != 0)
                {
                    CUser* user = userMgr->FindUser(*(unsigned int*)entry);
                    if (user == 0)
                    {
                        user = userMgr->CreateUser(
                            *(unsigned int*)entry, *(unsigned int*)(entry + 4),
                            entry + 0x14, *(int*)(entry + 8), gs);
                        user->SetUserInfo_CharNo(
                            *(char*)(entry + 0x10), *(char*)(entry + 0x11),
                            *(short*)(entry + 0x12), *(unsigned int*)(entry + 4),
                            entry + 0x14);
                        user->SetSex(*(unsigned char*)(entry + 0x46));
                        user->SetSsn(entry + 0x47);
                        user->SetTcpGameServer(tcpGs);
                        CMemoryCashManager* cash =
                            (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                        if (cash->QueryCashMemoryBlackList(user) != 1)
                        {
                            RequestBlackListToDBMW(*(unsigned int*)entry);
                        }
                        if (*(int*)(entry + 0x32) != 0)
                        {
                            CMemoryCashManager* cash2 =
                                (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                            if (cash2->QueryCashMemoryMember(user) != 1)
                            {
                                ((CMemberManager*)((char*)m_pclApp + 0x2d0))
                                    ->MemerMemLogin(*(unsigned int*)(entry + 0x32), user);
                            }
                        }
                        CMemoryCashManager* cash3 =
                            (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                        if (cash3->QueryCashMemoryBuddyInfo(user) != 1)
                        {
                            user->QueryBuddyInfo(
                                (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0));
                        }
                    }
                    else
                    {
                        char* dbid = NumberToString(*(unsigned int*)entry, 0);
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
        DNF_LOG_SCOPE_LINE(0x3ac, "%s", "CPacketTranslater::OnReplyUserInfo() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnReplyUserInfo() Exception Break");
        DNF_LOG_SCOPE_LINE(0x3b2, "%s", "CPacketTranslater::OnReplyUserInfo() Exception Break");
    }
}

void CPacketTranslater::OnHeartBeat(PacketHeader* pkt)
{
    if (m_pclApp != 0 &&
        (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0) != 0)
    {
        try
        {
            CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
            unsigned char channel = *(unsigned char*)((char*)pkt + 0xa);
            if (channel == 0xc8)
            {
                handler->ResetDBHeartBeat();
                if (handler->IsConnectedDBServer() != 1)
                {
                    handler->SetDBConnectFlag(true);
                    handler->SendDBMWConnectionCheck();
                    DNF_LOG_SCOPE_LINE(0x318, "./log/DBHeartBeat", "DB Server Connection Complete!");
                }
            }
            else if (channel == 0 || 0xbe < channel)
            {
                DNF_LOG_SCOPE_LINE(0x341,"./log/Except", "[ERROR - HEART BEAT] Channel Index(%d) Over.",
                    (unsigned int)channel);
            }
            else
            {
                handler->ResetHeartBeat(channel);
                if (handler->IsConnectedGameServer(channel) != 1)
                {
                    handler->SetConnectFlag(channel, true);
                    Packet_Tcp_Server_Connect pkt2;
                    pkt2.m_channel = 0xc9;
                    CServerInterface* gs = handler->GetGameServer((unsigned int)channel);
                    if (gs == 0)
                    {
                        DNF_LOG_SCOPE_LINE(0x337,"./log/Except",
                            "CPacketTranslater::OnHeartBeat() => Channel Index : %d\n",
                            (unsigned int)channel);
                    }
                    else
                    {
                        gs->SendToServer((char*)&pkt2, 0xb);
                    }
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x348, "%s", "CPacketTranslater::OnHeartBeat() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CPacketTranslater::OnHeartBeat() Exception Break");
            DNF_LOG_SCOPE_LINE(0x34e, "%s", "CPacketTranslater::OnHeartBeat() Exception Break");
        }
    }
}

void CPacketTranslater::OnCharLogin(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                DNF_LOG_SCOPE_LINE(0x457,"./log/User",
                    "[CHAR_LOGIN_ERR]\tDB ID : %s\tChar Key : %d\tGuild Key : %d\tJob : "
                    "%d\tname : %s\n",
                    dbid, *(unsigned int*)((char*)pkt + 0xf),
                    *(unsigned int*)((char*)pkt + 0x13),
                    (int)(char)*(char*)((char*)pkt + 0x17), (char*)pkt + 0x1f);
            }
            else
            {
                user->SetUserInfo_CharNo(
                    *(char*)((char*)pkt + 0x17), *(char*)((char*)pkt + 0x18),
                    *(short*)((char*)pkt + 0x19), *(unsigned int*)((char*)pkt + 0xf),
                    (char*)pkt + 0x1f);
                char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                DNF_LOG_SCOPE_LINE(0x3cf,"./log/User",
                    "[CHAR_LOGIN]\tDB ID(%s)\tChar Key(%d)\tGuild K(%d)\tMember K(%d)\tJob(%d)"
                    "\tname(%s)\tCh No(%d)\treturn_user(%d)\n",
                    dbid, *(unsigned int*)((char*)pkt + 0xf),
                    *(unsigned int*)((char*)pkt + 0x13),
                    *(unsigned int*)((char*)pkt + 0x1b),
                    (int)(char)*(char*)((char*)pkt + 0x17), (char*)pkt + 0x1f,
                    (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe),
                    (int)(char)*(char*)((char*)pkt + 0x3e));
                user->SetUserPosState(3);
                CMemoryCashManager* cash =
                    (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                if (cash->QueryCashMemoryBuddyInfo(user) != 1)
                {
                    user->QueryBuddyInfo(
                        (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0));
                }
                std::vector<unsigned int> vec;
                ((CBuddyRegisterManager*)((char*)m_pclApp + 0x300))
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
                            *(char*)((char*)pkt + 0x3e));
                    }
                }
                if (*(int*)((char*)pkt + 0x1b) != 0)
                {
                    CMemoryCashManager* cash2 =
                        (CMemoryCashManager*)m_pclApp->Get_MemoryCashManager();
                    if (cash2->QueryCashMemoryMember(user) != 1)
                    {
                        ((CMemberManager*)((char*)m_pclApp + 0x2d0))
                            ->MemerMemLogin(*(unsigned int*)((char*)pkt + 0x1b), user);
                    }
                }
                if (userMgr->InsertUser_CharName((char*)pkt + 0x1f, user) != 1)
                {
                    char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                    DNF_LOG_SCOPE_LINE(0x3fd,"./log/Except",
                        "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!", dbid,
                        (char*)pkt + 0x1f);
                }
                if (userMgr->InsertUser_CharNo(*(unsigned int*)((char*)pkt + 0xf), user) != 1)
                {
                    DNF_LOG_SCOPE_LINE(0x401,"./log/Except",
                        "uDBID(%d) uCharName(%s) is already exist at m_mapCharNoUsers!",
                        *(unsigned int*)((char*)pkt + 0xf), (char*)pkt + 0x1f);
                }
                user->GetDBID();
                ((CMemoryCashManager*)m_pclApp->Get_MemoryCashManager())
                    ->DeleteCashObjecct(user->GetDBID());
                CTowerRank* tower = (CTowerRank*)m_pclApp->getTowerRank();
                stTowerRankElement_t elements[5];
                for (int i = 0; i < 5; i++)
                {
                    elements[i].m_job = 0;
                    elements[i].m_pad = 0;
                    elements[i].m_score = 0;
                }
                Packet_Request_Charac_Tower_Ranking rankPkt;
                *(unsigned int*)((char*)&rankPkt + 0xa) = user->GetIdByChannel();
                *(unsigned int*)((char*)&rankPkt + 0xe) =
                    *(unsigned int*)((char*)pkt + 0xf);
                char hasData = 0;
                for (int t = 0; t < 4; t++)
                {
                    unsigned int cnt =
                        tower->getRankData((unsigned int)(t + 1), user->GetCharName(), 5,
                                           elements);
                    for (unsigned int i = 0; i < cnt; i++)
                    {
                        *(unsigned int*)((char*)&rankPkt + 2 +
                                         ((unsigned int)(unsigned char)elements[i].m_job * 4 +
                                          (unsigned int)t + 4) *
                                             2) =
                            (unsigned int)elements[i].m_score;
                        hasData = 1;
                    }
                }
                if (hasData != 0)
                {
                    user->SendToGameserver(
                        (char*)&rankPkt, *(unsigned short*)((char*)&rankPkt + 2));
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
            DNF_LOG_SCOPE_LINE(0x463, "%s", "CPacketTranslater::OnCharLogin() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CPacketTranslater::OnCharLogin() Exception Break");
            DNF_LOG_SCOPE_LINE(0x469, "%s", "CPacketTranslater::OnCharLogin() Exception Break");
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
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    if (*(char*)((char*)pkt + 0x1b) != 0 &&
        (unsigned char)*(char*)((char*)pkt + 0x1b) < 0x1e)
    {
        CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x1c);
        *(unsigned int*)((char*)pkt + 0x17) =
            target != 0 ? target->GetUniqCharNo() : 0xffffffff;
    }
    if (*(unsigned int*)((char*)pkt + 0x13) == 0 ||
        *(unsigned int*)((char*)pkt + 0x17) == 0 ||
        *(char*)((char*)pkt + 0x3a) == 0)
    {
        DNF_LOG_SCOPE_LINE(0xb46,"./log/Except",
            "CPacketTranslater::OnNoticeOtherChannelChatMsg, sender(%d), receiver(%d), "
            "msglen(%d)",
            *(unsigned int*)((char*)pkt + 0x13), *(unsigned int*)((char*)pkt + 0x17),
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a));
        throw CDNFException(
            "CPacketTranslater::OnNoticeOtherChannelChatMsg : packet->m_uSenderCharID &&  "
            "packet->m_uRecverCharID && packet->m_msgLen");
    }
    Packet_Monitor_Other_Channel_Chat_ToUser reply;
    reply.m_senderCharId = *(unsigned int*)((char*)pkt + 0xa);
    CUser* sender = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x13));
    if (sender == 0)
    {
        return;
    }
    CUser* receiver = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x17));
    if (receiver == 0)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        reply.m_type = 1;
        *(unsigned short*)((char*)&reply + 2) = 0x37;
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
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        *(unsigned short*)((char*)&reply + 2) = 0x37;
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
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        *(unsigned short*)((char*)&reply + 2) = 0x37;
        sender->SendToGameserver((char*)&reply, 0x37);
    }
    else
    {
        memcpy(reply.m_name, sender->GetCharName(), 0x1d);
        reply.m_idByChannel = receiver->GetIdByChannel();
        reply.m_uniqCharNo = receiver->GetUniqCharNo();
        reply.m_msgLen = *(unsigned char*)((char*)pkt + 0x3a);
        memcpy(reply.m_msg, (char*)pkt + 0x3b,
               (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a));
        *(unsigned short*)((char*)&reply + 2) =
            (unsigned short)((unsigned char)*(char*)((char*)pkt + 0x3a) + 0x37);
        receiver->SendToGameserver((char*)&reply,
                                   *(unsigned short*)((char*)&reply + 2));
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


    CUser* user =
        ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
            *(unsigned int*)((char*)pkt + 0xa));
    if (user != 0)
    {
        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
        DNF_LOG_SCOPE_LINE(0x47f, "./log/User", "Cera Payed User , DB ID : %s\n", dbid);
        void* gs = user->GetGameServer();
        if (gs == 0)
        {
            CMyFileLog log2("OnCeraUpdate", 0x48a);
            log2("./log/Except", "CPacketTranslater::OnCeraUpdate : pUser->GetGameServer() == 0",
                 dbid);
        }
        else
        {
            ((CServerInterface*)gs)->SendToServer((char*)pkt, 0xe);
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
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
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
                char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
                DNF_LOG_SCOPE_LINE(0x4b0,"./log/User",
                    "Fail: Event User DB ID : %s [EventType: %d][CharacNo: %d][ItemID: "
                    "%d][Stack: %d]\n",
                    dbid, *(unsigned int*)((char*)pkt + 0x12),
                    *(unsigned int*)((char*)pkt + 0xe),
                    *(unsigned int*)((char*)pkt + 0x16),
                    *(unsigned int*)((char*)pkt + 0x1a));
                return;
            }
        }
        else
        {
            gs = (CServerInterface*)user->GetGameServer();
        }
        unsigned char channel = gs->GetChannelNo();
        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
        DNF_LOG_SCOPE_LINE(0x4b7,"./log/User",
            "Event User DB ID : %s [EventType: %d][CharacNo: %d][ItemID: %d][Stack: "
            "%d][TableID: %d][Channel No: %d]\n",
            dbid, *(unsigned int*)((char*)pkt + 0x12),
            *(unsigned int*)((char*)pkt + 0xe), *(unsigned int*)((char*)pkt + 0x16),
            *(unsigned int*)((char*)pkt + 0x1a), *(unsigned int*)((char*)pkt + 0x1e),
            (unsigned int)channel);
        if (gs != 0)
        {
            gs->SendToServer((char*)pkt, 0x22);
        }
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventItemUpdate() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x4dd, "%s", "CPacketTranslater::OnEventItemUpdate() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventItemUpdate() Exception Break");
        DNF_LOG_SCOPE_LINE(0x4e3, "%s", "CPacketTranslater::OnEventItemUpdate() Exception Break");
    }
}

void CPacketTranslater::OnReplyQueryMember(PacketHeader* pkt)
{try
{


    if (*(char*)((char*)pkt + 0xa) == 1)
    {
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        if (handler != 0)
        {
            char ok = ((CMemberManager*)((char*)m_pclApp + 0x2d0))
                          ->LoadMember(*(unsigned int*)((char*)pkt + 0xb),
                                       *(STMemberDBInfo*)((char*)pkt + 0x17),
                                       *(unsigned int*)((char*)pkt + 0xf),
                                       *(unsigned int*)((char*)pkt + 0x13), handler);
            if (ok != 1)
            {
                DNF_LOG_SCOPE_LINE(0x4e5,"./log/MemberMember",
                    "CHECK MEMBER ID: CPacketTranslater::OnReplyQueryMember()\t"
                    "m_clMemberManager.LoadMember()\tmember id(%d)",
                    *(unsigned int*)((char*)pkt + 0xb));
            }
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0x4eb,"./log/Except",
            "[DB ERROR]CPacketTranslater::OnReplyQueryMember() packet->bSuccess : %d\n",
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa));
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
}

void CPacketTranslater::OnRequestMemberEnter(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        return;
    }
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
    CUser* requester = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xe));
    if (requester == 0)
    {
        return;
    }
    CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x12);
    if (target == 0)
    {
        SendRequestMemberEnterResult(requester, '1', (char*)pkt + 0x12);
        return;
    }
    if (memberMgr->IsAlreadyMemberMember(*(unsigned int*)((char*)pkt + 0xe),
                                         target->GetUniqCharNo()) != 0)
    {
        SendRequestMemberEnterResult(requester, '2', (char*)pkt + 0x12);
        DNF_LOG_SCOPE_LINE(0x599,"./log/MemberModify", "Err Already Member : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    if (requester->IsAbleToRegisterMember() != 1 || target->IsAbleToRegisterMember() != 1)
    {
        SendRequestMemberEnterResult(requester, '7', (char*)pkt + 0x12);
        DNF_LOG_SCOPE_LINE(0x5a2,"./log/MemberModify",
            "Err Member Register Restrict : requester(%d:%d) responser(%d:%d)",
            requester->GetUniqCharNo(), requester->IsAbleToRegisterMember(),
            target->GetUniqCharNo(), target->IsAbleToRegisterMember());
        return;
    }
    if (requester->IsBlackUser(target->GetUniqCharNo()) != 0)
    {
        SendRequestMemberEnterResult(requester, '6', (char*)pkt + 0x12);
        DNF_LOG_SCOPE_LINE(0x5a9,"./log/MemberModify", "Err Member Register Black : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && gm->isGM(target->GetDBID()) != 0)
    {
        SendRequestMemberEnterResult(requester, 'Z', (char*)pkt + 0x12);
        DNF_LOG_SCOPE_LINE(0x5b4,"./log/MemberModify", "Err Member Register GM : requester(%d) responser(%d)",
            requester->GetUniqCharNo(), target->GetUniqCharNo());
        return;
    }
    CMember* requesterMember = memberMgr->FindMember(requester->GetUniqCharNo());
    CMember* targetMember = memberMgr->FindMember(target->GetUniqCharNo());
    if (target->GetMemberEnterCallerId() != 0)
    {
        SendRequestMemberEnterResult(requester, ')', (char*)pkt + 0x12);
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
        SendRequestMemberEnterResult(requester, ')', (char*)pkt + 0x12);
    }
    else
    {
        int err = memberMgr->CheckMemberEnter(requester, requesterMember, target, targetMember);
        if (err == 0)
        {
            if (target->RecordCallMemberEnter(*(unsigned int*)((char*)pkt + 0xe), 1) == 1)
            {
                SendRequestMemberEnterResult(requester, 0, (char*)pkt + 0x12);
                Packet_Monitor_Request_Member_Enter_To_Responser rpkt;
                rpkt.m_idByChannel = target->GetIdByChannel();
                rpkt.m_uniqCharNo = target->GetUniqCharNo();
                rpkt.m_level = (unsigned short)requester->GetLevel();
                memcpy(rpkt.m_name, requester->GetCharName(), 0x1d);
                target->SendTcpGameserver(&rpkt);
            }
            else
            {
                SendRequestMemberEnterResult(requester, ')', (char*)pkt + 0x12);
            }
        }
        else
        {
            SendRequestMemberEnterResult(requester, (unsigned char)err, (char*)pkt + 0x12);
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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
        CUser* requester = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xe));
        if (requester != 0)
        {
            requester->GetMemberEnterCallerId();
            CUser* responser = userMgr->FindUser_CharNo(requester->GetMemberEnterCallerId());
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
                    unsigned char code = *(unsigned char*)((char*)pkt + 0x12);
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
                            memberMgr->FindMember(responser->GetUniqCharNo());
                        CMember* requesterMember =
                            memberMgr->FindMember(requester->GetUniqCharNo());
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
                            int err = memberMgr->CheckMemberEnter(
                                responser, responserMember, requester, requesterMember);
                            if (err == 0)
                            {
                                CServerHandler* handler =
                                    (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
                                if (handler != 0)
                                {
                                    if (responserMember == 0)
                                    {
                                        responserMember =
                                            memberMgr->CreateMemberInJoin(responser);
                                    }
                                    if (requesterMember == 0)
                                    {
                                        requesterMember =
                                            memberMgr->CreateMemberInJoin(requester);
                                    }
                                    short rl = responser->GetLevel();
                                    if (memberMgr->RegisterMember(responserMember, rl, requester,
                                                                  true) == 1)
                                    {
                                        short ql = requester->GetLevel();
                                        if (memberMgr->RegisterMember(requesterMember, ql,
                                                                      responser, true) == 1)
                                        {
                                            memberMgr->SendToDBMemberUpdateCharInfo(
                                                handler, responser->GetUniqCharNo(), 1);
                                            memberMgr->SendToDBMemberUpdateCharInfo(
                                                handler, requester->GetUniqCharNo(), 1);
                                            memberMgr->SaveMemberOnConnect(
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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
        unsigned int secederCharNo = *(unsigned int*)((char*)pkt + 0xe);
        CUser* seceder = userMgr->FindUser_CharNo(secederCharNo);
        if (seceder != 0)
        {
            CMember* member = memberMgr->FindMember(secederCharNo);
            if (member == 0)
            {
                SendRequestMemberDeleteResult(seceder, '1', (char*)pkt + 0x13);
            }
            else
            {
                CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
                if (handler != 0)
                {
                    CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x13);
                    unsigned char result = 0;
                    unsigned int targetKey = 0;
                    if (target == 0)
                    {
                        result = (unsigned char)member->DeleteMemberByName((char*)pkt + 0x13,
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
                        spkt.m_type = 2;
                        if (result == 1)
                        {
                            spkt.m_type = 1;
                        }
                        memcpy(spkt.m_name, seceder->GetCharName(), 0x1d);
                        target->SendTcpGameserver(&spkt);
                    }
                    SendRequestMemberDeleteResult(seceder, result, (char*)pkt + 0x13);
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
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
            CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xe));
            if (user != 0)
            {
                user->GetUniqCharNo();
                CMember* member = memberMgr->FindMember(user->GetUniqCharNo());
                if (member != 0)
                {
                    Packet_Monitor_Call_Member_List_ToUser rpkt;
                    rpkt.m_idByChannel = user->GetIdByChannel();
                    rpkt.m_uniqCharNo = user->GetUniqCharNo();
                    unsigned char* db = member->GetMemberDBInfo();
                    CUser* upperUser = userMgr->FindUser_CharNo(*(unsigned int*)db);
                    if (upperUser == 0)
                    {
                        rpkt.m_upperChannel = 0xff;
                    }
                    else if (upperUser->GetGameServer() == 0)
                    {
                        rpkt.m_upperChannel = 0xff;
                    }
                    else
                    {
                        if (upperUser->IsBlackUser(user->GetUniqCharNo()) != 0)
                        {
                            rpkt.m_upperBlack = 1;
                        }
                        rpkt.m_upperChannel =
                            ((CServerInterface*)upperUser->GetGameServer())->GetChannelNo();
                    }
                    rpkt.m_upperLevel = *(unsigned char*)(db + 4);
                    memcpy(rpkt.m_upperName, db + 5, 0x1d);
                    rpkt.m_upperExp = *(unsigned int*)(db + 0x23);
                    unsigned int upperExp = rpkt.m_upperExp;
                    unsigned int upperExpNext = rpkt.m_upperExpNext;
                    unsigned char upperExpLevel = rpkt.m_upperExpLevel;
                    memberMgr->GetMemberExpNextLevelNeedExpLevel(
                        upperExp, upperExpNext, upperExpLevel);
                    rpkt.m_upperExp = upperExp;
                    rpkt.m_upperExpNext = upperExpNext;
                    rpkt.m_upperExpLevel = upperExpLevel;
                    rpkt.m_lowerCount = *(unsigned char*)(db + 0x27);
                    for (int i = 0; i < (int)(unsigned int)*(unsigned char*)(db + 0x27); i++)
                    {
                        char* entry = rpkt.m_lowers[i];
                        unsigned int lowerCharNo = *(unsigned int*)(db + i * 0x27 + 0x28);
                        CUser* lowerUser = userMgr->FindUser_CharNo(lowerCharNo);
                        if (lowerUser == 0)
                        {
                            entry[0] = 0xff;
                        }
                        else if (lowerUser->GetGameServer() == 0)
                        {
                            entry[0] = 0xff;
                        }
                        else
                        {
                            if (lowerUser->IsBlackUser(user->GetUniqCharNo()) != 0)
                            {
                                entry[0x20] = 1;
                            }
                            entry[0] =
                                ((CServerInterface*)lowerUser->GetGameServer())->GetChannelNo();
                        }
                        entry[1] = *(unsigned char*)(db + i * 0x27 + 0x2c);
                        memcpy(entry + 2, db + i * 0x27 + 0x2d, 0x1d);
                        *(unsigned int*)(entry + 0x22) =
                            *(unsigned int*)(db + i * 0x27 + 0x4b);
                        memberMgr->GetMemberExpNextLevelNeedExpLevel(
                            *(unsigned int*)(entry + 0x22), *(unsigned int*)(entry + 0x26),
                            *(unsigned char*)(entry + 0x21));
                    }
                    int size = (int)(unsigned int)*(unsigned char*)(db + 0x27) * 0x2a + 0x3d;
                    user->SendToGameserver((char*)&rpkt, size);
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CPacketTranslater::OnCallMemberList() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x7cd, "./log/Except", "%s", e.what());
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
    if (*(unsigned int*)((char*)pkt + 0xa) != 0 && *(char*)((char*)pkt + 0xe) != 0)
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            CMember* member =
                ((CMemberManager*)((char*)m_pclApp + 0x2d0))->FindMember(
                    *(unsigned int*)((char*)pkt + 0xa));
            if (member != 0)
            {
                member->NoticeChatMsgToMemberMembers(
                    (char*)pkt + 0xf, (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe),
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
        CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CMember* member = memberMgr->FindMember(*(unsigned int*)((char*)pkt + 0xa));
        if (member == 0)
        {
            DNF_LOG_SCOPE_LINE(0x842,"./log/Except",
                "[MEMBER] CPacketTranslater::OnPayTaxToUpper : pclMember == 0!\tchar id(%d)\t"
                "money(%d)\tfatigue(%d)",
                *(unsigned int*)((char*)pkt + 0xa), *(unsigned int*)((char*)pkt + 0xe),
                *(unsigned int*)((char*)pkt + 0x12));
        }
        else
        {
            unsigned int upperCharId = (unsigned int)member->GetUpperMember_CharId();
            CUser* upperUser = userMgr->FindUser_CharNo(upperCharId);
            if (upperUser == 0)
            {
                DNF_LOG_SCOPE_LINE(0x849,"./log/Member",
                    "CPacketTranslater::OnPayTaxToUpper : pclUpperUser == 0!, Maybe, upper "
                    "Member is not connect!\tupper char id(%d)",
                    upperCharId);
            }
            else
            {
                CUser* payUser = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
                if (payUser == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x84f,"./log/Except",
                        "[MEMBER] CPacketTranslater::OnPayTaxToUpper : pclPayUser == 0!\tchar "
                        "id(%d)\tmoney(%d)\tfatigue(%d)",
                        *(unsigned int*)((char*)pkt + 0xa), *(unsigned int*)((char*)pkt + 0xe),
                        *(unsigned int*)((char*)pkt + 0x12));
                }
                else
                {
                    int lowerCnt = upperUser->GetConnLowerMemberCnt();
                    short level = upperUser->GetLevel();
                    unsigned int limit =
                        m_pclApp->Get_MemberManager()->GetLowerMemberEnterLimit(
                            (unsigned int)level);
                    unsigned char expLevel = payUser->GetUpperMemberExpLevel();
                    float rate = (float)(0.01 * (double)expLevel) +
                                 (float)(0.05 * ((double)lowerCnt / (double)limit));
                    int moneyTax =
                        (int)((double)*(unsigned int*)((char*)pkt + 0xe) * (double)rate);
                    int fatigueTax =
                        (int)((double)*(unsigned int*)((char*)pkt + 0x12) * (double)rate);
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
        DNF_LOG_SCOPE_LINE(0x886,"./log/Except", "CPacketTranslater::OnPayTaxToUpper() Exception Break : %s\n",
            "CPacketTranslater::OnPayTaxToUpper() Exception Break\n");
    }
}

void CPacketTranslater::OnUpdateChangableCharInfo(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnUpdateChangableCharInfo : 0 == m_pclApp");
    }
    CUser* user =
        ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
            *(unsigned int*)((char*)pkt + 0xa));
    if (user != 0)
    {
        user->SetUserChangableInfo(*(short*)((char*)pkt + 0xf), *(char*)((char*)pkt + 0x11));
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
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    char ok = userMgr->DeleteProhibitUser(*(unsigned int*)((char*)pkt + 0xa),
                                          (char)*(char*)((char*)pkt + 0xe));
    if (ok == 1)
    {
        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
        DNF_LOG_SCOPE_LINE(0x8a5,"./log/User", "[LOGOUT COMPLETE] m_id : %s\tchannel No : %d\n", dbid,
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
    }
    else
    {
        char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
        DNF_LOG_SCOPE_LINE(0x8a1,"./log/User",
            "[DELETE_ERR_] CPacketTranslater::OnLogoutComplete m_id : %s\tChannel No : %d\n",
            dbid, (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
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
    char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
    DNF_LOG_SCOPE_LINE(0x954,"./log/Web", "CPacketTranslater::OnUserRepel m_id(%s) , charNo(%d)\n", dbid,
        *(unsigned int*)((char*)pkt + 0xe));
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
    if (user != 0 &&
        (*(unsigned int*)((char*)pkt + 0xe) == 0 ||
         (user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xe))) != 0))
    {
        *(unsigned int*)((char*)pkt + 0xa) = user->GetIdByChannel();
        user->SendToGameserver((char*)pkt, 0x12);
    }


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    if (handler != 0)
    {
        ((CUserManager*)((char*)m_pclApp + 0x10))
            ->DeleteBlackUserOnCharacDelete(*(unsigned int*)((char*)pkt + 0xa));
        Packet_DB_Member_Delete_As_Charac_Delete dbPkt;
        dbPkt.m_charNo = *(unsigned int*)((char*)pkt + 0xe);
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
        PacketHeader* rpkt = pkt;
        unsigned short p2 = *(unsigned short*)((char*)pkt + 0x10);
        unsigned short p1 = *(unsigned short*)((char*)pkt + 0xe);
        unsigned int code = *(unsigned int*)((char*)pkt + 0xa);
        DNF_LOG_SCOPE_LINE(0x9f4,"./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), "
            "eventParam2(%d)\n",
            code, (unsigned int)p1, (unsigned int)p2);
        ((CEventActionManager*)*(int*)((char*)m_pclApp + 0x31c))
            ->OnStartAction((Packet_Monitor_Event_Start*)pkt);
        ((CServerHandler*)*(void**)((char*)m_pclApp + 0xa0))
            ->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventStart() 예외 발생 : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0xa3b, "./log/Except", "CPacketTranslater::OnEventStart() 예외 발생 : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventStart() 예외 발생");
        DNF_LOG_SCOPE_LINE(0xa41, "./log/Except", "CPacketTranslater::OnEventStart() 예외 발생\n");
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
        PacketHeader* rpkt = pkt;
        unsigned int code = *(unsigned int*)((char*)pkt + 0xa);
        DNF_LOG_SCOPE_LINE(0xa66, "./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n", code);
        ((CEventActionManager*)*(int*)((char*)m_pclApp + 0x31c))->OnEndAction(code);
        m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnEventEnd() 예외 발생 : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0xa73, "./log/Except", "CPacketTranslater::OnEventEnd() 예외 발생 : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnEventEnd() 예외 발생");
        DNF_LOG_SCOPE_LINE(0xa79, "./log/Except", "CPacketTranslater::OnEventEnd() 예외 발생\n");
    }
}

void CPacketTranslater::OnNotifyNewMail(PacketHeader* pkt)
{
    try
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            *(unsigned int*)((char*)pkt + 0xe) = user->GetIdByChannel();
            user->SendToGameserver((char*)pkt, 0x12);
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb66, "%s", "%s", "OnNotifyNewMail");
    }
}

void CPacketTranslater::OnWebQueryUserState(PacketHeader* pkt)
{try
{


    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    int found = 0;
    if (userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa)) == 0)
    {
        *(char*)((char*)pkt + 0x12) = 0;
    }
    else
    {
        *(char*)((char*)pkt + 0x12) = 1;
    }
    const char* state = *(char*)((char*)pkt + 0x12) == 1 ? "true" : "false";
    char* dbid = NumberToString(*(unsigned int*)((char*)pkt + 0xa), 0);
    DNF_LOG_SCOPE_LINE(0xb78, "./log/User", "WebQueryUserState Result[m_id: %s] : [%s] : %d\n", dbid, state);
    unsigned int addr = *(unsigned int*)((char*)pkt + 6);
    unsigned short port = *(unsigned short*)((char*)pkt + 4);
    if (m_pclApp->Get_UdpHandler()->SendToClient((char*)pkt, 0x13, port, (char*)0, addr) != 1)
    {
        throw CDNFException(strerror(errno));
    }


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xb7f, "./log/Except", "%s Exception Break : %s\n", "OnWebQueryUserState", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xb84, "./log/Except", "%s Exception Break\n", "OnWebQueryUserState");
    }
}

void CPacketTranslater::OnNoticeMessage(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeMessage : 0 == m_pclApp");
    }
    DNF_LOG_SCOPE_LINE(0xf8e, "./log/GM_msg", "CPacketTranslater::OnNoticeMessage()%s\n", (char*)pkt + 0xb);
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x10b);


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


    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    int found = 0;
    if (userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa)) == 0)
    {
        *(char*)((char*)pkt + 0xe) = 0;
    }
    else
    {
        *(char*)((char*)pkt + 0xe) = 1;
    }
    CServerInterface* gs =
        (CServerInterface*)m_pclApp->FindGameServer((int)*(unsigned int*)((char*)pkt + 0xa));
    if (gs == 0)
    {
        throw CDNFException(strerror(errno));
    }
    gs->SendToServer((char*)pkt, 0xf);


    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xfc8, "./log/Except", "%s Exception Break : %s\n", "OnRelayServerUserCheck", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xfcd, "./log/Except", "%s Exception Break\n", "OnRelayServerUserCheck");
    }
}

void CPacketTranslater::OnForbidChat(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnForbidChat : 0 == m_pclApp");
    }
    DNF_LOG_SCOPE_LINE(0xfdd,"./log/GM_msg", "CPacketTranslater::OnForbidChat() %s for %d secs\n", (char*)pkt + 0x12,
        *(unsigned int*)((char*)pkt + 0xa));
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x30);


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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        unsigned int dbid = *(unsigned int*)((char*)pkt + 0xa);
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
        *(char*)((char*)pkt + 0x11) = notPresent ? 0 : 1;
        if (*(char*)((char*)pkt + 0xe) == 0)
        {
            CDNFProhibitUser* p = userMgr->FindProhibitUser(dbid);
            if (p == 0)
            {
                p = new CDNFProhibitUser;
                p->SetUserConnectableTime(dbid, *(short*)((char*)pkt + 0xf), -1, true);
                if (userMgr->InsertProhibitUser(dbid, p) != 1)
                {
                    char* s = NumberToString(dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x922,"./log/ProhibitUser",
                        "[INSERT_ERR] CPacketTranslater::OnNoticeProhibitConnectUser m_id : "
                        "%s, flag( %d ), time( %d ) \n",
                        s, (int)(char)*(char*)((char*)pkt + 0xe),
                        (int)*(short*)((char*)pkt + 0xf));
                    delete p;
                }
                char* s = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x926,"./log/ProhibitUser",
                    "[INSERT_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser "
                    "m_id : %s, flag( %d ), time( %d ) \n",
                    s, (int)(char)*(char*)((char*)pkt + 0xe),
                    (int)*(short*)((char*)pkt + 0xf));
            }
            else
            {
                if (p->GetChannelNo() == -1)
                {
                    *(char*)((char*)pkt + 0xe) = 2;
                    *(unsigned short*)pkt = 0x4c9;
                    *(char*)((char*)pkt + 0x12) = (char)m_pclApp->Get_ServerGroup();
                    handler->GetTcpManagerServer()->SendTcpPacket(pkt);
                    char* s = NumberToString(dbid, 0);
                    DNF_LOG_SCOPE_LINE(0x90a,"./log/ProhibitUser",
                        "[ALREADY_INSERT] CPacketTranslater::OnNoticeProhibitConnectUser m_id "
                        ": %s, flag( %d ), time( %d ) \n",
                        s, (int)(char)*(char*)((char*)pkt + 0xe),
                        (int)*(short*)((char*)pkt + 0xf));
                    return;
                }
                char* s = NumberToString(dbid, 0);
                DNF_LOG_SCOPE_LINE(0x90e,"./log/ProhibitUser",
                    "[ALREADY_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser "
                    "m_id : %s, flag( %d ), time( %d ) \n",
                    s, (int)(char)*(char*)((char*)pkt + 0xe),
                    (int)*(short*)((char*)pkt + 0xf));
            }
            *(unsigned short*)pkt = 0x4c9;
            *(char*)((char*)pkt + 0x12) = (char)m_pclApp->Get_ServerGroup();
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
                    s, (int)(char)*(char*)((char*)pkt + 0xe),
                    (int)*(short*)((char*)pkt + 0xf));
            }
            char* s = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_LINE(0x8f2,"./log/ProhibitUser",
                "[DELETE_PROHIBIT_USER] CPacketTranslater::OnNoticeProhibitConnectUser m_id : "
                "%s, flag( %d ), time( %d ) \n",
                s, (int)(char)*(char*)((char*)pkt + 0xe),
                (int)*(short*)((char*)pkt + 0xf));
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x939,"%s", "CPacketTranslater::OnNoticeProhibitConnectUser() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x93e, "%s", "CPacketTranslater::OnNoticeProhibitConnectUser() Exception Break");
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
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        handler->SetManagerConnectFlag(true);
        DNF_LOG_SCOPE_LINE(0xc19, "./log/Manager", "Manager Server Connect Success");
        puts("** Manager Server Connect Success **");
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0xc1e,"%s", "CPacketTranslater::OnMonitorManagerConnectOK() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0xc23, "%s", "CPacketTranslater::OnMonitorManagerConnectOK() Exception Break");
    }
}

void CPacketTranslater::OnMonitorMegaPhoneMsg(PacketHeader* pkt)
{
    *(char*)((char*)pkt + 0xa) = (char)m_pclApp->Get_ServerGroup();
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
}

void CPacketTranslater::OnRegisterGM_mid(PacketHeader* pkt)
{
    PacketHeader* p = pkt;
    if (m_pclApp != 0)
    {
        m_pclApp->Add_GM_id(*(unsigned int*)((char*)p + 0xa));
    }
}

void CPacketTranslater::OnRegisterToBlackList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xc41, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == m_pclApp");
    }
    else
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        Packet_DBMW_Register_To_BlackList dbPkt;
        Packet_Register_To_BlackList_RESULT result;
        dbPkt.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
        memcpy(dbPkt.m_name, (char*)pkt + 0xe, 0x1d);
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc50, "./log/BlackList", "CPacketTranslater::OnRegisterToBlackList : 0 == pclUser");
        }
        else if (strcmp(user->GetCharName(), (char*)pkt + 0xe) == 0)
        {
            result.m_result = 3;
            result.m_idByChannel = user->GetIdByChannel();
            result.m_charNo = 0xffffffff;
            memcpy(result.m_name, (char*)pkt + 0xe, 0x1d);
            user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
        }
        else
        {
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, (char*)pkt + 0xe, 0x1d);
            if (user->GetBlackListSize() < 10)
            {
                CUser* target = userMgr->FindUser_CharName((char*)pkt + 0xe);
                if (target != 0)
                {
                    result.m_charNo = target->GetUniqCharNo();
                    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
                    if (gm != 0 && gm->isGM(target->GetDBID()) != 0)
                    {
                        result.m_result = 5;
                        user->SendToGameserver((char*)&result,
                                               *(unsigned short*)((char*)&result + 2));
                        return;
                    }
                    if (user->RegisterToBlackList(target->GetUniqCharNo(),
                                                  (char*)pkt + 0xe) != 1)
                    {
                        result.m_result = 2;
                        user->SendToGameserver((char*)&result,
                                               *(unsigned short*)((char*)&result + 2));
                        return;
                    }
                    dbPkt.m_charNo2 = target->GetUniqCharNo();
                    result.m_result = 1;
                    user->SendToGameserver((char*)&result,
                                           *(unsigned short*)((char*)&result + 2));
                }
                handler->SendToDB(&dbPkt);
            }
            else
            {
                result.m_result = 4;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
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
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    Packet_DMBW_Delete_To_BlackList dbPkt;
    Packet_Delete_To_BlackList_Result result;
    dbPkt.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
    memcpy(dbPkt.m_name, (char*)pkt + 0xe, 0x1d);
    CUser* target = userMgr->FindUser_CharName((char*)pkt + 0xe);
    if (target != 0)
    {
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xcb6, "./log/BlackList", "CPacketTranslater::OnDeleteToBlackList : 0 == pclUser");
            return;
        }
        result.m_idByChannel = user->GetIdByChannel();
        result.m_charNo = target->GetUniqCharNo();
        memcpy(result.m_name, (char*)pkt + 0xe, 0x1d);
        if (user->DeleteToBlackList(target->GetUniqCharNo()) != 1)
        {
            result.m_result = 2;
            user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            return;
        }
        dbPkt.m_charNo2 = target->GetUniqCharNo();
        result.m_result = 1;
        user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
    }
    handler->SendToDB(&dbPkt);


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


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xce9, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == m_pclApp");
    }
    else
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        Packet_Request_Result_BlackList reply;
        reply.m_idByChannel = *(unsigned int*)((char*)pkt + 0xe);
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xcf3, "./log/BlackList", "CPacketTranslater::OnRequestBlackList : 0 == pclUser");
        }
        else
        {
            unsigned char count = 0;
            user->GetBlackList(count, reply.m_blackList);
            reply.m_count = count;
            user->SendToGameserver((char*)&reply, *(unsigned short*)((char*)&reply + 2));
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
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xd1f, "./log/BlackList", "CPacketTranslater::OnDBMWResisterToBlackList : 0 == pclUser");
        }
        else
        {
            Packet_Register_To_BlackList_RESULT result;
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, (char*)pkt + 0xe, 0x1d);
            if (*(int*)((char*)pkt + 0x30) == 0x5a)
            {
                result.m_result = 5;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else if (*(int*)((char*)pkt + 0x2c) == -1)
            {
                result.m_result = 3;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else if (user->IsBlackUser(*(unsigned int*)((char*)pkt + 0x2c)) != 0)
            {
                result.m_result = 2;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else if (user->GetBlackListSize() < 10)
            {
                user->RegisterToBlackList(*(unsigned int*)((char*)pkt + 0x2c),
                                          (char*)pkt + 0xe);
                result.m_result = 1;
                result.m_charNo = *(unsigned int*)((char*)pkt + 0x2c);
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else
            {
                result.m_result = 4;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
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


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xd69, "./log/BlackList", "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == m_pclApp");
    }
    else
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xd70, "./log/BlackList", "CPacketTranslater::OnDBMWDeleteToBlackList : 0 == pclUser");
        }
        else
        {
            Packet_Delete_To_BlackList_Result result;
            result.m_idByChannel = user->GetIdByChannel();
            memcpy(result.m_name, (char*)pkt + 0xe, 0x1d);
            if (*(int*)((char*)pkt + 0x2c) == -1)
            {
                result.m_result = 3;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else if (user->DeleteToBlackList(*(unsigned int*)((char*)pkt + 0x2c)) != 1)
            {
                result.m_result = 2;
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
            }
            else
            {
                result.m_result = 1;
                result.m_charNo = *(unsigned int*)((char*)pkt + 0x2c);
                user->SendToGameserver((char*)&result, *(unsigned short*)((char*)&result + 2));
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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendToDB(&pkt);


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
    CUser* user =
        ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(charNo);
    if (user != 0)
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
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        handler->SendAllTcpGameServer(&bpkt);
    }
}

void CPacketTranslater::OnDBMWResponseBlackListOnLogin(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0xdc3,"./log/BlackList",
            "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == m_pclApp");
    }
    else
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user == 0)
        {
            DNF_LOG_SCOPE_LINE(0xdcb,"./log/BlackList",
                "CPacketTranslater::OnDBMWResponseBlackListOnLogin : 0 == pclUser");
        }
        else
        {
            for (int i = 0; *(int*)((char*)pkt + i * 0x28 + 0x32) != 0; i++)
            {
                if (user->IsBlackUser(*(unsigned int*)((char*)pkt + i * 0x28 + 0xe)) != 1)
                {
                    user->RegisterToBlackList(
                        *(unsigned int*)((char*)pkt + i * 0x28 + 0xe),
                        (char*)pkt + i * 0x28 + 0x12,
                        *(unsigned int*)((char*)pkt + i * 0x28 + 0x32));
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
            PacketHeader* reply = pkt;
            unsigned int channel = *(unsigned int*)((char*)pkt + 0xa);
            unsigned int ip = *(unsigned int*)((char*)pkt + 0x10);
            bool result = false;
            int code = *(int*)((char*)pkt + 0xa);
            short port = *(short*)((char*)pkt + 0xe);
            unsigned int ip2 = *(unsigned int*)((char*)pkt + 0x10);
            GetInstanceExchangeServer()->SetExchageServer(ip2, port, code, result);
            if (!result)
            {
                *(unsigned short*)((char*)reply + 0xe) =
                    GetInstanceExchangeServer()->GetExchangeServerPort();
                *(unsigned int*)((char*)reply + 0x10) =
                    GetInstanceExchangeServer()->GetExchangeServerIp();
                *(unsigned int*)((char*)reply + 0xa) =
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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CMemberManager* memberMgr = (CMemberManager*)((char*)m_pclApp + 0x2d0);
        CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            CMember* member = user->GetMember();
            CServerHandler* handler = *(CServerHandler**)((char*)m_pclApp + 0xa0);
            if (member != 0 && handler != 0)
            {
                if (memberMgr->FindMemberUser(
                        (unsigned int)member->GetUpperMember_CharId()) != 0)
                {
                    unsigned int maxExp =
                        memberMgr->GetMemberExpTable()->GetMaxMemberExp();
                    unsigned int newExp =
                        (unsigned int)member->IncConnUpperMemberExp(maxExp);
                    if (newExp != 0)
                    {
                        unsigned int upperCharId =
                            (unsigned int)member->GetUpperMember_CharId();
                        CUser* upperUser = userMgr->FindUser_CharNo(upperCharId);
                        if (upperUser != 0)
                        {
                            CMember* upperMember = upperUser->GetMember();
                            if (upperMember != 0)
                            {
                                unsigned int max2 =
                                    memberMgr->GetMemberExpTable()->GetMaxMemberExp();
                                unsigned int memberKey =
                                    (unsigned int)member->GetMemberKey();
                                upperMember->IncConnLowerMemberExp(memberKey, max2);
                            }
                        }
                        unsigned int upperId =
                            (unsigned int)member->GetUpperMember_CharId();
                        unsigned int key = (unsigned int)member->GetMemberKey();
                        memberMgr->SaveMemberExp(handler, key, upperId, newExp);
                        if (memberMgr->IsMemberExpLevelUp(newExp))
                        {
                            unsigned int u = (unsigned int)member->GetUpperMember_CharId();
                            memberMgr->NoticeLevelUpToLowers(u, newExp);
                        }
                    }
                }
                int idx = 0;
                unsigned int lowerCount = member->GetLowerMemberCount();
                unsigned int* proxy = member->GetLowerMember_Proxy();
                while (lowerCount != 0)
                {
                    lowerCount--;
                    CMember* lowerMember = memberMgr->FindMember(*proxy);
                    if (lowerMember != 0)
                    {
                        unsigned int maxE =
                            memberMgr->GetMemberExpTable()->GetMaxMemberExp();
                        unsigned int lowerNew =
                            (unsigned int)lowerMember->IncConnUpperMemberExp(maxE);
                        if (lowerNew != 0)
                        {
                            unsigned int maxE2 =
                                memberMgr->GetMemberExpTable()->GetMaxMemberExp();
                            member->IncConnLowerMemberExp(idx, *proxy, maxE2);
                            unsigned int lowerUpperId =
                                (unsigned int)lowerMember->GetUpperMember_CharId();
                            unsigned int lowerKey =
                                (unsigned int)lowerMember->GetMemberKey();
                            memberMgr->SaveMemberExp(handler, lowerKey, lowerUpperId,
                                                     lowerNew);
                            if (memberMgr->IsMemberExpLevelUp(lowerNew))
                            {
                                unsigned int lu =
                                    (unsigned int)lowerMember->GetUpperMember_CharId();
                                memberMgr->NoticeLevelUpToLowers(lu, lowerNew);
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
            CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
            handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
            DNF_LOG_SCOPE_LINE(0xf6f,"./log/WebNotice", "OnWebNoticeSingle : (%s,%d)\n", (char*)pkt + 0xb,
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa));
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
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0xfff, "./log/buddy", "CPacketTranslater::OnAddBuddy : 0 == m_pclApp");
        }
        else
        {
            CUser* user =
                ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                    *(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1012, "./log/buddy", "CPacketTranslater::OnAddBuddy\t pclUser is NULL");
            }
            else
            {
                CServerHandler* handler = m_pclApp->Get_ServerHandler();
                int r = user->AddBuddyDB(handler, (char*)pkt + 0x12);
                if (r != 0)
                {
                    Packet_Monitor_Add_Buddy_Reply reply;
                    reply.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
                    reply.m_idByChannel = *(unsigned int*)((char*)pkt + 0xe);
                    reply.m_result = (unsigned char)r;
                    user->SendToGameserver((char*)&reply,
                                           *(unsigned short*)((char*)&reply + 2));
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1018, "%s", "CPacketTranslater::OnAddBuddy() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x101d, "%s", "CPacketTranslater::OnAddBuddy() Exception Break");
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
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x105f, "./log/buddy", "CPacketTranslater::OnAddBuddyDBReply\tpclUser is NULL");
            }
            else
            {
                Packet_Monitor_Add_Buddy_Reply reply;
                reply.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
                reply.m_idByChannel = user->GetIdByChannel();
                if (*(char*)((char*)pkt + 0x35) == 0)
                {
                    user->AddBuddy(*(STBuddyDBInfo*)((char*)pkt + 0xe));
                    memcpy(reply.m_name, (char*)pkt + 0xe, 0x27);
                    CUser* other =
                        userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x30));
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
                    reply.m_result = *(unsigned char*)((char*)pkt + 0x35);
                    user->GetUniqCharNo();
                    ((CBuddyRegisterManager*)((char*)m_pclApp + 0x300))
                        ->addBuddyRegister(*(unsigned int*)((char*)pkt + 0x30),
                                           user->GetUniqCharNo());
                }
                else
                {
                    reply.m_result = *(unsigned char*)((char*)pkt + 0x35);
                }
                user->SendToGameserver((char*)&reply,
                                       *(unsigned short*)((char*)&reply + 2));
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1065, "%s", "CPacketTranslater::OnAddBuddyDBReply() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x106a, "%s", "CPacketTranslater::OnAddBuddyDBReply() Exception Break");
    }
}

void CPacketTranslater::OnDelBuddy(PacketHeader* pkt)
{
    try
    {
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x107e, "./log/buddy", "CPacketTranslater::OnDelBuddy : 0 == m_pclApp");
        }
        else
        {
            CUser* user =
                ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                    *(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1092, "./log/buddy", "CPacketTranslater::OnDelBuddy\t pclUser is NULL");
            }
            else
            {
                CServerHandler* handler = m_pclApp->Get_ServerHandler();
                int r = user->DelBuddyDB(handler, (char*)pkt + 0x12);
                if (r != 0)
                {
                    Packet_Monitor_Del_Buddy_Reply reply;
                    reply.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
                    reply.m_idByChannel = *(unsigned int*)((char*)pkt + 0xe);
                    memcpy(reply.m_name, (char*)pkt + 0x12, 0x1d);
                    reply.m_result = (unsigned char)r;
                    user->SendToGameserver((char*)&reply,
                                           *(unsigned short*)((char*)&reply + 2));
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1098, "%s", "CPacketTranslater::OnDelBuddy() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x109d, "%s", "CPacketTranslater::OnDelBuddy() Exception Break");
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
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x10cf, "./log/buddy", "CPacketTranslater::OnDelBuddyDBReply\tpclUser is NULL");
            }
            else
            {
                Packet_Monitor_Del_Buddy_Reply reply;
                reply.m_charNo = *(unsigned int*)((char*)pkt + 0xa);
                reply.m_idByChannel = user->GetIdByChannel();
                if (*(char*)((char*)pkt + 0x30) == 0)
                {
                    user->DelBuddy((char*)pkt + 0x12);
                    memcpy(reply.m_name, (char*)pkt + 0x12, 0x1d);
                    reply.m_result = *(unsigned char*)((char*)pkt + 0x30);
                    unsigned int uniq = user->GetUniqCharNo();
                    ((CBuddyRegisterManager*)((char*)m_pclApp + 0x300))
                        ->delBuddyRegister(*(unsigned int*)((char*)pkt + 0xe), uniq);
                }
                else
                {
                    reply.m_result = 3;
                }
                user->SendToGameserver((char*)&reply,
                                       *(unsigned short*)((char*)&reply + 2));
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x10d5, "%s", "CPacketTranslater::OnDelBuddyDBReply() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x10da, "%s", "CPacketTranslater::OnDelBuddyDBReply() Exception Break");
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
            CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
            if (0x20 < (unsigned char)*(char*)((char*)pkt + 0xe))
            {
                *(char*)((char*)pkt + 0xe) = 0x20;
            }
            CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
            if (user == 0)
            {
                DNF_LOG_SCOPE_LINE(0x1112,"./log/buddy",
                    "CPacketTranslater::OnQueryBuddyInfoDBReply\t  pclUser is NULL");
            }
            else
            {
                for (int i = 0;
                     i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe); i++)
                {
                    user->AddBuddy(*(STBuddyDBInfo*)((char*)pkt + i * 0x27 + 0xf));
                    user->GetUniqCharNo();
                    ((CBuddyRegisterManager*)((char*)m_pclApp + 0x300))
                        ->addBuddyRegister(*(unsigned int*)((char*)pkt + i * 0x27 + 0x31),
                                           user->GetUniqCharNo());
                }
                if (*(char*)((char*)pkt + 0xe) != 0)
                {
                    userMgr->SendConnectedBuddysList(user);
                }
                user->SetBuddyDBFlag(4);
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1118,"%s", "CPacketTranslater::OnQueryBuddyInfoDBReply() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x111d, "%s", "CPacketTranslater::OnQueryBuddyInfoDBReply() Exception Break");
    }
}

void CPacketTranslater::OnWebChangeUserHandicap(PacketHeader* pkt)
{
    if (m_pclApp == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1127, "./log/hack", "CPacketTranslater::OnWebChangeUserHandicap : 0 == m_pclApp");
    }
    else
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            Packet_Change_User_Handicap reply;
            reply.m_fieldA = *(unsigned int*)((char*)pkt + 0xa);
            reply.m_fieldE = *(unsigned int*)((char*)pkt + 0xe);
            reply.m_field12 = *(unsigned int*)((char*)pkt + 0x12);
            user->SendToGameserver((char*)&reply, *(unsigned short*)((char*)&reply + 2));
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
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            Packet_GM_Request_Mid reply;
            *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
            memcpy((char*)&reply + 0x16, (char*)pkt + 0x16, 0x1d);
            CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x16);
            if (target == 0)
            {
                *(unsigned int*)((char*)&reply + 0xe) = 0xffffffff;
            }
            else
            {
                *(unsigned int*)((char*)&reply + 0x12) = target->GetUniqCharNo();
                *(unsigned int*)((char*)&reply + 0xe) = target->GetDBID();
            }
            user->SendToGameserver((char*)&reply, *(unsigned short*)((char*)&reply + 2));
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
}

void CPacketTranslater::OnUserRepelByCharName(PacketHeader* pkt)
{try
{


    if (m_pclApp != 0)
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CUser* user = userMgr->FindUser(*(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            Packet_Monitor_User_Repel reply;
            CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x12);
            if (target != 0)
            {
                reply.m_idByChannel = target->GetIdByChannel();
                target->SendToGameserver((char*)&reply, 0x12);
            }
        }
        return;
    }
    throw CDNFException("CPacketTranslater::OnUserRepel : 0 == m_pclApp");


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
        CTowerRank* tower = (CTowerRank*)m_pclApp->getTowerRank();
        if (*(char*)((char*)pkt + 0xa) != 0)
        {
            tower->reset();
        }
        DNF_LOG_SCOPE_LINE(0x1172,"./log/DeathTower", "%d/%d\n", *(unsigned int*)((char*)pkt + 0xb),
            *(unsigned int*)((char*)pkt + 0xf));
        for (unsigned int i = 0; i < *(unsigned int*)((char*)pkt + 0xb); i++)
        {
            tower->registRank(
                (unsigned int)(unsigned char)*(char*)((char*)pkt + i * 0x65 + 0x1b),
                (unsigned int)*(unsigned short*)((char*)pkt + i * 0x65 + 0x17),
                (unsigned int)*(unsigned short*)((char*)pkt + i * 0x65 + 0x19),
                (const stDeathTowerRecordMemberInfo_t*)((char*)pkt + i * 0x65 + 0x1c));
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x117e,"%s", "CPacketTranslater::onReplyLoadTowerFullRank() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1183, "%s", "CPacketTranslater::onReplyLoadTowerFullRank() Exception Break");
    }
}

void CPacketTranslater::onRequestCharacTowerUpdateRank(PacketHeader* pkt)
{try
{


    CUser* user =
        ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
            *(unsigned int*)((char*)pkt + 0xe));
    if (user != 0)
    {
        unsigned int a = *(unsigned int*)((char*)pkt + 0x16);
        unsigned int b = *(unsigned int*)((char*)pkt + 0xe);
        char* name = user->GetCharName();
        unsigned int c = *(unsigned int*)((char*)pkt + 0x12);
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
{try
{


    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    CTowerRank* tower = (CTowerRank*)m_pclApp->getTowerRank();
    tower->processReloadRanking(handler, true, 5);
    handler->SendAllToGameServer((char*)pkt, 10);


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


    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0xb);


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
            if (handler->GetTcpDBServer()->GetSock() == *(int*)((char*)pkt + 6))
            {
                handler = m_pclApp->Get_ServerHandler();
                handler->GetTcpDBServer()->Connected();
            }
            else
            {
                handler = m_pclApp->Get_ServerHandler();
                if (handler->GetTcpManagerServer()->GetSock() == *(int*)((char*)pkt + 6))
                {
                    unsigned char group = m_pclApp->Get_ServerGroup();
                    handler = m_pclApp->Get_ServerHandler();
                    handler->GetTcpManagerServer()->Connected(group);
                }
                else
                {
                    unsigned int sock = *(unsigned int*)((char*)pkt + 6);
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
            if (handler->GetTcpDBServer()->GetSock() == *(int*)((char*)pkt + 6))
            {
                handler = m_pclApp->Get_ServerHandler();
                handler->GetTcpDBServer()->DisConnected();
            }
            else
            {
                handler = m_pclApp->Get_ServerHandler();
                if (handler->GetTcpManagerServer()->GetSock() == *(int*)((char*)pkt + 6))
                {
                    handler = m_pclApp->Get_ServerHandler();
                    handler->GetTcpManagerServer()->DisConnected();
                }
                else
                {
                    CTcpGameServer* tcp = (CTcpGameServer*)m_pclApp->FindTcpGameServer(
                        *(unsigned int*)((char*)pkt + 6));
                    m_pclApp->OnTcpGameServerDown(tcp);
                    unsigned char channel = tcp->GetChannelNo();
                    if (channel != 0)
                    {
                        handler = m_pclApp->Get_ServerHandler();
                        handler->UnregistGameServer((unsigned int)channel);
                    }
                    handler = m_pclApp->Get_ServerHandler();
                    handler->DeleteTcpGameServer(*(unsigned int*)((char*)pkt + 6));
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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x3d);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 10);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 10);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x19e);


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
            PacketHeader* rpkt = pkt;
            Packet_DBMW_Change_Char_Name pkt2;
            *(char*)((char*)&pkt2 + 0xa) = (char)m_pclApp->Get_ServerGroup();
            *(unsigned int*)((char*)&pkt2 + 0xb) = *(unsigned int*)((char*)pkt + 0xe);
            memcpy((char*)&pkt2 + 0xf, (char*)pkt + 0x16, 0x1d);
            m_pclApp->Get_ServerHandler()->SendToDB(&pkt2);
            unsigned int dbid = *(unsigned int*)((char*)pkt + 0xe);
            CUserManager* userMgr = m_pclApp->Get_UserManager();
            userMgr->ChangeBlackListCharName(dbid, (char*)pkt + 0x16);
            std::vector<unsigned int> vec;
            ((CBuddyRegisterManager*)((char*)m_pclApp + 0x300))
                ->findBuddyRegister(*(unsigned int*)((char*)pkt + 0xe), vec);
            for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end(); ++it)
            {
                CUser* user = userMgr->FindUser_CharNo(*it);
                if (user != 0)
                {
                    std::string name((char*)pkt + 0x16);
                    user->SetBuddyCharName(*(int*)((char*)pkt + 0xe), name);
                }
            }
            std::string name2((char*)pkt + 0x16);
            ((CMemoryCashManager*)m_pclApp->Get_MemoryCashManager())->InsertUpdatedCharacName(
                *(unsigned int*)((char*)pkt + 0xe), name2);
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
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            *(unsigned int*)((char*)pkt + 0xe) = user->GetIdByChannel();
            user->SendToGameserver((char*)pkt, 0x26);
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x137e, "%s", "%s", "OnNotifyAuctionMail");
    }
}

void CPacketTranslater::OnPvPChannelInfo(PacketHeader* pkt)
{
    try
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            Packet_PvPChannelUserCount pkt2;
            *(unsigned int*)((char*)&pkt2 + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
            *(unsigned int*)((char*)&pkt2 + 0xe) = *(unsigned int*)((char*)pkt + 0xe);
            *(unsigned int*)((char*)&pkt2 + 0x12) = *(unsigned int*)((char*)pkt + 0x12);
            m_pclApp->Get_ServerGroup();
            CServerHandler* handler = (CServerHandler*)((char*)m_pclApp + 0xa0);
            int count = handler->SendAllTcpGameServer(
                &pkt2, (int)(unsigned char)*(char*)((char*)pkt + 0x16));
            user->ResetChannelUserCount(count);
            if (count == 0 || *(int*)((char*)pkt + 0x12) == 0)
            {
                Packet_PvPChannelInfo reply;
                *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
                *(unsigned int*)((char*)&reply + 0xe) = *(unsigned int*)((char*)pkt + 0xe);
                *(unsigned int*)((char*)&reply + 0x12) = *(unsigned int*)((char*)pkt + 0x12);
                *(char*)((char*)&reply + 0x17) = 0;
                *(unsigned short*)((char*)&reply + 2) =
                    (unsigned short)((0 << 4) + 0x18);
                user->SendTcpGameserver(&reply);
            }
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x13aa, "./log/Except", "%s Exception Break\n", "OnPvPChannelInfo");
    }
}

void CPacketTranslater::OnPvPChannelUserCount(PacketHeader* pkt)
{
    try
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        CUser* user = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            user->SetChannelUserCount(*(int*)((char*)pkt + 0x16), *(int*)((char*)pkt + 0x1a),
                                      *(int*)((char*)pkt + 0x22), *(int*)((char*)pkt + 0x26));
            if (user->IsCompleteChannelUserCount())
            {
                Packet_PvPChannelInfo reply;
                *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
                *(unsigned int*)((char*)&reply + 0xe) = *(unsigned int*)((char*)pkt + 0xe);
                *(unsigned int*)((char*)&reply + 0x12) = *(unsigned int*)((char*)pkt + 0x12);
                unsigned char count = 0xff;
                user->GetChannelUserCount((STPvPChannelInfo*)((char*)&reply + 0x18), count);
                *(unsigned short*)((char*)&reply + 2) =
                    (unsigned short)((unsigned int)count * 0x10 + 0x18);
                user->SendTcpGameserver(&reply);
            }
        }
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x13d3, "./log/Except", "%s Exception Break\n", "OnPvPChannelUserCount");
    }
}

void CPacketTranslater::OnChannelType(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        try
        {
            unsigned int channel = *(unsigned int*)((char*)pkt + 0xe);
            if (m_pclApp->FindGameServer((int)channel) == 0)
            {
                throw CDNFException("CPacketTranslater::OnChannelType : pclGameServer == 0");
            }
            CTcpGameServer* tcpGs =
                (CTcpGameServer*)m_pclApp->FindTcpGameServer(channel);
            if (tcpGs != 0)
            {
                tcpGs->SetChannelType(*(int*)((char*)pkt + 0xe));
            }
        }
        catch (CDNFException& e)
        {
            DNF_LOG_SCOPE_LINE(0x1b21, "%s", "CPacketTranslater::OnChannelType() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            DNF_LOG_SCOPE_LINE(0x1b26, "%s", "CPacketTranslater::OnChannelType() Exception Break");
        }
    }
}

void CPacketTranslater::OnServerMessageInfo(PacketHeader* pkt)
{try
{


    DNF_LOG_SCOPE_LINE(0x1402, "./log/ServerEvent", "Packet_Monitor_Server_Message_Info");
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x5f);


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


    DNF_LOG_SCOPE_LINE(0x1418, "./log/ServerEvent", "Packet_Request_Reload_Power_War_Ranker");
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 10);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x4bd);


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
    DNF_LOG_SCOPE_LINE(0x1448,"./log/Secu", "IPCounterControl - type : %d, value : %d ",
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xb));
    CIPCounter* counter = (CIPCounter*)m_pclApp->getIPCounter();
    counter->setOption(*(unsigned char*)((char*)pkt + 0xa), *(unsigned char*)((char*)pkt + 0xb));


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
        PacketHeader* rpkt = pkt;
        if (*(char*)((char*)pkt + 0xa) == 0)
        {
            unsigned int grp = *(unsigned int*)((char*)pkt + 0xb);
            if (grp != ((unsigned int)m_pclApp->Get_ServerGroup() & 0xff))
            {
                unsigned int g = *(unsigned int*)((char*)pkt + 0xb);
                DNF_LOG_SCOPE_LINE(0x146d, "./log/ItemLimitEdition", "(Ignore another server msg: %d)", g);
                return;
            }
        }
        else
        {
            *(unsigned int*)((char*)pkt + 0xb) =
                (unsigned int)m_pclApp->Get_ServerGroup() & 0xff;
        }
        ((CServerHandler*)*(void**)((char*)m_pclApp + 0xa0))->SendToDB(pkt);
        DNF_LOG_SCOPE_LINE(0x1474,"./log/ItemLimitEdition",
            "(FullLoad: %d, ServerType:%d, LoadTargetNum: %d, IPGNO: "
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
            *(unsigned int*)((char*)pkt + 0xb), *(unsigned int*)((char*)pkt + 0xf),
            *(unsigned int*)((char*)pkt + 0x13), *(unsigned int*)((char*)pkt + 0x17),
            *(unsigned int*)((char*)pkt + 0x1b), *(unsigned int*)((char*)pkt + 0x1f),
            *(unsigned int*)((char*)pkt + 0x23), *(unsigned int*)((char*)pkt + 0x27),
            *(unsigned int*)((char*)pkt + 0x2b), *(unsigned int*)((char*)pkt + 0x2f),
            *(unsigned int*)((char*)pkt + 0x33), *(unsigned int*)((char*)pkt + 0x37),
            *(unsigned int*)((char*)pkt + 0x3b), *(unsigned int*)((char*)pkt + 0x3f),
            *(unsigned int*)((char*)pkt + 0x43), *(unsigned int*)((char*)pkt + 0x47),
            *(unsigned int*)((char*)pkt + 0x4b), *(unsigned int*)((char*)pkt + 0x4f),
            *(unsigned int*)((char*)pkt + 0x53), *(unsigned int*)((char*)pkt + 0x57),
            *(unsigned int*)((char*)pkt + 0x5b), *(unsigned int*)((char*)pkt + 0x5f),
            *(unsigned int*)((char*)pkt + 0x63), *(unsigned int*)((char*)pkt + 0x67),
            *(unsigned int*)((char*)pkt + 0x6b), *(unsigned int*)((char*)pkt + 0x6f));
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
        PacketHeader* rpkt = pkt;
        if (*(int*)((char*)pkt + 0xb) != 0)
        {
            if (*(char*)((char*)pkt + 0xa) != 0)
            {
                m_pclApp->getItemLimitEditionMgr()->clear();
            }
            for (unsigned int i = 0; i < *(unsigned int*)((char*)pkt + 0xb); i++)
            {
                unsigned int ipgno = *(unsigned int*)((char*)pkt + i * 0x48 + 0xf);
                if (ipgno > 799999 && ipgno < 1000000)
                {
                    m_pclApp->getItemLimitEditionMgr()->registItem(
                        *(stItemLimitEditionItemInfo_t*)((char*)pkt + i * 0x48 + 0xf));
                }
            }
            DNF_LOG_SCOPE_LINE(0x14c2,"./log/ItemLimitEdition",
                "(FullLoad: %d, LoadTargetNum: %d, IPGNO: "
                "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
                *(unsigned int*)((char*)pkt + 0xb), *(unsigned int*)((char*)pkt + 0xf),
                *(unsigned int*)((char*)pkt + 0x57), *(unsigned int*)((char*)pkt + 0x9f),
                *(unsigned int*)((char*)pkt + 0xe7), *(unsigned int*)((char*)pkt + 0x12f),
                *(unsigned int*)((char*)pkt + 0x177), *(unsigned int*)((char*)pkt + 0x1bf),
                *(unsigned int*)((char*)pkt + 0x207), *(unsigned int*)((char*)pkt + 0x24f),
                *(unsigned int*)((char*)pkt + 0x297), *(unsigned int*)((char*)pkt + 0x2df),
                *(unsigned int*)((char*)pkt + 0x327), *(unsigned int*)((char*)pkt + 0x36f),
                *(unsigned int*)((char*)pkt + 0x3b7), *(unsigned int*)((char*)pkt + 0x3ff),
                *(unsigned int*)((char*)pkt + 0x447), *(unsigned int*)((char*)pkt + 0x48f),
                *(unsigned int*)((char*)pkt + 0x4d7), *(unsigned int*)((char*)pkt + 0x51f),
                *(unsigned int*)((char*)pkt + 0x567), *(unsigned int*)((char*)pkt + 0x5af),
                *(unsigned int*)((char*)pkt + 0x5f7), *(unsigned int*)((char*)pkt + 0x63f),
                *(unsigned int*)((char*)pkt + 0x687));
            if (m_pclApp->getItemLimitEditionMgr()->isEmpty() != 1)
            {
                Packet_Item_Limit_Edition_Sell_Start pkt2;
                *(char*)((char*)&pkt2 + 0xa) = *(char*)((char*)pkt + 0xa);
                if (*(char*)((char*)pkt + 0xa) == 0)
                {
                    for (unsigned int i = 0; i < *(unsigned int*)((char*)pkt + 0xb); i++)
                    {
                        CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()
                                                      ->getItemInfo(
                                                          *(unsigned int*)((char*)pkt +
                                                                           i * 0x48 + 0xf));
                        if (item != 0)
                        {
                            item->makeItemInfo(
                                *(stItemLimitEditionItemInfo_t*)((char*)&pkt2 +
                                                                 i * 0x48 + 0xf));
                            *(int*)((char*)&pkt2 + 0xb) =
                                *(int*)((char*)&pkt2 + 0xb) + 1;
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
        PacketHeader* rpkt = pkt;
        unsigned int stype = *(unsigned int*)((char*)pkt + 0xa);
        if (stype == ((unsigned int)m_pclApp->Get_ServerGroup() & 0xff))
        {
            for (unsigned int i = 0; i < *(unsigned int*)((char*)pkt + 0xe); i++)
            {
                CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()->getItemInfo(
                    *(unsigned int*)((char*)pkt + 0x12 + i * 4));
                if (item != 0)
                {
                    m_pclApp->getItemLimitEditionMgr()->removeItem(
                        *(unsigned int*)((char*)pkt + 0x12 + i * 4));
                }
            }
            DNF_LOG_SCOPE_LINE(0x1519,"./log/ItemLimitEdition",
                "(ServerType: %d, SellEndNum: %d, IPGNO: "
                "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                *(unsigned int*)((char*)pkt + 0xa), *(unsigned int*)((char*)pkt + 0xe),
                *(unsigned int*)((char*)pkt + 0x12), *(unsigned int*)((char*)pkt + 0x16),
                *(unsigned int*)((char*)pkt + 0x1a), *(unsigned int*)((char*)pkt + 0x1e),
                *(unsigned int*)((char*)pkt + 0x22), *(unsigned int*)((char*)pkt + 0x26),
                *(unsigned int*)((char*)pkt + 0x2a), *(unsigned int*)((char*)pkt + 0x2e),
                *(unsigned int*)((char*)pkt + 0x32), *(unsigned int*)((char*)pkt + 0x36),
                *(unsigned int*)((char*)pkt + 0x3a), *(unsigned int*)((char*)pkt + 0x3e),
                *(unsigned int*)((char*)pkt + 0x42), *(unsigned int*)((char*)pkt + 0x46),
                *(unsigned int*)((char*)pkt + 0x4a), *(unsigned int*)((char*)pkt + 0x4e),
                *(unsigned int*)((char*)pkt + 0x52), *(unsigned int*)((char*)pkt + 0x56),
                *(unsigned int*)((char*)pkt + 0x5a), *(unsigned int*)((char*)pkt + 0x5e),
                *(unsigned int*)((char*)pkt + 0x62), *(unsigned int*)((char*)pkt + 0x66),
                *(unsigned int*)((char*)pkt + 0x6a), *(unsigned int*)((char*)pkt + 0x6e));
            m_pclApp->Get_ServerHandler()->SendAllTcpGameServer(pkt);
        }
        else
        {
            unsigned int v = *(unsigned int*)((char*)pkt + 0xa);
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
        CTcpGameServer* tcp = (CTcpGameServer*)m_pclApp->FindTcpGameServer(
            *(unsigned int*)((char*)pkt + 6));
        if (tcp != 0)
        {
            time_t now = time(0);
            char* buf = tcp->makePacketHeader(0x100a, 0x1e1);
            if (buf != 0)
            {
                CItemLimitEdition* item = m_pclApp->getItemLimitEditionMgr()->getItemInfo(
                    *(unsigned int*)((char*)pkt + 0xe));
                bool expired = (item == 0 || item->getSellEndTime() < (unsigned int)now);
                if (expired)
                {
                    *(unsigned int*)(buf + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
                    buf[0x16] = 1;
                    tcp->SendToGameServer(buf);
                }
                else
                {
                    *(unsigned int*)(buf + 0xa) = *(unsigned int*)((char*)pkt + 0xa);
                    *(unsigned int*)(buf + 0xe) = *(unsigned int*)((char*)pkt + 0xe);
                    if (item->isSellComplete() == 0)
                    {
                        unsigned int num = *(unsigned int*)((char*)pkt + 0xe);
                        m_pclApp->getItemLimitEditionMgr()->updateItem(
                            num, item->getSellNum() + 1);
                        buf[0x16] = 0;
                    }
                    else
                    {
                        buf[0x16] = 2;
                    }
                    *(unsigned int*)(buf + 0x12) = item->getSellNum();
                    memcpy(buf + 0x17, (char*)pkt + 0x12, 0x1ca);
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
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    int targetCharNo = 0;
    if (*(char*)((char*)pkt + 0xe) != 0 &&
        (unsigned char)*(char*)((char*)pkt + 0xe) < 0x1e)
    {
        CUser* target = userMgr->FindUser_CharName((char*)pkt + 0xf);
        targetCharNo = target != 0 ? (int)target->GetUniqCharNo() : -1;
    }
    if (targetCharNo != 0)
    {
        CUser* userA = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
        if (userA != 0)
        {
            Packet_Notice_Find_Factory_Hub_User reply;
            CUser* userB = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0xa));
            if (userB == 0)
            {
                reply.m_idByChannel = userA->GetIdByChannel();
                reply.m_found = 0;
                reply.m_nameLen = *(unsigned char*)((char*)pkt + 0xe);
                strncpy(reply.m_name, (char*)pkt + 0xf,
                        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
                reply.m_field2e = *(unsigned short*)((char*)pkt + 0x2d);
                reply.m_field30 = *(unsigned int*)((char*)pkt + 0x2f);
                *(unsigned short*)((char*)&reply + 2) = 0x34;
                userA->SendToGameserver((char*)&reply, 0x34);
            }
            else
            {
                reply.m_idByChannel = userB->GetIdByChannel();
                reply.m_found = 1;
                reply.m_nameLen = 0;
                reply.m_field2e = *(unsigned short*)((char*)pkt + 0x2d);
                reply.m_field30 = *(unsigned int*)((char*)pkt + 0x2f);
                *(unsigned short*)((char*)&reply + 2) = 0x34;
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
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
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
            CUser* target = ((CUserManager*)((char*)m_pclApp + 0x10))
                                ->FindUser_CharNo((unsigned int)charNo);
            if (target != 0)
            {
                Packet_Set_CleanPad_Point reply;
                reply.m_idByChannel = target->GetIdByChannel();
                reply.m_fieldE = *(unsigned short*)((char*)pkt + 0xe);
                *(unsigned short*)((char*)&reply + 2) = 0x10;
                target->SendToGameserver((char*)&reply, 0x10);
            }
            return;
        }
        throw CDNFException("CPacketTranslater::OnSetCleanPadPoint");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1660, "%s", "CPacketTranslater::OnSetCleanPadPoint() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnNoticeGuildChatMsg() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1666, "%s", "CPacketTranslater::OnSetCleanPadPoint() Exception Break");
    }
}

void CPacketTranslater::OnResponseIPCounterList(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnResponseIPCounterList : 0 == m_pclApp");
    }
    DNF_LOG_SCOPE_LINE(0x16c8,"./log/Secu", "[IP Counter] DataStats : %d, DataSize : %d ",
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xb));
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));


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
    DNF_LOG_SCOPE_LINE(0x16e8,"./log/Secu", "[D_IP Counter] DataStats : %d, DataSize : %d ",
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xb));
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));


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
        char buf[15];
        *(unsigned int*)buf = *(unsigned int*)pkt;
        *(unsigned int*)(buf + 4) = *(unsigned int*)((char*)pkt + 4);
        *(unsigned int*)(buf + 8) = *(unsigned int*)((char*)pkt + 8);
        *(unsigned short*)(buf + 0xc) = *(unsigned short*)((char*)pkt + 0xc);
        *(char*)(buf + 0xe) = *(char*)((char*)pkt + 0xe);
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        if (*(char*)((char*)pkt + 0xa) == 0)
        {
            handler->SendAllToGameServer(buf, *(unsigned short*)((char*)pkt + 2));
        }
        else
        {
            handler->SendToGameServer(*(unsigned char*)((char*)pkt + 0xa),
                                      (PacketHeader*)buf);
        }
        DNF_LOG_SCOPE_LINE(0x1710,"./log/ScreenShot", "Recv TakeScreenShot Command! channel(%d) time(%d)",
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xa),
            *(unsigned int*)((char*)pkt + 0xb));
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnTakeScreenShot() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1717, "%s", "CPacketTranslater::OnTakeScreenShot() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnTakeScreenShot() Exception Break");
        DNF_LOG_SCOPE_LINE(0x171d, "%s", "CPacketTranslater::OnTakeScreenShot() Exception Break");
    }
}

void CPacketTranslater::OnVillageMonsterFightResult(PacketHeader* pkt)
{try
{


    unsigned int users[4] = {0, 0, 0, 0};
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    for (int i = 0; i < 4; i++)
    {
        unsigned int key = *(unsigned int*)((char*)pkt + (i + 4) * 4 + 10);
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
        ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
            *(unsigned int*)((char*)pkt + 0xe));
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1764,"%s", "CPacketTranslater::OnVillageAttackedGMCommand() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1769, "%s", "CPacketTranslater::OnVillageAttackedGMCommand() Exception Break\n");
    }
}

void CPacketTranslater::OnVillageAttackedRank(PacketHeader* pkt) {}

void CPacketTranslater::OnMonitorFullLevelBroadCast(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
    }
}

void CPacketTranslater::OnSetARSInfo(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnSetARSInfo : 0 == m_pclApp");
    }
    DNF_LOG_SCOPE_LINE(0x183d, "./log/Secu", "[ARS_INFO] DBMW -> Monitor -> GameSvr");
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, 0x4bf);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    if (handler != 0)
    {
        DNF_LOG_SCOPE_LINE(0x181d, "./log/Secu", "[ARS_INFO] Web -> Monitor -> DBMW");
        handler->SendDBMWRequestARSInfo(*(unsigned char*)((char*)pkt + 0xa));
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
    CTcpGameServer* tcpGs =
        (CTcpGameServer*)m_pclApp->FindTcpGameServer(
            *(unsigned int*)((char*)pkt + 0xa));
    if (tcpGs != 0)
    {
        int type = *(int*)((char*)pkt + 0x12);
        std::string name2((char*)pkt + 0x40);
        std::string name1((char*)pkt + 0x22);
        *(char*)((char*)pkt + 0x15e) = (char)m_pclApp->AddAccusationCharac(
            name1, name2, type, *(char*)((char*)pkt + 0x15e));
        char* buf = tcpGs->makePacketHeader(0x1b66, 0x15f);
        if (buf != 0)
        {
            memcpy(buf, pkt, 0x15f);
            tcpGs->SendToGameServer(buf);
        }
    }
}

void CPacketTranslater::OnGameServerRegist(PacketHeader* pkt)
{
    stServerInfo* info = new stServerInfo;
    *(char*)((char*)info + 0) = *(char*)((char*)pkt + 0xb);
    *(char*)((char*)info + 1) = *(char*)((char*)pkt + 0xc);
    *(char*)((char*)info + 2) = *(char*)((char*)pkt + 0xa);
    *(unsigned short*)((char*)info + 0x14) = *(unsigned short*)((char*)pkt + 0x1d);
    strncpy((char*)info + 3, (char*)pkt + 0xd, 0x10);
    unsigned int sock = *(unsigned int*)((char*)pkt + 6);
    CTcpGameServer* tcp =
        (CTcpGameServer*)m_pclApp->Get_ServerHandler()->GetTcpGameServer(sock);
    if (tcp != 0)
    {
        DNF_LOG_SCOPE_LINE(0x190c,"./log/GameServer", "Get Packet - OnGameServerRegist from Channel:%d",
            (unsigned int)(unsigned char)*(char*)((char*)info + 1));
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
                tcp->SetChannelNo((unsigned char)*(char*)((char*)pkt + 0xc));
                CGameServer* gs =
                    m_pclApp->Get_ServerHandler()->GetGameServer(sock);
                gs->SetSocket(*(unsigned int*)((char*)pkt + 6));
                out[0xb] = 0;
                CMyFileLog log2("OnGameServerRegist", 0x1930);
                log2("./log/GameServer", "Game server regist success. Channel: %d",
                     (unsigned int)(unsigned char)*(char*)((char*)info + 1));
            }
            else
            {
                out[0xb] = 1;
                CMyFileLog log2("OnGameServerRegist", 0x1923);
                log2("./log/GameServer",
                     "Game server regist failed. Channel: %d is already exist.",
                     (unsigned int)(unsigned char)*(char*)((char*)info + 1));
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
    struct Packet_No_Cache_View
    {
        char m_header[0xa];
        unsigned int m_dbid;
    } __attribute__((packed));
    PacketHeader* p = pkt;
    if (((Packet_No_Cache_View*)p)->m_dbid == 0)
    {
        exchange_server::GetInstanceCacheCharacterMgr()->Reset();
    }
    else
    {
        exchange_server::CACHE_CHARACTER_TYPE type;
        if (exchange_server::GetInstanceCacheCharacterMgr()->GetCacheCharacter(
                ((Packet_No_Cache_View*)p)->m_dbid, &type))
        {
            register char* s;
            register int f0;
            register int f4;
            f4 = type.m_field4;
            f0 = type.m_field0;
            s = NumberToString(((Packet_No_Cache_View*)p)->m_dbid, 0);
            DNF_LOG_SCOPE_LINE(0x1970,"./log/ExchangeServer", "OnNoCache() (%s,%d,%d)\n", s, f0, f4);
        }
    }
}

void CPacketTranslater::OnDisableUserOneToOneChat_GM(PacketHeader* pkt)
{
    if (m_pclApp != 0)
    {
        unsigned int channel = *(unsigned int*)((char*)pkt + 0xa);
        if (m_pclApp->isGM_regFromChannel(channel) != 0)
        {
            CUser* target =
                ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharName((char*)pkt + 0x12);
            if (target != 0)
            {
                m_pclApp->DisableChatUserWithGM(channel, target->GetUniqCharNo());
            }
        }
    }
}

void CPacketTranslater::OnFindCharacName_useUID(PacketHeader* pkt)
{
    CTcpGameServer* tcpGs =
        (CTcpGameServer*)m_pclApp->FindTcpGameServer(
            *(unsigned int*)((char*)pkt + 0xe));
    if (tcpGs != 0)
    {
        char* buf = tcpGs->makePacketHeader(0x1f45, 0x34);
        *(unsigned int*)(buf + 10) = *(unsigned int*)((char*)pkt + 10);
        *(unsigned int*)(buf + 0xe) = *(unsigned int*)((char*)pkt + 0xe);
        buf[0x12] = 0;
        buf[0x13] = 0;
        buf[0x14] = 0;
        buf[0x15] = 0;
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            char* name = user->GetCharName();
            unsigned int len = (unsigned int)strlen(name);
            *(unsigned int*)(buf + 0x12) = len;
            if (0x1d < *(unsigned int*)(buf + 0x12))
            {
                buf[0x12] = 0;
                buf[0x13] = 0;
                buf[0x14] = 0;
                buf[0x15] = 0;
            }
            if (*(int*)(buf + 0x12) != 0)
            {
                strncpy(buf + 0x16, user->GetCharName(),
                        *(unsigned int*)(buf + 0x12));
            }
        }
        tcpGs->SendToGameServer(buf);
    }
}

void CPacketTranslater::OnRenew_GM_List(PacketHeader* pkt)
{
    WongWork::CGMAccounts* gm = (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm != 0 && pkt != 0)
    {
        if (*(char*)((char*)pkt + 0xa) == 0)
        {
            gm->clearGmList();
        }
        for (int i = 0; i < (int)(char)*(char*)((char*)pkt + 0xb); i++)
        {
            gm->AppendGM_Sys(*(unsigned int*)((char*)pkt + i * 4 + 0xc),
                             *(char*)((char*)pkt + i + 0x5c));
        }
        CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
        handler->SendToDB(pkt);
    }
}

void CPacketTranslater::OnLoadPeriodicMessage(PacketHeader* pkt)
{try
{


    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendToDB(pkt);
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
        unsigned int endHour = *(unsigned int*)((char*)pkt + 0x20e);
        unsigned int startHour = *(unsigned int*)((char*)pkt + 0x20a);
        DNF_LOG_SCOPE_LINE(0x19f7,"./log/PeriodicMessage",
            "DB Load Message : Message(%s), start_hour(%d), end_hour(%d)", (char*)pkt + 0xa,
            startHour, endHour);
        ((CPeriodicMessageMgr*)m_pclApp->GetPeriodicMessageManager())
            ->SetMessageData((char*)pkt + 0xa, (int)startHour, (int)endHour);
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a02,"%s", "CPacketTranslater::OnResultLoadPeriodicMessage() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a07, "%s", "CPacketTranslater::OnResultLoadPeriodicMessage() Exception Break");
    }
}

void CPacketTranslater::OnRegisterEventIdx(PacketHeader* pkt)
{try
{


    unsigned int idx = *(unsigned int*)((char*)pkt + 0xa);
    DNF_LOG_SCOPE_LINE(0x1a15,"./log/OnTimeEvent", "OnRegisterEventIdx:result =%d, Eventidx =%d",
        (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe), idx);
    if (*(char*)((char*)pkt + 0xe) != 0)
    {
        ((COnTimeEventManager*)*(void**)((char*)m_pclApp + 800))->SetEventIdx(idx);
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
        unsigned short errType = *(unsigned short*)((char*)pkt + 0x12);
        unsigned int idx = *(unsigned int*)((char*)pkt + 0xe);
        unsigned int id = *(unsigned int*)((char*)pkt + 0xa);
        if (m_pclApp == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1a35,"./log/OnTimeEvent",
                "OnRegisterEventUserIdx:id = %u , idx = %u, errortype = %d", id, idx,
                (unsigned int)errType);
        }
        else
        {
            unsigned int curIdx =
                ((COnTimeEventManager*)*(void**)((char*)m_pclApp + 800))->GetEvent_Idx();
            DNF_LOG_SCOPE_LINE(0x1a30,"./log/OnTimeEvent",
                "OnRegisterEventUserIdx:id = %u , rcv_idx = %u, cur_idx = %d, errortype = %d",
                id, idx, curIdx, (unsigned int)errType);
        }
        if (*(short*)((char*)pkt + 0x12) == 0 || *(short*)((char*)pkt + 0x12) == 3)
        {
            CUser* user = m_pclApp->Get_UserManager()->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
            if (user != 0)
            {
                user->SetEvent_idx(idx);
                if (*(short*)((char*)pkt + 0x12) == 3)
                {
                    user->Event_idx_modify_state();
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a4a,"%s", "CPacketTranslater::OnRegisterEventUserIdx() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a4f, "%s", "CPacketTranslater::OnRegisterEventUserIdx() Exception Break");
    }
}

void CPacketTranslater::OnRegisterEventItem(PacketHeader* pkt)
{
    try
    {
        PacketHeader* rpkt = pkt;
        unsigned int errortype = (unsigned int)*(unsigned short*)((char*)pkt + 0x12);
        unsigned int cnt = *(unsigned int*)((char*)pkt + 0xe);
        unsigned int idx = *(unsigned int*)((char*)pkt + 0xa);
        DNF_LOG_SCOPE_LINE(0x1a5a,"./log/OnTimeEvent", "OnRegisterEventItem:idx = %u , cnt = %u, errortype = %d",
            idx, cnt, errortype);
        if (*(short*)((char*)pkt + 0x12) == 0)
        {
            if (*(int*)((char*)pkt + 0xa) == 0 || *(int*)((char*)pkt + 0xe) == 0)
            {
                CMyFileLog log2("OnRegisterEventItem", 0x1a66);
                log2("./log/OnTimeEvent", "wrong item data", idx, cnt, errortype);
            }
            else if (m_pclApp != 0)
            {
                COnTimeEventManager* mgr =
                    *(COnTimeEventManager**)((char*)m_pclApp + 0x320);
                mgr->SetEventItem(*(unsigned int*)((char*)pkt + 0xa),
                                  *(unsigned int*)((char*)pkt + 0xe));
                mgr->StartEvent();
            }
        }
        else
        {
            CMyFileLog log3("OnRegisterEventItem", 0x1a60);
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
        if (m_pclApp != 0)
        {
            COnTimeEventManager* mgr =
                (COnTimeEventManager*)*(void**)((char*)m_pclApp + 800);
            unsigned int curIdx = mgr->GetEvent_Idx();
            unsigned int newIdx = *(unsigned int*)((char*)pkt + 0xa);
            DNF_LOG_SCOPE_LINE(0x1a85,"./log/OnTimeEvent",
                "OnResultRegisterEventIdx:event_idx(%d) , cur_idx(%d)", newIdx, curIdx);
            if (mgr->GetEvent_Idx() < newIdx)
            {
                mgr->SetEventIdx(newIdx);
            }
            mgr->SendContinueTimeToGS();
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1a92,"%s", "CPacketTranslater::OnResultRegisterEventIdx() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1a97, "%s", "CPacketTranslater::OnResultRegisterEventIdx() Exception Break");
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
    if (*(char*)((char*)pkt + 0xa) == 0)
    {
        village_attacked::SetRealConfig();
    }
    else
    {
        village_attacked::SetGMConfig(*(unsigned int*)((char*)pkt + 0xb),
                                      *(unsigned int*)((char*)pkt + 0xf),
                                      *(unsigned int*)((char*)pkt + 0x13));
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
        int charNo = 0;
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
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
            CUser* target = ((CUserManager*)((char*)m_pclApp + 0x10))
                                ->FindUser_CharNo((unsigned int)charNo);
            if (target != 0)
            {
                Packet_Punish_Cancel reply;
                reply.m_idByChannel = target->GetIdByChannel();
                reply.m_fieldE = *(unsigned short*)((char*)pkt + 0xe);
                reply.m_field10 = *(unsigned short*)((char*)pkt + 0x10);
                *(unsigned short*)((char*)&reply + 2) = 0x12;
                target->SendToGameserver((char*)&reply, 0x12);
            }
            return;
        }
        throw CDNFException("CPacketTranslater::OnMonitorPunishCancel");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnMonitorPunishCancel() Exception Break : %s\n", e.what());
        DNF_LOG_SCOPE_LINE(0x1bf5,"%s", "CPacketTranslater::OnMonitorPunishCancel() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnMonitorPunishCancel() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1bfb, "%s", "CPacketTranslater::OnMonitorPunishCancel() Exception Break\n");
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
            CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
            handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
            DNF_LOG_SCOPE_LINE(0x1c14,"./log/WebNotice", "OnBroadcastMsg : (%s,%d)\n", (char*)pkt + 0xf,
                (unsigned int)(unsigned char)*(char*)((char*)pkt + 0xe));
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1c18, "%s", "CPacketTranslater::OnBroadcastMsg Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1c1d, "%s", "CPacketTranslater::OnBroadcastMsg Exception Break");
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
        int charNo = 0;
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser(
                *(unsigned int*)((char*)pkt + 0xa));
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
            CUser* target = ((CUserManager*)((char*)m_pclApp + 0x10))
                                ->FindUser_CharNo((unsigned int)charNo);
            if (target != 0)
            {
                Packet_SecuService_Connect_Web reply;
                memcpy(&reply, pkt, 0x15);
                *(unsigned int*)((char*)&reply + 0xa) = target->GetIdByChannel();
                target->SendToGameserver((char*)&reply,
                                         *(unsigned short*)((char*)&reply + 2));
            }
            return;
        }
        throw CDNFException("CPacketTranslater::OnMonitorSecuServiceConnWeb");
    }
    catch (CDNFException& e)
    {
        printf("CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break : %s\n",
               e.what());
        DNF_LOG_SCOPE_LINE(0x1c4f,"%s", "CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        puts("CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break");
        DNF_LOG_SCOPE_LINE(0x1c55, "%s", "CPacketTranslater::OnMonitorSecuServiceConnWeb() Exception Break\n");
    }
}

void CPacketTranslater::OnResetTODAPCInfo(PacketHeader* pkt) {}

void CPacketTranslater::OnNoticeMemberChatMsgHyperLink(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::OnNoticeMemberChatMsgHyperLink : 0 == m_pclApp");
    }
    if (*(unsigned int*)((char*)pkt + 0xa) != 0 && *(char*)((char*)pkt + 0x147) != 0)
    {
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))->FindUser_CharNo(
                *(unsigned int*)((char*)pkt + 0xa));
        if (user != 0)
        {
            CMember* member =
                ((CMemberManager*)((char*)m_pclApp + 0x2d0))->FindMember(
                    *(unsigned int*)((char*)pkt + 0xa));
            if (member != 0)
            {
                member->NoticeChatMsgToMemberMembersHyperLink(
                    (char*)pkt + 0x148,
                    (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x147),
                    *(unsigned char*)((char*)pkt + 0xe),
                    (hyperlink_item_info*)((char*)pkt + 0xf), user);
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
    CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
    if (*(char*)((char*)pkt + 0x1b) != 0 &&
        (unsigned char)*(char*)((char*)pkt + 0x1b) < 0x1e)
    {
        CUser* target = userMgr->FindUser_CharName((char*)pkt + 0x1c);
        *(unsigned int*)((char*)pkt + 0x17) =
            target != 0 ? target->GetUniqCharNo() : 0xffffffff;
    }
    if (*(unsigned int*)((char*)pkt + 0x13) == 0 ||
        *(unsigned int*)((char*)pkt + 0x17) == 0 ||
        *(char*)((char*)pkt + 0x173) == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1d71,"./log/Except",
            "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink, sender(%d), "
            "receiver(%d), msglen(%d)",
            *(unsigned int*)((char*)pkt + 0x13), *(unsigned int*)((char*)pkt + 0x17),
            (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x173));
        throw CDNFException(
            "CPacketTranslater::OnNoticeOtherChannelChatMsgHyperLink : packet->m_uSenderCharID "
            "&&  packet->m_uRecverCharID && packet->m_msgLen");
    }
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link reply;
    reply.m_senderCharId = *(unsigned int*)((char*)pkt + 0xa);
    CUser* sender = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x13));
    if (sender == 0)
    {
        return;
    }
    CUser* receiver = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x17));
    if (receiver == 0)
    {
        reply.m_idByChannel = sender->GetIdByChannel();
        reply.m_uniqCharNo = sender->GetUniqCharNo();
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        reply.m_type = 1;
        reply.m_itemCount = *(unsigned char*)((char*)pkt + 0x3a);
        for (int i = 0; i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a); i++)
        {
            memcpy((char*)reply.m_items + i * 0x68, (char*)pkt + i * 0x68 + 0x3b, 0x68);
        }
        *(unsigned short*)((char*)&reply + 2) = 0x170;
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
        reply.m_itemCount = *(unsigned char*)((char*)pkt + 0x3a);
        for (int i = 0; i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a); i++)
        {
            memcpy((char*)reply.m_items + i * 0x68, (char*)pkt + i * 0x68 + 0x3b, 0x68);
        }
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        *(unsigned short*)((char*)&reply + 2) = 0x170;
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
        reply.m_itemCount = *(unsigned char*)((char*)pkt + 0x3a);
        for (int i = 0; i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a); i++)
        {
            memcpy((char*)reply.m_items + i * 0x68, (char*)pkt + i * 0x68 + 0x3b, 0x68);
        }
        memcpy(reply.m_name, (char*)pkt + 0x1c, 0x1d);
        *(unsigned short*)((char*)&reply + 2) = 0x170;
        sender->SendToGameserver((char*)&reply, 0x170);
    }
    else
    {
        memcpy(reply.m_name, sender->GetCharName(), 0x1d);
        reply.m_idByChannel = receiver->GetIdByChannel();
        reply.m_uniqCharNo = receiver->GetUniqCharNo();
        reply.m_itemCount = *(unsigned char*)((char*)pkt + 0x3a);
        for (int i = 0; i < (int)(unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3a); i++)
        {
            memcpy((char*)reply.m_items + i * 0x68, (char*)pkt + i * 0x68 + 0x3b, 0x68);
        }
        reply.m_msgLen = *(unsigned char*)((char*)pkt + 0x173);
        memcpy(reply.m_msg, (char*)pkt + 0x174,
               (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x173));
        *(unsigned short*)((char*)&reply + 2) =
            (unsigned short)((unsigned char)*(char*)((char*)pkt + 0x173) + 0x170);
        receiver->SendToGameserver((char*)&reply, *(unsigned short*)((char*)&reply + 2));
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
    *(char*)((char*)pkt + 0xa) = (char)m_pclApp->Get_ServerGroup();
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
}

void CPacketTranslater::onSocialEventRewardItemRequest(PacketHeader* pkt)
{try
{


    if (m_pclApp == 0)
    {
        throw CDNFException("CPacketTranslater::onSocialEventRewardItemRequest");
    }
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendToDB(pkt);


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
        PacketHeader* rpkt = pkt;
        LimitNpcBuyItemManager* mgr = m_pclApp->getLimitNpcBuyItemManager();
        mgr->registItemClear();
        unsigned int i = 0;
        while (i < *(unsigned int*)((char*)pkt + 0xa) && i < 0x1e)
        {
            NpcBuyLimitItem* item = (NpcBuyLimitItem*)((char*)pkt + i * 0xc + 0xe);
            mgr = m_pclApp->getLimitNpcBuyItemManager();
            mgr->registItem(*item);
            unsigned int c = *(unsigned int*)((char*)pkt + i * 0xc + 0x16);
            unsigned int b = *(unsigned int*)((char*)pkt + i * 0xc + 0x12);
            unsigned int a = *(unsigned int*)((char*)pkt + i * 0xc + 0xe);
            DNF_LOG_SCOPE_LINE(0x1dd0,"./log/NpcBuyLimitItem", "Load-> itemId: %d, maxCount: %d, sellCount: %d)",
                a, b, c);
            i++;
        }
        ((CServerHandler*)*(void**)((char*)m_pclApp + 0xa0))
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
        PacketHeader* rpkt = pkt;
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))
                ->FindUser(*(unsigned int*)((char*)pkt + 0xe));
        if (user == 0)
        {
            unsigned int cn = *(unsigned int*)((char*)pkt + 0x12);
            char* s = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 0);
            DNF_LOG_SCOPE_LINE(0x1dee,"./log/Except",
                "CPacketTranslater::onSocialEventRewardItemInfo(), buyUser(%s), "
                "characNo(%u)",
                s, cn);
            throw CDNFException(
                "CPacketTranslater::onSocialEventRewardItemInfo : Not Exist User characNo");
        }
        int result = m_pclApp->getLimitNpcBuyItemManager()->sellNpcLimitBuyItem(
            (LimitNpcBuyItemInfo*)pkt);
        if (result < 1)
        {
            user->SendTcpGameserver(pkt);
        }
        else
        {
            *(unsigned int*)((char*)pkt + 0x1a) = 0;
            *(int*)((char*)pkt + 0x22) = result;
            user->SendTcpGameserver(pkt);
            unsigned int buyCount = *(unsigned int*)((char*)pkt + 0x1a);
            unsigned int itemId = *(unsigned int*)((char*)pkt + 0x16);
            unsigned int errorNo = *(unsigned int*)((char*)pkt + 0x22);
            unsigned int charNo = *(unsigned int*)((char*)pkt + 0x12);
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
        PacketHeader* rpkt = pkt;
        CUser* user =
            ((CUserManager*)((char*)m_pclApp + 0x10))
                ->FindUser(*(unsigned int*)((char*)pkt + 0xe));
        if (user == 0)
        {
            unsigned int cn = *(unsigned int*)((char*)pkt + 0x12);
            char* s = NumberToString(*(unsigned int*)((char*)pkt + 0xe), 0);
            DNF_LOG_SCOPE_LINE(0x1e1c,"./log/Except",
                "CPacketTranslater::onSocialEventRewardItemInfoAll(), buyUser(%s), "
                "characNo(%u)",
                s, cn);
            throw CDNFException(
                "CPacketTranslater::onSocialEventRewardItemInfoAll : Not Exist User");
        }
        m_pclApp->getLimitNpcBuyItemManager()->getNpcLimitBuyItemInfoAll(
            (LimitNpcBuyItemInfoAll*)pkt);
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
        PacketHeader* rpkt = pkt;
        LimitNpcBuyItemChangeInfo change;
        if (*(int*)((char*)pkt + 0x16) == 0)
        {
            unsigned int itemId = *(unsigned int*)((char*)pkt + 0xa);
            LimitNpcBuyItemManager* mgr = m_pclApp->getLimitNpcBuyItemManager();
            mgr->getNpcLimitBuyItemCount(itemId, change);
            ((CServerHandler*)*(void**)((char*)m_pclApp + 0xa0))
                ->SendAllTcpGameServer(&change);
            ((CServerHandler*)*(void**)((char*)m_pclApp + 0xa0))->SendToDB(pkt);
            unsigned int buyCount = *(unsigned int*)((char*)pkt + 0x12);
            unsigned int itemId2 = *(unsigned int*)((char*)pkt + 0xa);
            unsigned int charNo = *(unsigned int*)((char*)pkt + 0xe);
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
    CUser* requester = 0;
    CUser* target = 0;
    if (m_pclApp != 0)
    {
        CUserManager* userMgr = (CUserManager*)((char*)m_pclApp + 0x10);
        PacketHeader* rpkt = pkt;
        try
        {
            requester = userMgr->FindUser_CharNo(*(unsigned int*)((char*)pkt + 0x1f));
            if (requester != 0)
            {
                Packet_Monitor_Reply_Charac_Info reply;
                *(unsigned int*)((char*)&reply + 0xa) = 0;
                *(unsigned int*)((char*)&reply + 0xe) = requester->GetIdByChannel();
                strncpy((char*)&reply + 0x17, (char*)pkt + 0xa, 0x1d);
                std::string sname((char*)pkt + 0xa);
                target = userMgr->FindUser_CharName(sname);
                bool notfound = (target == 0);
                if (notfound)
                {
                    *(char*)((char*)&reply + 0x16) = 0;
                }
                else
                {
                    CServerInterface* gs = (CServerInterface*)target->GetGameServer();
                    *(char*)((char*)&reply + 0x16) = gs->GetChannelNo();
                    *(unsigned int*)((char*)&reply + 0x12) = target->GetIdByChannel();
                    *(unsigned short*)((char*)&reply + 0x35) = target->GetLevel();
                    *(char*)((char*)&reply + 0x37) = target->GetJob();
                    *(char*)((char*)&reply + 0x38) = target->GetGrowthType();
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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllTcpGameServer(&reply);
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
        char* c = (char*)m_pclApp->getCollectItems();
        unsigned int cur = *(unsigned int*)(c + 4);
        if (cur < *(unsigned int*)c)
        {
            if (*(int*)(c + 4) == 0)
            {
                SendColletItemsReward(*(unsigned int*)((char*)pkt + 0xe),
                                      *(unsigned int*)((char*)pkt + 0xa), (char*)pkt + 0x1b,
                                      (int)(unsigned char)*(char*)((char*)pkt + 0x1a),
                                      TimeGateRewardType::TYPE_0);
            }
            else
            {
                int cur2 = *(int*)(c + 4);
                int add = *(int*)((char*)pkt + 0x12);
                if ((unsigned int)(cur2 + add) < *(unsigned int*)c)
                {
                    int cur3 = *(int*)(c + 4);
                    unsigned int rem = *(unsigned int*)(c + 4);
                    if ((cur3 - rem % 0x14) + 0x14 <=
                        (unsigned int)(*(int*)(c + 4) + *(int*)((char*)pkt + 0x12)))
                    {
                        SendColletItemsReward(*(unsigned int*)((char*)pkt + 0xe),
                                              *(unsigned int*)((char*)pkt + 0xa),
                                              (char*)pkt + 0x1b,
                                              (int)(unsigned char)*(char*)((char*)pkt + 0x1a),
                                              TimeGateRewardType::TYPE_1);
                    }
                }
                else
                {
                    SendColletItemsReward(*(unsigned int*)((char*)pkt + 0xe),
                                          *(unsigned int*)((char*)pkt + 0xa),
                                          (char*)pkt + 0x1b,
                                          (int)(unsigned char)*(char*)((char*)pkt + 0x1a),
                                          TimeGateRewardType::TYPE_2);
                    char* c2 = (char*)m_pclApp->getCollectItems();
                    *(long*)(c2 + 8) = (long)time(0);
                }
            }
            char* c3 = (char*)m_pclApp->getCollectItems();
            *(int*)(c3 + 4) = *(int*)(c3 + 4) + *(int*)((char*)pkt + 0x12);
        }
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1fcb, "%s", "CPacketTranslater::onCollectItems() Exception Break : %s\n", e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1fd0, "%s", "CPacketTranslater::onCollectItems() Exception Break");
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
    *(char*)((char*)pkt + 0xa) = (char)m_pclApp->Get_ServerGroup();
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendToDB(pkt);


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
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendToDB(pkt);


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
        PacketHeader* rpkt = pkt;
        for (int i = 0; i < 0x32; i++)
        {
            if (*(char*)((char*)pkt + 10 + i) != 0)
            {
                unsigned char ch = *(unsigned char*)((char*)pkt + 10 + i);
                CTcpGameServer* tcp =
                    m_pclApp->Get_ServerHandler()->GetTcpGameServerByCh(ch);
                if (tcp != 0)
                {
                    char* buf = tcp->makePacketHeader(0x27f2, 0x10a);
                    if (buf != 0)
                    {
                        *(char*)(buf + 10) = *(char*)((char*)pkt + 0x3c);
                        char* out = buf;
                        memset(buf + 0xb, 0, 0xff);
                        memcpy(out + 0xb, (char*)pkt + 0x3d,
                               (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x3c));
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
        m_pclApp->SetMiniCraneRandomSeed();
        *(unsigned int*)((char*)pkt + 0xa) = (unsigned int)m_pclApp->getMiniCraneSeed();
        if (pkt == 0)
        {
            throw CDNFException("CPacketTranslater::OnUpdateMiniCraneSeed, packet is null");
        }
        if (m_pclApp != 0 && *(int*)((char*)m_pclApp + 0xa0) != 0)
        {
            CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
            handler->SendAllToGameServer((char*)pkt, *(unsigned short*)((char*)pkt + 2));
            return;
        }
        throw CDNFException("CPacketTranslater::OnUpdateMiniCraneSeed, m_pclApp == 0");
    }
    catch (CDNFException& e)
    {
        DNF_LOG_SCOPE_LINE(0x1b82,"%s", "CPacketTranslater::OnUpdateMiniCraneSeed() Exception Break : %s\n",
            e.what());
    }
    catch (...)
    {
        DNF_LOG_SCOPE_LINE(0x1b87, "%s", "CPacketTranslater::OnUpdateMiniCraneSeed() Exception Break");
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
    if (pkt == 0)
    {
        DNF_LOG_SCOPE_LINE(0x22e9, "./log/AradOnly", "[Server Event] Packet_StartGameEventFromServer is null.");
        throw 0x22ea;
    }
    Packet_Monitor_Event_Start epkt;
    epkt.m_fieldA = *(unsigned int*)((char*)pkt + 0xa);
    epkt.m_fieldB = *(unsigned short*)((char*)pkt + 0x16);
    epkt.m_fieldC = *(unsigned short*)((char*)pkt + 0x18);
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllTcpGameServer(&epkt);
    DNF_LOG_SCOPE_LINE(0x22f2,"./log/AradOnly", "[Server Event] start event. (event:%d, param:%d,%d)",
        *(unsigned int*)((char*)pkt + 0xa), *(unsigned short*)((char*)pkt + 0x16),
        *(unsigned short*)((char*)pkt + 0x18));


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
    if (pkt == 0)
    {
        DNF_LOG_SCOPE_LINE(0x230b, "./log/AradOnly", "[Server Event] Packet_StopGameEventFromServer is null.");
        throw 0x230c;
    }
    Packet_Monitor_Event_End epkt;
    epkt.m_fieldA = *(unsigned int*)((char*)pkt + 0xa);
    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllTcpGameServer(&epkt);
    DNF_LOG_SCOPE_LINE(0x2312,"./log/AradOnly", "[Server Event] end event. (event:%d)",
        *(unsigned int*)((char*)pkt + 0xa));


    }
    catch (int line)
    {
        DNF_LOG_SCOPE_LINE(0x2316, "./log/AradOnly", "[6ThBirthday] error onEndGameEventFromServer line. (line:%u)", line);
    }
}

void CPacketTranslater::onReloadCountryCode(PacketHeader* pkt)
{try
{


    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllTcpGameServer(pkt);
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


    CServerHandler* handler = (CServerHandler*)*(void**)((char*)m_pclApp + 0xa0);
    handler->SendAllTcpGameServer(pkt);
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
    m_fieldA = 0;
    m_fieldC = 0;
}

Packet_Item_Limit_Edition_Sell_end::Packet_Item_Limit_Edition_Sell_end()
    : PacketHeader(0x1005, 0x82)
{
    m_fieldB = 0;
    m_fieldA = 0;
}

Packet_Item_Limit_Edition_Update::Packet_Item_Limit_Edition_Update()
    : PacketHeader(0x1006, 0x10e)
{
    m_fieldA = 0;
    m_fieldB = 0;
}

Packet_Monitor_Event_Start::Packet_Monitor_Event_Start() : PacketHeader(0x44f, 0x12) {}

Packet_Monitor_Event_End::Packet_Monitor_Event_End() : PacketHeader(0x450, 0xe) {}

Packet_Monitor_Take_Screen_Shot::Packet_Monitor_Take_Screen_Shot()
    : PacketHeader(0x9d3, 0xf)
{
    m_fieldA = 0;
    m_fieldB = 0;
}

Packet_DBMW_Query_Msg::Packet_DBMW_Query_Msg() : PacketHeader(0x177d, 0x1013)
{
    m_fieldA = 0;
    m_fieldB = 0;
    for (int i = 0; i <= 0x1000; i++)
    {
        *(char*)((char*)this + 0x12 + i) = 0;
    }
}

Packet_VillageAttackedRewardServer::Packet_VillageAttackedRewardServer()
    : PacketHeader(0x177a, 0xe)
{
    m_fieldA = 0;
}

Packet_VillageAttackedEnd::Packet_VillageAttackedEnd() : PacketHeader(0x1774, 0x16)
{
    m_dungeonRemain = 0;
    m_fieldE = 0;
    m_field12 = 0;
}

Packet_VillageAttackedUpdate::Packet_VillageAttackedUpdate() : PacketHeader(0x1777, 0x16)
{
    m_remainTime = 0;
    m_fieldE = 0;
    m_field12 = 0;
}

Packet_VillageAttackedScore::Packet_VillageAttackedScore() : PacketHeader(0x1778, 0x26)
{
    m_idByChannel = 0;
    m_uniqCharNo = 0;
    m_remainTime = 0;
    m_field16 = 0;
    m_field1a = 0;
    m_cur = 0;
    m_max = 0;
}

Packet_VillageAttackedReward::Packet_VillageAttackedReward() : PacketHeader(0x1775, 0x1a)
{
    m_idByChannel = 0;
    m_uniqCharNo = 0;
    m_rewardType = 0;
    m_count = 0;
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
    m_fieldA = 0;
    m_upperCharNo = 0;
    m_lowerCharNo = 0;
    m_type = 0;
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
    m_field33 = 0;
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
    memset(m_data, 0, 0x50);
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
    m_fieldA = 0;
    m_charNo = 0;
    m_type = 0;
    m_nameLen = 0;
    memset(m_name, 0, 0x1e);
}

Packet_MTG_OntimeEvent_RewardStart::Packet_MTG_OntimeEvent_RewardStart()
    : PacketHeader(0x17c0, 0x1a)
{
    m_eventIdx = 0;
}

Packet_MTG_OntimeEvent_RewardEnd::Packet_MTG_OntimeEvent_RewardEnd()
    : PacketHeader(0x17c2, 0xa)
{
}

Packet_Arad_DeleteEffect::Packet_Arad_DeleteEffect(int group, int code)
    : PacketHeader(0x27fa, 0x12)
{
    *(int*)((char*)this + 0xa) = group;
    *(int*)((char*)this + 0xe) = code;
}

Packet_DB_InsertMail::Packet_DB_InsertMail() : PacketHeader(0x177c, 0x133)
{
    *(unsigned int*)((char*)this + 0xa) = 0;
    *(unsigned int*)((char*)this + 0xe) = 0;
    *(unsigned int*)((char*)this + 0x12) = 0;
    *(unsigned int*)((char*)this + 0x16) = 0;
    *(unsigned int*)((char*)this + 0x12f) = 0;
    for (int i = 0; i <= 0x14; i++)
    {
        *(char*)((char*)this + 0x1a + i) = 0;
    }
    for (int i = 0; i <= 0xff; i++)
    {
        *(char*)((char*)this + 0x2f + i) = 0;
    }
}

Packet_DBMW_Statistic_Login_Logout::Packet_DBMW_Statistic_Login_Logout()
    : PacketHeader(0x17b8, 0x618),
      m_fieldA(0),
      m_field608(0),
      m_field60c(0),
      m_field610(0),
      m_field614(0)
{
    memset((char*)this + 0xe, 0, 0x5fa);
}

Packet_Monitor_Call_Member_List_ToUser::Packet_Monitor_Call_Member_List_ToUser()
    : PacketHeader(0x4be, 0x1e1)
{
    memset((char*)this + 0x12, 0, 0x1cf);
}

Packet_Punish_Cancel::Packet_Punish_Cancel() : PacketHeader(0xb64, 0x12)
{
    m_idByChannel = 0;
    m_fieldE = 0;
    m_field10 = 0;
}

Packet_Set_CleanPad_Point::Packet_Set_CleanPad_Point() : PacketHeader(0xb60, 0x10)
{
    m_idByChannel = 0;
    m_fieldE = 0;
}

Packet_SecuService_Connect_Web::Packet_SecuService_Connect_Web()
    : PacketHeader(0xb65, 0x15)
{
    m_idByChannel = 0;
    m_fieldE = 0;
    m_fieldF = 0;
    memset(m_data, 0, 5);
}

Packet_Monitor_User_Repel::Packet_Monitor_User_Repel() : PacketHeader(0x4c1, 0x12) {}

Packet_Monitor_Other_Channel_Chat_ToUser::Packet_Monitor_Other_Channel_Chat_ToUser()
    : PacketHeader(0x3f3, 0x137)
{
    m_fieldE = 0;
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
    m_fieldE = 0;
    m_idByChannel = 0xffffffff;
    m_uniqCharNo = 0;
    m_type = 0;
    m_itemCount = 0;
    m_msgLen = 0;
    memset(m_name, 0, 0x1e);
    memset(m_items, 0, 0x138);
    memset(m_msg, 0, 0x100);
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
    m_fieldA = 0;
    m_fieldB = 0;
    m_fieldC = 0;
}

Packet_InnerPakcet_Login::Packet_InnerPakcet_Login() : PacketHeader(0xfa0, 0xa) {}

Packet_InnerPakcet_Logout::Packet_InnerPakcet_Logout() : PacketHeader(0xfa1, 0xa) {}

Packet_Monitor_UDP_HeartBeat::Packet_Monitor_UDP_HeartBeat() : PacketHeader(0x3ec, 0xb)
{
    m_fieldA = 0xff;
}

Packet_MiniCraneSeed::Packet_MiniCraneSeed() : PacketHeader(0x27f8, 0xe)
{
    m_fieldA = 0;
}

Packet_DB_Query_Member::Packet_DB_Query_Member() : PacketHeader(0x4b2, 0xe)
{
    m_fieldA = 0;
}

Packet_DB_Query_Member_Member::Packet_DB_Query_Member_Member() : PacketHeader(0x4b0, 0xe)
{
    m_fieldA = 0;
}

Packet_Send_Time_Sync::Packet_Send_Time_Sync() : PacketHeader(0x1f4b, 0xe)
{
    m_fieldA = 0;
    m_fieldC = 0;
}
