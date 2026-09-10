bool CBattle_Field::MakeBasicRoundInfo(int memberCount,
    std::vector<TournamentMonster>& monsters,
    std::vector<TournamentInfo>& roundInfo)
{
    std::random_shuffle(monsters.begin(), monsters.end());
    std::vector<TournamentMonster>::iterator it = monsters.begin();
    if (it == monsters.end())
        return false;

    int round = 0;
    for (round = 0; round <= 14 && it != monsters.end();)
    {
        TournamentInfo info;
        for (int j = 0; j < memberCount; ++j)
        {
            info.m_monster[j] = *it;
            info.m_group += it->m_rate;
            it++;
        }
        roundInfo.push_back(info);
        ++round;
    }
    // ORIG 尾块：cmpl $0xf; je →mov$1 岛；false 臂原地 fall-through（P47 镜像）。
    if (round != 15)
        return false;
    return true;
