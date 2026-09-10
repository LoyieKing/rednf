void CPcroomPremiumCmd::parse()
{
    trim(m_parser->m_args);
    m_duration = atoi(m_parser->m_args.c_str());
