int Dispather_RegenerationRandomOption::check_state(CUser* user, PacketBuf& /*packet*/)

{
    // 报告缺失/推断：ORIG 0x08232984 W（nm+objdump 全文转写，无独立 md 报告）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x8a0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(0x8a4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
