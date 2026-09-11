// ORIG parse 0x084ac96c：trim；find(" ") 切两段分别 atoi 入 +0x08/+0x0c。
// b28d-solo-GmCmds_E：86=86 AE——与 CPickup::parse 同修（`size_t pos`→`int pos`，
// P218 同族 signed 索引局部旋钮）；原 P75/N18a 判词作废。
void CRefund::parse()
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1);
    m_param1 = atoi(first.c_str());
    m_param2 = atoi(second.c_str());
}