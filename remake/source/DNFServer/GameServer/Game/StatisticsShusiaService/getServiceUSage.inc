int StatisticsShusiaService::getServiceUSage(int serviceIdx)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_serviceUsage.find(serviceIdx);
    if (it != m_serviceUsage.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
