// ORIG _ZN15cUserHistoryLog15HadesHistoryLogEjjjj @0x08685418
void cUserHistoryLog::HadesHistoryLog(unsigned int a, unsigned int b, unsigned int c,
                                      unsigned int d)
{
    m_user->LogHistory("Hades,%d,%d,%d,%d", a, b, c, d);
}