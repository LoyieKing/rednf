void CSetMultiboxFailCnt::parse()
{
    m_failCnt = 0;
    trim(m_parser->m_args);
    m_failCnt = atoi(m_parser->m_extra[1].c_str());   // ORIG 直接 operator[](1)，无边界检查
