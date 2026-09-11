// ORIG parse 0x084a9db0：trim(args)；+0x08 = atoi(args)。
void CPcroomPremiumCmd::parse()
{
    trim(m_parser->m_args);
    m_duration = atoi(m_parser->m_args.c_str());
}