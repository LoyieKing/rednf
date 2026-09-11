// ---------------------------------------------------------------------------
// CBattle_Field::MakeBasicRoundInfo @ 0x08303896
// 报告要点：random_shuffle(monsters)；begin()==end() 直接返回 false；
// while(round<=14 && it!=end)：每轮构造 TournamentInfo，内层 for(j<memberCount)
// 复制 *it 到 m_monster[j] 并 m_group += it->m_rate(+4)、++it，push_back 后 ++round；
// 退出时返回 round==15。ORIG 内层循环不做 end 检查、无 memberCount 边界检查，
// roundInfo 不预先 clear —— 全部忠实保留。
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
}