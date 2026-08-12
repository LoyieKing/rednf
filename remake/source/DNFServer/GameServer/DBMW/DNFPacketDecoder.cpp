// df_dbmw_r - DNFPacketDecoder (ORIG DNFPacketDecoder.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
CPacketTracer* CPacketTracerInstance();
CAppLoadChecker* CAppLoadCheckerInstance();

template class std::allocator<std::_Rb_tree_node<std::pair<const unsigned int, stPacketProcess> > >;

CPacketDecoder::CPacketDecoder()
{
    m_udpQueue = 0;
    m_udpQLock = 0;
    m_udpBLock = 0;
    m_tcpQueue = 0;
    m_tcpRecvQLock = 0;
    m_tcpRecvBLock = 0;
    m_serverHandler = 0;
    for (int i = 0x3e8; i <= 0x27ff; i++)
        m_table[i] = 0;
    m_table[0x404] = CPacketTranslater::OnQueryGuild;
    m_table[0x402] = CPacketTranslater::OnQueryGuildMember;
    m_table[0x409] = CPacketTranslater::OnSaveGuild;
    m_table[0x4b2] = CPacketTranslater::OnQueryMember;
    m_table[0x4b4] = CPacketTranslater::OnSaveMember;
    m_table[0x4b5] = CPacketTranslater::OnSaveMemberUpdateCharInfo;
    m_table[0x4c3] = CPacketTranslater::OnMemberDeleteAsCharDelete;
    m_table[0x40f] = CPacketTranslater::OnSaveGuildMember;
    m_table[0x448] = CPacketTranslater::OnSavePowerSecedeTime;
    m_table[0x413] = CPacketTranslater::OnDBMWConnectionCheck;
    m_table[0x418] = CPacketTranslater::OnSaveGuildWarInfo;
    m_table[0x412] = CPacketTranslater::OnEndGuildWar;
    m_table[0x5de] = CPacketTranslater::OnRegisterToBlackList;
    m_table[0x5df] = CPacketTranslater::OnDeleteToBlackList;
    m_table[0x5e1] = CPacketTranslater::OnRequestBlackListOnLogin;
    m_table[0x641] = CPacketTranslater::OnSaveMemberExp;
    m_table[0x673] = CPacketTranslater::OnAddBuddy;
    m_table[0x675] = CPacketTranslater::OnDelBuddy;
    m_table[0x676] = CPacketTranslater::OnQueryBuddyInfo;
    m_table[0x425] = CPacketTranslater::OnQueryGuildAllMembersProxy;
    m_table[0x427] = CPacketTranslater::OnQueryUnconnGuildMemberProxy;
    m_table[0x42a] = CPacketTranslater::OnSendMailCoinGuildEvent;
    m_table[0x42b] = CPacketTranslater::OnChangeUnconnectedGuildMemberGrade;
    m_table[0x42d] = CPacketTranslater::OnChangeGuildNotifyMessage;
    m_table[0x432] = CPacketTranslater::OnSendGuildLetter;
    m_table[0x438] = CPacketTranslater::OnGuildJoin;
    m_table[0x439] = CPacketTranslater::OnGuildSecede;
    m_table[0x4cc] = CPacketTranslater::onQueryTowerFullRank;
    m_table[0x43b] = CPacketTranslater::OnGuildMasterDelegate;
    m_table[0x43f] = CPacketTranslater::OnRequestGuildCreate;
    m_table[0x9cf] = CPacketTranslater::OnSendHWspec;
    m_table[0x677] = CPacketTranslater::OnQueryGuildBooting;
    m_table[0x6ad] = CPacketTranslater::OnSavePowerWarPoint;
    m_table[0x452] = CPacketTranslater::OnSaveUnchangableGuildInfo;
    m_table[0x451] = CPacketTranslater::OnChangeCharacName;
    m_table[0x9d1] = CPacketTranslater::OnErrorLineSave;
    m_table[0xc36] = CPacketTranslater::OnPartyStatisticSave;
    m_table[0xc38] = CPacketTranslater::OnPartyJobStatisticSave;
    m_table[0xc3a] = CPacketTranslater::OnPartyCharacStatisticSave;
    m_table[0xc3c] = CPacketTranslater::OnDeathTowerValueStatisticSave;
    m_table[0xc3e] = CPacketTranslater::OnDeathTowerPlayDataJobStatisticSave;
    m_table[0xc40] = CPacketTranslater::OnDeathTowerPlayDataPartyStatisticSave;
    m_table[0xc32] = CPacketTranslater::OnHellPartyStatisticItemSave;
    m_table[0xc34] = CPacketTranslater::OnCubeStatisticDataSave;
    m_table[0xc27] = CPacketTranslater::OnQueryFirstLoadSpecDb;
    m_table[0xc29] = CPacketTranslater::OnQueryReloadSpecDb;
    m_table[0xc2b] = CPacketTranslater::OnInsertFrameLagStatistics;
    m_table[0xc2d] = CPacketTranslater::OnWriteQueryStatistics;
    m_table[0xc2c] = CPacketTranslater::OnInsertDailyBadSpecStatistics;
    m_table[0xc30] = CPacketTranslater::OnInsertUsedMemoryStatistic;
    m_table[0xfaa] = CPacketTranslater::OnInsertUdpCharacteristic;
    m_table[0xfa0] = CPacketTranslater::OnInnerPacketLogin;
    m_table[0xfa1] = CPacketTranslater::OnInnerPacketLogout;
    m_table[0x1068] = CPacketTranslater::OnTcpServerLogin;
    m_table[0x1069] = CPacketTranslater::OnTcpServerLogout;
    m_table[0x106a] = CPacketTranslater::OnTcpServerHeartbeat;
    m_table[0x6d7] = CPacketTranslater::OnSavePowerWarUserRank;
    m_table[0x6d6] = CPacketTranslater::OnSavePowerWarGuildRank;
    m_table[0x6d9] = CPacketTranslater::OnSavePowerWarPointReward;
    m_table[0x6da] = CPacketTranslater::OnSavePowerWarStatueRanker;
    m_table[0x6dd] = CPacketTranslater::OnCreateGuildAgit;
    m_table[0x6df] = CPacketTranslater::OnDeleteGuildAgit;
    m_table[0x6e1] = CPacketTranslater::OnLoadGuildAgit;
    m_table[0x6e4] = CPacketTranslater::OnUpgradeGuildAgit;
    m_table[0xc42] = CPacketTranslater::OnSaveAssertManagerInfo;
    m_table[0xc44] = CPacketTranslater::OnSavePacketOverflowStatistic;
    m_table[0x4d3] = CPacketTranslater::OnWriteGuildMemberMemo;
    m_table[0xc46] = CPacketTranslater::OnWriteUserTingTimeCheck;
    m_table[0xc47] = CPacketTranslater::OnTingUserCollect;
    m_table[0xfb1] = CPacketTranslater::OnRecvLoadingTimeReport;
    m_table[0xfb2] = CPacketTranslater::OnRecvPowerwarLoadingReport;
    m_table[0xfb3] = CPacketTranslater::OnRecvPowerwarLagReport;
    m_table[0xc4a] = CPacketTranslater::OnSaveFatigueBatteryStatistic;
    m_table[0x710] = CPacketTranslater::OnUpdateGuildCargo;
    m_table[0x711] = CPacketTranslater::OnInsertGuildCargoHistory;
    m_table[0x714] = CPacketTranslater::OnUpgradeGuildCargo;
    m_table[0x1007] = CPacketTranslater::onItemLimitEditionLoadDataReq;
    m_table[0x1006] = CPacketTranslater::onItemLimitEditionUpdateData;
    m_table[0x9d2] = CPacketTranslater::OnServeQueueLoadStatistic;
    m_table[0x1037] = CPacketTranslater::OnBloodDungeonStatistic;
    m_table[0x1038] = CPacketTranslater::OnRequestIPCounterList;
    m_table[0xc49] = CPacketTranslater::OnReasonCrashDownQuery;
    m_table[0x177c] = CPacketTranslater::OnDBMWInsertMail;
    m_table[0x177d] = CPacketTranslater::OnDBMWQueryMsg;
    m_table[0x177b] = CPacketTranslater::OnDBMWVillageAttackRank;
    m_table[0x17a3] = CPacketTranslater::OnDBMWDisjointAvatarStatic;
    m_table[0x17a5] = CPacketTranslater::OnDBMWCreateEmblemStatic;
    m_table[0x17b7] = CPacketTranslater::OnDBMWRandomboxStatic;
    m_table[0x17b8] = CPacketTranslater::OnDBMWLoginLogoutStatistics;
    m_table[0xb62] = CPacketTranslater::OnRequestARSInfo;
    m_table[0x6f4] = CPacketTranslater::OnSavePowerWarBonusPoint;
    m_table[0x17c5] = CPacketTranslater::OnDBMWTechnicalReportCommonQuery;
    m_table[0x1b68] = CPacketTranslater::OnServerMatchData;
    m_table[0x1b69] = CPacketTranslater::OnSecretShopStatistic;
    m_table[0x1bbf] = CPacketTranslater::OnQueryTodayGuildMemeber;
    m_table[0x1b6b] = CPacketTranslater::OnManagerEventTriggerAck;
    m_table[0x1f47] = CPacketTranslater::OnGoldcardEventStatistic;
    m_table[0x232b] = CPacketTranslater::OnDBLoadRequestGuildBoardOpen;
    m_table[0x232f] = CPacketTranslater::OnDBLoadRequestGuildBoardWrite;
    m_table[0x2333] = CPacketTranslater::OnDBLoadRequestGuildBoardDelete;
    m_table[0x233e] = CPacketTranslater::OnDBLoadRequestWebGuildBoardWrite;
    m_table[0x1f48] = CPacketTranslater::OnLoadPeriodicMessage;
    m_table[0x1bc4] = CPacketTranslater::OnRequestApproveJoinGuild;
    m_table[0x1f4d] = CPacketTranslater::OnRenew_GM_List;
    m_table[0x2340] = CPacketTranslater::OnReqOntimeEventIdx;
    m_table[0x2345] = CPacketTranslater::OnReqOntimeEventItem;
    m_table[0x2347] = CPacketTranslater::OnReqOntimeEventIdxUpdate;
    m_table[0x271d] = CPacketTranslater::OnUpdateTowerOfDespairStatistic;
    m_table[0x17ad] = CPacketTranslater::OnUpdateChannelOccNum;
    m_table[0x2721] = CPacketTranslater::OnInsertUpdate;
    m_table[0x27d8] = CPacketTranslater::onLoadLimitNpcBuyItemInfo;
    m_table[0x27dd] = CPacketTranslater::onUpdateLimitNpcBuyItemInfo;
    m_table[0x27e0] = CPacketTranslater::onCompatibilityIndex;
    m_table[0x27e1] = CPacketTranslater::OnP2PStatistics;
    m_table[0x27ea] = CPacketTranslater::onCollectItemsUpdate;
    m_table[0x27eb] = CPacketTranslater::onCollectItemsGm;
    m_table[0x27ec] = CPacketTranslater::OnPcRoomPlayTimeReward;
    m_table[0x27fb] = CPacketTranslater::onStartGameEventFromServer;
    m_table[0x27fc] = CPacketTranslater::onEndGameEventFromServer;
    m_table[0x27fd] = CPacketTranslater::onInsertHolePunchingResult;
}
CPacketDecoder::~CPacketDecoder()
{
    m_udpQueue = 0;
    m_udpQLock = 0;
}
CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}
void CPacketDecoder::TcpProcess()
{
    if (!m_tcpQueue || !m_tcpRecvQLock)
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    CTcpRecvBuffer* buf = 0;
    while (!m_tcpQueue->empty())
    {
        buf = m_tcpQueue->front();
        m_tcpQueue->pop();
        if (buf)
        {
            PacketHeader* p = (PacketHeader*)buf;
            int size = m_tcpQueue->size();
            if (CAppLoadCheckerInstance()->CheckTcpRecvQ(size))
            {
                CAppLoadCheckerInstance()->RequestDB(m_serverHandler, 1, size);
            }
            if (!MsgDecode(p))
            {
                {
                    CGuard<CMutex> guard(m_tcpRecvBLock);
                    delete buf;
                }
                printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n",
                       p, p->packetId);
                throw CDNFException(
                    "CPacketDecode::MsgDecode() Undefined Packet Arrived Exception Break!");
            }
            {
                CGuard<CMutex> guard(m_tcpRecvBLock);
                delete buf;
            }
        }
    }
}
void CPacketDecoder::UdpProcess()
{
    if (!m_udpQueue || !m_udpQLock)
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    CUdpRecvBuffer* buf = 0;
    while (!m_udpQueue->empty())
    {
        buf = m_udpQueue->front();
        m_udpQueue->pop();
        if (buf)
        {
            PacketHeader* p = (PacketHeader*)buf;
            int size = m_udpQueue->size();
            if (CAppLoadCheckerInstance()->CheckUdpRecvQ(size))
            {
                CAppLoadCheckerInstance()->RequestDB(m_serverHandler, 2, size);
            }
            if (!MsgDecode(p))
            {
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                printf("[false == this->MsgDecode]packetHeader : %x\tpacket id : %d\n",
                       p, p->packetId);
                throw CDNFException(
                    "CPacketDecoder::MsgDecode() Undefined Packet Arrive Exception Break!");
            }
            {
                CGuard<CMutex> guard(m_udpBLock);
                delete buf;
            }
        }
    }
}
bool CPacketDecoder::MsgDecode(PacketHeader* header)
{
    if (!header)
        return 0;
    if (header->packetId <= 0x27ff && header->packetId > 0x3e7)
    {
        static CPacketCounter<1000, 10240> packet_counter(0, "PacketDispatcher");
        packet_counter.IncrementPacketCount(header->packetId);
        if (!m_table[header->packetId])
            return 0;
        packet_counter.BeforeProcess();
        CPacketTracerInstance()->StartPacketProcessLog(header->packetId);
        m_table[header->packetId](header);
        CPacketTracerInstance()->EndPacketProcessLog(header->packetId);
        packet_counter.AfterProcess(header->packetId);
        __asm__ __volatile__("nop");
        return 1;
    }
    printf("Game Message with identifier %i has arrived.\n", header->packetId);
    register unsigned int id = header->packetId;
    CMyFileLog log(__FUNCTION__, 0x1da);
    log("./log/Decoder.log",
        "CPacketDecoder::MsgDecode() Game Message with identifier %i has arrived.\n",
        id);
    return 0;
}
void CPacketDecoder::Attach(CApplication* app)
{
    if (app)
    {
        m_udpQueue = app->Get_UdpPacketParseQ();
        m_tcpQueue = app->Get_TcpNetSystem()->Get_TcpSwapQPacket()->GetParseQ();
        m_udpQLock = app->Get_QLock();
        m_udpBLock = app->Get_BLock();
        m_tcpRecvQLock = app->Get_TcpNetSystem()->Get_TcpRecvQLock();
        m_tcpRecvBLock = app->Get_TcpNetSystem()->Get_TcpRecvBLock();
        m_serverHandler = app->Get_ServerHandler();
    }
}
void CPacketDecoder::Process() { UdpProcess(); TcpProcess(); }
void CPacketDecoder::SetTCPQueue(TcpRecvQueue* q) { m_tcpQueue = q; }
void CPacketDecoder::SetUdpQueue(UdpRecvQueue* q) { m_udpQueue = q; }
