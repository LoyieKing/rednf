// nsl::IHandler.cpp
#include "IHandler.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "TCPSendThread.h"

namespace nsl {

IHandler::~IHandler()
{
}

void IHandler::init()
{
    setSendThread(pApp->super_Threads.getTCPSendThread());
    pTimeHandler = pApp->super_IHandlers.getTimeHandler(0);
}

void IHandler::setSendThread(TCPSendThread* pSendThread)
{
    sendTCP_ = pSendThread;
}

} // namespace nsl
