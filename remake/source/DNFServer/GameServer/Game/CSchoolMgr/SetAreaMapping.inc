void CSchoolArea::SetAreaMapping(unsigned int schoolNo, int areaIdx)
{
    m_areaMap.insert(std::make_pair(schoolNo, areaIdx));
