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
    if (*(unsigned short*)pkt == 0)
    {
        getManager()->relayToTCP(pkt);
    }
}

UDPHandlerS2S::UDPHandlerS2S()
{
}

void UDPHandlerS2S::dispatch(char* buf, int size, int flag)
{
    short type = *(short*)buf;
    if (type != 1)
    {
        if (type == 0x9c4)
        {
            if (*(char*)(buf + 0xe) == 0)
            {
                getManager()->postDisconnectEvent2TCPUser(*(unsigned int*)(buf + 0xa), 3);
            }
        }
        else if (type == 0)
        {
            getManager()->setAuthenticated(*(unsigned int*)(buf + 0xa));
        }
    }
}



} // namespace RelayServiceApp
