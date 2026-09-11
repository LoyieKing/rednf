// ORIG T 0x085998a6：当前秒距上次生成 >20s 判有效并刷新时间戳。
bool CPad::isValidKeyGenTime()
{
    if ((GlobalData::s_systemTime_.getCurSec() - m_lastGenTime30) > 0x14)
    {
        m_lastGenTime30 = GlobalData::s_systemTime_.getCurSec();
        return true;
    }
    return false;
}