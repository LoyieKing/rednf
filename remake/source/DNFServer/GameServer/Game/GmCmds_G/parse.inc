// trim(args)；atoi → +0x08。
void CVillageAttackedStartCmd::parse()
{
    trim(m_parser->m_args);
    reinterpret_cast<IntSlotView*>(this)->m_slot = atoi(m_parser->m_args.c_str());
}