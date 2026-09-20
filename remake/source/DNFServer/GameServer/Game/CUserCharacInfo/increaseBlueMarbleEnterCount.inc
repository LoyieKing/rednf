void CUserCharacInfo::increaseBlueMarbleEnterCount()
{
    if (m_selected)
    {
    ++m_selected->m_field14BD;
    }
}