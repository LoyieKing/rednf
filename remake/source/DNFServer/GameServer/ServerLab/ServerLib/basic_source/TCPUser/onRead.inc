int TCPUser::onRead()
{
    G_TraceLog()->sysLog(8, "S=======================================================================[%d]", mUserId);
    int nRead = 0;
    int size = mRecvBuffer.AvailableSize();
    if (0 < size)
    {
        nRead = pSock_->recv(mRecvBuffer.GetRear(), size);
        if (0 < nRead)
        {
            G_TraceLog()->sysLog(4, "available size : %d, nRead %d\n", size, nRead);
            mRecvBuffer.AdjustRear(nRead);
            if (!mRecvBuffer.Parse(this))
            {
                postDisconnected(1);
            }
        }
        else
        {
            if (nRead == -1)
            {
                postDisconnected(0);
            }
        }
    }
    return nRead;
