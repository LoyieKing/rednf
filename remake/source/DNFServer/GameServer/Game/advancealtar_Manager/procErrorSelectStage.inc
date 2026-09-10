void Manager::procErrorSelectStage(CUser* user, ENUM_ERROR err)
{
    if (user == 0 || err == 0)
    {
        return;
    }

    PacketGuard guard;
    guard.put_header(1, 0x10);
    guard.put_byte(0);
    guard.put_int(static_cast<int>(err));
    guard.finalize(true);
    user->Send(guard);
