int CDeathTower::handleSelectEPLPCommand(CUser* user, int cmd)
{
    // ORIG 0x8466208：slot 具名槽 -0xc（0x8466224）；&& 物化岛共享 mov$1/$0
    // （0x8466245/0x846624c）+ test/je 反判（范式22/40）；key/idx 缓存 ebx/esi。
    int slot = m_party->GetMemberSlotNo(user);
    // ORIG 0x8466227/0x8466241：正向 && 短路（call valid 结果直 test al; je ERR，
    // 无 xor），ERR=return 0 尾岛@84662c0，body fall-through（范式17）。
    if (slot != -1 && m_party->checkValidUser(slot))
    {
        m_party->SetEPLPState(3);
        // [b28d-D13] P0 清绑（N96）：key 内联实参（P24）。
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4e, cmd,
            _genTimerKey((TIMER_MESSAGE)0x4e), 0x4e);
        return 1;
    }
    return 0;
}