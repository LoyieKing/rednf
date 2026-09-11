// ORIG parse 0x084b0b08：trim(args)；args=="all" → +0x08=1 否则 0。
void CIndependentDrop::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "all")
        ((GmDID*)this)->all = 1;
    else
        ((GmDID*)this)->all = 0;
}