void StatisticsShusiaService::incServiceUsage(int serviceIdx)
{
    // b28d-35: DIFF→N82（帧 0x30 vs 0x40、全帧槽平移 8）。ORIG @0x0860c358。
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it == m_serviceUsage.end())
    {
        m_serviceUsage.insert(std::make_pair(serviceIdx, 1));
    }
    else
    {
        it->second += 1;
    }
