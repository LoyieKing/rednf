// ORIG _ZN13user_creature8CStomachC1Ev T 0x08336cd8（C2 同址）：
// this+0 = getCurSec()；this+4 = 0x42c80000（float 100.0f）。
CStomach::CStomach()
{
    m_lastTime = GlobalData::s_systemTime_.getCurSec();
    m_value = 100.0f;   // ORIG mov $0x42c80000
}