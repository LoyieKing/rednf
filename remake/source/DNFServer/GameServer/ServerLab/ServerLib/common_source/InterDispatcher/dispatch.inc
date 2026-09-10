bool InterDispatcher::dispatch(Message* msg)
{
    CMsgCell* pMsg = msg->getCellFromMessage();
    INTERNALMSG_HEADER* pInternalMsgHeader = pMsg->GetInternalMsg();
    int nProtoId = pInternalMsgHeader->getInternalMsgID();
    int nCategory = pInternalMsgHeader->getCategory();
    int ret = 0;
    IInterHandler* pInterHandler = pApp->super_IHandlers.getInterHandler(nCategory);
    if (pInterHandler != NULL)
    {
        IInterHandler::interFuncType handle = pInterHandler->searchInterFunc(nProtoId);
        if (handle != NULL)
        {
            ret = (pApp->super_IHandlers.getInterHandler(nCategory)->*handle)(pMsg);
            if (ret != 0)
            {
                G_TraceLog()->sysLog(7, "InterDispatcher : protocol: %d , Error: %s", nProtoId, GetErrorStr(ret));
                return false;
            }
        }
        else
        {
            G_TraceLog()->sysLog(7, "InterDispatcher : can't find handler for category: %d, protocol : %d.", nCategory, nProtoId);
            return false;
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "InterDispatcher : category : %d", nCategory);
        return false;
    }
    return true;
