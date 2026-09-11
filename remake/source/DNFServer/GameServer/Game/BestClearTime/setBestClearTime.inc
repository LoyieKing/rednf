void BestClearTime::setBestClearTime(int dungeonIdx, int difficulty, int time)
{
    // ORIG 0x80c8928。
    // 难度只允许 0..4；时间为负按 0 处理。
    if (difficulty > 4)
        return;
    if (time < 0)
        time = 0;

    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY>::iterator it =
        m_map.find(dungeonIdx);
    if (it == m_map.end())
    {
        // 无该副本记录：以全零 SIG_BEST_CLEAR_TIME_DIFFICULTY 插入，
        // 再写入当前难度的时间。
        SIG_BEST_CLEAR_TIME_DIFFICULTY tmp;
        tmp.m_time[difficulty] = time;
        m_map.insert(std::make_pair(dungeonIdx, tmp));
        setNewRecord(true);
        alter();
        return;
    }

    if (it->second.m_time[difficulty] == 0)
    {
        // 该难度尚无记录：直接写入。
        it->second.m_time[difficulty] = time;
        setNewRecord(true);
        alter();
    }
    else if (it->second.m_time[difficulty] > time)
    {
        // 已有记录且新时间更短：覆盖。
        it->second.m_time[difficulty] = time;
        setNewRecord(true);
        alter();
    }
}