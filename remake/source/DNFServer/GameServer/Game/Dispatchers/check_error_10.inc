int Dispatcher_BingoQuiz::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    // ORIG 0x0825ead0/0x0825eadc：|| 双比较 bool 物化（cmpl je / test jne + mov$1/$0 merge），
    // 命中返回 -1（0x0825eaf5 mov $0xffffffff）。
    if (user == 0 || user->getCurCharacR() == 0)
        return -1;
    // ORIG 0x0825eb0a：cmp $3 + setne 物化。
    if (user->get_state() != 3)
        return 0x1e;
    // ORIG 0x0825eb30：vtable+0x34 虚调 IsEventing(0)，!bool → xor$1 0x0825eb44。
    // 表达式内联（无具名槽）：ORIG eax 跨 vtable 计算不落栈（0x0825eb2b-eb42）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
        return 0x13;
    // ORIG 0x0825eb5d/0x0825eb72：两次独立 getBingoData call + movzbl 单级 +5/+4。
    if (user->m_bingo.m_isWin)
        return 0x12;                                   // ORIG test %al; je 真值判
    if (user->m_bingo.m_pad4 > 2)
        return 0x16;                                   // ORIG cmp $2,%al; setg
    return 0;                                          // ORIG 0x0825eb8e mov $0 直返
}