// ORIG parse 0x084aa73a：trim 后以首个空格切分 args；
// 前段 atoi → 槽位(+0x08)，后段 atoi → 耐久(+0x0c)。ORIG 未校验 find 失配。
void CEnduranceCmd::parse()
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string slotStr = m_parser->m_args.substr(0, pos);
    std::string enduranceStr = m_parser->m_args.substr(pos + 1, std::string::npos);

    m_slot = atoi(slotStr.c_str());
    m_endurance = static_cast<unsigned short>(atoi(enduranceStr.c_str()));
}