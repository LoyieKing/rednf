int DisPatcher_MoveMap::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return LineFunc(0x241,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
