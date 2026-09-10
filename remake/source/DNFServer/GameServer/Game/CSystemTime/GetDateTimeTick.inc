int GetDateTimeTick()
{
    int tick = GlobalData::s_systemTime_.getCurSec();
    return tick;
