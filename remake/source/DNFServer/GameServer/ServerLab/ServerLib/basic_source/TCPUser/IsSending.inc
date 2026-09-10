bool TCPUser::IsSending() const
{
    if (mPendingSend)
    {
        return true;
    }
    return mbSending;
