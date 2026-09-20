void CUserCharacInfo::set_aura_avatar_option(stAuraAvatarOption& option)
{
    if (m_selected)
        m_selected->m_auraAvatarOption = option;
}