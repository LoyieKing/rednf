// df_statics_r — DNFPacketDecoder（ORIG DNFPacketDecoder.o 拆分）
#include <stdio.h>
#include <time.h>
#include <queue>

#include "DNFPacketDecoder.h"
#include "DNFApplication.h"
#include "DNFUdpRecvBuffer.h"
#include "DNFPacketTranslater.h"
#include "DNFPacketTracer.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "Thread.h"

CPacketDecoder* CPacketDecoderInstance()
{
    static CPacketDecoder instance;
    return &instance;
}
CPacketDecoder::CPacketDecoder()
{
    m_queue = 0;
    m_lock = 0;
    m_poolLock = 0;
    for (int i = 1000; i < 0x2800; i++)
    {
        m_handlers[i] = 0;
    }
    m_handlers[0x3ec] = (void*)&CPacketTranslater::OnHeartBeat;
    m_handlers[0x44f] = (void*)&CPacketTranslater::OnEventStart;
    m_handlers[0x450] = (void*)&CPacketTranslater::OnEventEnd;
    m_handlers[0x578] = (void*)&CPacketTranslater::OnMonitorManagerConnectOK;
    m_handlers[0x9ce] = (void*)&CPacketTranslater::OnClientSpecStatistic;
    m_handlers[0xc26] = (void*)&CPacketTranslater::OnFrameLagStatisticsAdd;
    m_handlers[0xc28] = (void*)&CPacketTranslater::OnFrameLagStatisticsResultLoadSpec;
    m_handlers[0xc2a] = (void*)&CPacketTranslater::OnFrameLagStatisticsResultReloadSpec;
    m_handlers[0xc2e] = (void*)&CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify;
    m_handlers[0xc2f] = (void*)&CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck;
    m_handlers[0xfa6] = (void*)&CPacketTranslater::OnPartyResultStatistic;
    m_handlers[0xfa7] = (void*)&CPacketTranslater::OnPartyPingStatistic;
    m_handlers[0xfa8] = (void*)&CPacketTranslater::OnPvpPingStatistic;
    m_handlers[0xfa9] = (void*)&CPacketTranslater::OnAbnormalExitStatistic;
    m_handlers[0xc35] = (void*)&CPacketTranslater::OnDungeonStatisticParty;
    m_handlers[0xc37] = (void*)&CPacketTranslater::OnDungeonStatisticPartyJob;
    m_handlers[0xc39] = (void*)&CPacketTranslater::OnDungeonStatisticPartyCharac;
    m_handlers[0xc3b] = (void*)&CPacketTranslater::OnDeathTowerStatisticValue;
    m_handlers[0xc3d] = (void*)&CPacketTranslater::OnDeathTowerStatisticPlayDataJob;
    m_handlers[0xc3f] = (void*)&CPacketTranslater::OnDeathTowerStatisticPlayDataParty;
    m_handlers[0xc43] = (void*)&CPacketTranslater::OnPacketOverflowStatistic;
    m_handlers[0xc41] = (void*)&CPacketTranslater::OnAssertManagerStatistic;
    m_handlers[0xc45] = (void*)&CPacketTranslater::OnUserTingTimeCheck;
    m_handlers[0xc31] = (void*)&CPacketTranslater::OnHellPartyStatisticItem;
    m_handlers[0xfb0] = (void*)&CPacketTranslater::OnLoadingTimeReportStatistics;
    m_handlers[0xc4a] = (void*)&CPacketTranslater::OnFatigueBatteryMoneyStatistics;
    m_handlers[0x1036] = (void*)&CPacketTranslater::OnBloodDungeonStatistic;
    m_handlers[0xc33] = (void*)&CPacketTranslater::OnCubeStatistic;
    m_handlers[0xc48] = (void*)&CPacketTranslater::OnReasonCrashDownData;
    m_handlers[0x17a2] = (void*)&CPacketTranslater::OnUpdateDisjointAvatarStatic;
    m_handlers[0x17a4] = (void*)&CPacketTranslater::OnUpdateCreateEmblemStatic;
    m_handlers[0x17ad] = (void*)&CPacketTranslater::OnUserCountStatistic;
    m_handlers[0x17b6] = (void*)&CPacketTranslater::OnRandomboxStatistic;
    m_handlers[0x17b9] = (void*)&CPacketTranslater::OnLagStatisticsAdd;
    m_handlers[0x17ca] = (void*)&CPacketTranslater::OnValueStatistic;
    m_handlers[0x17cb] = (void*)&CPacketTranslater::OnCirculationStatistic;
    m_handlers[0x1b67] = (void*)&CPacketTranslater::OnServerMatchData;
    m_handlers[0x1b69] = (void*)&CPacketTranslater::OnSecretShopStatistic;
    m_handlers[0x1f46] = (void*)&CPacketTranslater::OnGoldCardEventStatistic;
    m_handlers[0x271c] = (void*)&CPacketTranslater::OnTowerOfDespairStatistic;
    m_handlers[0x2720] = (void*)&CPacketTranslater::OnStatistic;
    m_handlers[0x2722] = (void*)&CPacketTranslater::OnStatisticGmCmd;
    m_handlers[0x2723] = (void*)&CPacketTranslater::OnMoneyLog;
    m_handlers[0x27e0] = (void*)&CPacketTranslater::OnCompatibilityIndex;
    m_handlers[0x27e1] = (void*)&CPacketTranslater::OnP2PStatistic;
    m_handlers[0x27ed] = (void*)&CPacketTranslater::OnFairPvpPingStatistic;
    m_handlers[0x27f3] = (void*)&CPacketTranslater::OnFileStatistic;
    m_handlers[0x27fd] = (void*)&CPacketTranslater::OnHolePunchingSuccessRateStatistic;
}
int CPacketDecoder::MsgDecode(PacketHeader* pkt)
{
    if (pkt == 0)
    {
        return 0;
    }
    if (*(unsigned short*)pkt < 0x2800 && 999 < *(unsigned short*)pkt)
    {
        static CPacketCounter<1000, 10240> packet_counter((char*)0, "PacketDispatcher");
        packet_counter.IncrementPacketCount(*(unsigned short*)pkt);
        if (m_handlers[*(unsigned short*)pkt] == 0)
        {
            DNF_LOG_SCOPE_LINE(0xca, "./log/Decoder",
                "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
                *(unsigned short*)pkt);
            return 0;
        }
        packet_counter.BeforeProcess();
        ((void (*)(PacketHeader*))m_handlers[*(unsigned short*)pkt])(pkt);
        packet_counter.AfterProcess(*(unsigned short*)pkt);
        return 1;
    }
    printf("Game Message with identifier %d has arrived.\n", *(unsigned short*)pkt);
    DNF_LOG_SCOPE_LINE(0xea,"./log/Decoder",
        "CPacketDecoder::MsgDecode() Game Message with identifier %d has arrived.\n",
        *(unsigned short*)pkt);
    return 0;
}
void CPacketDecoder::Process()
{
    if (m_queue == 0 || m_lock == 0)
    {
        throw CDNFException("CPacketDecoder is Not Ready!\n");
    }
    PacketHeader* pkt = 0;
    {
        CGuard<CMutex> g((CMutex*)m_lock);
        std::queue<CUdpRecvBuffer*>* q = (std::queue<CUdpRecvBuffer*>*)m_queue;
        if (!q->empty())
        {
            pkt = (PacketHeader*)q->front();
            q->pop();
        }
    }
    if (pkt != 0)
    {
        if (MsgDecode(pkt) != 1)
        {
            {
                CGuard<CMutex> g((CMutex*)m_poolLock);
                CUdpRecvBuffer::operator delete(pkt);
            }
            throw CDNFException(
                "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
        }
        {
            CGuard<CMutex> g((CMutex*)m_poolLock);
            CUdpRecvBuffer::operator delete(pkt);
        }
    }
}
void CPacketDecoder::Attach(CApplication* app)
{
    if (app != 0)
    {
        m_queue = app->Get_QPacket();
        m_lock = app->Get_QLock();
        m_poolLock = app->Get_BLock();
    }
}
CPacketDecoder::~CPacketDecoder()
{
    m_queue = 0;
    m_lock = 0;
}
template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::CPacketCounter(char* name, char* title)
{
    Reset();
    time_t t = time(0);
    *(time_t*)((char*)this + 4) = t;
    if (name == 0)
    {
        sprintf((char*)this + 0x1d540, "./log/%s", title);
    }
    else
    {
        sprintf((char*)this + 0x1d540, "./log/%s/%s", name, title);
    }
    *(unsigned char*)((char*)this + 0x1d640) = 1;
}
template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter()
{
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id < 0x2800 && 999 < id &&
        (*(unsigned char*)((char*)this + 0x1d640) == 1 ||
         *(unsigned int*)((char*)this + (id - 1000) * 4 + 8) < 0xb))
    {
        *(unsigned int*)((char*)this + (id - 1000) * 4 + 8) += 1;
    }
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::BeforeProcess()
{
    *(unsigned int*)((char*)this + 0x9068) = *(unsigned int*)this;
    if (*(int*)((char*)this + 0x9068) == -1)
    {
        *(unsigned int*)((char*)this + 0x9068) = 0;
    }
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::AfterProcess(int id)
{
    if (id < 0x2800 && 999 < id &&
        (*(unsigned char*)((char*)this + 0x1d640) == 1 ||
         *(unsigned int*)((char*)this + (id - 1000) * 4 + 8) < 0xb))
    {
        int diff = *(int*)this;
        if (diff != -1)
        {
            int start;
            if (*(unsigned char*)((char*)this + 0x1d640) == 0)
            {
                start = *(int*)((char*)this + (id + 0x2030) * 4 + 8);
                *(unsigned int*)((char*)this + (id - 1000) * 4 + 8) += 1;
                *(unsigned char*)((char*)this + id + 0x11ce0) = 0;
            }
            else
            {
                start = *(int*)((char*)this + 0x9068);
            }
            diff = diff - start;
            *(int*)((char*)this + (id + 0x4d50) * 4) += diff;
        }
    }
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    for (int i = 0; i < 0x2418; i++)
    {
        *(unsigned int*)((char*)this + i * 4 + 8) = 0;
        *(unsigned int*)((char*)this + (i + 0x5138) * 4) = 0;
        *(unsigned int*)((char*)this + (i + 0x2418) * 4 + 8) = 0;
        *(unsigned char*)((char*)this + i + 0x120c8) = 0;
    }
    *(unsigned int*)this = 0;
    *(unsigned char*)((char*)this + 0x1d641) = 0;
}
