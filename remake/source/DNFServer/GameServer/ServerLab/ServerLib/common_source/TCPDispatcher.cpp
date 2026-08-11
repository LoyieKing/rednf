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
    int nProtoID = pPCK->getPacketID();
    int nCategory = pPCK->getCategory();
    int ret = 0;
    u->setLastAccessTime();
    if ((nCategory == 0xffffffff) && (nProtoID == 0))
    {
        return false;
    }
    else if (nCategory < 0 || nCategory >= MaxCategory)
    {
        return true;
    }
    else
    {
        INetWorkHandler* netWorkHandler = pApp->super_IHandlers.getNetWorkHandler(nCategory);
        if (netWorkHandler != NULL)
        {
            INetWorkHandler::networkFuncType handle = netWorkHandler->searchNetworkFunc(nProtoID);
            if (handle != NULL)
            {
                ret = (pApp->super_IHandlers.getNetWorkHandler(nCategory)->*handle)(pMsg, u);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7, "TCP : '%d' \xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. Error: %d", nProtoID, ret);
                    return true;
                }
            }
            else
            {
                G_TraceLog()->sysLog(7, "TCP : could not find handler for Category :'%d' Protocol: '%d'.", nCategory, nProtoID);
                return true;
            }
        }
        else
        {
            G_TraceLog()->sysLog(7,
                "TCP : '%d' <- protocol category \xb0\xa1 \xb9\xfc\xc0\xa7\xb8\xa6 \xb9\xfe\xbe\xee\xb3\xb5\xbd\xc0\xb4\xcf\xb4\xd9, ip:%d.%d.%d.%d",
                nCategory, (int)u->pSock_->getPeerAdrs()[0],
                (int)u->pSock_->getPeerAdrs()[1], (int)u->pSock_->getPeerAdrs()[2],
                (int)u->pSock_->getPeerAdrs()[3]);
            return true;
        }
        return false;
    }
}

} // namespace nsl
