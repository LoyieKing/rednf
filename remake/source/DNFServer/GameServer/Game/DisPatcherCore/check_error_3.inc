int DisPatcher_GetUserInfo::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081c3e7c：get_state(user)==0 → -1，否则 0。
    if (user->get_state() == 0)
        return -1;
    return 0;
}
