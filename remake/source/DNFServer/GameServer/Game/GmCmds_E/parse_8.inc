// ORIG parse 0x084b399a：parser 非空时按 args[1] 文本映射 1/2/0。
void CRecipeSuccess::parse()
{
    if (m_parser != 0)
    {
        if (m_parser->m_extra[1] == "success")       // ORIG P48：两臂独立 op[] 调用
        {
            m_result = 1;
        }
        else if (m_parser->m_extra[1] == "fail")
        {
            m_result = 2;
        }
        else
        {
            m_result = 0;
        }
        return;                              // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}