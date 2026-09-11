void WarRoom::Start()
{
    PacketGuard packet;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            packet.clear();
            packet.put_header(0, 0x5a);
            packet.finalize(true);
            (m_slotsUser[i])->Send(packet);
            if (m_pDungeon != 0)
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    m_pDungeon->get_index(), false);
            }
        }
    }
    ChangeState(WARROOM_STATE_1);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            // b28d-solo-WarRoom [P235/P301]：改用 union 具型视角 m_slotsUser[i]
            // （同址 +0x12c）消除 `(CUser*)m_slots[i]` cast 的二段装载，复现 ORIG
            // 0x086bc3xx 的 `mov 0xc(%eax,%edx,4),%ebx` 单段直落 callee-saved。
            m_slotsUser[i]->getHackAnalyzer()->beginCollectHackInfo(m_slotsUser[i]);
        }
    }
}