// ORIG parse 0x084a97c0：trim(parser->args)；m_str0 = parser->args（目标角色名）。
void CRequestDisjointCmd::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}