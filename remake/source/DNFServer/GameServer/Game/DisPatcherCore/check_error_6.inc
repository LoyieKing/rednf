unsigned int DisPatcher_LeaveParty::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081c436e：party 落局部槽 -0xc 后 `cmpl $0x0,-0xc`（非寄存器 test）。
    CParty* pParty = (CParty*)user->GetParty();
    if (pParty == 0)
        return 0x12;
    return 0;
}
