void CUserCharacInfo::SetInvisible()
{
    m_selected->m_tagVisibleFlags.m_flag = (unsigned char)(m_selected->m_tagVisibleFlags.m_flag & ~1u);
}
