int DisPatcher_GiveUpGame::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 5 && user->get_state() != 10
        && user->get_state() != 0xc && user->get_state() != 0xd)
    {
        return 0x13;
    }
    return 0;
