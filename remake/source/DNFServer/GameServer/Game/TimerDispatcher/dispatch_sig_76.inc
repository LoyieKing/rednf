bool TimerInsertUserPlayTime::dispatch_sig(int /*p1*/, int /*p2*/, memberIdentificationNumber_t /*p3*/)
{
    // ORIG 0x08636d0a：统计单例落盘 → 重排当日 04:30 定时器。
    GetInstanceUserPlayTimeStatisticMgr()->InsertUserPlayTime();
    // ORIG +0x1e..：bool 结果落 -0x9 槽，reload+xor$1（Midnight 同形，探针 t5）。
    const bool registered = RegistNextTimer();
    if (!registered)
        return 0;
    return 1;
}