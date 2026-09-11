// ORIG parse 0x084acbe4：trim(args)；+0x08 = (byte)atoi(args.c_str())。
void CBindCubeTest::parse()
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}