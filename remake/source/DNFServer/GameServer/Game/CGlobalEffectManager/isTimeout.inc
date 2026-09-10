bool CGlobalEffect::isTimeout(long curSec) const
{
    return m_endTime <= curSec;
