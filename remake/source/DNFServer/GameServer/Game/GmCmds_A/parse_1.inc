// ORIG parse 0x084ac2aa：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemByMail::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}