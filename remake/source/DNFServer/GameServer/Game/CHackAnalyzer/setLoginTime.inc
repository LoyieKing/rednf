void Secu_AccountHacking::setLoginTime()
{
    // ORIG T 0x08279020（setLoginTime.md）：+0xc = s_systemTime_.getCurSec()。
    m_loginTime = GlobalData::s_systemTime_.getCurSec();
}