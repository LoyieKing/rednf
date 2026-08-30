// ============================================================================
// df_game_r 还原 —— 拍卖域代理/发包（G4 商城/交易批次）
// 参照 docs/class_func_reports/CAuctionServerProxy.md、
// CCeraAuctionServerProxy.md、CDispatchServerPacket.md + ORIG 反汇编。
// CAuctionAveragePrice 由 CAuctionAveragePrice.cpp 提供。
// ============================================================================

#include <cstring>

#include <cstdio>

#include "BaseServerProxy.h"
#include "CNetwork.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CVillageMonsterMgr.h"
#include "CEnvironment.h"
#include "PacketDesign.h"
#include "SigTypes.h"     // SIG_AUCTION_* 权威布局（本 TU GetInBuffer 实例化）
#include "TimerDispatcher.h"  // TimerSig / Timer_CheckAuctionReady 权威声明（独占本 TU 实现）
#include "LogManager.h"   // LogManager::logFormat
#include "LogManager.h"   // LogManager::logFormat
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn
#include "RestrictGeolocation.h"      // 0x27fe case：initGeolocation
#include "ServiceRestrictManager.h"   // 0x27ff case：loadPolicy
#include "CHadesServerProxy.h"       // 0xfe7 case：ConnTcpHadesSvr
#include "CSwitchLog.h"              // 0x2717 case：SetLogWriteFlag

// ---- 拍卖服务器注册/就绪检查包（ORIG 弱符号 ctor，继承 PACKET_HEADER）----
struct PCK_AUCTION_REGIST_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GA() { PACKET_CTOR_BODY(0, 0, 0x16); }
};
struct PCK_AUCTION_REGIST_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GP() { PACKET_CTOR_BODY(0x12, 0, 0x16); }
};
struct PCK_AUCTION_CHECK_AUCTION_READY_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA() { PACKET_CTOR_BODY(0, 0xd, 0x12); }
};
struct PCK_AUCTION_CHECK_AUCTION_READY_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_CHECK_AUCTION_READY_GP() { PACKET_CTOR_BODY(0x12, 0xd, 0x12); }
};

// ============================================================================
// CAuctionServerProxy（ORIG 0x82f7a90 起，BaseServerProxy 派生）
// ============================================================================
class CAuctionServerProxy : public BaseServerProxy
{
public:
    CAuctionServerProxy(char* ip, int port, char* name);
    virtual ~CAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    bool m_running;   // +0x30e0c
};

CAuctionServerProxy::CAuctionServerProxy(char* ip, int port, char* name)
    : BaseServerProxy(ip, port, name)
{
    m_running = 0;
    Init();
}

CAuctionServerProxy::~CAuctionServerProxy()
{
}

void CAuctionServerProxy::Init()
{
    m_running = 0;
}

void CAuctionServerProxy::OnDisconnect()
{
    Init();
}

bool CAuctionServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_AuctionServerPacket(recvLen);
}

void CAuctionServerProxy::SendPacket(nsl::PACKET_HEADER* packet)
{
    SendTcpPacket((char*)packet, (int)packet->getSize());
}

void CAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GA packet;
    int envField = G_CEnvironment()->m_serverEnvir.m_gcNo;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

void CAuctionServerProxy::SendCheckAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

bool CAuctionServerProxy::IsRunning()
{
    return m_running;
}

void CAuctionServerProxy::SetRunning(bool flag)
{
    m_running = flag;
}

// ============================================================================
// CCeraAuctionServerProxy（ORIG 0x82f7ba0 起）
// ============================================================================
class CCeraAuctionServerProxy : public BaseServerProxy
{
public:
    CCeraAuctionServerProxy(char* ip, int port, char* name);
    virtual ~CCeraAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckCeraAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    bool m_running;   // +0x30e0c
};

CCeraAuctionServerProxy::CCeraAuctionServerProxy(char* ip, int port, char* name)
    : BaseServerProxy(ip, port, name)
{
    m_running = 0;
    Init();
}

CCeraAuctionServerProxy::~CCeraAuctionServerProxy()
{
}

void CCeraAuctionServerProxy::Init()
{
    m_running = 0;
}

void CCeraAuctionServerProxy::OnDisconnect()
{
    Init();
}

bool CCeraAuctionServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_AuctionServerPacket(recvLen);
}

void CCeraAuctionServerProxy::SendPacket(nsl::PACKET_HEADER* packet)
{
    SendTcpPacket((char*)packet, (int)packet->getSize());
}

void CCeraAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GP packet;
    int envField = *(int*)((char*)G_CEnvironment() + 0x198);
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

void CCeraAuctionServerProxy::SendCheckCeraAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GP packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

bool CCeraAuctionServerProxy::IsRunning()
{
    return m_running;
}

void CCeraAuctionServerProxy::SetRunning(bool flag)
{
    m_running = flag;
}

// ============================================================================
// CDispatchServerPacket（拍卖/点券服务器回包分发，ORIG 0x8473660/0x847ac3c/0x847c1fe）
// 语义：从拍卖/点券代理收包，按 packetID 组 SIG 流转投 DISPATCH_Q。
// ============================================================================
// ORIG：dispatch_sig T 0x863a808、RegistNextTimer T 0x863a858、
//   D1/D2 W 0x863e8ce、D0 W 0x863e8fe；无 C1（类从未被构造）。
// 类声明见 TimerDispatcher.h（TimerSig 派生，对象尺寸 4：仅 vptr）。
// dispatch_sig：两代理 IsRunning()==false 时分别补发就绪检查包，恒返 1。
// ---------------------------------------------------------------------------
void Timer_CheckAuctionReady::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                static_cast<TIMER_MESSAGE>(0x98), 0x3c, 0, 0);
}

int Timer_CheckAuctionReady::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        GlobalData::s_auction_proxy->SendCheckAuctionReadyPacket();
    }
    if (!GlobalData::s_cera_auction_proxy->IsRunning())
    {
        GlobalData::s_cera_auction_proxy->SendCheckCeraAuctionReadyPacket();
    }
    return 1;
}

Timer_CheckAuctionReady::~Timer_CheckAuctionReady() {}

int CDispatchServerPacket::dispatchPacket(char* data)
{
    // ORIG 0x8473660：上游服务器包按首 short(sig) 重写头并转投 DISPATCH_Q。
    unsigned short payloadLen =
        (unsigned short)(*(unsigned short*)(data + 2) + 8);
    (void)payloadLen;
    CStreamGuard guard(true);
    if (payloadLen < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x36));
    }
    else if (payloadLen <= 0x3fff)  // [推断] 中段池边界未逐指令核对
    {
        guard.Set(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3a));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3e));
    }
    unsigned int sig = *(unsigned short*)data;
    if (sig == 0x3ea)
    {
        guard << 66;
        guard << -1;
        guard->put_binary(data, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3ee)
    {
        guard << 67;
        guard << -1;
        guard->put_binary(data, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3f2)
    {
        guard << 70;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3f3)
    {
        guard << 94;
        guard << *(int*)(data + 15);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3f8)
    {
        guard << 289;
        guard << -1;
        guard->put_binary(data, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3f9)
    {
        guard << 301;
        guard << -1;
        guard->put_binary(data, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3fd)
    {
        guard << 86;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 90);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3fe)
    {
        guard << 87;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 85);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x400)
    {
        guard << 90;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x401)
    {
        guard << 91;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x407)
    {
        guard << 93;
        guard << *(int*)(data + 11);
        guard->put_binary(data, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x40a)
    {
        guard << 96;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 313);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x40c)
    {
        guard << 98;
        guard << *(int*)(data + 41);
        guard->put_binary(data, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x40d)
    {
        guard << 95;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 399);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x423)
    {
        guard << 198;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x424)
    {
        guard << 0;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x42c)
    {
        guard << 215;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x42d)
    {
        guard << 216;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 119);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x430)
    {
        guard << 101;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 58);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x431)
    {
        guard << 102;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 44);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x434)
    {
        guard << 219;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x435)
    {
        guard << 220;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 71);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x437)
    {
        guard << 222;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x439)
    {
        guard << 234;
        guard << *(int*)(data + 20);
        guard->put_binary(data, 82);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x43d)
    {
        guard << 231;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x43e)
    {
        guard << 233;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x444)
    {
        guard << 291;
        guard << *(int*)(data + 18);
        guard->put_binary(data, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x445)
    {
        guard << 298;
        guard << -1;
        guard->put_binary(data, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x447)
    {
        guard << 388;
        guard << *(int*)(data + 18);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x44c)
    {
        guard << 71;
        guard << -1;
        guard->put_binary(data, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x44e)
    {
        guard << 85;
        guard << -1;
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x44f)
    {
        guard << 133;
        guard << -1;
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4b6)
    {
        guard << 108;
        guard << *(int*)(data + 11);
        guard->put_binary(data, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4b7)
    {
        guard << 109;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4b9)
    {
        guard << 112;
        guard << *(int*)(data + 13);
        guard->put_binary(data, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4ba)
    {
        guard << 111;
        guard << *(int*)(data + 13);
        guard->put_binary(data, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4bc)
    {
        guard << 114;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4bd)
    {
        guard << 115;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4bf)
    {
        guard << 116;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4c0)
    {
        guard << 118;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4cb)
    {
        guard << 229;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 98);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4cf)
    {
        guard << 230;
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x514)
    {
        guard << 130;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x546)
    {
        guard << 170;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x5dd)
    {
        guard << 173;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x5e0)
    {
        guard << 171;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 415);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x642)
    {
        guard << 186;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x672)
    {
        guard << 188;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6b0)
    {
        guard << 379;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6d8)
    {
        guard << 372;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6dc)
    {
        guard << 369;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6e0)
    {
        guard << 378;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6e2)
    {
        guard << 371;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6ea)
    {
        guard << 409;
        guard << -1;
        guard->put_binary(data, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70a)
    {
        guard << 426;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70c)
    {
        guard << 429;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70d)
    {
        guard << 430;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70f)
    {
        guard << 432;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x712)
    {
        guard << 427;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x76c)
    {
        guard << 211;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x9d3)
    {
        guard << 437;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x9dd)
    {
        guard << 148;
        guard << -1;
        guard->put_binary(data, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x9de)
    {
        guard << 184;
        guard << -1;
        guard->put_binary(data, 20);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xa8c)
    {
        guard << 293;
        guard << -1;
        guard->put_binary(data, 61);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb54)
    {
        guard << 294;
        guard << -1;
        guard->put_binary(data, 0);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb57)
    {
        guard << 341;
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb58)
    {
        guard << 343;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb5f)
    {
        guard << 401;
        guard << -1;
        guard->put_binary(data, 12);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb60)
    {
        guard << 418;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb64)
    {
        guard << 603;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb65)
    {
        guard << 628;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfa2)
    {
        guard << 307;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 6136);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfa3)
    {
        guard << 308;
        guard << -1;
        guard->put_binary(data, 42);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfab)
    {
        guard << 352;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfac)
    {
        guard << 353;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfae)
    {
        guard << 368;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfaf)
    {
        guard << 382;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfe7)
    {
        // ORIG 0x847a140：GlobalData::s_hades_proxy->ConnTcpHadesSvr() 后直返。
        GlobalData::s_hades_proxy->ConnTcpHadesSvr();
    }
        else if (sig == 0x1004)
    {
        guard << 389;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1006)
    {
        guard << 391;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x100a)
    {
        guard << 392;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1039)
    {
        guard << 402;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x103a)
    {
        guard << 404;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1774)
    {
        guard << 442;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1775)
    {
        guard << 446;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1777)
    {
        guard << 444;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1778)
    {
        guard << 445;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x177e)
    {
        guard << 448;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x17c0)
    {
        guard << 463;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b5b)
    {
        guard << 470;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b5d)
    {
        guard << 471;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b61)
    {
        guard << 473;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b62)
    {
        guard << 474;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b64)
    {
        guard << 476;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b65)
    {
        guard << 477;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b6a)
    {
        guard << 477;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b6c)
    {
        guard << 521;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1bbd)
    {
        guard << 532;
        guard << *(int*)(data + 15);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1bc1)
    {
        guard << 533;
        guard << *(int*)(data + 18);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1bc7)
    {
        guard << 541;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1bc8)
    {
        guard << 542;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f42)
    {
        guard << 504;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f45)
    {
        guard << 523;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f4b)
    {
        guard << 529;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f4c)
    {
        guard << 530;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2332)
    {
        guard << 436;
        guard << *(int*)(data + 15);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2713)
    {
        // ORIG je 847a35c：cMyTrace(0xdcc,5) "TOD: bug reload" 后直返。
        cMyTrace trace(__PRETTY_FUNCTION__, 0xdcc, 5);
        trace("TOD: bug reload");
    }
        else if (sig == 0x2717)
    {
        // ORIG 0x847a272：CSwitchLog::SetLogWriteFlag(data[10]) 后直返。
        CSwitchLog::SetLogWriteFlag(data[10]);
    }
        else if (sig == 0x2719)
    {
        guard << 678;
        guard << *(int*)(data + 15);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x271a)
    {
        guard << 679;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27d9)
    {
        guard << 709;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27da)
    {
        guard << 710;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27dc)
    {
        guard << 712;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27df)
    {
        guard << 718;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27e7)
    {
        guard << 762;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27e8)
    {
        guard << 763;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27f2)
    {
        guard << 770;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27f8)
    {
        guard << 815;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27f9)
    {
        // ORIG je 84797de：make_internal_stream_jpn(guard, 0x2a, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2a, -1);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27fa)
    {
        // ORIG je 847985a：make_internal_stream_jpn(guard, 0x2b, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2b, -1);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27fe)
    {
        // ORIG je 847ab96：initGeolocation 失败 → logFormat(1,...,0x1143,"GEO IPRELOAD FAIL")，恒返 1。
        if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
        {
            LogManager::logFormat(1, "DispatchServerPacket.cpp", __PRETTY_FUNCTION__,
                                  0x1143, "GEO IPRELOAD FAIL");
        }
    }
        else if (sig == 0x27ff)
    {
        // ORIG je 847abd8：ServiceRestrictManager::loadPolicy()，恒返 1。
        ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    }
        else if (sig == 0x1)
    {
        guard << 389;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2)
    {
        guard << 389;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3e8)
    {
        guard << 65;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 51);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3ef)
    {
        guard << 68;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3f7)
    {
        guard << 206;
        guard << -1;
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3fc)
    {
        guard << 89;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x3ff)
    {
        guard << 88;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x406)
    {
        guard << 92;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 9023);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x40b)
    {
        guard << 97;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x414)
    {
        guard << 153;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 348);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x428)
    {
        guard << 200;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x42f)
    {
        guard << 100;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x432)
    {
        guard << 218;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x436)
    {
        guard << 221;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x43b)
    {
        guard << 232;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x43f)
    {
        guard << 237;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x446)
    {
        guard << 292;
        guard << *(int*)(data + 18);
        guard->put_binary(data, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x44d)
    {
        guard << 74;
        guard << -1;
        guard->put_binary(data, 34);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x450)
    {
        guard << 134;
        guard << -1;
        guard->put_binary(data, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4b8)
    {
        guard << 110;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4bb)
    {
        guard << 113;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4be)
    {
        guard << 117;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 481);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4c1)
    {
        guard << 122;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x4d2)
    {
        guard << 374;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x5dc)
    {
        guard << 172;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x5e2)
    {
        guard << 174;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x674)
    {
        guard << 189;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6b2)
    {
        guard << 408;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 23);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6de)
    {
        guard << 370;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x6e5)
    {
        guard << 398;
        guard << -1;
        guard->put_binary(data, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70b)
    {
        guard << 428;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x70e)
    {
        guard << 431;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x713)
    {
        guard << 434;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x9dc)
    {
        guard << 146;
        guard << -1;
        guard->put_binary(data, 267);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x9e0)
    {
        guard << 183;
        guard << -1;
        guard->put_binary(data, 266);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb55)
    {
        guard << 294;
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb5e)
    {
        guard << 381;
        guard << -1;
        guard->put_binary(data, 1213);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xb61)
    {
        guard << 461;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xc1c)
    {
        guard << 239;
        guard << *(int*)(data + 14);
        guard->put_binary(data, 38);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfa5)
    {
        guard << 309;
        guard << -1;
        guard->put_binary(data, 128);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfad)
    {
        guard << 354;
        guard << *(int*)(data + 11);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0xfe6)
    {
        guard << 694;
        if (*(int*)(data + 0xe) != 0)
            guard << *(int*)(data + 0xe);
        else
            guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1005)
    {
        guard << 390;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x100f)
    {
        guard << 416;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1773)
    {
        guard << 441;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1776)
    {
        guard << 443;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x177a)
    {
        guard << 447;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x17c2)
    {
        guard << 465;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b5f)
    {
        guard << 472;
        guard << *(int*)(data + 14);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b63)
    {
        guard << 475;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b66)
    {
        guard << 202;
        guard << *(int*)(data + 10);
        guard->put_binary(data, 351);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1b6d)
    {
        guard << 522;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1bc3)
    {
        guard << 539;
        guard << *(int*)(data + 18);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f40)
    {
        guard << 503;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x1f4a)
    {
        guard << 528;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2328)
    {
        guard << 435;
        guard << *(int*)(data + 15);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2711)
    {
        guard << 608;
        guard << -1;
        guard->put_binary(data, 270);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x2712)
    {
        // ORIG je 847a31b：cMyTrace(0xdbf,5) "TOD: bug random select" 后直返。
        cMyTrace trace(__PRETTY_FUNCTION__, 0xdbf, 5);
        trace("TOD: bug random select");
    }
        else if (sig == 0x2718)
    {
        guard << 677;
        guard << *(int*)(data + 10);
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x271b)
    {
        guard << 680;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27db)
    {
        guard << 711;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27e2)
    {
        guard << 759;
        guard << -1;
        guard->put_binary(data, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        else if (sig == 0x27e9)
    {
        guard << 764;
        guard << -1;
        guard->put_binary(data, *(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    // ORIG 末尾 mov $1,%ebx → 恒返 1（_processUDPData 调用侧 test %al 判成功）。
    return 1;
}

// ---------------------------------------------------------------------------
// dispatchAuctionPacket —— 拍卖服务器（GA）回包分发，ORIG T 0x847ac3c
// （报告 docs/class_func_reports/CDispatchServerPacket/dispatchAuctionPacket.md）。
// category==0x13 → 委派 dispatchPointPacket；否则按 packetID 组 SIG 流投
// DISPATCH_Q（QUEUE_IDX 1）。恒返回 1（epilogue mov $0x1,%eax 实测）。
// ---------------------------------------------------------------------------
int CDispatchServerPacket::dispatchAuctionPacket(char* data)
{
    nsl::PACKET_HEADER* header = (nsl::PACKET_HEADER*)data;
    int packetId = (int)header->getPacketID();
    if ((int)header->getCategory() == 0x13)
    {
        dispatchPointPacket(data);
        return 1;
    }

    switch (packetId)
    {
    case 0:  // 拍卖服务注册回执（包+0xe 结果码）
    {
        int result = *(int*)(data + 0xe);
        if (result == 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x128f), true);
            guard << 0xf9;
            guard << -1;
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_result = 0;
            sig->m_flag = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else if (result == 2)
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        else if (result == 1)
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a3, "AUCTION REGIST SERVER ID DUPLICATE");
        }
        else
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a7, "AUCTION REGIST UNHANDLED ERROR");
        }
        break;
    }
    case 1:  // 服务关闭通知
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12b2), true);
        guard << 0xf9;
        guard << -1;
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_result = 0;
        sig->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（AG：两组价格来自包体）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12cd), true);
        guard << 0xf0;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_ASK_PRICE* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
        sig->m_result = 0;
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_itemIdx = *(int*)(data + 0x1a);
        for (int i = 0; i < 3; ++i)
        {
            sig->m_priceCur[i] = *(int*)(data + 0xe + (i + 4) * 4);
            sig->m_priceTop[i] = *(int*)(data + 0xa + (i + 8) * 4);
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（AG：含 8×int 明细）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12f0), true);
        guard << 0xf1;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_count = *(unsigned short*)(data + 0x1a);
        sig->m_flagA = *(unsigned char*)(data + 0x1c);
        sig->m_flagB = *(unsigned short*)(data + 0x1d);
        for (int i = 0; i < 8; ++i)
        {
            sig->m_itemNums[i] = *(int*)(data + 0x1f + i * 4);
        }
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 4:  // SIG_AUCTION_RESULT_REGIST
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1319), true);
        guard << 0xf2;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_REGIST* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_okFlag = data[0x1a];
        sig->m_errCode = data[0x1b];
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 5:  // SIG_AUCTION_RESULT_BIDDING（AG：无卖家名段）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1336), true);
        guard << 0xf3;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_BIDDING* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_BIDDING>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_money = *(int*)(data + 0x1a);
        sig->m_flagA = data[0x1e];
        sig->m_flagB = data[0x1f];
        sig->m_field0e = *(int*)(data + 0x20);
        sig->m_bidPrice = *(int*)(data + 0x24);
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x135a), true);
        guard << 0xf4;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_okFlag = data[0x1a];
        sig->m_errCode = data[0x1b];
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 7:  // 拍卖列表（条目 0x89 字节）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1377), true);
        guard << 0xf5;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        guard << *(unsigned int*)(data + 0x1b);
        int listBytes = (int)(*(unsigned short*)(data + 0x1f) * 0x89 + 2);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x1390, "onAUCTION_ITEM_LIST_AG, resize failed.");
        }
        else
        {
            unsigned short cnt = *(unsigned short*)(data + 0x1f);
            guard << cnt;
            guard.put_binary(data + 0x21, (int)(cnt * 0x89));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 8:  // 我的登记物品（条目 0x75 字节；AG：先写计数再拷贝）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13a3), true);
        guard << 0xf6;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        int listBytes = (int)((unsigned char)data[0x1b] * 0x75 + 1);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13bb, "onAUCTION_MY_REGISTED_ITEM_INFO_AG, resize failed.");
        }
        else
        {
            unsigned char cnt = (unsigned char)data[0x1b];
            guard << cnt;
            guard.put_binary(data + 0x1c, (int)(cnt * 0x75));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 9:  // 我的竞购信息（条目 0x7d 字节）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13ce), true);
        guard << 0xf7;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        int listBytes = (int)(*(unsigned short*)(data + 0x1b) * 0x7d + 2);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13e6, "onAUCTION_MY_BIDDING_INFO_AG, resize failed.");
        }
        else
        {
            unsigned short cnt = *(unsigned short*)(data + 0x1b);
            guard << cnt;
            guard.put_binary(data + 0x1d, (int)(cnt * 0x7d));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xb:  // SIG_AUCTION_LOG_MESSAGE_AG（消费方 cUserHistoryLog::Auction）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x142a), true);
        guard << 0xf8;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_LOG_MESSAGE_AG* sig =
            guard.GetInBuffer<SIG_AUCTION_LOG_MESSAGE_AG>();
        sig->m_type = (unsigned char)data[0x16];
        sig->m_key = (unsigned long long)*(unsigned int*)(data + 0x17) |
                     ((unsigned long long)*(unsigned int*)(data + 0x1b) << 32);
        sig->m_arg09 = *(unsigned int*)(data + 0x1f);
        sig->m_arg0d = *(int*)(data + 0x23);
        sig->m_arg11 = *(unsigned int*)(data + 0x27);
        sig->m_arg15 = (unsigned char)data[0x2b];
        sig->m_arg16 = *(int*)(data + 0x2c);
        sig->m_arg1a = *(unsigned short*)(data + 0x30);
        sig->m_arg1c = *(int*)(data + 0x32);
        sig->m_arg20 = (unsigned char)data[0x36];
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xc:  // 服务就绪通知（flag=1）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1447), true);
        guard << 0xf9;
        guard << -1;
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_flag = 1;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xd:  // 就绪探测回执：成功→通知；否则续约定时器
    {
        if (data[0x12] == 0)
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        else
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1464), true);
            guard << 0xf9;
            guard << -1;
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 1;
            sig->m_result = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        break;
    }
    case 0xe:  // SIG_AUCTION_RESULT_BUY_ITEM_APIECE（单件购买回执，sig 0x20d）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1485), true);
        guard << 0x20d;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_BUY_ITEM_APIECE* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_BUY_ITEM_APIECE>();
        sig->m_result = *(int*)(data + 0x16);
        sig->m_characNo = *(int*)(data + 0x1a);
        sig->m_flagA = data[0x1e];
        sig->m_flagB = data[0x1f];
        sig->m_arg0a = *(int*)(data + 0x20);
        sig->m_price = *(int*)(data + 0x24);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    default:
        LogManager::logFormat(
            1, "DispatchServerPacket.cpp",
            __PRETTY_FUNCTION__,
            0x149c, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
        break;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// dispatchPointPacket —— 点券拍卖（GP）回包分发，ORIG T 0x847c1fe
// （报告 docs/class_func_reports/CDispatchServerPacket/dispatchPointPacket.md）。
// 仅 category==0x13 时处理；按 packetID 组 SIG 流投 DISPATCH_Q，恒返回 1。
// 注：packetID 经 nsl::PACKET_HEADER::getPacketID 符号扩展（movsbl），
// 0x2718..0x271b 分支在 ORIG 中不可达，仍忠实保留。
// ---------------------------------------------------------------------------
int CDispatchServerPacket::dispatchPointPacket(char* data)
{
    nsl::PACKET_HEADER* header = (nsl::PACKET_HEADER*)data;
    int packetId = (int)header->getPacketID();
    if ((int)header->getCategory() != 0x13)
    {
        return 1;
    }

    switch (packetId)
    {
    case 0:  // 点券拍卖服务注册回执
    {
        int result = *(int*)(data + 0xe);
        if (result == 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x14b4), true);
            guard << 0xf9;
            guard << -1;
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_result = 1;
            sig->m_flag = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        if (result == 2)
        {
            Timer_CheckAuctionReady::RegistNextTimer();
            return 1;
        }
        if (result == 1)
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x14c8, "AUCTION REGIST SERVER ID DUPLICATE");
            return 1;
        }
        LogManager::logFormat(
            1, "DispatchServerPacket.cpp",
            __PRETTY_FUNCTION__,
            0x14cc, "AUCTION REGIST UNHANDLED ERROR");
        return 1;
    }
    case 1:  // 服务关闭通知
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x14d5), true);
        guard << 0xf9;
        guard << -1;
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_flag = 0;
        sig->m_result = 1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（PG：价格组清零）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x14e9), true);
        guard << 0xf0;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_ASK_PRICE* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
        sig->m_result = 1;
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_itemIdx = *(int*)(data + 0x1a);
        for (int i = 0; i < 3; ++i)
        {
            sig->m_priceCur[i] = 0;
            sig->m_priceTop[i] = 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（PG：不写明细数组）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1502), true);
        guard << 0xf1;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_count = *(unsigned short*)(data + 0x1a);
        sig->m_flagA = *(unsigned char*)(data + 0x1c);
        sig->m_flagB = *(unsigned short*)(data + 0x1d);
        sig->m_result = 1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 4:  // SIG_AUCTION_RESULT_REGIST
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1523), true);
        guard << 0xf2;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_REGIST* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_okFlag = data[0x1a];
        sig->m_errCode = data[0x1b];
        sig->m_result = 1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 5:  // SIG_AUCTION_RESULT_BIDDING（PG：附卖家名与尾字段）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1536), true);
        guard << 0xf3;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_BIDDING* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_BIDDING>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_money = *(int*)(data + 0x1a);
        sig->m_flagA = data[0x1e];
        sig->m_flagB = data[0x1f];
        sig->m_field0e = *(int*)(data + 0x20);
        sig->m_bidPrice = *(int*)(data + 0x24);
        sig->m_result = 1;
        strncpy(sig->m_sellerName, data + 0x28, strlen(data + 0x28));
        sig->m_field36 = *(int*)(data + 0x48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x154e), true);
        guard << 0xf4;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
            guard.GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
        sig->m_characNo = *(int*)(data + 0x16);
        sig->m_okFlag = data[0x1a];
        sig->m_errCode = data[0x1b];
        sig->m_result = 1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 7:  // 拍卖列表 PG（条目 0x89 字节）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1561), true);
        guard << 0xf5;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        guard << *(unsigned int*)(data + 0x1b);
        int listBytes = (int)(*(unsigned short*)(data + 0x1f) * 0x89 + 2);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x1579, "onAUCTION_ITEM_LIST_PG, resize failed.");
        }
        else
        {
            unsigned short cnt = *(unsigned short*)(data + 0x1f);
            guard << cnt;
            guard.put_binary(data + 0x21, (int)(cnt * 0x89));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 8:  // 我的登记物品 PG（条目 0x75 字节；PG：先 1 字节计数再拷贝）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1584), true);
        guard << 0xf6;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        int listBytes = (int)((unsigned char)data[0x1b] * 0x75 + 1);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x159b, "onAUCTION_MY_REGISTED_ITEM_INFO_PG, resize failed.");
        }
        else
        {
            guard.put_binary(data + 0x1b, 1);
            guard.put_binary(data + 0x1c, (int)((unsigned char)data[0x1b] * 0x75));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 9:  // 我的竞购信息 PG（条目 0x7d 字节）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x15a6), true);
        guard << 0xf7;
        guard << *(int*)(data + 0x12);
        guard << *(int*)(data + 0x16);
        guard << (unsigned char)data[0x1a];
        int listBytes = (int)(*(unsigned short*)(data + 0x1b) * 0x7d + 2);
        Stream* stream = guard.Get();
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            stream->resize(stream->in_ptr() + listBytes);
        }
        if (stream->size() < stream->in_ptr() + listBytes)
        {
            guard << 0;
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x15bd, "onAUCTION_MY_BIDDING_INFO_PG, resize failed.");
        }
        else
        {
            unsigned short cnt = *(unsigned short*)(data + 0x1b);
            guard << cnt;
            guard.put_binary(data + 0x1d, (int)(cnt * 0x7d));
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0xa:  // ORIG 静默忽略（if(id<0xb) return 1）
        return 1;
    case 0xb:  // SIG_AUCTION_LOG_MESSAGE_PG（消费方 cUserHistoryLog::AuctionPoint）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x15ce), true);
        guard << 0xfa;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_LOG_MESSAGE_PG* sig =
            guard.GetInBuffer<SIG_AUCTION_LOG_MESSAGE_PG>();
        sig->m_type = (unsigned char)data[0x16];
        sig->m_key = (unsigned long long)*(unsigned int*)(data + 0x17) |
                     ((unsigned long long)*(unsigned int*)(data + 0x1b) << 32);
        sig->m_arg09 = *(unsigned int*)(data + 0x1f);
        sig->m_arg0d = *(int*)(data + 0x23);
        sig->m_arg11 = *(unsigned int*)(data + 0x27);
        sig->m_arg15 = (unsigned char)data[0x2b];
        sig->m_arg16 = *(int*)(data + 0x2c);
        sig->m_arg1a = *(unsigned short*)(data + 0x30);
        sig->m_arg1c = *(int*)(data + 0x32);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0xc:  // 服务就绪通知（flag=1）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x15e5), true);
        guard << 0xf9;
        guard << -1;
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_flag = 1;
        sig->m_result = 1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0xd:  // 就绪探测回执：非零→通知；零→续约定时器
    {
        if (data[0x12] != 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15fa), true);
            guard << 0xf9;
            guard << -1;
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard.GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_result = 1;
            sig->m_flag = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        Timer_CheckAuctionReady::RegistNextTimer();
        return 1;
    }
    case 0xe:  // SIG_AUCTION_OWNER_IS_VIP（sig 0x2da）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1646), true);
        guard << 0x2da;
        guard << *(int*)(data + 0x12);
        SIG_AUCTION_OWNER_IS_VIP* sig =
            guard.GetInBuffer<SIG_AUCTION_OWNER_IS_VIP>();
        sig->m_characNo = *(int*)(data + 0x12);
        sig->m_arg04 = *(int*)(data + 0x16);
        sig->m_arg08 = *(int*)(data + 0x1a);
        sig->m_flagC = data[0x1e];
        sig->m_arg0d = *(int*)(data + 0x1f);
        sig->m_arg11 = *(int*)(data + 0x23);
        strcpy(sig->m_ownerName, data + 0x27);
        sig->m_arg22 = *(int*)(data + 0x34);
        sig->m_arg26 = *(int*)(data + 0x38);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0x2718:  // 转发类分支（ORIG 不可达但保留）：sig 0x2a5
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1613), true);
        guard << 0x2a5;
        guard << *(int*)(data + 10);
        guard.put_binary(data, (int)*(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0x2719:  // sig 0x2a6
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1620), true);
        guard << 0x2a6;
        guard << *(int*)(data + 0xf);
        guard.put_binary(data, (int)*(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0x271a:  // sig 0x2a7
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x162c), true);
        guard << 0x2a7;
        guard << *(int*)(data + 10);
        guard.put_binary(data, (int)*(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    case 0x271b:  // sig 0x2a8
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1638), true);
        guard << 0x2a8;
        guard << -1;
        guard.put_binary(data, (int)*(unsigned short*)(data + 2));
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    default:
        LogManager::logFormat(
            1, "DispatchServerPacket.cpp",
            __PRETTY_FUNCTION__,
            0x166f, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
        break;
    }
    return 1;
}
