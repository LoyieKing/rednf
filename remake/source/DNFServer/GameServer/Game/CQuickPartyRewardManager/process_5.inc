void ReturnAncientDungeonTicket::process(RewardData_Param param,
                                         RewardData_Result& /*result*/)
{
    param.m_party->increase_ancient_dungeon_ticket();
