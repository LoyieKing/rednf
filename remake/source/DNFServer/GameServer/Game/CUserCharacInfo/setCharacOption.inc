void CUserCharacInfo::setCharacOption(CharacterOption& option)
{
    if (m_selected)
        m_selected->m_characterOption = option;
}