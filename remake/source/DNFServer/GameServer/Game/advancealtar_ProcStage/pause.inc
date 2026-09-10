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
