void CStartEvent::parse()
{
    m_eventId = 0;
    m_param1 = 0;
    m_param2 = 0;
    m_eventCount = 0;
    trim(m_parser->m_args);
    getParam(m_parser->m_args);
}