void CUserCharacInfo::SetExpAffectItemEffect(bool flag, float rate)
{
    if (m_selected)
    {
    CUR->m_isExpItemAffected = flag;
    CUR->m_expAffectRate = rate;
    }
}