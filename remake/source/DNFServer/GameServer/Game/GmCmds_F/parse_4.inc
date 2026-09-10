void CSetTutorialFlagCmd::parse()
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned int>(atoi(m_parser->m_args.c_str()));
