void TCPUser::DecPendingWorkNum()
{
    if (mPendingWork != 0)
    {
        mPendingWork = mPendingWork - 1;
    }
