// ORIG 0x8403ab8：无基类 dispatch 前置检查（同 DB_SaveSkill），
// GetOutBuffer<SIG_INSERT_TICKET> → InsertTicket 通过返 1。
bool DB_InsertTicket::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_INSERT_TICKET* sig;
    sig = stream->GetOutBuffer<SIG_INSERT_TICKET>();
    if (sig == 0)
        return 0;
    r = InsertTicket(sig);
    if (!r)
        return 0;
    return 1;
}