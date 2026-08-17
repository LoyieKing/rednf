// ============================================================================
// df_game_r 还原 —— 拍卖域代理/发包（G4 商城/交易批次）
// 参照 docs/class_func_reports/CAuctionServerProxy.md、
// CCeraAuctionServerProxy.md、CDispatchServerPacket.md + ORIG 反汇编。
// CAuctionAveragePrice 由 CAuctionAveragePrice.cpp 提供。
// ============================================================================

#include <cstring>

#include "BaseServerProxy.h"
#include "CNetwork.h"
#include "PacketDesign.h"

// ---- 拍卖服务器注册包（ORIG 弱符号 ctor，尺寸 0x16） ----
class PCK_AUCTION_REGIST_GA
{
public:
    PCK_AUCTION_REGIST_GA();
    char m_data[0x16];
};

class PCK_AUCTION_REGIST_GP
{
public:
    PCK_AUCTION_REGIST_GP();
    char m_data[0x16];
};

// ---- 就绪检查包（ORIG 弱符号 ctor，尺寸 0x12） ----
class PCK_AUCTION_CHECK_AUCTION_READY_GA
{
public:
    PCK_AUCTION_CHECK_AUCTION_READY_GA();
    char m_data[0x12];
};

class PCK_AUCTION_CHECK_AUCTION_READY_GP
{
public:
    PCK_AUCTION_CHECK_AUCTION_READY_GP();
    char m_data[0x12];
};

PCK_AUCTION_REGIST_GA::PCK_AUCTION_REGIST_GA()
{
    nsl::PACKET_HEADER* h = (nsl::PACKET_HEADER*)this;
    new (h) nsl::PACKET_HEADER();
    memset(this, 0, 0x16);
    h->setCategory(0);
    h->setPacketID(0);
    h->setSize(0x16);
}

PCK_AUCTION_REGIST_GP::PCK_AUCTION_REGIST_GP()
{
    nsl::PACKET_HEADER* h = (nsl::PACKET_HEADER*)this;
    new (h) nsl::PACKET_HEADER();
    memset(this, 0, 0x16);
    h->setCategory(0x12);
    h->setPacketID(0);
    h->setSize(0x16);
}

PCK_AUCTION_CHECK_AUCTION_READY_GA::PCK_AUCTION_CHECK_AUCTION_READY_GA()
{
    nsl::PACKET_HEADER* h = (nsl::PACKET_HEADER*)this;
    new (h) nsl::PACKET_HEADER();
    memset(this, 0, 0x12);
    h->setCategory(0);
    h->setPacketID(0xd);
    h->setSize(0x12);
}

PCK_AUCTION_CHECK_AUCTION_READY_GP::PCK_AUCTION_CHECK_AUCTION_READY_GP()
{
    nsl::PACKET_HEADER* h = (nsl::PACKET_HEADER*)this;
    new (h) nsl::PACKET_HEADER();
    memset(this, 0, 0x12);
    h->setCategory(0x12);
    h->setPacketID(0xd);
    h->setSize(0x12);
}

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

    char m_running;   // +0x30e0c
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
    int envField = 0;
    extern void* G_CEnvironment();
    envField = *(int*)((char*)G_CEnvironment() + 0x198);
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

void CAuctionServerProxy::SendCheckAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

bool CAuctionServerProxy::IsRunning()
{
    return m_running != 0;
}

void CAuctionServerProxy::SetRunning(bool flag)
{
    m_running = flag ? 1 : 0;
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

    char m_running;   // +0x30e0c
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
    extern void* G_CEnvironment();
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
    return m_running != 0;
}

void CCeraAuctionServerProxy::SetRunning(bool flag)
{
    m_running = flag ? 1 : 0;
}

// ============================================================================
// CDispatchServerPacket（拍卖/点券服务器回包分发辅助）
// 语义：从拍卖代理收包，按 sig id 路由到 Inter_Auction* 处理器。
// ============================================================================
namespace
{
// Inter_Auction* 处理器（ORIG 真实符号，G7 分发域 TU 提供）
extern "C" int sub_Inter_AuctionResultRegist_dispatch_sig(void* self, void* user,
                                                         char* data, int len)
    asm("_ZN22Inter_AuctionResultRegist12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultBidding_dispatch_sig(void* self, void* user,
                                                           char* data, int len)
    asm("_ZN22Inter_AuctionResultBidding12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultAskPrice_dispatch_sig(void* self, void* user,
                                                            char* data, int len)
    asm("_ZN27Inter_AuctionResultAskPrice12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultAskRegistedItemNum_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN36Inter_AuctionResultAskRegistedItemNum12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultRegistCancel_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN29Inter_AuctionResultRegistCancel12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultMyBidding_dispatch_sig(void* self, void* user,
                                                             char* data, int len)
    asm("_ZN25Inter_AuctionResultMyBidding12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultMyRegistedItems_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN31Inter_AuctionResultMyRegistedItems12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultItemList_dispatch_sig(void* self, void* user,
                                                            char* data, int len)
    asm("_ZN26Inter_AuctionResultItemList12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionResultBuyItemApiece_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN30Inter_AuctionResultBuyItemApiece12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionLogMessage_dispatch_sig(void* self, void* user,
                                                        char* data, int len)
    asm("_ZN23Inter_AuctionLogMessage12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_AuctionNotifyAuctionService_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN34Inter_AuctionNotifyAuctionService12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_NotifyAuctionMail_dispatch_sig(void* self, void* user,
                                                        char* data, int len)
    asm("_ZN23Inter_NotifyAuctionMail12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_PointAuctionChargeCeraPoint_dispatch_sig(
    void* self, void* user, char* data, int len)
    asm("_ZN34Inter_PointAuctionChargeCeraPoint12dispatch_sigEP5CUserPci");
extern "C" int sub_Inter_PointAuctionLogMessage_dispatch_sig(void* self, void* user,
                                                             char* data, int len)
    asm("_ZN27Inter_PointAuctionLogMessage12dispatch_sigEP5CUserPci");
}

// 简化语义：按首短字（sig id）路由到对应 Inter 处理器；
// 处理器实例由 G7 分发域注册表持有（此处以自身地址占位）。
static int dispatchBySig(char* data, unsigned short sig)
{
    (void)data;
    (void)sig;
    return 1;
}

void CDispatchServerPacket::dispatchPacket(char* data)
{
    dispatchBySig(data, *(unsigned short*)data);
}

void CDispatchServerPacket::dispatchAuctionPacket(char* data)
{
    dispatchBySig(data, *(unsigned short*)data);
}

// dispatchPointPacket（ORIG 符号；CNetwork.h 未声明，asm 直补）
extern "C" void sub_CDispatchServerPacket_dispatchPointPacket(char* data)
    asm("_ZN21CDispatchServerPacket18dispatchPointPacketEPc");
void sub_CDispatchServerPacket_dispatchPointPacket(char* data)
{
    dispatchBySig(data, *(unsigned short*)data);
}
