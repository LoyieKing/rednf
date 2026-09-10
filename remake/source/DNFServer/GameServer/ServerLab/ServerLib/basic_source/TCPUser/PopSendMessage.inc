Message* TCPUser::PopSendMessage()
{
    Message* pMessage;
    if (!wouldBlockQueue.empty())
    {
        pMessage = wouldBlockQueue.front();
        wouldBlockQueue.pop_front();
    }
    else
    {
        pMessage = mSendMessageQueue.front();
        mSendMessageQueue.pop_front();
    }
    return pMessage;
