void CGetCashInven::parse()
{
    trim(m_parser->m_args);
    m_cashType = static_cast<short>(atoi(m_parser->m_args.c_str()));
