void CParty::setTitleIndex(unsigned char idx)
{
    struct V { char m_pad[0x11b]; unsigned char m_f; };
    ((V*)this)->m_f = idx;
