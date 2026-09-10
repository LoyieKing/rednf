bool PowerParameterScript::checkPowerWarServer(int a, int b)
{
    std::pair<int, int> p;
    std::vector<std::pair<int, int> >::iterator it = m_powerWarServerList.begin();
    std::vector<std::pair<int, int> >::iterator e = m_powerWarServerList.end();
    for (; it != e; ++it)
    {
        p = *it;
        if (p.first == a && p.second == b)
            return true;
    }
    return false;
