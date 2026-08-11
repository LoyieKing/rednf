// df_relay_r — UDPHandler/UDPHandlerRelay/UDPHandlerS2S（GCC 4.1.2, 无 DWARF）
#include "UDPHandler.h"
#include "UDPUser.h"
#include "RelayService.h"
#include "Helper.h"

namespace RelayServiceApp
{
UDPHandler::UDPHandler()
{
}

UDPHandlerRelay::UDPHandlerRelay()
{
}

UDPHandlerRelay::~UDPHandlerRelay()
{
}

void UDPHandlerRelay::dispatch(char* buf, int size, int flag)
{
    PacketHeader* pkt = (PacketHeader*)buf;
    switch (pkt->m_type)
    {
    case 0:
        getManager()->relayToTCP(pkt);
        break;
    }
}

UDPHandlerS2S::UDPHandlerS2S()
{
}

void UDPHandlerS2S::dispatch(char* buf, int size, int flag)
{
    // 语义还原（2026-08-11 用户规矩：不允许硬套 asm）。
    // ORIG 的 switch 判定链/块序/nop 落地无法用纯 C++ 逐字节复现，
    // 按规矩归入 caliber_issues.csv（REMAIN）。
    short type = *(short*)buf;
    if (type != 1)
    {
        if (type == 0x9c4)
        {
            PacketHeaderS2S* p = (PacketHeaderS2S*)buf;
            if (p->m_g == 0)
            {
                getManager()->postDisconnectEvent2TCPUser(p->m_f, 3);
            }
        }
        else if (type == 0)
        {
            PacketHeaderS2S* q = (PacketHeaderS2S*)buf;
            getManager()->setAuthenticated(q->m_f);
        }
    }
}



} // namespace RelayServiceApp
