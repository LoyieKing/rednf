bool CDisconnectDetecter::_IsTimeout(int time)
{
    if (GlobalData::s_systemTime_.getCurSec() - time > 30)
    {
        return true;
    }
    return false;
