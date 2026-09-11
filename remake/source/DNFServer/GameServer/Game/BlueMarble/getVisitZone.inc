bool BlueMarble::getVisitZone(int seat) const
{
    std::set<int>::const_iterator it = m_set.find(seat);
    if (it == m_set.end())
    {
        return 0;
    }
    return 1;
}