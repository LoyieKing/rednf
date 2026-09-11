// ORIG 0x084b7ff8：return m_endTime <= curSec。
bool CGlobalEffect::isTimeout(long curSec) const
{
    return m_endTime <= curSec;
}