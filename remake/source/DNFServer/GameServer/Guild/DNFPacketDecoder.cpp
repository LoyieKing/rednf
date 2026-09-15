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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0023、DNF-GLD-DIFF-0024 已消除） | guild | CPacketDecoder::CPacketDecoder | 详见 function_reports/guild/_ZN14CPacketDecoderC1Ev.md、function_reports/guild/_ZN14CPacketDecoderC2Ev.md
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
        m_table[i][0][4] = 0;
    }
    m_table[1004][0][0] = &CPacketTranslater::OnLogin;
    m_table[1005][0][0] = &CPacketTranslater::OnLogout;
    m_table[1006][0][0] = &CPacketTranslater::OnReplyUserInfo;
    m_table[1008][0][0] = &CPacketTranslater::OnHeartBeat;
    m_table[1011][0][0] = &CPacketTranslater::OnCharLogin;
    m_table[1033][0][0] = &CPacketTranslater::OnReplyQueryGuild;
    m_table[1031][0][0] = &CPacketTranslater::OnDBReplyQueryGuildMember;
    m_table[1025][0][0] = &CPacketTranslater::OnNoticeGuildEnter;
    m_table[1026][0][0] = &CPacketTranslater::OnNoticeGuildSecede;
    m_table[1027][0][0] = &CPacketTranslater::OnNoticeGuildMarkChange;
    m_table[1024][0][0] = &CPacketTranslater::OnNoticeGuildDismiss;
    m_table[1028][0][0] = &CPacketTranslater::OnNoticeGuildChatMsg;
    m_table[1076][0][0] = &CPacketTranslater::OnSetGuildMemberGrade;
    m_table[1029][0][0] = &CPacketTranslater::OnCallGuildMembers;
    m_table[1064][0][0] = &CPacketTranslater::OnCallGuildAllMembers;
    m_table[1066][0][0] = &CPacketTranslater::OnDBReplyGuildAllMembers;
    m_table[1067][0][0] = &CPacketTranslater::OnDBReplyUnconnGuildMember;
    m_table[1036][0][0] = &CPacketTranslater::OnCallGuildLevelUp;
    m_table[1041][0][0] = &CPacketTranslater::OnCallGuildInfo;
    m_table[1040][0][0] = &CPacketTranslater::OnNoticeGuildCreate;
    m_table[1222][0][0] = &CPacketTranslater::OnCharacterDelete;
    m_table[1017][0][0] = &CPacketTranslater::OnUpdateChangableCharInfo;
    m_table[1107][0][0] = &CPacketTranslater::OnEventStart;
    m_table[1108][0][0] = &CPacketTranslater::OnEventEnd;
    m_table[1045][0][0] = &CPacketTranslater::OnLoadGuildWarEnterableGuilds;
    m_table[1046][0][0] = &CPacketTranslater::OnNoticeGuildWarEnd;
    m_table[1049][0][0] = &CPacketTranslater::OnNoticeGuildMailArrive;
    m_table[1050][0][0] = &CPacketTranslater::OnNoticeGuildWarStart;
    m_table[1051][0][0] = &CPacketTranslater::OnNoticeGuildWarPointChange;
    m_table[1048][0][0] = &CPacketTranslater::OnRequestGuildWarInfo;
    m_table[1504][0][0] = &CPacketTranslater::OnRegisterToBlackList;
    m_table[1505][0][0] = &CPacketTranslater::OnDeleteToBlackList;
    m_table[1508][0][0] = &CPacketTranslater::OnRequestBlackList;
    m_table[1506][0][0] = &CPacketTranslater::OnDBMWResisterToBlackList;
    m_table[1507][0][0] = &CPacketTranslater::OnDBMWDeleteToBlackList;
    m_table[1509][0][0] = &CPacketTranslater::OnDBMWResponseBlackListOnLogin;
    m_table[1042][0][0] = &CPacketTranslater::OnIncreaseGuildExp;
    m_table[1069][0][0] = &CPacketTranslater::OnBuyGuildSkill;
    m_table[1071][0][0] = &CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade;
    m_table[1073][0][0] = &CPacketTranslater::OnNotifyMessageToGuild;
    m_table[1404][0][0] = &CPacketTranslater::OnMonitorManagerConnectOK;
    m_table[1078][0][0] = &CPacketTranslater::OnMonitorSendGuildLetter;
    m_table[1079][0][0] = &CPacketTranslater::OnDBMWReplySendGuildLetter;
    m_table[1080][0][0] = &CPacketTranslater::OnCallGuildInvite;
    m_table[1082][0][0] = &CPacketTranslater::OnReplyGuildInvite;
    m_table[1084][0][0] = &CPacketTranslater::OnDBMWGuildJoin;
    m_table[1085][0][0] = &CPacketTranslater::OnRequestGuildSecede;
    m_table[1086][0][0] = &CPacketTranslater::OnDBReplyGuildSecede;
    m_table[1087][0][0] = &CPacketTranslater::OnRequestGuildMasterDelegate;
    m_table[1088][0][0] = &CPacketTranslater::OnDBReplyGuildMasterDelegate;
    m_table[1091][0][0] = &CPacketTranslater::OnRequestGuildCreate;
    m_table[1092][0][0] = &CPacketTranslater::OnDBReplyGuildCreate;
    m_table[1093][0][0] = &CPacketTranslater::OnSetGuildMemberGradeFromWeb;
    m_table[1094][0][0] = &CPacketTranslater::OnGuildMasterDelegateFromWeb;
    m_table[1095][0][0] = &CPacketTranslater::OnCheckGuildMemberConnectionFromWeb;
    m_table[4004][0][0] = &CPacketTranslater::OnInnerPacketLogin;
    m_table[4005][0][0] = &CPacketTranslater::OnInnerPacketLogout;
    m_table[1096][0][0] = &CPacketTranslater::OnChangeGuildName;
    m_table[1717][0][0] = &CPacketTranslater::OnPowerWarStartInfo;
    m_table[1704][0][0] = &CPacketTranslater::OnChangePowerWarPoint;
    m_table[1708][0][0] = &CPacketTranslater::OnPacketJoinPower;
    m_table[1709][0][0] = &CPacketTranslater::OnPacketSecedePower;
    m_table[1710][0][0] = &CPacketTranslater::OnSetPowerWarCfg;
    m_table[1659][0][0] = &CPacketTranslater::OnLoadFromDBOnGuildBooting;
    m_table[1706][0][0] = &CPacketTranslater::OnGMPowerWarStart;
    m_table[1707][0][0] = &CPacketTranslater::OnGMPowerWarEnd;
    m_table[1109][0][0] = &CPacketTranslater::OnChangeCharName;
    m_table[1111][0][0] = &CPacketTranslater::OnChangeGuildNameFromWeb;
    m_table[1305][0][0] = &CPacketTranslater::OnNotifyNewGroupMail;
    m_table[1759][0][0] = &CPacketTranslater::OnInCreaseDecreasePowerWarPoint;
    m_table[1760][0][0] = &CPacketTranslater::OnCreateGuildAgit;
    m_table[1762][0][0] = &CPacketTranslater::OnDeleteGuildAgit;
    m_table[1761][0][0] = &CPacketTranslater::OnDBCreateGuildAgit;
    m_table[1763][0][0] = &CPacketTranslater::OnDBDeleteGuildAgit;
    m_table[1764][0][0] = &CPacketTranslater::OnPowerWarProcessInfo;
    m_table[1765][0][0] = &CPacketTranslater::OnDBLoadGuildAgit;
    m_table[1767][0][0] = &CPacketTranslater::OnUpgradeGuildAgit;
    m_table[1768][0][0] = &CPacketTranslater::OnDBUpgradeGuildAgit;
    m_table[1769][0][0] = &CPacketTranslater::OnGuildExpLimit;
    m_table[1237][0][0] = &CPacketTranslater::OnWriteGuildMemberMemo;
    m_table[1804][0][0] = &CPacketTranslater::OnLoadGuildCargo;
    m_table[1805][0][0] = &CPacketTranslater::OnLoadGuildCargoHistory;
    m_table[1806][0][0] = &CPacketTranslater::OnGuildCargo;
    m_table[1807][0][0] = &CPacketTranslater::OnGuildCargoHistory;
    m_table[1808][0][0] = &CPacketTranslater::OnGuildCargoCheckPushItem;
    m_table[1809][0][0] = &CPacketTranslater::OnGuildCargoPushItem;
    m_table[1810][0][0] = &CPacketTranslater::OnGuildCargoPopItem;
    m_table[1811][0][0] = &CPacketTranslater::OnGuildCargoMoveItem;
    m_table[1815][0][0] = &CPacketTranslater::OnGuildCargoUpgrade;
    m_table[9005][0][0] = &CPacketTranslater::OnGuildRequestGuildBoardOpen;
    m_table[9008][0][0] = &CPacketTranslater::OnDBLoadReplyGuildBoardOpen;
    m_table[9009][0][0] = &CPacketTranslater::OnGuildRequestGuildBoardWrite;
    m_table[9012][0][0] = &CPacketTranslater::OnDBLoadReplyGuildBoardWrite;
    m_table[9013][0][0] = &CPacketTranslater::OnGuildRequestGuildBoardDelete;
    m_table[9016][0][0] = &CPacketTranslater::OnDBLoadReplyGuildBoardDelete;
    m_table[9024][0][0] = &CPacketTranslater::OnWebGuildBoardWrite;
    m_table[9025][0][0] = &CPacketTranslater::OnWebGuildBoardDelete;
    m_table[9027][0][0] = &CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite;
    m_table[1718][0][0] = &CPacketTranslater::OnGuildApplyOriginalPowerSide;
    m_table[1305][0][0] = &CPacketTranslater::OnNotifyNewGroupMail;
    m_table[8005][0][0] = &CPacketTranslater::OnGameServerRegist;
    m_table[7104][0][0] = &CPacketTranslater::OnAddGuildFund;
    m_table[7106][0][0] = &CPacketTranslater::OnRefreshGuildInfo;
    m_table[7108][0][0] = &CPacketTranslater::OnReplyTodayGuildMember;
    m_table[7110][0][0] = &CPacketTranslater::OnApproveJoinGuild;
    m_table[7113][0][0] = &CPacketTranslater::OnDBResponseApproveJoinGuild;
    m_table[7114][0][0] = &CPacketTranslater::OnGuildAttendanceInfo;
    m_table[7117][0][0] = &CPacketTranslater::OnGuildDebug;
    m_table[8017][0][0] = &CPacketTranslater::OnRenew_GM_List;
    m_table[10012][0][0] = &CPacketTranslater::OnNoticeGuildChatMsgHyperLink;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0022 已消除） | guild | CPacketDecoder::MsgDecode | 详见 function_reports/guild/_ZN14CPacketDecoder9MsgDecodeEP12PacketHeader.md
bool CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    if (*(unsigned short*)pkt <= 0x27ff && *(unsigned short*)pkt > 0x3e7)
    {
        static CPacketCounter<1000, 10240> packet_counter(0, "PacketDispatcher");
        packet_counter.IncrementPacketCount(*(unsigned short*)pkt);
        if (m_table[*(unsigned short*)pkt][0][4] == 0)
        {
            DNF_LOG_SCOPE_LINE(0x10d,"./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.<0 == m_decodProcFunc>\n",
                (unsigned int)*(unsigned short*)pkt);
            return 0;
        }
        packet_counter.BeforeProcess();
        m_table[*(unsigned short*)pkt][0][4](pkt);
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0021 已消除） | guild | CPacketDecoder::UdpProcess | 详见 function_reports/guild/_ZN14CPacketDecoder10UdpProcessEv.md
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
        // [P353] 正向包裹形（monitor/dbmw 同源已 identical）
        if (buf != 0)
        {
            CUdpRecvBuffer* pkt = buf;
            int qsize = (int)((std::queue<CUdpRecvBuffer*>*)m_udpParseQ)->size();
            if (CAppLoadCheckerInstance()->CheckUdpRecvQ(qsize))
            {
                CAppLoadCheckerInstance()->RequestDB(m_serverHandler, 2, qsize);
            }
            if (!MsgDecode((PacketHeader*)pkt))
            {
                {
                    CGuard<CMutex> g(m_udpBLock);
                    CUdpRecvBuffer::operator delete(buf);
                }
                printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                       (unsigned int)*(unsigned short*)pkt);
                throw CDNFException(
                    "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
            }
            {
                CGuard<CMutex> g(m_udpBLock);
                CUdpRecvBuffer::operator delete(buf);
            }
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0020 已消除） | guild | CPacketDecoder::TcpProcess | 详见 function_reports/guild/_ZN14CPacketDecoder10TcpProcessEv.md
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
        // [P353] 正向包裹形（monitor/dbmw 同源已 identical）；早返 continue 恒反产
        //   1 条多余尾 jmp
        if (buf != 0)
        {
            CTcpRecvBuffer* pkt = buf;
            int qsize = (int)((std::queue<CTcpRecvBuffer*>*)m_tcpParseQ)->size();
            if (CAppLoadCheckerInstance()->CheckTcpRecvQ(qsize))
            {
                CAppLoadCheckerInstance()->RequestDB(m_serverHandler, 1, qsize);
            }
            if (!MsgDecode((PacketHeader*)pkt))
            {
                {
                    CGuard<CMutex> g(m_tcpRecvBLock);
                    CTcpRecvBuffer::operator delete(buf);
                }
                printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", pkt,
                       (unsigned int)*(unsigned short*)pkt);
                throw CDNFException(
                    "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
            }
            {
                CGuard<CMutex> g(m_tcpRecvBLock);
                CTcpRecvBuffer::operator delete(buf);
            }
        }
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

// [2026-09-14 修复] 这两个 setter 原本是**空函数体**，而 ORIG 是实打实的赋值：
//   · ORIG df_guild_r 0x08070864 `SetUdpQueue`：`mov eax,[ebp+8]; mov edx,[ebp+0xc];
//     mov DWORD PTR [eax],edx; ret`            ⇒ m_udpParseQ（+0） = q
//   · ORIG df_guild_r 0x08070872 `SetTCPQueue`：同型，落点是 `[eax+0xc]`
//                                              ⇒ m_tcpParseQ（+0xc） = q
//   后果（整机 A/B 实测）：`CApplication::SwitchQueueTCP()` 在 `SwitchQueue()`
//   成功之后调 `SetTCPQueue(GetParseQueue())`，而空实现让解码器**一直读它
//   Attach 时抓到的那条队列**；`InitPeer` 里对端记下的是**创建时刻**的
//   `GetRecvQ()`，而 CSwapQueue 每次交换都会让 recv/parse 两个槽互换 ⇒
//   「对端压入的队列」与「解码器读的队列」从第一次交换之后就永久错位。
//   表现：guild 收得到心跳（走 UDP / 或恰好同槽）却**永远收不到登录包**
//   （id=4000 → 槽 4004 `OnInnerPacketLogin`），于是从不回复、game 侧超时关连接、
//   我们这边 `Recv ERROR = 0 … nRead(0)` → `DisConnSig()+DeletePeer()` → 无限重连，
//   `m_gameServers` 永远为空 ⇒ `Game Server Index Over Index` 刷屏。
//   对照：monitor / manager / dbmw 的这两个函数都**与 ORIG 逐指令相同**（都是真赋值），
//   只有 guild 是空壳。
void CPacketDecoder::SetUdpQueue(std::queue<CUdpRecvBuffer*>* q)
{
    m_udpParseQ = q;
}

void CPacketDecoder::SetTCPQueue(std::queue<CTcpRecvBuffer*>* q)
{
    m_tcpParseQ = q;
}

CPacketDecoder::~CPacketDecoder()
{
    m_udpParseQ = 0;
    m_udpQLock = 0;
}
