// ORIG T 0x0863771a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x2a,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 CompleteLoadAfterAssault 超时重载。
void TimerCompleteLoadAfterAssaultTimeout::registNextTimer(long sec,
                                                           int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x2a, (int)sec, (unsigned int)p2,
                                0);
}