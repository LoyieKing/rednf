// ORIG parse 0x084ac668：trim(args)；+0x08(int 占位槽) = atoi(args)（金额）。
void CSetGuildFund::parse()
{
    trim(m_parser->m_args);
    m_fund = atoi(m_parser->m_args.c_str());
}