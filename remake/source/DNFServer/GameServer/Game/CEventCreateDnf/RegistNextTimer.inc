void CEventCreateDnf::RegistNextTimer()
{
    // b28d-D42 verdict=DIFF [N7+N82] ORIG 0x08163d4c orig/ours=74/76 ndiff=33。
    // 残差：①间接 call 目标 %edx↔%eax（N7）；②tm 字段槽基 -0x44/-0x4c 平移 4
    // 与部分指令序（P112/N82 跨版本槽分配）。语义已对齐（P28 赋值序已按 ORIG）。
    // LinkE2 按 ORIG T 0x08163d4c sz0x12f 逐指令转写：
    // !IsEventing(0)（vtable 槽调用，虚函数 IsEventing）→ 直接返回。
    if (!IsEventing(0))
        return;

    // curSec = CSystemTime::getCurSec()（全局 s_systemTime_ @0x941f714，ORIG 0x080cbc9e）
    std::time_t curSec = static_cast<std::time_t>(GlobalData::s_systemTime_.getCurSec());
    std::tm local;
    localtime_r(&curSec, &local);

    // tm_sec=0 / tm_min=0 / tm_hour=m_endHour（this+0x11 byte，ORIG movzbl 0x11）
    // b28d-D42 P28：ORIG 0x08163d4c 的赋值序为 tm_hour 先（-0x44）、再 tm_min
    // （-0x48）、tm_sec（-0x4c）。
    local.tm_hour = m_endHour;
    local.tm_min = 0;
    local.tm_sec = 0;
    const std::time_t startSec = mktime(&local);

    // startSec >= curSec → setState(kReward=2)（ORIG jl 8163ddd 跳过路径取反）
    if (startSec >= curSec)
        setState(State::kReward);

    // InsertTimer(G_TimerQueue(), OBJ_TYPE 2, 0, TIMER_MESSAGE 0xae,
    //   m_reloadInterval, 0, 0)（ORIG 0x8630e16；0xae 无具名枚举值，cast 忠实；
    //   sec 参数 = this+0x12 reloadInterval）
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0xae,
                                static_cast<unsigned int>(m_reloadInterval), 0, 0);

    // b28d-D42 + round1 R19：ORIG 0x08163e20 无 next 具名槽——`movzwl 0x12(%eax);
    // movzwl %ax,%ebx` 先物化 reloadInterval，ctor 前 `call getCurSec; add %eax,%ebx`
    // 跨 ctor 保活（N80/P0）⇒ 删具名 next、内联回 vararg 位（P227 全表达式临时链）。
    cMyTrace("CEventCreateDnf::RegistNextTimer()", 0x55, 0)(
        "[Taiwan, CreateDnfEvent] next db load time:%d",
        static_cast<int>(m_reloadInterval) + GlobalData::s_systemTime_.getCurSec());
}