// trim(args)；atoi → +0x08。
void CVillageAttackedStartCmd::parse()
{
    trim(m_parser->m_args);
    m_villageNo = atoi(m_parser->m_args.c_str());
}