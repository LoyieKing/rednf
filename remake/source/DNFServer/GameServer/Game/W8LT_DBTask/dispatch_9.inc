// ORIG 0x84177da：GetOutBuffer<SIG_LOAD_SKILL> → SaveSkill 全绿返 1
// （无基类 dispatch 前置检查，与 DB_LoadTicket::dispatch 不同）。
bool DB_SaveSkill::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_LOAD_SKILL* sig;
    sig = stream->GetOutBuffer<SIG_LOAD_SKILL>();
    if (sig == 0)
        return 0;
    r = SaveSkill(sig);
    if (!r)
        return 0;
    return 1;
}