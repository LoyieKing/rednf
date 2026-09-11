// ORIG parse 0x084ac80a：trim(args)；*(short*)(this+8) = atoi(c_str(args))。
void CGetCashInven::parse()
{
    trim(m_parser->m_args);
    m_cashType = static_cast<short>(atoi(m_parser->m_args.c_str()));
}