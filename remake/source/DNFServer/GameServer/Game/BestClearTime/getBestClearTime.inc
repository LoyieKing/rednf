int BestClearTime::getBestClearTime(int dungeonIdx, int difficulty)
{
    // ORIG 0x80c8ac0：find 未命中或难度 > 4 返回 0，否则返回记录值。
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::iterator it =
        m_map.find(dungeonIdx);
    if (it == m_map.end() || difficulty > 4)
        return 0;
    return it->second.m_time[difficulty];
}