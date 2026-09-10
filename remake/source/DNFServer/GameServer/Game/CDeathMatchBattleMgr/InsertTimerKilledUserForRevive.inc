void CDeathMatchBattleMgr::InsertTimerKilledUserForRevive(int roomIdx, int a)
{
    // ORIG 0x8568d90：key 先物化 %ebx、G_TimerQueue 次之 %esi（P24），无具名槽；
    // 源形=内联实参单语句（P134），禁中间局部（具名 key/queue 槽化致 0x38 帧）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)5, roomIdx,
                                (TIMER_MESSAGE)(a + 0x33), 3,
                                m_pRoom->gen_timer_key((TIMER_MESSAGE)(a + 0x33)), 0);
