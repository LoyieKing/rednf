// ORIG parse 0x084b34a4：+0x08(int 占位槽) = 0；trim(args)；
// atoi(parser->m_extra[1])（第二个分词）。
void CSetMultiboxFailCnt::parse()
{
    m_failCnt = 0;
    trim(m_parser->m_args);
    m_failCnt = atoi(m_parser->m_extra[1].c_str());   // ORIG 直接 operator[](1)，无边界检查
}