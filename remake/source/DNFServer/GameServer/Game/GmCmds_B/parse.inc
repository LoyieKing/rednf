// ORIG parse 0x084ab524：trim(parser->args)；m_str0 = parser->args（新角色名）。
void CChangeCharacName::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}