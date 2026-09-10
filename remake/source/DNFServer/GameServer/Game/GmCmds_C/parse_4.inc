void CGuildDebug::parse()
{
    trim(m_parser->m_args);
    m_param1 = atoi(m_parser->m_args.c_str());
    m_param2 = 0;
    m_f1 = 0;  // +0x10（ORIG 无其它写点，恒为 0）
