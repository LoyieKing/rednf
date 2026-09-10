int Dispatcher_UpgradeAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8224fb0：state<=2 → 0；否则 CAccountCargo::UpgradeAccountCargo(user)（静态）。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    CAccountCargo::UpgradeAccountCargo(user);
    return 0;
