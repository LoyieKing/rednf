// ORIG 0x84036b6：基类 dispatch 放行后 GetOutBuffer<SIG_INSERT_CHANNEL>，
// InsertChannel 通过后直接返 1（无 SendResult）。
bool DB_InsertChannel::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_INSERT_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_INSERT_CHANNEL>();
    if (sig == 0)
        return 0;
    r = InsertChannel(sig);
    if (!r)
        return 0;
    return 1;
}