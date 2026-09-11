// ORIG parse 0x084b0a34：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
void CChoiceItem::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = static_cast<unsigned int>(atoi(m_parser->m_args.c_str()));
}