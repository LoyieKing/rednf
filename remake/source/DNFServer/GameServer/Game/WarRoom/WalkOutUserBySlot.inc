int WarRoom::WalkOutUserBySlot(int slot)
{
    // [b28d-solo-WarRoom] AE | ORIG 0x086bd256 | P235 union 具型视角消 cast 二段
    //   （ORIG 0x086bd2f6 `mov 0xc(%eax,%edx,4),%ebx` 单段直落）后逐条全等。
    if (m_slots[slot] == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__, 0x5cc,
                              "WarRoom::WalkOutUserBySlot no slot(%d)", slot);
        return 4;
    }
    (m_slotsUser[slot])->getHackAnalyzer()->reportHackInfo();
    (m_slotsUser[slot])->getHackAnalyzer()->resetHackInfo();
    m_slotState[slot] = 0xff;
    // ORIG 086bd2f6: 槽值单步直落 %ebx 跨 G_GameWorld 调用；4.4 恒 eax 落值+
    // mov 保活两步（N18a；P226 裸语句表达式形实测同两步不可翻，b28BM13）。
    G_GameWorld()->out_from_warroom(m_slotsUser[slot]);
    if (m_pDungeon != 0)
    {
        (m_slotsUser[slot])->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                                           (unsigned char)m_levelBand);
    }
    m_slots[slot] = 0;
    m_liveFlag[slot] = 0;
    PacketGuard packet;
    MakeSlotInfo((char*)&packet, slot);
    G_GameWorld()->send_all(packet);
    return 0;
}