int CAssaultPlace::_SendCloseAssault(CUser* user, int tick)
{
    // ORIG 0x82e7b2a：`test eax;setne al;test al;je bb1` 判 R!=0；
    // 两路径统一 `mov 0x10(%ebp),%eax` ⇒ 恒返回 tick（钳位直写参数槽）。
    // N96 清绑：call 结果直比出 setne 物化（P197④）。
    if (user->getCurCharacR() != 0)
    {
        if (tick < 0x1e)
            tick = 0x1e;
        {
            PacketGuard packet;
            _MakeCloseAssault(&packet, tick);
            user->Send(packet);
        }
    }
    return tick;
