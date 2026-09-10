void CItemLockTimer::NextItemLockTimer(CUser* user)
{
    if (m_count != 0)
    {
        // ORIG：getCurCharacNo→%esi、GetUID→%ebx 跨 G_TimerQueue call 缓存，
        // 实参内联复现（P24/P52，禁具名局部槽）。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x88, CHECK_ITEM_LOCK_TIME,
                                    0, (unsigned int)user->getCurCharacNo());
    }
