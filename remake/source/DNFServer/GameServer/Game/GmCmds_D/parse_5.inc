// ORIG parse 0x084b1f8c：先清 0；trim(args)；args=="on" → 1。
void CIndependentDropByForce::parse()
{
    ((GmDFld<bool>*)this)->f08 = 0;
    trim(m_parser->m_args);
    if (m_parser->m_args == "on")
        ((GmDFld<bool>*)this)->f08 = 1;
    else
        ((GmDFld<bool>*)this)->f08 = 0;
}