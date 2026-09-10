void CParser::parse()                           // 0x084a8ab6
{
    m_line = m_buf;                             // operator=(string&, const char*)
    m_pos = static_cast<int>(m_line.find(" ", 0));
    // ORIG：substr 的 sret 临时直接赋给成员（head@-0x10 先、tail@-0xc 后），块尾即毁。
    {
        m_cmdName = m_line.substr(0, m_pos);
    }
    {
        m_args = m_line.substr(m_pos + 1, std::string::npos);
    }
    trim(m_cmdName);
    m_id = atoi(m_cmdName.c_str());
