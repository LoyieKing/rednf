CStomach::CStomach()
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
    m_value = 100.0f;   // ORIG mov $0x42c80000
