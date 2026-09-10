int Dispatcher_Request_PvpExpOfWeek::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    PacketGuard pg;
    pg.put_header(0, 0x39);
    pg.put_int(user->get_pvp_exp_of_week());
    pg.finalize(true);
    user->Send(pg);
    return 0;
