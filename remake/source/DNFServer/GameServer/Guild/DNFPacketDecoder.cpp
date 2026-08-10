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
    memset(m_data, 0, sizeof(m_data));
    // handler 表：偏移 = (packetId + 4) * 4 + 0xc
    static const struct { unsigned int off; void (*fn)(PacketHeader*); } tbl[] = {
        {0xfbc, CPacketTranslater::OnLogin},
        {0xfc0, CPacketTranslater::OnLogout},
        {0xfc4, CPacketTranslater::OnReplyUserInfo},
        {0xfcc, CPacketTranslater::OnHeartBeat},
        {0xfd8, CPacketTranslater::OnCharLogin},
        {0x1030, CPacketTranslater::OnReplyQueryGuild},
        {0x1028, CPacketTranslater::OnDBReplyQueryGuildMember},
        {0x1010, CPacketTranslater::OnNoticeGuildEnter},
        {0x1014, CPacketTranslater::OnNoticeGuildSecede},
        {0x1018, CPacketTranslater::OnNoticeGuildMarkChange},
        {0x100c, CPacketTranslater::OnNoticeGuildDismiss},
        {0x101c, CPacketTranslater::OnNoticeGuildChatMsg},
        {0x10dc, CPacketTranslater::OnSetGuildMemberGrade},
        {0x1020, CPacketTranslater::OnCallGuildMembers},
        {0x10ac, CPacketTranslater::OnCallGuildAllMembers},
        {0x10b4, CPacketTranslater::OnDBReplyGuildAllMembers},
        {0x10b8, CPacketTranslater::OnDBReplyUnconnGuildMember},
        {0x103c, CPacketTranslater::OnCallGuildLevelUp},
        {0x1050, CPacketTranslater::OnCallGuildInfo},
        {0x104c, CPacketTranslater::OnNoticeGuildCreate},
        {0x1324, CPacketTranslater::OnCharacterDelete},
        {0xff0, CPacketTranslater::OnUpdateChangableCharInfo},
        {0x1158, CPacketTranslater::OnEventStart},
        {0x115c, CPacketTranslater::OnEventEnd},
        {0x1060, CPacketTranslater::OnLoadGuildWarEnterableGuilds},
        {0x1064, CPacketTranslater::OnNoticeGuildWarEnd},
        {0x1070, CPacketTranslater::OnNoticeGuildMailArrive},
        {0x1074, CPacketTranslater::OnNoticeGuildWarStart},
        {0x1078, CPacketTranslater::OnNoticeGuildWarPointChange},
        {0x106c, CPacketTranslater::OnRequestGuildWarInfo},
        {0x178c, CPacketTranslater::OnRegisterToBlackList},
        {0x1790, CPacketTranslater::OnDeleteToBlackList},
        {0x179c, CPacketTranslater::OnRequestBlackList},
        {0x1794, CPacketTranslater::OnDBMWResisterToBlackList},
        {0x1798, CPacketTranslater::OnDBMWDeleteToBlackList},
        {0x17a0, CPacketTranslater::OnDBMWResponseBlackListOnLogin},
        {0x1054, CPacketTranslater::OnIncreaseGuildExp},
        {0x10c0, CPacketTranslater::OnBuyGuildSkill},
        {0x10c8, CPacketTranslater::OnDBMWChangeUnconnectedGuildMemberGrade},
        {0x10d0, CPacketTranslater::OnNotifyMessageToGuild},
        {0x15fc, CPacketTranslater::OnMonitorManagerConnectOK},
        {0x10e4, CPacketTranslater::OnMonitorSendGuildLetter},
        {0x10e8, CPacketTranslater::OnDBMWReplySendGuildLetter},
        {0x10ec, CPacketTranslater::OnCallGuildInvite},
        {0x10f4, CPacketTranslater::OnReplyGuildInvite},
        {0x10fc, CPacketTranslater::OnDBMWGuildJoin},
        {0x1100, CPacketTranslater::OnRequestGuildSecede},
        {0x1104, CPacketTranslater::OnDBReplyGuildSecede},
        {0x1108, CPacketTranslater::OnRequestGuildMasterDelegate},
        {0x110c, CPacketTranslater::OnDBReplyGuildMasterDelegate},
        {0x1118, CPacketTranslater::OnRequestGuildCreate},
        {0x111c, CPacketTranslater::OnDBReplyGuildCreate},
        {0x1120, CPacketTranslater::OnSetGuildMemberGradeFromWeb},
        {0x1124, CPacketTranslater::OnGuildMasterDelegateFromWeb},
        {0x1128, CPacketTranslater::OnCheckGuildMemberConnectionFromWeb},
        {0x3e9c, CPacketTranslater::OnInnerPacketLogin},
        {0x3ea0, CPacketTranslater::OnInnerPacketLogout},
        {0x112c, CPacketTranslater::OnChangeGuildName},
        {0x1ae0, CPacketTranslater::OnPowerWarStartInfo},
        {0x1aac, CPacketTranslater::OnChangePowerWarPoint},
        {0x1abc, CPacketTranslater::OnPacketJoinPower},
        {0x1ac0, CPacketTranslater::OnPacketSecedePower},
        {0x1ac4, CPacketTranslater::OnSetPowerWarCfg},
        {0x19f8, CPacketTranslater::OnLoadFromDBOnGuildBooting},
        {0x1ab4, CPacketTranslater::OnGMPowerWarStart},
        {0x1ab8, CPacketTranslater::OnGMPowerWarEnd},
        {0x1160, CPacketTranslater::OnChangeCharName},
        {0x1168, CPacketTranslater::OnChangeGuildNameFromWeb},
        {0x1470, CPacketTranslater::OnNotifyNewGroupMail},
        {0x1b88, CPacketTranslater::OnInCreaseDecreasePowerWarPoint},
        {0x1b8c, CPacketTranslater::OnCreateGuildAgit},
        {0x1b94, CPacketTranslater::OnDeleteGuildAgit},
        {0x1b90, CPacketTranslater::OnDBCreateGuildAgit},
        {0x1b98, CPacketTranslater::OnDBDeleteGuildAgit},
        {0x1b9c, CPacketTranslater::OnPowerWarProcessInfo},
        {0x1ba0, CPacketTranslater::OnDBLoadGuildAgit},
        {0x1ba8, CPacketTranslater::OnUpgradeGuildAgit},
        {0x1bac, CPacketTranslater::OnDBUpgradeGuildAgit},
        {0x1bb0, CPacketTranslater::OnGuildExpLimit},
        {0x1360, CPacketTranslater::OnWriteGuildMemberMemo},
        {0x1c3c, CPacketTranslater::OnLoadGuildCargo},
        {0x1c40, CPacketTranslater::OnLoadGuildCargoHistory},
        {0x1c44, CPacketTranslater::OnGuildCargo},
        {0x1c48, CPacketTranslater::OnGuildCargoHistory},
        {0x1c4c, CPacketTranslater::OnGuildCargoCheckPushItem},
        {0x1c50, CPacketTranslater::OnGuildCargoPushItem},
        {0x1c54, CPacketTranslater::OnGuildCargoPopItem},
        {0x1c58, CPacketTranslater::OnGuildCargoMoveItem},
        {0x1c68, CPacketTranslater::OnGuildCargoUpgrade},
        {0x8cc0, CPacketTranslater::OnGuildRequestGuildBoardOpen},
        {0x8ccc, CPacketTranslater::OnDBLoadReplyGuildBoardOpen},
        {0x8cd0, CPacketTranslater::OnGuildRequestGuildBoardWrite},
        {0x8cdc, CPacketTranslater::OnDBLoadReplyGuildBoardWrite},
        {0x8ce0, CPacketTranslater::OnGuildRequestGuildBoardDelete},
        {0x8cec, CPacketTranslater::OnDBLoadReplyGuildBoardDelete},
        {0x8d0c, CPacketTranslater::OnWebGuildBoardWrite},
        {0x8d10, CPacketTranslater::OnWebGuildBoardDelete},
        {0x8d18, CPacketTranslater::OnDBLoadReplyWebGuildBoardWrite},
        {0x1ae4, CPacketTranslater::OnGuildApplyOriginalPowerSide},
        {0x7d20, CPacketTranslater::OnGameServerRegist},
        {0x6f0c, CPacketTranslater::OnAddGuildFund},
        {0x6f14, CPacketTranslater::OnRefreshGuildInfo},
        {0x6f1c, CPacketTranslater::OnReplyTodayGuildMember},
        {0x6f24, CPacketTranslater::OnApproveJoinGuild},
        {0x6f30, CPacketTranslater::OnDBResponseApproveJoinGuild},
        {0x6f34, CPacketTranslater::OnGuildAttendanceInfo},
        {0x6f40, CPacketTranslater::OnGuildDebug},
        {0x7d50, CPacketTranslater::OnRenew_GM_List},
        {0x9c7c, CPacketTranslater::OnNoticeGuildChatMsgHyperLink},
    };
    for (unsigned int i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++)
    {
        *(void(**)(PacketHeader*))(m_data + tbl[i].off) = tbl[i].fn;
    }
}

int CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    unsigned short id = *(unsigned short*)pkt;
    if (id < 0x2800 && 999 < id)
    {
        static CPacketCounter<1000, 10240> g_decoderCounter(0, "PacketDispatcher");
        g_decoderCounter.IncrementPacketCount(id);
        void (**fn)(PacketHeader*) = (void (**)(PacketHeader*))(m_data + (id + 4) * 4 + 0xc);
        if (*fn == 0)
        {
            DNF_LOG_SCOPE_LINE(0x10d,"./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.<0 == m_decodProcFunc>\n",
                (unsigned int)id);
            return 0;
        }
        g_decoderCounter.BeforeProcess();
        (*fn)(pkt);
        g_decoderCounter.AfterProcess(id);
        return 1;
    }
    printf("Undefined Packet Err : Game Message with identifier %d has arrived.\n",
           (unsigned int)id);
    DNF_LOG_SCOPE_LINE(0x123,"./log/Decoder",
        "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.<Invalid Packet ID>\n",
        (unsigned int)id);
    return 0;
}

void CPacketDecoder::UdpProcess()
{
    if (*(void**)m_data == 0 || *(void**)(m_data + 4) == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    std::queue<CUdpRecvBuffer*>* q = *(std::queue<CUdpRecvBuffer*>**)m_data;
    CUdpRecvBuffer* buf;
    while (true)
    {
        do
        {
            if (q->empty())
            {
                return;
            }
            buf = q->front();
            q->pop();
        } while (buf == 0);
        int qsize = (int)q->size();
        CAppLoadChecker* checker = CAppLoadCheckerInstance();
        if (checker->CheckUdpRecvQ(qsize) != 0)
        {
            CServerHandler* handler = *(CServerHandler**)(m_data + 0x18);
            checker = CAppLoadCheckerInstance();
            checker->RequestDB(handler, 2, qsize);
        }
        if (MsgDecode((PacketHeader*)buf) != 1)
        {
            CMutex* mtx = *(CMutex**)(m_data + 8);
            CGuard<CMutex> g(mtx);
            CUdpRecvBuffer::operator delete(buf);
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", buf,
                   (unsigned int)*(unsigned short*)buf);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        CMutex* mtx = *(CMutex**)(m_data + 8);
        CGuard<CMutex> g(mtx);
        CUdpRecvBuffer::operator delete(buf);
    }
}

void CPacketDecoder::TcpProcess()
{
    if (*(void**)(m_data + 0xc) == 0 || *(void**)(m_data + 0x10) == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    std::queue<CTcpRecvBuffer*>* q = *(std::queue<CTcpRecvBuffer*>**)(m_data + 0xc);
    CTcpRecvBuffer* buf;
    while (true)
    {
        do
        {
            if (q->empty())
            {
                return;
            }
            buf = q->front();
            q->pop();
        } while (buf == 0);
        int qsize = (int)q->size();
        CAppLoadChecker* checker = CAppLoadCheckerInstance();
        if (checker->CheckTcpRecvQ(qsize) != 0)
        {
            CServerHandler* handler = *(CServerHandler**)(m_data + 0x18);
            checker = CAppLoadCheckerInstance();
            checker->RequestDB(handler, 1, qsize);
        }
        if (MsgDecode((PacketHeader*)buf) != 1)
        {
            CMutex* mtx = *(CMutex**)(m_data + 0x14);
            CGuard<CMutex> g(mtx);
            CTcpRecvBuffer::operator delete(buf);
            printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n", buf,
                   (unsigned int)*(unsigned short*)buf);
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        CMutex* mtx = *(CMutex**)(m_data + 0x14);
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
        *(void**)m_data = app->Get_UdpPacketParseQ();
        *(void**)(m_data + 4) = app->Get_UdpQLock();
        *(void**)(m_data + 8) = app->Get_UdpBLock();
        CTcpNetSystem* net = app->Get_TcpNetSystem();
        if (net != 0)
        {
            typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> > TcpRecvQueue;
            CSwapQueue<TcpRecvQueue, 2>* swapq =
                (CSwapQueue<TcpRecvQueue, 2>*)net->Get_TcpSwapQPacket();
            *(void**)(m_data + 0xc) = swapq->GetParseQ();
        }
        else
        {
            *(void**)(m_data + 0xc) = 0;
        }
        *(void**)(m_data + 0x10) = net != 0 ? net->Get_TcpRecvQLock() : 0;
        *(void**)(m_data + 0x14) = net != 0 ? net->Get_TcpRecvBLock() : 0;
        *(void**)(m_data + 0x18) = app->Get_ServerHandler();
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
}

