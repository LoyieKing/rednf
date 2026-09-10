void DungeonDifficulty::parse()
{
    m_difficulty = 0;
    trim(m_parser->m_args);
    m_difficulty = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
