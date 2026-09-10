unsigned short CMissionList_Charac::getIssue_mode(bool isDaily) const
{
    if (isDaily)
    {
        return m_field148;
    }
    return m_field14a;
