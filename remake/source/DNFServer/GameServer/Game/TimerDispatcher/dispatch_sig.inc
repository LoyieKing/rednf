int Timer_DailySchedule::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x812c920: CDailyScheduleManager::CheckAndSendToAllUserSchedule(s_DailyScheduleManager); RegistNextTimer(); return 1
    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();
    RegistNextTimer();
    return 1;
