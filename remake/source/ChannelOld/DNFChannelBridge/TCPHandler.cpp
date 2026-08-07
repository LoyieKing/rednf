#include "ChannelServiceApp.h"

ChannelServiceApp::TCPHandler::TCPHandler()
{
}

ChannelServiceApp::ChannelService::FPMessageHandler ChannelServiceApp::ChannelService::GetMessageHandler(int n)
{
    if ((n < 0) || (0x1ff < n))
    {
        return NULL;
    }
    return m_pfnMsg[n];
}

ChannelServiceApp::TCPHandlerRelay::TCPHandlerRelay()
{
}

ChannelServiceApp::TCPHandlerRelay::~TCPHandlerRelay()
{
}

void ChannelServiceApp::TCPHandlerRelay::dispatch(TCPUser* u, char* szBlock, int nSize, int Catagory)
{
    LPPACKET_HEADER pPCK = (LPPACKET_HEADER)szBlock;
    int nProtoID = pPCK->getPacketID();
    DWORD ret = 0;
    ChannelService::FPMessageHandler Handler = getManager()->GetMessageHandler(nProtoID);
    if (Handler != NULL)
    {
        ret = (getManager()->*Handler)(pPCK, u);
        u->setLastAccessTime();
    }
}
