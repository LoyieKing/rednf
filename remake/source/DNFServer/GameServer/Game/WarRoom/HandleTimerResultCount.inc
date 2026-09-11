void WarRoom::HandleTimerResultCount()
{
    ReviveAll();
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086c0421：双空岛 086c04a6(nop;jmp 086c04aa)/086c04a9(nop;fall)，
        // body 尾 jmp 086c04aa（同 ReviveAll 双 goto 模式）。
        CUser* u;
        if (m_slots[i] == 0)
            goto TRES_NEXT;
        if (m_slotData[i] == 0)
            goto TRES_SKIP;
        AddBanList(m_slotsUser[i]);
        u = m_slotsUser[i];
        WalkOutUserBySlot(i);
        SendWalkOut(u, (ENUM_WALKOUT_TYPE)m_slotData[i]);
TRES_SKIP:
        ;
TRES_NEXT:
        ;
    }
    BattleReset();
    CheckFatuigue();
    if (GetWaiterCount() < 3)
        ChangeState(WARROOM_STATE_0);
    else
        ChangeState(WARROOM_STATE_1);
}