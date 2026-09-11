// ORIG parse 0x084aeb5e：clear() 后 sscanf(args,"%d%d",&+0x08,&+0x0c)。
void CBoosterTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d", &m_mode, &m_count);
}