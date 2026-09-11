int Dispatcher_SeriaRoom_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e5e56：!user → -1（cmpl jne 正向跳）。
    if (user == 0)
    {
        return -1;
    }
    // ORIG 0x81e5e71：cmp $3 + setne 物化（技巧9），!=3 → 7；尾 mov$0 岛。
    if ((bool)(user->get_state() != 3))
    {
        return 7;
    }
    return 0;
}