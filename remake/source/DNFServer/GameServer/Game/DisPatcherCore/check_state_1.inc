int DisPatcher_DeleteCharac::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 2)
    {
        return LineFunc(0x170,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
