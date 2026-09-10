void CHeroMissionCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        m_missionNo = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[2].c_str()));
        m_param = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[3].c_str()));
        return;                                             // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
