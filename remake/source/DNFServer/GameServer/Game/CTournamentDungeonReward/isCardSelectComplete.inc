bool CTournamentDungeonReward::isCardSelectComplete(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if (members.empty())
        return false;

    if ((int)type == 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                // ORIG 求值序 = &flag → end() → begin() → find → 比较侧 end()；
                // 具名 found 使 4.4 的 find 结果先落槽再调 end（P293 同族）。
                std::vector<int>::const_iterator found =
                    std::find(members.begin(), members.end(),
                              m_rewards[i][j].m_flag08);
                if (found == members.end())
                    return false;
            }
        }
    }
    else
    {
        for (int j = 0; j < 2; ++j)
        {
            std::vector<int>::const_iterator found =
                std::find(members.begin(), members.end(),
                          m_rewards[(int)type][j].m_flag08);
            if (found == members.end())
                return false;
        }
    }
    return true;
