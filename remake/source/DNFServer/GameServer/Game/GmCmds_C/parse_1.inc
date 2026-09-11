// ORIG parse 0x084ab914：atoi(args) → +0x08。
void CEndEvent::parse()
{
    trim(m_parser->m_args);
    m_eventCode = atoi(m_parser->m_args.c_str());
}