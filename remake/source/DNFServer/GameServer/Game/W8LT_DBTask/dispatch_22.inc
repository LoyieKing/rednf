bool DB_SaveRank::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_SAVE_RANK* sig;
    sig = stream->GetOutBuffer<SIG_SAVE_RANK>();
    if (sig == 0)
        return 0;
    r = SaveRankAll(sig);
    if (!r)
        return 0;
    return 1;
