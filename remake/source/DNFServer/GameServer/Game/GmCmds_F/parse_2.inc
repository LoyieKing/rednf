void CSetGuildFund::parse()
{
    trim(m_parser->m_args);
    m_fund = atoi(m_parser->m_args.c_str());
