int CHARAC_LOAD_MERCENARY::GetCompetitionState() const
{
    const int remaining = m_field2c - GlobalData::s_systemTime_.getCurSec();
    if (remaining > 0)
        return 1;
    if (m_field30 == 0xff)
        return 0;
    return 2;
