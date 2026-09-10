void TimerQueue::InsertTimer(TimerEntry::OBJ_TYPE objType, int param1,
                             TIMER_MESSAGE msg, int sec, int param2, unsigned int param3) throw()
{
    InsertTimerInMilisecond(objType, param1, msg, SECOND_TO_MILISECOND(sec), param2, param3);
