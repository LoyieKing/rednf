void WarRoom::InvalidUserCheck()
{
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086bb595：双跳落点异块 —— je +0xd5(nop;fall end) / je +0xd6(end) /
        // body 尾 jmp +0xd6。slot-null goto 落独立空块，R!=0 与 body 尾直接去 loop-tail。
        // ORIG 086bb595 块序：[body][jmp E][X: nop][E: addl]；je1→X，je2→E。
        if (m_slots[i] == 0)
            goto INVALID_SLOT_NULL;
        if ((m_slotsUser[i])->getCurCharacR() == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x236, 5)
                ("WarRoom::InvalidUserCheck slot(%d) warRoomIndex(%d)",
                 i, (m_slotsUser[i])->GetWarRoomIndex());
            m_slotState[i] = 0xff;
            m_slots[i] = 0;
            m_liveFlag[i] = 0;
            goto INVALID_NEXT;
        }
        goto INVALID_NEXT;
INVALID_SLOT_NULL:
        ;
INVALID_NEXT:
        ;
    }
}