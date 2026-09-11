// ORIG parse 0x084aacb8：与 CChangeNPCFavorCmd::parse 同构。
void CChangeNPCMaxDayCmd::parse()
{
    trim(m_parser->m_args);
    int pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1, std::string::npos);
    // b28d-22：P0 — ORIG 0x084aacb8 同构，atoi 结果直写成员。
    m_npcId = atoi(first.c_str());
    m_maxDay = atoi(second.c_str());
}