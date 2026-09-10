void CBingoGmTest::parse()
{
    trim(m_parser->m_args);
    m_subCmd = atoi(m_parser->m_args.c_str());
