void CBoosterTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d", &m_mode, &m_count);
