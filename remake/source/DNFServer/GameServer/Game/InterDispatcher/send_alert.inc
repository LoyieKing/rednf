void Inter_HadesAction::send_alert(CUser* user, unsigned int code)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0, 0x1bf);
    guard.put_int(static_cast<int>(code));
    guard.finalize(true);
    user->Send(guard);
