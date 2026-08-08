// nsl::TCPDispatcher.cpp
#include "TCPDispatcher.h"
#include "IHandlers.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "Message.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "PacketDesign.h"

namespace nsl {

TCPDispatcher::TCPDispatcher()
{
    MaxCategory = 100000;
    NullHandler = NULL;
}

void TCPDispatcher::SetMaxCategory(int Category)
{
    MaxCategory = Category;
}

void TCPDispatcher::SetNullSessionHandler(NullSessionHandler* handler)
{
    NullHandler = handler;
}

bool TCPDispatcher::dispatch(TCPUser* u, Message* msg)
{
    CMsgCell* pMsg = msg->getCellFromMessage();
    PACKET_HEADER* pPCK = pMsg->GetPacket();
    unsigned int nProtoID = pPCK->getPacketID();
    unsigned int nCategory = pPCK->getCategory();
    int ret = 0;
    u->setLastAccessTime();
    if ((nCategory == 0xffffffff) && (nProtoID == 0))
    {
        return false;
    }
    else if (((int)nCategory < 0) || ((int)nCategory >= MaxCategory))
    {
        return true;
    }
    else
    {
        INetWorkHandler* netWorkHandler = pApp->super_IHandlers.getNetWorkHandler(nCategory);
        if (netWorkHandler == NULL)
        {
            unsigned char* adrs = u->pSock_->getPeerAdrs();
            G_TraceLog()->sysLog(7, "NetWorkHandler is NULL. category=%d, ip=%d.%d.%d.%d", nCategory, adrs[0], adrs[1], adrs[2], adrs[3]);
            return true;
        }
        else
        {
            INetWorkHandler::networkFuncType handle = netWorkHandler->searchNetworkFunc(nProtoID);
            if (handle == NULL)
            {
                G_TraceLog()->sysLog(7, "TCP : could not find handler for Category :'%d' Protocol: '%d'.", nCategory, nProtoID);
                return true;
            }
            ret = (netWorkHandler->*handle)(pMsg, u);
            if (ret == 0)
            {
                return false;
            }
            G_TraceLog()->sysLog(7, "TCP : protocol : %d , Error: %d", nProtoID, ret);
            return true;
        }
    }
}

} // namespace nsl
