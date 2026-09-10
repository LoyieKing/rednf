void AttendanceMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                       unsigned int /*param*/)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08166002 条件 1 为 `limit <= field4`
    // （cmp %ax,%dx; setbe; jne 跳尾 return）；tm 槽序 today=-0x40 浅于
    // target=-0x6c ⇒ 4.4「先声明浅」需 today 先声明。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    {
        struct tm today;       // ORIG -0x40
        struct tm target;      // ORIG -0x6c
        time_t cur;            // ORIG -0x14
        time_t todaySec;       // ORIG -0x10
        time_t targetSec;      // ORIG -0xc
        cur = GlobalData::s_systemTime_.getCurSec();
        localtime_r(&cur, &today);
        localtime_r(&cur, &target);
        today.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        today.tm_min = 0;
        today.tm_sec = 0;
        todaySec = mktime(&today);
        target.tm_mon = m_condition.m_extra[1] - 1;
        target.tm_mday = m_condition.m_extra[2] + value.m_field8;
        target.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        target.tm_min = 0;
        target.tm_sec = 0;
        targetSec = mktime(&target);
        if (0x15180 < todaySec - targetSec)
        {
            value.reset();
        }
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816613e 条件 3 为
        // `limit <= field4+1`（cmp %eax,%edx; setle; je 跳 else 累加块）。
        if (m_condition.m_extra[0] <= value.m_field4 + 1)
        {
            value.m_field4 = m_condition.m_extra[0];
        }
        else
        {
            value.m_field4 = value.m_field4 + 1;
        }
        value.m_field8 = value.m_field8 +
            static_cast<unsigned short>((todaySec - targetSec) / 0x15180);
    }
