bool CTournamentDungeonReward::SelectCard(int slotNo, char cardA, char cardB)
{
    if (cardA < 0 || cardA > 1)
        return false;
    if (cardB < 0 || cardB > 1)
        return false;
    if (slotNo < 0 || slotNo > 3)
        return false;

    // ORIG cmp $0xff; jne fail —— 命中未选槽（==0xff）直落写入体，失败块居尾
    if (static_cast<unsigned char>(
            m_rewards[(int)cardA][(int)cardB].m_flag08) == 0xff)
    {
        m_rewards[(int)cardA][(int)cardB].m_flag08 = static_cast<char>(slotNo);
        return true;
    }
    return false;
