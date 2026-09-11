// ---------------------------------------------------------------------------
// 定时器注册实现（ORIG 汇编逐条转写；均为 G_TimerQueue()->InsertTimer 包装，
// OBJ_TYPE=2 表示全局对象定时器，param1/key 均为 0）。
// 注：Timer_SecretShopStatistic::RegistNextTimer 与 TimerDispatcher.cpp 重复，
// 已删除（2026-08-29），保留 TimerDispatcher.cpp 权威实现。
// ---------------------------------------------------------------------------
void Timer_DailySchedule::RegistNextTimer()
{
    // ORIG 0x812c940：msg=0x91，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x91, 0x3c, 0, 0);
}