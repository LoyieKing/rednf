void CMissionList_Charac::setIssue(bool isNewbie, bool isDaily)
{
    if (isDaily)
    {
        m_field148 = (short)(m_field148 * 2);
        m_field148 = (short)((unsigned short)m_field148 +
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    else
    {
        m_field14a = (short)(m_field14a * 2);
        m_field14a = (short)((unsigned short)m_field14a +
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    m_field14c = (short)(m_field14c * 2);
    m_field14c = (short)((unsigned short)m_field14c +
                         (unsigned short)(isNewbie ? 1 : 0));
