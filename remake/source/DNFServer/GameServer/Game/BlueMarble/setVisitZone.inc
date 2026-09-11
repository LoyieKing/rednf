int BlueMarble::setVisitZone(int zone)
{
    std::set<int>::const_iterator it = m_set.find(zone);
    if (it == m_set.end())
    {
        m_set.insert(zone);
    }
}