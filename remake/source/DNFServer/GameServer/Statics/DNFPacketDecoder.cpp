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
    m_handlers[0x3ec] = &CPacketTranslater::OnHeartBeat;
    m_handlers[0x44f] = &CPacketTranslater::OnEventStart;
    m_handlers[0x450] = &CPacketTranslater::OnEventEnd;
    m_handlers[0x578] = &CPacketTranslater::OnMonitorManagerConnectOK;
    m_handlers[0x9ce] = &CPacketTranslater::OnClientSpecStatistic;
    m_handlers[0xc26] = &CPacketTranslater::OnFrameLagStatisticsAdd;
    m_handlers[0xc28] = &CPacketTranslater::OnFrameLagStatisticsResultLoadSpec;
    m_handlers[0xc2a] = &CPacketTranslater::OnFrameLagStatisticsResultReloadSpec;
    m_handlers[0xc2e] = &CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify;
    m_handlers[0xc2f] = &CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck;
    m_handlers[0xfa6] = &CPacketTranslater::OnPartyResultStatistic;
    m_handlers[0xfa7] = &CPacketTranslater::OnPartyPingStatistic;
    m_handlers[0xfa8] = &CPacketTranslater::OnPvpPingStatistic;
    m_handlers[0xfa9] = &CPacketTranslater::OnAbnormalExitStatistic;
    m_handlers[0xc35] = &CPacketTranslater::OnDungeonStatisticParty;
    m_handlers[0xc37] = &CPacketTranslater::OnDungeonStatisticPartyJob;
    m_handlers[0xc39] = &CPacketTranslater::OnDungeonStatisticPartyCharac;
    m_handlers[0xc3b] = &CPacketTranslater::OnDeathTowerStatisticValue;
    m_handlers[0xc3d] = &CPacketTranslater::OnDeathTowerStatisticPlayDataJob;
    m_handlers[0xc3f] = &CPacketTranslater::OnDeathTowerStatisticPlayDataParty;
    m_handlers[0xc43] = &CPacketTranslater::OnPacketOverflowStatistic;
    m_handlers[0xc41] = &CPacketTranslater::OnAssertManagerStatistic;
    m_handlers[0xc45] = &CPacketTranslater::OnUserTingTimeCheck;
    m_handlers[0xc31] = &CPacketTranslater::OnHellPartyStatisticItem;
    m_handlers[0xfb0] = &CPacketTranslater::OnLoadingTimeReportStatistics;
    m_handlers[0xc4a] = &CPacketTranslater::OnFatigueBatteryMoneyStatistics;
    m_handlers[0x1036] = &CPacketTranslater::OnBloodDungeonStatistic;
    m_handlers[0xc33] = &CPacketTranslater::OnCubeStatistic;
    m_handlers[0xc48] = &CPacketTranslater::OnReasonCrashDownData;
    m_handlers[0x17a2] = &CPacketTranslater::OnUpdateDisjointAvatarStatic;
    m_handlers[0x17a4] = &CPacketTranslater::OnUpdateCreateEmblemStatic;
    m_handlers[0x17ad] = &CPacketTranslater::OnUserCountStatistic;
    m_handlers[0x17b6] = &CPacketTranslater::OnRandomboxStatistic;
    m_handlers[0x17b9] = &CPacketTranslater::OnLagStatisticsAdd;
    m_handlers[0x17ca] = &CPacketTranslater::OnValueStatistic;
    m_handlers[0x17cb] = &CPacketTranslater::OnCirculationStatistic;
    m_handlers[0x1b67] = &CPacketTranslater::OnServerMatchData;
    m_handlers[0x1b69] = &CPacketTranslater::OnSecretShopStatistic;
    m_handlers[0x1f46] = &CPacketTranslater::OnGoldCardEventStatistic;
    m_handlers[0x271c] = &CPacketTranslater::OnTowerOfDespairStatistic;
    m_handlers[0x2720] = &CPacketTranslater::OnStatistic;
    m_handlers[0x2722] = &CPacketTranslater::OnStatisticGmCmd;
    m_handlers[0x2723] = &CPacketTranslater::OnMoneyLog;
    m_handlers[0x27e0] = &CPacketTranslater::OnCompatibilityIndex;
    m_handlers[0x27e1] = &CPacketTranslater::OnP2PStatistic;
    m_handlers[0x27ed] = &CPacketTranslater::OnFairPvpPingStatistic;
    m_handlers[0x27f3] = &CPacketTranslater::OnFileStatistic;
    m_handlers[0x27fd] = &CPacketTranslater::OnHolePunchingSuccessRateStatistic;
}
bool CPacketDecoder::MsgDecode(PacketHeader* pkt)
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
        m_handlers[*(unsigned short*)pkt](pkt);
        packet_counter.AfterProcess(*(unsigned short*)pkt);
        // ORIG 在 AfterProcess 调用后有一条 1 字节对齐 nop（§8 对齐伪影），
        // 使 return 1 前的 mov 对齐；缺失会导致后续分支目标偏移注释整体 +1。
        __asm__("nop");
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
    CUdpRecvBuffer* pkt = 0;
    {
        CGuard<CMutex> g((CMutex*)m_lock);
        // ORIG：每次调用都重载 m_queue（无 q 局部变量，避免栈槽往返）。
        if (!((std::queue<CUdpRecvBuffer*>*)m_queue)->empty())
        {
            pkt = ((std::queue<CUdpRecvBuffer*>*)m_queue)->front();
            ((std::queue<CUdpRecvBuffer*>*)m_queue)->pop();
        }
    }
    if (pkt != 0)
    {
        // ORIG：强转结果经 -0x1c 临时槽往返后作为 MsgDecode 参数。
        PacketHeader* hdr = (PacketHeader*)pkt;
        if (MsgDecode(hdr) != 1)
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
    m_t = time(0);
    if (name != 0)
    {
        sprintf(m_path, "./log/%s/%s", name, title);
    }
    else
    {
        sprintf(m_path, "./log/%s", title);
    }
    m_bProcess = 1;
}
template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter()
{
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    // ORIG 分支形态：显式 return + !b && counts>=0xb 跳过；布尔物化 xor $1/test/je，
    // 退出经 nop+jmp 链，自增用 ++（sub 同寄存器形态）。
    if (id >= 0x2800) return;
    if (id <= 999) return;
    if (!m_bProcess && m_counts[id - 1000] >= 0xb) return;
    ++m_counts[id - 1000];
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::BeforeProcess()
{
    m_snapshot[0] = m_count;
    // ORIG 为 while+break 形态（cmp/sete/test/je 布尔物化），if 直接跳会变 jne。
    while ((int)m_snapshot[0] == -1)
    {
        m_snapshot[0] = 0;
        break;
    }
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::AfterProcess(int id)
{
    if (id >= 0x2800) return;
    if (id <= 999) return;
    if (!m_bProcess && m_counts[id - 1000] >= 0xb) return;
    int diff = (int)m_count;
    // ORIG：while+return 产生 cmp/sete/test/je 布尔物化。
    while (diff == -1)
    {
        diff = 0;
        return;
    }
    int diff2;
    if (m_bProcess)
    {
        diff2 = diff - (int)m_snapshot[0];
    }
    else
    {
        diff2 = diff - (int)m_snapshot[id - 1000];
        ++m_counts[id - 1000];
        m_pending[id - 1000] = 0;
    }
    m_diffs[id - 1000] += diff2;
}
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    for (int i = 0; i < 0x2418; i++)
    {
        m_counts[i] = 0;
        m_diffs[i] = 0;
        m_snapshot[i] = 0;
        m_pending[i] = 0;
    }
    m_count = 0;
    m_bInit = 0;
}
