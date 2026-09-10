void CTournamentDungeonReward::_ForceSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if ((int)type > 1)
        return;

    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<unsigned char>(m_rewards[(int)type][slot].m_flag08) == 0xff)
        {
            // ORIG 增量段调 operator++(it,0)（后置，带独立 sret 临时槽 -0x10）⇒
            // 源形必写 it++（P28 增量侧推论）。显式 itEnd 缓存实测 ndiff 63→73
            // 反劣（4.4 不按 P13 缓存 end），故维持 `it != members.end()`。
            for (std::vector<int>::const_iterator it = members.begin();
                 it != members.end(); it++)
            {
                if (!isMemberSelectCard(type, *it))
                    m_rewards[(int)type][slot].m_flag08 = *it;
            }
        }
    }
