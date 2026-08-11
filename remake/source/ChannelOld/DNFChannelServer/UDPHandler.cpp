#include "ChannelServiceApp.h"

DWORD tagPacketHeader::getPacketID()
{
    return msg_no;
}

ChannelServiceApp::UDPHandler::UDPHandler()
{
}

ChannelServiceApp::ChannelService::FPMessageHandlerExtra ChannelServiceApp::ChannelService::GetMessageHandlerExtra(int n)
{
    if ((n < 0) || (0x1ff < n))
    {
        return NULL;
    }
    return m_pfnMsgExtra[n];
}

ChannelServiceApp::UDPHandlerRelay::UDPHandlerRelay()
{
}

void ChannelServiceApp::UDPHandlerRelay::dispatch(char* szBlock, int nSize, int Catagory)
{
    LPPACKET_HEADER pPCK = (LPPACKET_HEADER)szBlock;
    int nProtoID = pPCK->getPacketID();
    DWORD ret = 0;
    ChannelService::FPMessageHandlerExtra Handler = TManager<ChannelService>::getManager()->GetMessageHandlerExtra(nProtoID);
    if (Handler != NULL)
    {
        ret = (TManager<ChannelService>::getManager()->*Handler)(pPCK);
        if (ret == 0)
        {
            gLogInfo << "ERROR :" << __FUNCTION__ << "ID" << nProtoID << endl;
        }
    }
    else
    {
        gFileLogInfo.Lock();
    }
    gFileLogInfo << "ERROR : Not find Handler ID : " << nProtoID << endl;
    gFileLogInfo.Unlock();
}
