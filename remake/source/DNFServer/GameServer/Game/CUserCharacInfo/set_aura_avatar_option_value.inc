void CUserCharacInfo::set_aura_avatar_option_value(int index, int value)
{
    if (m_selected && index >= 0 && index <= 2)
        m_selected->m_auraValues[index] = value;
}