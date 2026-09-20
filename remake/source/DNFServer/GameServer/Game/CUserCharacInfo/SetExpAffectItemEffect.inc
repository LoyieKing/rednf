void CUserCharacInfo::SetExpAffectItemEffect(bool flag, float rate)
{
    if (m_selected)
    {
    m_selected->m_fieldEE4 = flag;
    m_selected->m_fieldEE5 = rate;
    }
}