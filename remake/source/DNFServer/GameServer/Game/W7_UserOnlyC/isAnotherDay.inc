bool LimitItemUsage::isAnotherDay()
{
    // 具名字段单步 mov disp（P0：ORIG 无中间槽/地址算术）。
    // P0：ORIG 仅 serverSchedule/createTime 两槽；curSec 为内联右值直传。
    const long serverSchedule =
        static_cast<long>(G_CEnvironment()->m_serverEnvir.m_fatigueTime);
    const long createTime = static_cast<long>(
        *reinterpret_cast<const CommonTime*>(this));   // operator long()
    // P56：ORIG 0x08688ffa `test %al,%al; je mov$0; mov$1; jmp` 分支形态，
    // 直 return 表达式会折成单条。
    if (CheckDailyScheduleTime(
            serverSchedule, createTime,
            static_cast<long>(GlobalData::s_systemTime_.getCurSec())))
        return true;
    return false;
