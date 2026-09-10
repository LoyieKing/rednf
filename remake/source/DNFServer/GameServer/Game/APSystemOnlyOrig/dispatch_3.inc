bool DB_UpdateRewardMedal::dispatch(int a, int b, Stream* stream)
{
    (void)a;
    (void)b;
    _SIG_UPDATE_REWARD_MEDAL* sig =
        stream->GetOutBuffer<_SIG_UPDATE_REWARD_MEDAL>();
    // ORIG: sig==0 → return 0 岛；else Update 后 return 1。
    // `return sig!=0` 会差成末段 setne 物化（P198②）。
    if (sig == 0)
        return 0;
    Update(*sig);
    return 1;
