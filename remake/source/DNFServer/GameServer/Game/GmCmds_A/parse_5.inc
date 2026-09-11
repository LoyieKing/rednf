// ORIG parse 0x084b35c4：trim(args)；+0x08 = atoi(args.c_str())。
void CBingoGmTest::parse()
{
    trim(m_parser->m_args);
    m_subCmd = atoi(m_parser->m_args.c_str());
}