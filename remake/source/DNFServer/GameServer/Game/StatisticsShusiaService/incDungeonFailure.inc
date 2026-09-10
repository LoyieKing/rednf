void StatisticsShusiaService::incDungeonFailure(int dungeonIdx)
{
    // b28d-35: DIFF→N82（帧 0x30 vs 0x40、全帧槽平移 8）。ORIG @0x0860c1ba。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_dungeonFailure.find(dungeonIdx);
    if (it == m_dungeonFailure.end())
    {
        m_dungeonFailure.insert(std::make_pair(dungeonIdx, 1));
    }
    else
    {
        it->second += 1;
    }
