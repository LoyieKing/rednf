void CInGameEventSetDay::parse()
{
    trim(m_parser->m_args);
    m_day = atoi(m_parser->m_args.c_str());
