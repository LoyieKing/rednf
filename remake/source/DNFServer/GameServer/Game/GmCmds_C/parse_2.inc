void CEnduranceCmd::parse()
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string slotStr = m_parser->m_args.substr(0, pos);
    std::string enduranceStr = m_parser->m_args.substr(pos + 1, std::string::npos);

    m_slot = atoi(slotStr.c_str());
    m_endurance = static_cast<unsigned short>(atoi(enduranceStr.c_str()));
