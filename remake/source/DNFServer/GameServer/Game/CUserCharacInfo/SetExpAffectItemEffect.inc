void CUserCharacInfo::SetExpAffectItemEffect(bool flag, float rate)
{
    if (m_selected)
    {
    m_selected->m_expItemAffected = flag;
    m_selected->m_expAffectRate = rate;
    }
}