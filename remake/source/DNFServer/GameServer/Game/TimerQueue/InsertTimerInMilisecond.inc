void TimerQueue::InsertTimerInMilisecond(TimerEntry::OBJ_TYPE objType, int param1,
                                         TIMER_MESSAGE msg, long long milisecond,
                                         int param2, unsigned int param3) throw()
{
    TimerEntry entry;
    entry.objType = objType;
    entry.param1 = param1;
    entry.msg = msg;
    entry.time = GlobalData::s_systemTime_.getCurTickCount() + milisecond;
    entry.param2 = param2;
    entry.param3 = param3;
    InsertTimer(entry);
}