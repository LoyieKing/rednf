void CPickup::parse()
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1);
    m_param1 = atoi(first.c_str());
    m_param2 = atoi(second.c_str());
