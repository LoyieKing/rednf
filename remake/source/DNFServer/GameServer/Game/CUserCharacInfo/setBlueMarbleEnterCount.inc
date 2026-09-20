void CUserCharacInfo::setBlueMarbleEnterCount(unsigned char count)
{
    if (m_selected)
    {
    m_selected->m_field14BD = count;
    }
}