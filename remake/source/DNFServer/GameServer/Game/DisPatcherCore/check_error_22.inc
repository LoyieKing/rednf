unsigned int DisPatcher_UseCoin::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    if (user->get_state() == 5)
    {
        return Dungeon_check_error(user, msg, param);
    }
    if (user->get_state() == 8)
    {
        return WarRoom_check_error(user, msg, param);
    }
    return 0x13;
}
