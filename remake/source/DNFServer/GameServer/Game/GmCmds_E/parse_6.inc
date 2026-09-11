// ORIG parse 0x084ab202：trim(args)；*(short*)(this+8) = (short)atoi(args)。
void CPowerWarStraightVictory::parse()
{
    trim(m_parser->m_args);
    m_straight = static_cast<short>(atoi(m_parser->m_args.c_str()));
}