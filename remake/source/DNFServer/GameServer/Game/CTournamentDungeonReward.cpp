#include "CTournamentDungeonReward.h"
#include "PacketGuard.h"

void CTournamentDungeonReward::MakeSelectPacket(PacketGuard* packet)
{
    for (int type = 0; type < 2; ++type)
    {
        packet->put_byte(2);
        for (int member = 0; member < 2; ++member)
            packet->put_byte((int)(char)m_reward[type][member].m_select);
    }
}

void CTournamentDungeonReward::_MakeRewardPacket(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, PacketGuard* packet)
{
    if ((int)type > 1)
        return;
    packet->put_byte(2);
    for (int member = 0; member < 2; ++member)
    {
        packet->put_int(m_reward[(int)type][member].m_itemIdx);
        packet->put_int(m_reward[(int)type][member].m_count);
        packet->put_short(0);
    }
}

void CTournamentDungeonReward::MakeRewardPacket(PacketGuard* packet, bool both)
{
    _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, packet);
    if (both)
        _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, packet);
    else
        packet->put_byte(0);
}
