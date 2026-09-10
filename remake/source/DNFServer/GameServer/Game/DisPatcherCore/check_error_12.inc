int DisPatcher_SortItem::check_error(CUser* user, MSG_BASE& msg)

{
    if (user == 0)
    {
        return LineFunc(0x1d56,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() <= 2)
    {
        return 0xffffffff;
    }
    return 0;
}
