// nsl::InterDispatcher.cpp
#include "InterDispatcher.h"
#include "IHandlers.h"
#include "Message.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "ServiceError.h"
#include "InternalMsgDesign.h"
#include "MsgCell.h"

namespace nsl {

InterDispatcher::InterDispatcher()
{
}

bool InterDispatcher::dispatch(Message* msg)
{
    CMsgCell* pMsg = msg->getCellFromMessage();
    INTERNALMSG_HEADER* pInternalMsgHeader = pMsg->GetInternalMsg();
    unsigned int nProtoId = pInternalMsgHeader->getInternalMsgID();
    unsigned int nCategory = pInternalMsgHeader->getCategory();
    int ret = 0;
    IInterHandler* pInterHandler = pApp->super_IHandlers.getInterHandler(nCategory);
    if (pInterHandler == NULL)
    {
        G_TraceLog()->sysLog(7, "InterDispatcher : category : %d", nCategory);
        return false;
    }
    IInterHandler::interFuncType handle = pInterHandler->searchInterFunc(nProtoId);
    if (handle == NULL)
    {
        G_TraceLog()->sysLog(7, "InterDispatcher : can't find handler for category: %d, protocol : %d.", nCategory, nProtoId);
        return false;
    }
    ret = (pInterHandler->*handle)(pMsg);
    if (ret == 0)
    {
        return true;
    }
    const char* errorStr = GetErrorStr(ret);
    G_TraceLog()->sysLog(7, "InterDispatcher : protocol: %d , Error: %s", nProtoId, errorStr);
    return false;
}

} // namespace nsl
