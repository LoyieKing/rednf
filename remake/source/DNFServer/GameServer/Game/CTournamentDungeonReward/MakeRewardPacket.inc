void CTournamentDungeonReward::MakeRewardPacket(PacketGuard* packet, bool flag)
{
    _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, packet);
    if (flag)
        _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, packet);
    else
        packet->put_byte(0);
