// df_relay_r — TCPThread（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <string.h>
#include "TCPThread.h"
#include "RelayService.h"
#include "TCPHandler.h"
#include "Helper.h"
#include "ScriptData.h"

namespace RelayServiceApp
{
TCPThread::TCPThread()
{
    m_port = 0;
    m_handler = 0;
}

TCPThread::~TCPThread()
{
}

void TCPThread::loop(void* pParam)
{
    unsigned short port = (unsigned short)getPort();
    printf("In %s : port='%d'\n", __FUNCTION__, (unsigned int)port);
    TReactor<EpollReactor<TCPUser, TCPSocket, TCPSocket>, TCPUser, TCPSocket, TCPSocket> *reactor =
        (TReactor<EpollReactor<TCPUser, TCPSocket, TCPSocket>, TCPUser, TCPSocket, TCPSocket> *)
            getManager()->m_reactor.getReactor();
    reactor->init(G_ScriptData()->mRelayNum + 0x69);
    reactor->startup();
    reactor->setManagerToEpoll(getManager());
    TCPSocket listenSocket;
    if (!listenSocket.open())
    {
        printf("listenSocket.open : ERROR(%s)\n", strerror(*__errno_location()));
    }
    if (G_ScriptData()->mReservedA != 0)
    {
        puts("Set Relay Server for PVP Channel");
        listenSocket.setOptNagle(false);
    }
    if (!listenSocket.bind(port, false))
    {
        printf("listenSocket.bind : ERROR(%s)\n", strerror(*__errno_location()));
    }
    if (!listenSocket.listen(100))
    {
        printf("listenSocket.listen : ERROR(%s)\n", strerror(*__errno_location()));
    }
    reactor->handleEvents(2000, listenSocket, 7);
    reactor->shutdown();
    setTerminated();
}


} // namespace RelayServiceApp
