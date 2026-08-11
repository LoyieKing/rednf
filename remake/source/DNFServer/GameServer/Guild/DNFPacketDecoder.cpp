// df_guild_r — DNFPacketDecoder (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Guild_Change_Power_War_Point.h"

#include "DNFPacketDecoder.h"
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

CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}

CPacketDecoder::CPacketDecoder()
{
    m_udpParseQ = 0;
    m_udpQLock = 0;
    m_udpBLock = 0;
    m_tcpParseQ = 0;
    m_tcpRecvQLock = 0;
    m_tcpRecvBLock = 0;
    m_serverHandler = 0;
    for (int i = 1000; i < 0x2800; i++)
    {
        m_table[i + 4] = 0;
    }
    m_table[1004] = &CPacketTranslater::OnLogin;
    m_table[1005] = &CPacketTranslater::OnLogout;
    m_table[1006] = &CPacketTranslater::OnReplyUserInfo;
    m_table[1008] = &CPacketTranslater::OnHeartBeat;
    m_table[1011] = &CPacketTranslater::OnCharLogin;
    m_table[1033] = &CPacketTranslater::OnReplyQueryGuild;
    m_table[1031] = &CPacketTranslater::OnDBReplyQueryGuildMember;
    m_table[1025] = &CPacketTranslater::OnNoticeGuildEnter;
    m_table[1026] = &CPacketTranslater::OnNoticeGuildSecede;
    m_table[1027] = &CPacketTranslater::OnNoticeGuildMarkChange;
    m_table[1024] = &CPacketTranslater::OnNoticeGuildDismiss;
    m_table[1028] = &CPacketTranslater::OnNoticeGuildChatMsg;
    m_table[1076] = &CPacketTranslater::OnSetGuildMemberGrade;
    m_table[1029] = &CPacketTranslater::OnCallGuildMembers;
    m_table[1064] = &CPacketTranslater::OnCallGuildAllMembers;
    m_table[1066] = &CPacketTranslater::OnDBReplyGuildAllMembers;
    m_table[1067] = &CPacketTranslater::OnDBReplyUnconnGuildMember;
    m_table[1036] = &CPacketTranslater::OnCallGuildLevelUp;
    m_table[1041] = &CPacketTranslater::OnCallGuildInfo;
    m_table[1040] = &CPacketTranslater::OnNoticeGuildCreate;
    m_table[1222] = &CPacketTranslater::OnCharacterDelete;
    m_table[1017] = &CPacketTranslater::OnUpdateChangableCharInfo;
    m_table[1107] = &CPacketTranslater::OnEventStart;
    m_table[1108] = &CPacketTranslater::OnEventEnd;
    m_table[1045] = &CPacketTranslater::OnLoadGuildWarEnterableGuilds;
    m_table[1046] = &CPacketTranslater::OnNoticeGuildWarEnd;
    m_table[1049] = &CPacketTranslater::OnNoticeGuildMailArrive;
    m_table[1050] = &CPacketTranslater::OnNoticeGuildWarStart;
    m_table[1051] = &CPacketTranslater::OnNoticeGuildWarPointChange;
    m_table[1048] = &CPacketTranslater::OnRequestGuildWarInfo;
    m_table[1504] = &CPacketTranslater::OnRegisterToBlackList;
    m_table[1505] = &CPacketTranslater::OnDeleteToBlackList;
    m_table[1508] = &CPacketTranslater::OnRequestBlackList;
    m_table[1506] = &CPacketTranslater::OnDBMWResisterToBlackList;
    m_table[1507] = &CPacketTranslater::OnDBMWDeleteToBlackList;
    m_table[1509] = &CPacketTranslater::OnDBMWResponseBlackListOnLogin;
    m_table[1042] = &CPacketTranslater::OnIncreaseGuildExp;
    m_table[1069] = &CPacketTranslater::OnBuyGuildSkill;
    m_table[1071] = &CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade;
    m_table[1073] = &CPacketTranslater::OnNotifyMessageToGuild;
    m_table[1404] = &CPacketTranslater::OnMonitorManagerConnectOK;
    m_table[1078] = &CPacketTranslater::OnMonitorSendGuildLetter;
    m_table[1079] = &CPacketTranslater::OnDBMWReplySendGuildLetter;
    m_table[1080] = &CPacketTranslater::OnCallGuildInvite;
    m_table[1082] = &CPacketTranslater::OnReplyGuildInvite;
    m_table[1084] = &CPacketTranslater::OnDBMWGuildJoin;
    m_table[1085] = &CPacketTranslater::OnRequestGuildSecede;
    m_table[1086] = &CPacketTranslater::OnDBReplyGuildSecede;
    m_table[1087] = &CPacketTranslater::OnRequestGuildMasterDelegate;
    m_table[1088] = &CPacketTranslater::OnDBReplyGuildMasterDelegate;
    m_table[1091] = &CPacketTranslater::OnRequestGuildCreate;
    m_table[1092] = &CPacketTranslater::OnDBReplyGuildCreate;
    m_table[1093] = &CPacketTranslater::OnSetGuildMemberGradeFromWeb;
    m_table[1094] = &CPacketTranslater::OnGuildMasterDelegateFromWeb;
    m_table[1095] = &CPacketTranslater::OnCheckGuildMemberConnectionFromWeb;
    m_table[4004] = &CPacketTranslater::OnInnerPacketLogin;
    m_table[4005] = &CPacketTranslater::OnInnerPacketLogout;
    m_table[1096] = &CPacketTranslater::OnChangeGuildName;
    m_table[1717] = &CPacketTranslater::OnPowerWarStartInfo;
    m_table[1704] = &CPacketTranslater::OnChangePowerWarPoint;
    m_table[1708] = &CPacketTranslater::OnPacketJoinPower;
    m_table[1709] = &CPacketTranslater::OnPacketSecedePower;
    m_table[1710] = &CPacketTranslater::OnSetPowerWarCfg;
    m_table[1659] = &CPacketTranslater::OnLoadFromDBOnGuildBooting;
    m_table[1706] = &CPacketTranslater::OnGMPowerWarStart;
    m_table[1707] = &CPacketTranslater::OnGMPowerWarEnd;
    m_table[1109] = &CPacketTranslater::OnChangeCharName;
    m_table[1111] = &CPacketTranslater::OnChangeGuildNameFromWeb;
    m_table[1305] = &CPacketTranslater::OnNotifyNewGroupMail;
    m_table[1759] = &CPacketTranslater::OnInCreaseDecreasePowerWarPoint;
    m_table[1760] = &CPacketTranslater::OnCreateGuildAgit;
    m_table[1762] = &CPacketTranslater::OnDeleteGuildAgit;
    m_table[1761] = &CPacketTranslater::OnDBCreateGuildAgit;
    m_table[1763] = &CPacketTranslater::OnDBDeleteGuildAgit;
    m_table[1764] = &CPacketTranslater::OnPowerWarProcessInfo;
    m_table[1765] = &CPacketTranslater::OnDBLoadGuildAgit;
    m_table[1767] = &CPacketTranslater::OnUpgradeGuildAgit;
    m_table[1768] = &CPacketTranslater::OnDBUpgradeGuildAgit;
    m_table[1769] = &CPacketTranslater::OnGuildExpLimit;
    m_table[1237] = &CPacketTranslater::OnWriteGuildMemberMemo;
    m_table[1804] = &CPacketTranslater::OnLoadGuildCargo;
    m_table[1805] = &CPacketTranslater::OnLoadGuildCargoHistory;
    m_table[1806] = &CPacketTranslater::OnGuildCargo;
    m_table[1807] = &CPacketTranslater::OnGuildCargoHistory;
    m_table[1808] = &CPacketTranslater::OnGuildCargoCheckPushItem;
    m_table[1809] = &CPacketTranslater::OnGuildCargoPushItem;
    m_table[1810] = &CPacketTranslater::OnGuildCargoPopItem;
    m_table[1811] = &CPacketTranslater::OnGuildCargoMoveItem;
    m_table[1815] = &CPacketTranslater::OnGuildCargoUpgrade;
    m_table[9005] = &CPacketTranslater::OnGuildRequestGuildBoardOpen;
    m_table[9008] = &CPacketTranslater::OnDBLoadReplyGuildBoardOpen;
    m_table[9009] = &CPacketTranslater::OnGuildRequestGuildBoardWrite;
    m_table[9012] = &CPacketTranslater::OnDBLoadReplyGuildBoardWrite;
    m_table[9013] = &CPacketTranslater::OnGuildRequestGuildBoardDelete;
    m_table[9016] = &CPacketTranslater::OnDBLoadReplyGuildBoardDelete;
    m_table[9024] = &CPacketTranslater::OnWebGuildBoardWrite;
    m_table[9025] = &CPacketTranslater::OnWebGuildBoardDelete;
    m_table[9027] = &CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite;
    m_table[1718] = &CPacketTranslater::OnGuildApplyOriginalPowerSide;
    m_table[1305] = &CPacketTranslater::OnNotifyNewGroupMail;
    m_table[8005] = &CPacketTranslater::OnGameServerRegist;
    m_table[7104] = &CPacketTranslater::OnAddGuildFund;
    m_table[7106] = &CPacketTranslater::OnRefreshGuildInfo;
    m_table[7108] = &CPacketTranslater::OnReplyTodayGuildMember;
    m_table[7110] = &CPacketTranslater::OnApproveJoinGuild;
    m_table[7113] = &CPacketTranslater::OnDBResponseApproveJoinGuild;
    m_table[7114] = &CPacketTranslater::OnGuildAttendanceInfo;
    m_table[7117] = &CPacketTranslater::OnGuildDebug;
    m_table[8017] = &CPacketTranslater::OnRenew_GM_List;
    m_table[10012] = &CPacketTranslater::OnNoticeGuildChatMsgHyperLink;
}

int CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    if (*(unsigned short*)pkt <= 0x27ff && *(unsigned short*)pkt > 0x3e7)
    {
        static CPacketCounter<1000, 10240> packet_counter(0, "PacketDispatcher");
        packet_counter.IncrementPacketCount(*(unsigned short*)pkt);
        if (m_table[*(unsigned short*)pkt + 4] == 0)
        {
            DNF_LOG_SCOPE_LINE(0x10d,"./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.<0 == m_decodProcFunc>\n",
                (unsigned int)*(unsigned short*)pkt);
            return 0;
        }
        packet_counter.BeforeProcess();
        m_table[*(unsigned short*)pkt + 4](pkt);
        packet_counter.AfterProcess(*(unsigned short*)pkt);
        return 1;
    }
    printf("Undefined Packet Err : Game Message with identifier %d has arrived.\n",
           (unsigned int)*(unsigned short*)pkt);
    DNF_LOG_SCOPE_LINE(0x123,"./log/Decoder",
        "Undefined Packet Err: CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
        (unsigned int)*(unsigned short*)pkt);
    return 0;
}

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
        if (CAppLoadCheckerInstance()->CheckUdpRecvQ(qsize) != 0)
        {
            CServerHandler* handler = (CServerHandler*)m_serverHandler;
            CAppLoadCheckerInstance()->RequestDB(handler, 2, qsize);
        }
        if (MsgDecode((PacketHeader*)pkt) != 1)
        {
            CMutex* mtx = (CMutex*)m_udpBLock;
            CGuard<CMutex> g(mtx);
            CUdpRecvBuffer::operator delete(buf);
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                   (unsigned int)*(unsigned short*)pkt);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        CMutex* mtx = (CMutex*)m_udpBLock;
        CGuard<CMutex> g(mtx);
        CUdpRecvBuffer::operator delete(buf);
    }
}

void CPacketDecoder::TcpProcess()
{
    if (m_tcpParseQ == 0 || m_tcpRecvQLock == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    CTcpRecvBuffer* buf = 0;
    while (!((std::queue<CTcpRecvBuffer*>*)m_tcpParseQ)->empty())
    {
        buf = ((std::queue<CTcpRecvBuffer*>*)m_tcpParseQ)->front();
        ((std::queue<CTcpRecvBuffer*>*)m_tcpParseQ)->pop();
        if (buf == 0)
        {
            continue;
        }
        CTcpRecvBuffer* pkt = buf;
        int qsize = (int)((std::queue<CTcpRecvBuffer*>*)m_tcpParseQ)->size();
        if (CAppLoadCheckerInstance()->CheckTcpRecvQ(qsize) != 0)
        {
            CServerHandler* handler = (CServerHandler*)m_serverHandler;
            CAppLoadCheckerInstance()->RequestDB(handler, 1, qsize);
        }
        if (MsgDecode((PacketHeader*)pkt) != 1)
        {
            CMutex* mtx = (CMutex*)m_tcpRecvBLock;
            CGuard<CMutex> g(mtx);
            CTcpRecvBuffer::operator delete(buf);
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                   (unsigned int)*(unsigned short*)pkt);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        CMutex* mtx = (CMutex*)m_tcpRecvBLock;
        CGuard<CMutex> g(mtx);
        CTcpRecvBuffer::operator delete(buf);
    }
}

void CPacketDecoder::Process()
{
    UdpProcess();
    TcpProcess();
}

void CPacketDecoder::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_udpParseQ = app->Get_UdpPacketParseQ();
        typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> > TcpRecvQueue;
        m_tcpParseQ = ((CSwapQueue<TcpRecvQueue, 2>*)
            app->Get_TcpNetSystem()->Get_TcpSwapQPacket())->GetParseQ();
        m_udpQLock = app->Get_UdpQLock();
        m_udpBLock = app->Get_UdpBLock();
        m_tcpRecvQLock = app->Get_TcpNetSystem()->Get_TcpRecvQLock();
        m_tcpRecvBLock = app->Get_TcpNetSystem()->Get_TcpRecvBLock();
        m_serverHandler = app->Get_ServerHandler();
    }
}

void CPacketDecoder::SetUdpQueue(std::queue<CUdpRecvBuffer*>* q)
{
}

void CPacketDecoder::SetTCPQueue(std::queue<CTcpRecvBuffer*>* q)
{
}

CPacketDecoder::~CPacketDecoder()
{
    m_udpParseQ = 0;
    m_udpQLock = 0;
}
