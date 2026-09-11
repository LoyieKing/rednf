// ============================================================================
// ORIG 0x0828523c（报告 ForceSelectCard.md）：_ForceSelectCard(0)；victory 时
// 再 _ForceSelectCard(1)。
// ============================================================================
void CTournamentDungeonReward::ForceSelectCard(std::vector<int>& members, bool victory)
{
    _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, members);
    if (victory)
        _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, members);
}