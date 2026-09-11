// ORIG parse 0x084b2110：难度字节清零后 atoi(args) 写入 +0x08。
void DungeonDifficulty::parse()
{
    m_difficulty = 0;
    trim(m_parser->m_args);
    m_difficulty = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}