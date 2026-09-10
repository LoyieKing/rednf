void CBindCubeTest::parse()
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
