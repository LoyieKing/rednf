void CEventCreateDnf::RegistStartTimer(bool immediate)
{
    (void)immediate;
    if (!IsEventing(0))
        return;
    // ORIG 0x08163e7c：SetEventInfo(0,0,0) → 按 startHour 计算今日起始时刻
    SetEventInfo(0, 0, 0);
    const std::time_t now = static_cast<std::time_t>(GlobalData::s_systemTime_.getCurSec());
    std::tm local;
    localtime_r(&now, &local);
    local.tm_hour = m_startHour;
    local.tm_min = 0;
    local.tm_sec = 0;
    const std::time_t startSec = mktime(&local);

    // b28d-D42 verdict=NEAR [N7] ORIG 0x08163e7c orig/ours=124/124。
    // 修法：P272 臂序反写（`startSec >= now` 使 kWaiting 块直落）+ P227 trace
    // 临时链；残余仅间接 call 目标寄存器 %edx↔%eax（N7，4.4.7 恒 %eax）。
    // P272：ORIG 0x08163fc2 为 `jl <kIng 块>` + kWaiting 块直落
    // （else-first）⇒ 条件取反成 `startSec >= now` 复现臂序。
    if (startSec >= now)
    {
        // 起始时刻未到：setState(kWaiting) + 注册到 startSec 的定时器。
        setState(State::kWaiting);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xae,
                                    static_cast<int>(startSec - now), 0, 0);
        cMyTrace("CEventCreateDnf::RegistStartTimer(bool)", 0x6e, 0)(
            "[Taiwan, CreateDnfEvent] next db load time:%d",
            static_cast<int>(startSec));
    }
    else
    {
        // 起始时刻已过：DB makeRequest + setState(kIng) +
        // 以 reloadInterval 步长推进到下一个 >=now 的时刻，注册定时器。
        DBSelectCreateDnfEventInfo::makeRequest();
        setState(State::kIng);
        int next = static_cast<int>(m_reloadInterval) + static_cast<int>(startSec);
        while (next < static_cast<int>(now))
            next += static_cast<int>(m_reloadInterval);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xae,
                                    next - static_cast<int>(now), 0, 0);
        cMyTrace("CEventCreateDnf::RegistStartTimer(bool)", 0x7e, 0)(
            "[Taiwan, CreateDnfEvent] next db load time:%d", next);
    }
