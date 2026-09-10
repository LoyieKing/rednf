void CChoiceItem::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = static_cast<unsigned int>(atoi(m_parser->m_args.c_str()));
