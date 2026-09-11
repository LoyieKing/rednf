unsigned int CSchoolMgr::GetUserCount(unsigned int schoolNo)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        return it->second;
    }
    return 0;
}