// ORIG parse 0x084aa29e：trim(parser->args)；m_str0 = parser->args。
void CItemInfoCmd::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}