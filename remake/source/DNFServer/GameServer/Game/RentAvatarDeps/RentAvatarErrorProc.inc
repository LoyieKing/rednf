void RentAvatarErrorProc(CUser* user, int errCode)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0x1, 0x16);
    guard.put_byte(0);
    guard.put_byte(errCode);
    guard.finalize(true);
    user->Send(guard);
