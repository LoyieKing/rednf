// ORIG parse 0x084aa9c8：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemPollutionCmd::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}