void CParty::makeBestClearTimePacket(PacketGuard& packet, int clearTime)
{
    packet.put_byte(bf(this).isQuestMaze() ? 1 : 0);
    packet.put_byte(get_member_count());
    int bestTime = 0;
    bool isNewRecord = false;
    for (int slot = 0; slot < 4; ++slot)
    {
        if (!checkValidUser(slot))
            continue;
        bestTime = reinterpret_cast<BestClearTime*>(
            m_member[slot].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0x12))
                       ->getBestClearTime(bf(this).get_dungeon_index(),
                                          bf(this).get_dungeon_diff());
        isNewRecord = reinterpret_cast<BestClearTime*>(
            m_member[slot].m_pUser->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0x12))
                          ->checkNewRecord();
        if (bestTime == 0)
            bestTime = clearTime;
        // ORIG +0x140: put_short(get_unique_id & 0xffff)，随后 put_int(bestTime) / put_byte(newRecord)
        packet.put_short((int)(m_member[slot].m_pUser->get_unique_id() & 0xffff));
        packet.put_int(bestTime);
        packet.put_byte(isNewRecord ? 1 : 0);
    }
