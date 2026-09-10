int Dispatcher_CreateAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08224f7c：state<=2 → 0；否则 CAccountCargo::CreateAccountCargo(user)（静态）。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    CAccountCargo::CreateAccountCargo(user);
    return 0;
