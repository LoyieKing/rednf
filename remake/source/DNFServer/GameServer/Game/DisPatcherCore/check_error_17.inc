int DisPatcher_SetPos::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081c9994：get_state(user)==3 → 0，否则 0x13（setne 形式）。
    if (user->get_state() != 3)
        return 0x13;
    return 0;
}
