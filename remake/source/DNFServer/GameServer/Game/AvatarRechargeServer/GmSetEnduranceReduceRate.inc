void AvatarRechargeServer::GmSetEnduranceReduceRate(CUser* user, bool enable,
                                                    int rate)
{
    // [b28c-L9] ORIG 0x0819136a `mov 0x10(%ebp),%eax; mov %al,-0x1c(%ebp)` 是 bool
    // 形参的单次截断 spill（非具名局部——再加 `bool bEn` 会多一对
    // `movzbl -0x1c,%eax; mov %al,-0x11` 中转）。
    m_bUseEnduranceReduceRate = enable;
    if (enable)
    {
        if (rate < 0)
            rate = 0;
        else if (rate > 100)
            rate = 1000;
        else
            rate *= 10;
        m_enduranceReduceRate = rate;

        // [b28c-L9] 旧「字面量直填 N 封顶」判词作废：ORIG 0x081913b6
        // `movl $str,-0x10(%ebp); mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`
        // 与 0x081913db 的 -0xc 同形 ⇒ 原码是具名 char* 局部（P0），
        // 两臂各一枚（-0x10 深 / -0xc 浅）。
        char* msgOn = (char*)"succ avatar Endurance reduce rate mode on! change rate";
        ARAD::notifySystemMessage(user, msgOn);
    }
    else
    {
        m_enduranceReduceRate = 0;
        char* msgOff = (char*)"avatar Endurance reduce rate mode off";
        ARAD::notifySystemMessage(user, msgOff);
    }
