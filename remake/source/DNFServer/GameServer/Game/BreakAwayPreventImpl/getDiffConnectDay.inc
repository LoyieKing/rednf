unsigned int CBreakAwayPreventSystem::getDiffConnectDay(unsigned int timeSec)
{
    if (timeSec == 0)
        return 0;

    long prevDaySec = getPrevScheduleTime(0, static_cast<long>(timeSec));
    // b28c-28（P0 槽-局部双向对应律）：ORIG 0x0831c806 段为
    // `mov (%eax),%eax; mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax`——
    // max 结果有**具名局部槽往返**（⇒ 原码非直 return 表达式），
    // 直 return 时 4.4 折成 `mov (%eax),%eax; leave` 少 2 条。
    // 负天数按 unsigned 包装返回（int 解释为负），不可用 max<long> 钳制为 0。
    unsigned int days = std::max<unsigned int>(
        0u, static_cast<unsigned int>(
                (static_cast<long>(GlobalData::s_systemTime_.getCurSec())
                 - prevDaySec) /
                0x15180L));
    return days;
