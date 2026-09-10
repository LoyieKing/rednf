bool DB_StatBreakJar::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→msg 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    MSG_STAT_BREAKJAR* msg;
    msg = stream->GetOutBuffer<MSG_STAT_BREAKJAR>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
