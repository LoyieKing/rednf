// ORIG parse 0x084b09d0：*(int*)(this+8) = atoi(c_str(trim(args)))。
void CInGameEventSetDay::parse()
{
    trim(m_parser->m_args);
    m_day = atoi(m_parser->m_args.c_str());
}