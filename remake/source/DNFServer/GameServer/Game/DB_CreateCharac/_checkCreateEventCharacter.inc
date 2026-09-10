bool DB_CreateCharac::_checkCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）；
    // getCurSec 每边独立调用（ORIG 两次 call）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        if (sig->m_eventCharacFlag != 0)
        {
            sig->m_resultCode = 4;
            return false;
        }
    }
    else
    {
        sig->m_resultCode = 4;
        return false;
    }
    return true;
