void CUserCharacInfo::SetCurCharacOption(CharacterOption& option)
{
    if (m_selected)
        m_selected->m_characterOption = option;
}