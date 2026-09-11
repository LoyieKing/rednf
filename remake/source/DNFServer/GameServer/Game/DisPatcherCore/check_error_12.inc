// ORIG 0x081c9d64 T _ZN19DisPatcher_SortItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_SortItem/check_error.md 逐块映射）：user 空 →
// LineFunc(0x1d56)；state<=2 → -1；否则 0。
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
