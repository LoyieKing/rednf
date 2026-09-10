void CUser::SendLimitItemUsageNotiInfo(int a, int b, int c)
{
    PacketGuard guard;
    guard.put_header(0, 0x11f);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(c);
    guard.finalize(true);
    Send(guard);
