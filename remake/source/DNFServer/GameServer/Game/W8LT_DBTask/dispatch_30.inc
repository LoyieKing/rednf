bool DB_UpdateGrowType::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_UPDATE_GROWTYPE* sig;
    sig = stream->GetOutBuffer<SIG_UPDATE_GROWTYPE>();
    if (sig == 0)
        return 0;
    r = UpdateGrowType(sig);
    if (!r)
        return 0;
    return 1;
