// ----------------------------------------------------------------------------
// ORIG T 0x0812eaa6（pause.md 全文）：pause=true 且 state==3 → state=2、ret1；
 // pause=false 且 state∈{2,1} → state=3、ret1；其余 ret0。
// ----------------------------------------------------------------------------
int ProcStage::pause(bool pauseOn)
{
    if (pauseOn)
    {
        if (m_state != State::S_N3)
        {
            return 0;
        }
        m_state = State::S_N2;
    }
    else
    {
        if (m_state != State::S_N2 && m_state != State::S_N1)
        {
            return 0;
        }
        m_state = State::S_N3;
    }
    return 1;
}