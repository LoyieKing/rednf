void CChangeExpertJobExpCmd::parse()
{
    trim(m_parser->m_args);
    m_exp = atoi(m_parser->m_args.c_str());       // P0：无具名局部（ORIG 无槽）
