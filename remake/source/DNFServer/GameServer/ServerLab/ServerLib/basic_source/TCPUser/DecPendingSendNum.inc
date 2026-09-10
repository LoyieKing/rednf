void TCPUser::DecPendingSendNum()
{
    if (mPendingSend != 0)
    {
        mPendingSend = mPendingSend - 1;
    }
