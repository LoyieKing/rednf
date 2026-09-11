// ---- ReturnAncientDungeonTicket（T 0x0826d598）----
// 仅调 CParty::increase_ancient_dungeon_ticket（不触 result/m_data）。
void ReturnAncientDungeonTicket::process(RewardData_Param param,
                                         RewardData_Result& /*result*/)
{
    param.m_party->increase_ancient_dungeon_ticket();
}