int DisPatcher_PvPTimeout::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c5ebe：`cmp $7; setne; test; je` = if(!=7) return 0x13; return 0。
    if (user->get_state() != 7)
        return 0x13;
    return 0;
