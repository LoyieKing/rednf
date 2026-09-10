int DisPatcher_LeaveParty::check_state(CUser* user, PacketBuf& /*packet*/)

{
    if (user->get_state() != 3)
    {
        return LineFunc(0x208,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
