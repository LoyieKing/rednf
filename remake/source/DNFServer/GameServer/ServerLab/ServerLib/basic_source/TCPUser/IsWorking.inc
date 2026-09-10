bool TCPUser::IsWorking() const
{
    if (mPendingWork)
    {
        return true;
    }
    return mbWorking;
