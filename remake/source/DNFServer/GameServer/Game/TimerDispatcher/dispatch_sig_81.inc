bool TimerUpdateItemGenerateStatistics::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08637170：两级链式（%eax 直落 (%esp)，无具名槽），帧 0x18；
    // getCurSec 结果寄存器内 add $0x15180 后直传 registNextTimer。
    G_CDataManager()->getItemGenerator()->onTimerEvent(0x6b);
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    return 1;
}