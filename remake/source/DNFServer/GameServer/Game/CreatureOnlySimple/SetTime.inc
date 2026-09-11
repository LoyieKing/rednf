// ORIG _ZN13user_creature14COverSkillTime7SetTimeEv T 0x0833d644：
// call CSystemTime::getCurSec(GlobalData::s_systemTime_) 后 *(int*)this = eax。
void COverSkillTime::SetTime()
{
    m_time = GlobalData::s_systemTime_.getCurSec();
}