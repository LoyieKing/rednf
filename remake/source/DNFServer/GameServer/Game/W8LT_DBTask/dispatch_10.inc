bool DB_LoadTicket::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_LOAD_TICKET* sig;
    sig = stream->GetOutBuffer<SIG_LOAD_TICKET>();
    if (sig == 0)
        return 0;
    if (!LoadTicket(sig))
        sig->m_field4 = 1;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
