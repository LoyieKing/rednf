int Dispatcher_PVPRequestFight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821c??：无 user==0；cmp $0x7; setne %al; test %al,%al; je（技巧9）。
    if ((bool)(user->get_state() != 7))
    {
        return LineFunc(0x9c25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x9c2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!room->request_fight(user))
    {
        return LineFunc(0x9c30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}