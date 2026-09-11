// ORIG T 0x081b2e86：返回 1=竞技中、0=未报名、2=已结束且已报名。
int CHARAC_LOAD_MERCENARY::GetCompetitionState() const
{
    const int remaining = m_field2c - GlobalData::s_systemTime_.getCurSec();
    if (remaining > 0)
        return 1;
    if (m_field30 == 0xff)
        return 0;
    return 2;
}