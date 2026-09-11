// GI 移位：dispatch_sig 原位于 Timer_DailySchedule 族内（3656 行区），纯移位上送
// 至本 TU 首个强实体 TimerDispatcher::C2 之前，使 _GLOBAL__I_ 后缀对齐 ORIG 的
// _GLOBAL__I__ZN19Timer_DailySchedule12dispatch_sigEiij。语义零变化。
bool Timer_DailySchedule::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x812c920: CDailyScheduleManager::CheckAndSendToAllUserSchedule(s_DailyScheduleManager); RegistNextTimer(); return 1
    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();
    RegistNextTimer();
    return 1;
}