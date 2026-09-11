// ---------------------------------------------------------------------------
// attachTimer —— ORIG 0x081a98d4：
// G_TimerQueue()->InsertTimer(OBJ_TYPE 2, 0, TIMER_MESSAGE 0xac, sec 0x78, 0, 0)。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::attachTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xac, 0x78, 0, 0);
}