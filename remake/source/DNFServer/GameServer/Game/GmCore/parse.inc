// [b28d-solo-GmCore] 具名块级局部（head/tail）令 4.4 槽序反转为 -0xc/-0x10；改
// **匿名表达式临时**（substr 结果直接赋给成员）后 4.4 复现 ORIG 的 -0x10/-0xc
// 与块尾析构点（探针 fF/fH 实证；具名→匿名是槽向旋钮，语义不变）。
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
}