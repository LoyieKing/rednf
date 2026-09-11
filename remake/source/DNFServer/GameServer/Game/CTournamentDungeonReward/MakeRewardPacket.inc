// ORIG 0x8284caa：卡型 0 必发；flag 时补卡型 1，否则 byte0 占位
void CTournamentDungeonReward::MakeRewardPacket(PacketGuard* packet, bool flag)
{
    _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, packet);
    if (flag)
        _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, packet);
    else
        packet->put_byte(0);
}