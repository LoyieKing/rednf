int Dispatcher_BingoReward::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    // ORIG 0x0825e7b8/0x0825e7c4：|| 双比较 bool 物化，命中返回 -1（0x0825e7dd）。
    if (user == 0 || user->getCurCharacR() == 0)
        return -1;
    // ORIG 0x0825e7f2：cmp $3 + setne 物化。
    if (user->get_state() != 3)
        return 0x1e;
    // ORIG 0x0825e818：vtable+0x34 虚调 IsEventing(0)，!bool → xor$1 0x0825e82c。
    // 表达式内联（无具名槽）：ORIG eax 跨 vtable 计算不落栈（0x0825e813-eb2a）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
        return 0x13;
    // ORIG 0x0825e845/0x0825e85a：两次独立 getBingoData call + movzbl 单级 +5/+4。
    if (static_cast<BingoCheckView*>(user->getBingoData())->m_b5)
        return 0x12;                                   // ORIG test %al; je 真值判
    if (static_cast<BingoCheckView*>(user->getBingoData())->m_b4 <= 2)
        return 0x16;                                   // ORIG cmp $2,%al; setle
    return 0;                                          // ORIG 尾 mov $0 直返
