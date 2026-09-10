void CTournamentDungeonReward::ForceSelectCard(std::vector<int>& members, bool victory)
{
    _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, members);
    if (victory)
        _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, members);
