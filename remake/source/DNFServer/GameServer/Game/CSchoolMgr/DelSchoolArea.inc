void CSchoolArea::DelSchoolArea(unsigned int schoolNo)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        m_areaStack.push(it->second);
        m_areaMap.erase(it);
    }
