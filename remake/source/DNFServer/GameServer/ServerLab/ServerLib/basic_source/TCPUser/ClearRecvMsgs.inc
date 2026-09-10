void TCPUser::ClearRecvMsgs()
{
    while (!mRecvBuffer.ClearUsedMsgs())
    {
    }
