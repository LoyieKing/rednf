bool COverSkillTime::checkTime()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    float diff = static_cast<float>(curSec - m_time);
    return diff > 0.0f && diff < 20.0f;
