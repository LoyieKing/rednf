void ProcStage::procTimeLine()
{
    if (m_state != State::S_N3)
        goto L_out;
    if (m_summonObjectMs.isEndSummon())
    {
        bool made = makeCurrentPhaseSummonObject();
        if (!made)
        {
            m_state = State::S_N4;
        }
    }
    incCurrentMs();
    if (m_state == State::S_N3)
        m_summonObjectMs.summonObjectAndSendPacket();
L_out:;
