int CSchoolArea::GetAreaIndex(unsigned int schoolNo)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        return it->second;
    }
    return -1;
