// ----------------------------------------------------------------------------
// ORIG T 0x0812e0d6（procTimeLine.md 全文）：state==3 时 isEndSummon →
//   makeCurrentPhaseSummonObject 失败置 state=4；incCurrentMs 后仍为 3 则
//   summonObjectAndSendPacket()。
// ----------------------------------------------------------------------------
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
}