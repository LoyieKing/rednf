void EventNewCharacterReward::StartEvent(Word_Param param)
{
    m_job = param.m_duration;
    m_field10 = param.m_field2;
    m_addInfo.m_info0 = static_cast<unsigned short>(m_job);
    m_addInfo.m_info1 = static_cast<unsigned short>(m_field10);
    if (m_job >= 0 && m_job <= 10)
    {
        // P37 临时链式：args 表达式分派到 callee-saved 寄存器（ORIG esi/ebx）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22, 0)(
            "[Taiwan, EventCharacterBonus] Event parameters (job:%d, exp_ratio:%d)",
            m_job, m_field10);
        this->StartEvent();
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x27, 0)(
            "[Taiwan, EventCharacterBonus] Invalid event parameters (job:%d, exp_ratio:%d)",
            m_job, m_field10);
    }
