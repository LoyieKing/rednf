void CParty::setTitle(const char* title)
{
    strncpy(m_title, title, 0x20);
