int WarRoom::IsJoinable(CUser* user)
{
    if ((GetState() == 4) || (GetState() == 3) || (GetState() == -1) ||
        (GetState() == 2))
        return 0x13;
    if (user->isCompetitionMercenary() != 0)
        return 0x15;
    if (user->getCurCharacMoney() <
        G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
        return 10;
    // ORIG 0x86bad6b：bool 返回 `!= 1` 折成 xor $0x1 判定；true 路径早退。
    if (user->CheckFatigue() != 1)
        return 0x43;
    {
        int level = user->get_charac_level();
        switch ((unsigned char)m_levelBand)
        {
        case 2:
            if ((level < 0x14) || (0x1d < level))
                return 0xe;
            break;
        case 3:
            if ((level < 0x1e) || (0x27 < level))
                return 0xe;
            break;
        case 4:
            if ((level < 0x28) || (0x31 < level))
                return 0xe;
            break;
        case 5:
            if ((level < 0x32) || (0x3b < level))
                return 0xe;
            break;
        case 6:
            if ((level < 0x3c) || (0x46 < level))
                return 0xe;
            break;
        default:
            return 0xe;
        }
        UpdateBanList();
        if (IsBanUser(user))
            return 0x1e;
        for (int i = 0; i <= 5; ++i)
        {
            if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
                return 0;
        }
        return 4;
    }
}