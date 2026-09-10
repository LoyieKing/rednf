void CEndEvent::parse()
{
    trim(m_parser->m_args);
    m_eventCode = atoi(m_parser->m_args.c_str());
