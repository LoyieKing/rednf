// ORIG parse 0x084b0032：clear；sscanf(args,"%d%d")。
void CRandomObjectTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d",
           &m_mode, &m_count);
}