bool CTournamentDungeonReward::isMemberSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo)
{
    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<char>(m_rewards[(int)type][slot].m_flag08) == cardNo)
            return true;
    }
    return false;
