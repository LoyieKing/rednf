bool CPad::isValidKeyGenTime()
{
    if ((GlobalData::s_systemTime_.getCurSec() - m_lastGenTime30) > 0x14)
    {
        m_lastGenTime30 = GlobalData::s_systemTime_.getCurSec();
        return true;
    }
    return false;
