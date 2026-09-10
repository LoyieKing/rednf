int Timer_TenMinuteSchedule::dispatch_sig(int /*p1*/, int p2, unsigned int /*p3*/)
{
    // ORIG 0x8639668：检查第二参数（0x10(%ebp)）：==0 → InsertValueStatistic +
    // RegistNextTimer(1)；==1 → InsertCirculationStatistic + RegistNextTimer(0)；
    // 其他值两分支都不执行。ORIG 形：bool ok=false（movb $0,-0x9）先行初始化，
    // 分支内 ok = RegistNextTimer(n)（mov %al,-0x9），尾部 reload+xor$1 物化返回。
    bool ok = false;
    if (p2 == 0)
    {
        GetInstanceValueStatistic()->InsertValueStatistic();
        ok = RegistNextTimer(1);
    }
    else if (p2 == 1)
    {
        GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        ok = RegistNextTimer(0);
    }
    if (!ok)
        return 0;
    return 1;
