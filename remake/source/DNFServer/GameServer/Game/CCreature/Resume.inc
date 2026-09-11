// ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime = 当前秒。
void CStomach::Resume()
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
}