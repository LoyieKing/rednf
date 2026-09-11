int Dispatcher_RegisterToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cdc70：仅 get_state()<=2 → -1（无 user 空指针检查）。
    if (user->get_state() <= 2)
    {
        return -1;
    }
    return 0;
}