void CIndependentDrop::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "all")
        ((GmDID*)this)->all = 1;
    else
        ((GmDID*)this)->all = 0;
