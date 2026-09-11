// ORIG 0x8417fb4：基类 dispatch 前置检查后 GetOutBuffer<SIG_UPDATE_CHANNEL>，
// UpdateChannel 全绿返 1。
bool DB_UpdateChannel::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_UPDATE_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_UPDATE_CHANNEL>();
    if (sig == 0)
        return 0;
    r = UpdateChannel(sig);
    if (!r)
        return 0;
    return 1;
}