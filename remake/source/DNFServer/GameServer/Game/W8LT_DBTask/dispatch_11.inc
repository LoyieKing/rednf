bool DB_DeleteChannel::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_DELETE_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_DELETE_CHANNEL>();
    if (sig == 0)
        return 0;
    r = DeleteChannel(sig);
    if (!r)
        return 0;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
