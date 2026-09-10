void CSecretShopNpc::parse()
{
    trim(m_parser->m_args);
    m_npcIdx = atoi(m_parser->m_args.c_str());
