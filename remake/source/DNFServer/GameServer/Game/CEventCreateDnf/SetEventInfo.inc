void CEventCreateDnf::SetEventInfo(int hour, int rate, int reloadInterval)
{
    // b28d-D42 verdict=AE(IDENTICAL) ORIG 0x081640d4 orig/ours=96/93。
    // 修法：P227 全表达式 cMyTrace 临时链（3 处）+ P272 两臂发射序反写
    // （kIng 0x95 在前、kReward 0x9a 在后，判据 hour>=m_startHour && <m_endHour）。
    // ORIG 0x081640d4：GetState()==2 且 hour 落在 [m_endHour, m_startHour) →
    // 只打 trace（保持 kReward），不更新 rate。
    if (GetState() == State::kReward && hour < m_startHour && hour >= m_endHour)
    {
        // b28d-D42 P227：ORIG 0x081640d4 的 cMyTrace 为全表达式临时链
        // （varargs 在 ctor 前右起求值并保活 callee-saved）。
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x8e, 0)(
            "[Taiwan, CreateDnfEvent] State:kReward rate:%d", (int)m_rate);
        return;
    }
    // b28d-D42 P272：ORIG 0x081640d4 两臂发射序为 kIng(0x95) 在前、
    // kReward(0x9a) 在后且判据 `jle`（正极性德摩根形）⇒ 反写条件。
    if (hour >= m_startHour && hour < m_endHour)
    {
        setState(State::kIng);
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x95, 0)(
            "[Taiwan, CreateDnfEvent] State:kIng rate:%d", rate);
    }
    else
    {
        setState(State::kReward);
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x9a, 0)(
            "[Taiwan, CreateDnfEvent] State:kReward rate:%d", rate);
    }

    m_rate = static_cast<unsigned short>(rate);
    if (reloadInterval != 0)
        m_reserved = static_cast<unsigned char>(reloadInterval);
