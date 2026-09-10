void CLevelUp::parse()
{
    if (m_parser != 0)
    {
        int level = static_cast<int>(                       // ORIG 中间变量 -0xc(%ebp)
            StringToNumber(m_parser->m_extra[1].c_str()));
        setLevel(level);
        return;                                             // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
