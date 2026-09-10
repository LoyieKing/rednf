int Dispatcher_CallPartyMemberRealtimeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return LineFunc(0x2781,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG：cmp $3 + setne 物化（技巧9），!=3 → -1；尾 mov$0 岛。
    if ((bool)(user->get_state() != 3))
    {
        return -1;
    }
    return 0;
