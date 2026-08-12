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
    PacketHeaderS2S* pkt = (PacketHeaderS2S*)buf;
    switch (pkt->m_a)
    {
    case 1:
        return;
    case 0:
        {
            PacketHeaderS2S* q = (PacketHeaderS2S*)buf;
            getManager()->setAuthenticated(q->m_f);
        }
        return;
    case 0x9c4:
        {
            PacketHeaderS2S* p = (PacketHeaderS2S*)buf;
            if (p->m_g == 0)
            {
                getManager()->postDisconnectEvent2TCPUser(p->m_f, 3);
            }
        }
    }
}



} // namespace RelayServiceApp
