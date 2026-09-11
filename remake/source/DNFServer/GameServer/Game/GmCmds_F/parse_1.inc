// ORIG parse 0x084ab9da：trim(args)；+0x08(int 占位槽) = atoi(args)（npc id）。
void CSecretShopNpc::parse()
{
    trim(m_parser->m_args);
    m_npcIdx = atoi(m_parser->m_args.c_str());
}