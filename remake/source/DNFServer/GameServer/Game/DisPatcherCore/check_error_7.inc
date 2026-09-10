int DisPatcher_WalkoutParty::check_error(CUser* user, MSG_BASE& msg)

{
    if (user == 0)
    {
        return LineFunc(0x2053,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081cb062：`cmp $3; setne; test; je` = if(!=3) return -1; return 0。
    if (user->get_state() != 3)
        return 0xffffffff;
    return 0;
}
