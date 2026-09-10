bool DB_LoadRewardMedal::dispatch(int a, int b, Stream* stream)
{
    if (!DBDispatcher::dispatch(a, b, stream))
    {
        return false;
    }
    _SIG_LOAD_REWARD_MEDAL* sig = stream->GetOutBuffer<_SIG_LOAD_REWARD_MEDAL>();
    if (sig == 0)
    {
        return false;
    }
    if (Select(*sig))
    {
        return SendResult(a, b, *sig);
    }
    return false;
