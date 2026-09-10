int DisPatcher_SetPVPReady::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081c9b9c：`cmp $6; setne; test; je` = if(!=6) return 0x13; return 0。
    if (user->get_state() != 6)
        return 0x13;
    return 0;
}
