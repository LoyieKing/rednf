int Dispatcher_WarRoomWpPerMonster::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081d5e1c：帧 0x18——pretty 直传无指针槽（具名 pretty 局部=
    // -0xc 槽膨胀，技巧36）；尾 state!=8 走 bool 物化 `setne;test;je`（技9/19）。
    if (user == 0)
    {
        return LineFunc(0x39af, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x39b2, __PRETTY_FUNCTION__, 0, 0);
    }
    if ((bool)(user->get_state() != 8))
    {
        return -1;
    }
    return 0;
}