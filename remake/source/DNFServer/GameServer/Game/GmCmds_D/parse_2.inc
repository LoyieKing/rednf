// ORIG parse 0x084abcb0：*(int*)(this+8) = atoi(c_str(trim(args)))。
void CItemDropInDungeon::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}