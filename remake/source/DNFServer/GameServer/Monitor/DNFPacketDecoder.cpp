// df_monitor_r — DNFPacketDecoder（从 MonitorTypes/App/Table 拆分）
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

#include "DNFPacketDecoder.h"
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
#include "DNFPacketBuffer.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "PacketCounter.h"
#include "ServerLoadChecker.h"

#define REG_HANDLER(id, fn) m_handlers[id] = (void*)CPacketTranslater::fn

CPacketDecoder::CPacketDecoder()
{
    int i;
    m_udpParseQ = 0;
    m_udpQLock = 0;
    m_net.m_udpBLock = 0;
    m_net.m_parseQ = 0;
    m_net.m_recvQ = 0;
    m_net.m_bLock = 0;
    m_net.m_handler = 0;
    for (i = 1000; i < 0x2800; i++)
    {
        m_handlers[i] = 0;
    }
    REG_HANDLER(1000, OnLogin);  // Packet_Monitor_UDP_Login
    REG_HANDLER(1001, OnLogout);  // Packet_Monitor_UDP_Logout
    REG_HANDLER(1002, OnReplyUserInfo);  // Packet_Monitor_UDP_Reply_UserInfo
    REG_HANDLER(1004, OnHeartBeat);  // Packet_Monitor_UDP_HeartBeat
    REG_HANDLER(1007, OnCharLogin);  // Packet_Monitor_Char_Info
    REG_HANDLER(1011, OnNoticeOtherChannelChatMsg);  // Packet_Monitor_Other_Channel_Chat
    REG_HANDLER(1100, OnCeraUpdate);
    REG_HANDLER(1101, OnEventItemUpdate);
    REG_HANDLER(1203, OnReplyQueryMember);
    REG_HANDLER(1207, OnRequestMemberEnter);  // Packet_Monitor_Request_Member_Enter
    REG_HANDLER(1209, OnMemberEnterReply);  // Packet_Monitor_Member_Enter_Reply
    REG_HANDLER(1211, OnMemberSecede);  // Packet_Monitor_Member_Secede
    REG_HANDLER(1214, OnCallMemberList);  // Packet_Monitor_Call_Member_List
    REG_HANDLER(1215, OnNoticeMemberChatMsg);  // Packet_Monitor_Member_Chat
    REG_HANDLER(1216, OnPayTaxToUpper);  // Packet_Monitor_Member_Pay_Tax
    REG_HANDLER(1013, OnUpdateChangableCharInfo);  // Packet_Monitor_Char_Changable_Info
    REG_HANDLER(1012, OnLogoutComplete);  // Packet_Monitor_UDP_Logout_Complete
    REG_HANDLER(1217, OnUserRepel);
    REG_HANDLER(1218, OnCharacterDelete);  // Packet_Monitor_Charac_Delete
    REG_HANDLER(1103, OnEventStart);  // Packet_Monitor_Event_Start
    REG_HANDLER(1104, OnEventEnd);  // Packet_Monitor_Event_End
    REG_HANDLER(1300, OnNotifyNewMail);  // Packet_Monitor_Notify_New_Mail
    REG_HANDLER(2000, OnWebQueryUserState);
    REG_HANDLER(2524, OnNoticeMessage);  // Packet_Monitor_Notice_Message
    REG_HANDLER(2500, OnRelayServerUserCheck);
    REG_HANDLER(2525, OnForbidChat);  // Packet_Forbid_Chat_By_Monitor
    REG_HANDLER(1224, OnNoticeProhibitConnectUser);
    REG_HANDLER(1400, OnMonitorManagerConnectOK);
    REG_HANDLER(1350, OnMonitorMegaPhoneMsg);  // Packet_Monitor_MegaPhone
    REG_HANDLER(1500, OnRegisterToBlackList);  // Packet_Register_To_BlackList
    REG_HANDLER(1501, OnDeleteToBlackList);  // Packet_Delete_To_BlackList
    REG_HANDLER(1504, OnRequestBlackList);  // Packet_Request_BlackList
    REG_HANDLER(1502, OnDBMWResisterToBlackList);
    REG_HANDLER(1503, OnDBMWDeleteToBlackList);
    REG_HANDLER(1505, OnDBMWResponseBlackListOnLogin);
    REG_HANDLER(2526, OnExchangeServerInfo);  // Packet_Exchange_Server_Info
    REG_HANDLER(1600, OnNoticeCharLiveOnTenMin);  // Packet_Monitor_Notice_Charac_Live_On_Ten_Min
    REG_HANDLER(2528, OnWebNoticeSingle);
    REG_HANDLER(1650, OnAddBuddy);  // Packet_Monitor_Add_Buddy
    REG_HANDLER(1651, OnAddBuddyDBReply);
    REG_HANDLER(1652, OnDelBuddy);  // Packet_Monitor_Del_Buddy
    REG_HANDLER(1653, OnDelBuddyDBReply);
    REG_HANDLER(1654, OnQueryBuddyInfoDBReply);
    REG_HANDLER(1014, OnWebChangeUserHandicap);
    REG_HANDLER(1900, OnGMRequestMid);  // Packet_GM_Request_Mid
    REG_HANDLER(1232, OnUserRepelByCharName);  // Packet_Monitor_User_Repel_ByCharName
    REG_HANDLER(1229, onReplyLoadTowerFullRank);
    REG_HANDLER(1230, onRequestCharacTowerUpdateRank);  // Packet_Request_Charac_Tower_Update_Ranking
    REG_HANDLER(1231, onRequestReloadTowerRanker);
    REG_HANDLER(2900, onWebReqReloadAutoPunishRule);
    REG_HANDLER(4000, OnInnerPacketLogin);
    REG_HANDLER(4001, OnInnerPacketLogout);
    REG_HANDLER(2700, OnNoticeSlang);
    REG_HANDLER(2901, onLoadCleanPadPoint);
    REG_HANDLER(2902, onLoadBlackIPMonitor);
    REG_HANDLER(2903, onLoadBlackIPMonitorPartLoad);
    REG_HANDLER(2904, onLoadBlackIPMonitorDeleteIP);
    REG_HANDLER(1105, OnChangeCharName);  // Packet_Change_Char_Name
    REG_HANDLER(3100, OnNotifyAuctionMail);
    REG_HANDLER(4002, OnPvPChannelInfo);  // Packet_PvPChannelInfo
    REG_HANDLER(4003, OnPvPChannelUserCount);  // Packet_PvPChannelUserCount
    REG_HANDLER(4004, OnChannelType);  // Packet_ChannelType
    REG_HANDLER(4014, OnServerMessageInfo);  // Packet_Monitor_Server_Message_Info
    REG_HANDLER(1770, OnRequestReloadPowerWarRanker);
    REG_HANDLER(2910, onLoadPunishUserReq);
    REG_HANDLER(2911, onIPCounterControl);
    REG_HANDLER(4103, onItemLimitEditionLoadDataReq);
    REG_HANDLER(4104, onItemLimitEditionLoadDataRpy);
    REG_HANDLER(4101, onItemLimitEditionSellEnd);
    REG_HANDLER(4105, onItemLimitEditionBuyableRequest);  // Packet_Item_Limit_Edition_Buyable_Query
    REG_HANDLER(4110, OnMonitorFindFactoryHubUser);  // Packet_Monitor_Find_Factory_Hub_User
    REG_HANDLER(2912, OnSetCleanPadPoint);
    REG_HANDLER(4153, OnResponseIPCounterList);
    REG_HANDLER(4154, OnResponseFullIPCounterList);
    REG_HANDLER(2515, OnTakeScreenShot);
    REG_HANDLER(6002, OnVillageMonsterFightResult);  // Packet_VillageMonsterFightResult
    REG_HANDLER(6009, OnVillageAttackedGMCommand);  // Packet_VillageAttackedGMCommand
    REG_HANDLER(6011, OnVillageAttackedRank);
    REG_HANDLER(6014, OnMonitorFullLevelBroadCast);  // Packet_Monitor_Max_Level_BroadCast
    REG_HANDLER(2913, OnSetARSInfo);
    REG_HANDLER(2914, OnWebRequestARSInfo);
    REG_HANDLER(7014, OnCheckOverlappedAccusation);
    REG_HANDLER(8001, OnGameServerRegist);  // Packet_Game_Server_Regist
    REG_HANDLER(7021, OnNoCache);  // Packet_No_Cache
    REG_HANDLER(8003, OnDisableUserOneToOneChat_GM);  // Packet_Disable_User_OneToOneChat_Police
    REG_HANDLER(8004, OnRegisterGM_mid);  // Packet_Register_GM_MID
    REG_HANDLER(8005, OnFindCharacName_useUID);
    REG_HANDLER(8013, OnRenew_GM_List);  // Packet_Sync_GM_List
    REG_HANDLER(8008, OnLoadPeriodicMessage);
    REG_HANDLER(8009, OnResultLoadPeriodicMessage);
    REG_HANDLER(9025, OnRegisterEventIdx);
    REG_HANDLER(9027, OnRegisterEventUserIdx);
    REG_HANDLER(9030, OnRegisterEventItem);
    REG_HANDLER(9032, OnResultRegisterEventIdx);
    REG_HANDLER(9034, OnGameMonitorGMVillageAttacked);  // Packet_Game_Monitor_GM_Village_Attacked
    REG_HANDLER(2916, OnMonitorPunishCancel);
    REG_HANDLER(10001, OnBroadcastMsg);  // Packet_Broadcast_Msg
    REG_HANDLER(2917, OnMonitorSecuServiceConnWeb);
    REG_HANDLER(10002, OnResetTODAPCInfo);  // Packet_TOD_DoRandomSelect
    REG_HANDLER(10010, OnNoticeMemberChatMsgHyperLink);  // Packet_Monitor_Member_Chat_Hyper_Link
    REG_HANDLER(10009, OnNoticeOtherChannelChatMsgHyperLink);  // Packet_Monitor_Other_Channel_Chat_Hyper_Link
    REG_HANDLER(10011, OnMonitorMegaPhoneMsgHyperLink);  // Packet_Monitor_MegaPhone_Hyper_Link
    REG_HANDLER(10200, onSocialEventRewardItemRequest);
    REG_HANDLER(10204, onSocialEventRewardItemResponse);
    REG_HANDLER(10201, onSocialEventRewardItemInfo);
    REG_HANDLER(10202, onSocialEventRewardItemInfoAll);
    REG_HANDLER(10205, onSocialEventRewardItemUpdate);
    REG_HANDLER(10206, onRequestCharacInfoByCharacName);  // Packet_Monitor_Request_Charac_Info
    REG_HANDLER(10210, OnWebNoticeInGameAD);
    REG_HANDLER(10214, onCollectItems);  // Packet_CollectItems
    REG_HANDLER(10215, onCollectItemsResult);
    REG_HANDLER(10219, onCollectItemsGm);  // Packet_CollectItemsGm
    REG_HANDLER(10220, OnPcRoomPlayTimeReward);  // Packet_PcRoomPlayTimeReward
    REG_HANDLER(10225, OnWebEmergencyPatchMessage);
    REG_HANDLER(10232, OnUpdateMiniCraneSeed);  // Packet_MiniCraneSeed
    REG_HANDLER(10235, onStartGameEventFromServer);
    REG_HANDLER(10236, onEndGameEventFromServer);
    REG_HANDLER(10238, onReloadCountryCode);
    REG_HANDLER(10239, onReloadSecurityRestrictPolicy);
}

CPacketDecoder::~CPacketDecoder()
{
    m_udpParseQ = 0;
    m_udpQLock = 0;
}

void CPacketDecoder::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_udpParseQ = app->Get_UdpPacketParseQ();
        m_net.m_parseQ = app->Get_TcpNetSystem()->Get_TcpSwapQPacket()->GetParseQ();
        m_udpQLock = app->Get_UdpQLock();
        m_net.m_udpBLock = app->Get_UdpBLock();
        m_net.m_recvQ = app->Get_TcpNetSystem()->Get_TcpRecvQLock();
        m_net.m_bLock = app->Get_TcpNetSystem()->Get_TcpRecvBLock();
        m_net.m_handler = app->Get_ServerHandler();
    }
}

void CPacketDecoder::Process()
{
    UdpProcess();
    TcpProcess();
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0034 | monitor | 与ORIG差异=DIFF | CPacketDecoder::TcpProcess | 详见 function_reports/monitor/_ZN14CPacketDecoder10TcpProcessEv.md
void CPacketDecoder::TcpProcess()
{
    if (m_net.m_parseQ == 0 || m_net.m_recvQ == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    CTcpRecvBuffer* buf = 0;
    while (!((std::queue<CTcpRecvBuffer*>*)m_net.m_parseQ)->empty())
    {
        buf = ((std::queue<CTcpRecvBuffer*>*)m_net.m_parseQ)->front();
        ((std::queue<CTcpRecvBuffer*>*)m_net.m_parseQ)->pop();
        if (buf == 0)
        {
            continue;
        }
        CTcpRecvBuffer* pkt = buf;
        int qsize = (int)((std::queue<CTcpRecvBuffer*>*)m_net.m_parseQ)->size();
        if (CAppLoadCheckerInstance()->CheckTcpRecvQ(qsize))
        {
            CAppLoadCheckerInstance()->RequestDB((CServerHandler*)m_net.m_handler, 1, qsize);
        }
        if (!MsgDecode((PacketHeader*)pkt))
        {
            {
                CGuard<CMutex> g((CMutex*)m_net.m_bLock);
                CTcpRecvBuffer::operator delete(buf);
            }
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                   (unsigned int)*(unsigned short*)pkt);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        {
            CGuard<CMutex> g((CMutex*)m_net.m_bLock);
            CTcpRecvBuffer::operator delete(buf);
        }
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0035 | monitor | 与ORIG差异=DIFF | CPacketDecoder::UdpProcess | 详见 function_reports/monitor/_ZN14CPacketDecoder10UdpProcessEv.md
void CPacketDecoder::UdpProcess()
{
    if (m_udpParseQ == 0 || m_udpQLock == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    CUdpRecvBuffer* buf = 0;
    while (!((std::queue<CUdpRecvBuffer*>*)m_udpParseQ)->empty())
    {
        buf = ((std::queue<CUdpRecvBuffer*>*)m_udpParseQ)->front();
        ((std::queue<CUdpRecvBuffer*>*)m_udpParseQ)->pop();
        if (buf == 0)
        {
            continue;
        }
        CUdpRecvBuffer* pkt = buf;
        int qsize = (int)((std::queue<CUdpRecvBuffer*>*)m_udpParseQ)->size();
        if (CAppLoadCheckerInstance()->CheckUdpRecvQ(qsize))
        {
            CAppLoadCheckerInstance()->RequestDB((CServerHandler*)m_net.m_handler, 2, qsize);
        }
        if (!MsgDecode((PacketHeader*)pkt))
        {
            {
                CGuard<CMutex> g((CMutex*)m_net.m_udpBLock);
                CUdpRecvBuffer::operator delete(buf);
            }
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                   (unsigned int)*(unsigned short*)pkt);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        {
            CGuard<CMutex> g((CMutex*)m_net.m_udpBLock);
            CUdpRecvBuffer::operator delete(buf);
        }
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0036 | monitor | 与ORIG差异=DIFF | CPacketDecoder::MsgDecode | 详见 function_reports/monitor/_ZN14CPacketDecoder9MsgDecodeEP12PacketHeader.md
char CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    if (*(unsigned short*)pkt < 0x2800 && 999 < *(unsigned short*)pkt)
    {
        static CPacketCounter<1000, 10240> packet_counter(0, "PacketDispatcher");
        packet_counter.IncrementPacketCount(*(unsigned short*)pkt);
        if (m_handlers[*(unsigned short*)pkt] == 0)
        {
            DNF_LOG_SCOPE_LINE(0x1db,"./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.<0 == m_decodProcFunc>\n",
                *(unsigned short*)pkt);
            return 0;
        }
        packet_counter.BeforeProcess();
        ((void(*)(PacketHeader*))m_handlers[*(unsigned short*)pkt])(pkt);
        packet_counter.AfterProcess(*(unsigned short*)pkt);
        return 1;
    }
    printf("Undefined Packet Err : Game Message with identifier %d has arrived.\n", *(unsigned short*)pkt);
    DNF_LOG_SCOPE_LINE(0x1fa,"./log/Decoder",
        "Undefined Packet Err: CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
        *(unsigned short*)pkt);
    return 0;
}

CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}

void CPacketDecoder::SetUdpQueue(
    std::queue<CUdpRecvBuffer*, std::deque<CUdpRecvBuffer*, std::allocator<CUdpRecvBuffer*> > >* q)
{
    m_udpParseQ = q;
}

void CPacketDecoder::SetTCPQueue(
    std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > >* q)
{
    m_net.m_parseQ = q;
}
