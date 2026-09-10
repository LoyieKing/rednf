void CGrowhWeaponEvent::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "gw")
    {
        m_resetFlag = 1;
    }
    else
    {
        m_resetFlag = 0;
        m_timepiece = atoi(m_parser->m_args.c_str());
    }
