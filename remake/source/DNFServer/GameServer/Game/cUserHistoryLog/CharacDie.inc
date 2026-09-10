void cUserHistoryLog::CharacDie(char const* name, unsigned short unkD,
                                unsigned short unkF)
{
    m_user->LogHistory("Die,\"%s\",%d,%d", name,
                       static_cast<unsigned>(unkD), static_cast<unsigned>(unkF));
